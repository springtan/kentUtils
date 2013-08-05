/* customFactory - a polymorphic object for handling
 * creating various types of custom tracks. */

#include <pthread.h>
#include "common.h"
#include "errCatch.h"
#include "hash.h"
#include "linefile.h"
#include "portable.h"
#include "obscure.h"
#include "binRange.h"
#include "pipeline.h"
#include "jksql.h"
#include "net.h"
#include "bed.h"
#include "psl.h"
#include "gff.h"
#include "wiggle.h"
#include "genePred.h"
#include "trackDb.h"
#include "hgConfig.h"
#include "hdb.h"
#include "hui.h"
#include "customTrack.h"
#include "customPp.h"
#include "customFactory.h"
#include "trashDir.h"
#include "jsHelper.h"
#include "encode/encodePeak.h"
#include "udc.h"
#include "bbiFile.h"
#include "bigWig.h"
#include "bigBed.h"
#ifdef USE_BAM
#include "hgBam.h"
#endif//def USE_BAM
#include "vcf.h"
#include "makeItemsItem.h"
#include "bedDetail.h"
#include "pgSnp.h"
#include "regexHelper.h"
#include "chromInfo.h"

static boolean doExtraChecking = FALSE;

/*** Utility routines used by many factories. ***/

char *customFactoryNextTilTrack(struct customPp *cpp)
/* Return next line.  Return NULL at end of input or at line starting with
 * "track." */
{
char *line = customPpNext(cpp);
if (line != NULL && startsWithWord("track", line))
    {
    customPpReuse(cpp, line);
    line = NULL;
    }
return line;
}

char *customFactoryNextRealTilTrack(struct customPp *cpp)
/* Return next "real" line (not blank, not comment).
 * Return NULL at end of input or at line starting with
 * "track." */
{
char *line = customPpNextReal(cpp);
if (line != NULL && startsWithWord("track", line))
    {
    customPpReuse(cpp, line);
    line = NULL;
    }
return line;
}

void customFactoryCheckChromNameDb(char *genomeDb, char *word, struct lineFile *lf)
/* Make sure it's a chromosome or a contig.  Well, at the moment,
 * just make sure it's a chromosome. */
{
if (!hgIsOfficialChromName(genomeDb, word))
    lineFileAbort(lf, "%s not a recognized sequence (note: sequence names are case sensitive)",
		  word);
}

void customFactorySetupDbTrack(struct customTrack *track)
/* Fill in fields most database-resident custom tracks need. */
{
struct tempName tn;
char prefix[16];
static int dbTrackCount = 0;
struct sqlConnection *ctConn = hAllocConn(CUSTOM_TRASH);
++dbTrackCount;
if ( dbTrackCount > 1000 )
    errAbort("ERROR: too many tracks in this submission (more than 1000). Using a <A HREF='/goldenPath/help/hgTrackHubHelp.html' TARGET=_blank>Track Hub</A> may be a better option for this data.");
safef(prefix, sizeof(prefix), "t%d", dbTrackCount);
track->dbTableName = sqlTempTableName(ctConn, prefix);
ctAddToSettings(track, "dbTableName", track->dbTableName);
trashDirFile(&tn, "ct", "ct", ".err");
track->dbStderrFile = cloneString(tn.forCgi);
track->dbDataLoad = TRUE;
track->dbTrack = TRUE;
hFreeConn(&ctConn);
}

static char *ctGenomeOrCurrent(struct customTrack *ct)
/* return database setting */
{
char *ctDb = ctGenome(ct);
if (ctDb == NULL)
    ctDb = ct->genomeDb;
return ctDb;
}

static boolean sameType(char *a, char *b)
/* Case-sensitive comparison of first word if multiple words,
 * so that we can compare types like "bed" vs. "bed 3 ." etc.
 * Tolerates one null input but not two. */
{
if (a == NULL && b == NULL)
    errAbort("sameType should not be called when both inputs are NULL.");
else if (a == NULL || b == NULL)
    return FALSE;
char *aCopy = cloneString(a);
char *bCopy = cloneString(b);
char *aWord = firstWordInLine(aCopy);
char *bWord = firstWordInLine(bCopy);
boolean same = sameString(aWord, bWord);
freeMem(aCopy);
freeMem(bCopy);
return same;
}

/*** BED Factory ***/

static boolean rowIsBed(char **row, int wordCount, char *db)
/* Return TRUE if row is consistent with BED format. */
{
return wordCount >= 3 && wordCount <= bedKnownFields
	&& hgIsOfficialChromName(db, row[0])
	&& isdigit(row[1][0]) && isdigit(row[2][0]);
}

static boolean bedRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a bed track */
{
if (type != NULL && !sameType(type, fac->name))
    return FALSE;
char *line = customFactoryNextRealTilTrack(cpp);
if (line == NULL)
    return FALSE;
char *dupe = cloneString(line);
char *row[bedKnownFields+1];
int wordCount = chopLine(dupe, row);
char *ctDb = ctGenomeOrCurrent(track);
boolean isBed = rowIsBed(row, wordCount, ctDb);
freeMem(dupe);
if (isBed)
    track->fieldCount = wordCount;
customPpReuse(cpp, line);
return isBed;
}

static boolean microarrayRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a microarray track */
{
return bedRecognizer(fac, cpp, type, track) && (track->fieldCount == 15);
}

static boolean coloredExonRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a colored-exon track */
{
return bedRecognizer(fac, cpp, type, track) && (track->fieldCount >= 14);
}

static struct pipeline *bedLoaderPipe(struct customTrack *track)
/* Set up pipeline that will load wig into database. */
{
/* running the single command:
 *	hgLoadBed -customTrackLoader -tmpDir=/data/tmp
 *		-maxChromNameLength=${nameLength} customTrash tableName stdin
 * -customTrackLoader turns on options: -noNameIx -noHistory -ignoreEmpty
 *	-allowStartEqualEnd -allowNegativeScores -verbose=0
 */
struct dyString *tmpDy = newDyString(0);
char *cmd1[] = {"loader/hgLoadBed", "-customTrackLoader",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL};
char *tmpDir = cfgOptionDefault("customTracks.tmpdir", "/data/tmp");
struct stat statBuf;
int index = 2;

if (stat(tmpDir,&statBuf))
    errAbort("can not find custom track tmp load directory: '%s'<BR>\n"
	"create directory or specify in hg.conf customTracks.tmpdir", tmpDir);
dyStringPrintf(tmpDy, "-tmpDir=%s", tmpDir);
cmd1[index++] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-maxChromNameLength=%d", track->maxChromName);
cmd1[index++] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
if(startsWithWord("bedGraph", track->dbTrackType))
    {
    /* we currently assume that last field is the bedGraph field. */
    dyStringPrintf(tmpDy, "-bedGraph=%d", track->fieldCount);
    cmd1[index++] = dyStringCannibalize(&tmpDy);
    }
cmd1[index++] = CUSTOM_TRASH;
cmd1[index++] = track->dbTableName;
cmd1[index++] = "stdin";
assert(index <= ArraySize(cmd1));

/* the "/dev/null" file isn't actually used for anything, but it is used
 * in the pipeLineOpen to properly get a pipe started that isn't simply
 * to STDOUT which is what a NULL would do here instead of this name.
 *	This function exits if it can't get the pipe created
 *	The dbStderrFile will get stderr messages from hgLoadBed into the
 *	our private error log so we can send it back to the user
 */
return pipelineOpen1(cmd1, pipelineWrite | pipelineNoAbort,
	"/dev/null", track->dbStderrFile);
}

static void pipelineFailExit(struct customTrack *track)
/* show up to three lines of error message to stderr and errAbort */
{
struct dyString *errDy = newDyString(0);
struct lineFile *lf;
char *line;
int i;
dyStringPrintf(errDy, "track load error (track name='%s'):<BR>\n", track->tdb->track);
lf = lineFileOpen(track->dbStderrFile, TRUE);
i = 0;
while( (i < 3) && lineFileNext(lf, &line, NULL))
    {
    dyStringPrintf(errDy, "%s<BR>\n", line);
    ++i;
    // break out of loop after wibSizeLimit msg to avoid printing stuff from other commands in the pipe.
    if(strstr(line, "wibSizeLimit"))
        break;
    }
lineFileClose(&lf);
if (i < 1)
    dyStringPrintf(errDy, "unknown failure<BR>\n");
unlink(track->dbStderrFile);
errAbort("%s",dyStringCannibalize(&errDy));
}

static struct customTrack *bedFinish(struct customTrack *track,
	boolean dbRequested)
/* Finish up bed tracks (and others that create track->bedList). */
{
/* Add type based on field count */
char buf[20];
safef(buf, sizeof(buf), "%s %d .", track->tdb->type != NULL && startsWithWord("bedGraph", track->tdb->type) ? "bedGraph" : "bed", track->fieldCount);
track->tdb->type = cloneString(buf);
track->dbTrackType = cloneString(buf);
safef(buf, sizeof(buf), "%d", track->fieldCount);
ctAddToSettings(track, "fieldCount", cloneString(buf));

/* If necessary add track offsets. */
int offset = track->offset;
if (offset != 0)
    {
    /* Add track offsets if any */
    struct bed *bed;
    for (bed = track->bedList; bed != NULL; bed = bed->next)
	{
	bed->chromStart += offset;
	bed->chromEnd += offset;
	if (track->fieldCount > 7)
	    {
	    bed->thickStart += offset;
	    bed->thickEnd += offset;
	    }
	}
    track->offset = 0;	/*	so DB load later won't do this again */
    hashMayRemove(track->tdb->settingsHash, "offset"); /* nor the file reader*/
    }

/* If necessary load database */
if (dbRequested)
    {
    customFactorySetupDbTrack(track);
    struct pipeline *dataPipe = bedLoaderPipe(track);
    FILE *out = pipelineFile(dataPipe);
    struct bed *bed;
    for (bed = track->bedList; bed != NULL; bed = bed->next)
	{
	bedOutputN(bed, track->fieldCount, out, '\t', '\n');
	}
    fflush(out);		/* help see error from loader failure */
    if(ferror(out) || pipelineWait(dataPipe))
	pipelineFailExit(track);	/* prints error and exits */
    unlink(track->dbStderrFile);	/* no errors, not used */
    pipelineFree(&dataPipe);
    }
return track;
}

static struct bed *customTrackBed(char *row[64], int wordCount, int chromSize, struct lineFile *lf)
/* Convert a row of strings to a bed.
 * Intended to replace old customTrackBed,
 * currently new code is activated by hg.conf switch */
{
struct bed * bed;
AllocVar(bed);
loadAndValidateBed(row, wordCount, wordCount, lf, bed, NULL, TRUE);
if (bed->chromEnd > chromSize)
    lineFileAbort(lf, "chromEnd larger than chrom %s size (%d > %d)",
    	bed->chrom, bed->chromEnd, chromSize);
return bed;
}

static struct bed *customTrackBedOld(char *db, char *row[13], int wordCount,
	struct hash *chromHash, struct lineFile *lf)
/* Convert a row of strings to a bed. */
{
struct bed * bed;
int count;
AllocVar(bed);
bed->chrom = hashStoreName(chromHash, row[0]);
customFactoryCheckChromNameDb(db, bed->chrom, lf);

bed->chromStart = lineFileNeedNum(lf, row, 1);
bed->chromEnd = lineFileNeedNum(lf, row, 2);
if (bed->chromEnd < 1)
    lineFileAbort(lf, "chromEnd less than 1 (%d)", bed->chromEnd);
if (bed->chromEnd < bed->chromStart)
    lineFileAbort(lf, "chromStart after chromEnd (%d > %d)",
    	bed->chromStart, bed->chromEnd);
int chromSize = hChromSize(db, bed->chrom);
if (bed->chromEnd > chromSize)
    lineFileAbort(lf, "chromEnd larger than chrom %s size (%d > %d)",
    	bed->chrom, bed->chromEnd, chromSize);
if (wordCount > 3)
     bed->name = cloneString(row[3]);
if (wordCount > 4)
     bed->score = lineFileNeedNum(lf, row, 4);
if (wordCount > 5)
     {
     strncpy(bed->strand, row[5], sizeof(bed->strand));
     if (bed->strand[0] != '+' && bed->strand[0] != '-' && bed->strand[0] != '.')
	  lineFileAbort(lf, "Expecting + or - in strand");
     }
if (wordCount > 6)
     bed->thickStart = lineFileNeedNum(lf, row, 6);
else
     bed->thickStart = bed->chromStart;
if (wordCount > 7)
     {
     bed->thickEnd = lineFileNeedNum(lf, row, 7);
     if (bed->thickEnd < bed->thickStart)
	 lineFileAbort(lf, "thickStart after thickEnd");
     if ((bed->thickStart != 0) &&
	 ((bed->thickStart < bed->chromStart) ||
	  (bed->thickStart > bed->chromEnd)))
        lineFileAbort(lf,
	     "thickStart out of range (chromStart to chromEnd, or 0 if no CDS)");
     if ((bed->thickEnd != 0) &&
	 ((bed->thickEnd < bed->chromStart) ||
	  (bed->thickEnd > bed->chromEnd)))
        lineFileAbort(lf,
	     "thickEnd out of range for %s:%d-%d, thick:%d-%d (chromStart to chromEnd, or 0 if no CDS)",
		       bed->name, bed->chromStart, bed->chromEnd,
		       bed->thickStart, bed->thickEnd);
     }
else
     bed->thickEnd = bed->chromEnd;
if (wordCount > 8)
    {
    char *comma;
    /*	Allow comma separated list of rgb values here	*/
    comma = strchr(row[8], ',');
    if (comma)
	{
	int rgb = bedParseRgb(row[8]);
	if (rgb < 0)
	    lineFileAbort(lf,
	        "Expecting 3 comma separated numbers for r,g,b bed item color.");
	else
	    bed->itemRgb = rgb;
	}
    else
	bed->itemRgb = lineFileNeedNum(lf, row, 8);
    }
if (wordCount > 9)
    bed->blockCount = lineFileNeedNum(lf, row, 9);
if (wordCount > 10)
    {
    sqlSignedDynamicArray(row[10], &bed->blockSizes, &count);
    if (count != bed->blockCount)
	lineFileAbort(lf,  "expecting %d elements in array", bed->blockCount);
    }
if (wordCount > 11)
    {
    int i;
    int lastEnd, lastStart;
    sqlSignedDynamicArray(row[11], &bed->chromStarts, &count);
    if (count != bed->blockCount)
	lineFileAbort(lf, "expecting %d elements in array", bed->blockCount);
    // tell the user if they appear to be using absolute starts rather than
    // relative... easy to forget!  Also check block order, coord ranges...
    lastStart = -1;
    lastEnd = 0;
    for (i=0;  i < bed->blockCount;  i++)
	{
/*
printf("%d:%d %s %s s:%d c:%u cs:%u ce:%u csI:%d bsI:%d ls:%d le:%d<BR>\n", lineIx, i, bed->chrom, bed->name, bed->score, bed->blockCount, bed->chromStart, bed->chromEnd, bed->chromStarts[i], bed->blockSizes[i], lastStart, lastEnd);
*/
	if (bed->chromStarts[i]+bed->chromStart >= bed->chromEnd)
	    {
	    if (bed->chromStarts[i] >= bed->chromStart)
		lineFileAbort(lf,
		    "BED chromStarts offsets must be relative to chromStart, "
		    "not absolute.  Try subtracting chromStart from each offset "
		    "in chromStarts.");
	    else
		lineFileAbort(lf,
		    "BED chromStarts[i]+chromStart must be less than chromEnd.");
	    }
	lastStart = bed->chromStarts[i];
	lastEnd = bed->chromStart + bed->chromStarts[i] + bed->blockSizes[i];
	}
    if (bed->chromStarts[0] != 0)
	lineFileAbort(lf,
	    "BED blocks must span chromStart to chromEnd.  "
	    "BED chromStarts[0] must be 0 (==%d) so that (chromStart + "
	    "chromStarts[0]) equals chromStart.", bed->chromStarts[0]);
    i = bed->blockCount-1;
    if ((bed->chromStart + bed->chromStarts[i] + bed->blockSizes[i]) !=
	bed->chromEnd)
	{
	lineFileAbort(lf,
	    "BED blocks must span chromStart to chromEnd.  (chromStart + "
	    "chromStarts[last] + blockSizes[last]) must equal chromEnd.");
	}
    }
if (wordCount > 12)
    // get the microarray/colored-exon fields
    {
    bed->expCount = (int)sqlUnsigned(row[12]);
    sqlSignedDynamicArray(row[13], &bed->expIds, &count);
    if (count != bed->expCount)
	lineFileAbort(lf, "expecting %d elements in expIds array (bed field 14)",
		      bed->expCount);
    if (wordCount == 15)
	{
	sqlFloatDynamicArray(row[14], &bed->expScores, &count);
	if (count != bed->expCount)
	    lineFileAbort(lf, "expecting %d elements in expScores array (bed field 15)",
			  bed->expCount);
	}
    }
return bed;
}

static struct customTrack *bedLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up bed data until get next track line. */
{
char *line;
char *db = ctGenomeOrCurrent(track);
char *lastChrom = NULL;
int chromSize = -1;
boolean newCustomTrackValidate = sameOk(cfgOption("newCustomTrackValidate"), "on");
while ((line = customFactoryNextRealTilTrack(cpp)) != NULL)
    {
    char *row[bedKnownFields];
    int wordCount = chopLine(line, row);
    struct lineFile *lf = cpp->fileStack;
    lineFileExpectAtLeast(lf, track->fieldCount, wordCount);

    /* since rows are often sorted, we can reduce repetitive checking */
    if (differentStringNullOk(row[0], lastChrom))
	{
	customFactoryCheckChromNameDb(db, row[0], lf);
	chromSize = hChromSize(db, row[0]);
	freez(&lastChrom);
	lastChrom = cloneString(row[0]);
	}

    struct bed *bed = NULL;

    /* Intended to replace old customTrackBed */
    if (newCustomTrackValidate)
	{
	bed = customTrackBed(row, wordCount, chromSize, lf);
	bed->chrom = hashStoreName(chromHash, row[0]);
	}
    else
	{
	bed = customTrackBedOld(db, row, wordCount, chromHash, lf);
	}

    slAddHead(&track->bedList, bed);
    }
slReverse(&track->bedList);
return bedFinish(track, dbRequested);
}

static struct customTrack *bedGraphLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up bedGraph data until get next track line. */
{
char buf[20];
bedLoader(fac, chromHash, cpp, track, dbRequested);

/* Trailing period in "bedGraph N ." confuses bedGraphMethods, so replace type */
freeMem(track->tdb->type);
safef(buf, sizeof(buf), "bedGraph %d", track->fieldCount);
track->tdb->type = cloneString(buf);
return track;
}

static struct customTrack *microarrayLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up microarray data until get next track line. */
{
struct customTrack *ct = bedLoader(fac, chromHash, cpp, track, dbRequested);
freeMem(track->tdb->type);
/* /\* freeMem(track->dbTrackType); *\/ */
track->tdb->type = cloneString("array");
/* track->dbTrackType = cloneString("expRatio"); */
return ct;
}

static struct customTrack *coloredExonLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up microarray data until get next track line. */
{
struct customTrack *ct = bedLoader(fac, chromHash, cpp, track, dbRequested);
freeMem(track->tdb->type);
track->tdb->type = cloneString("coloredExon");
return ct;
}

static struct customFactory bedFactory =
/* Factory for bed tracks */
    {
    NULL,
    "bed",
    bedRecognizer,
    bedLoader,
    };

static struct customFactory bedGraphFactory =
    {
    NULL,
    "bedGraph",
    bedRecognizer,
    bedGraphLoader,
    };

static struct customFactory microarrayFactory =
/* Factory for bed tracks */
    {
    NULL,
    "array",
    microarrayRecognizer,
    microarrayLoader,
    };

static struct customFactory coloredExonFactory =
/* Factory for bed tracks */
    {
    NULL,
    "coloredExon",
    coloredExonRecognizer,
    coloredExonLoader,
    };

/**** ENCODE PEAK Factory - closely related to BED but not quite ***/

static boolean encodePeakRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling an encodePeak track */
{
enum encodePeakType pt = 0;
if (type != NULL && !sameType(type, fac->name) &&
    !sameString(type, "narrowPeak") && !sameString(type, "broadPeak") && !sameString(type, "gappedPeak"))
    return FALSE;
char *line = customFactoryNextRealTilTrack(cpp);
if (line == NULL)
    return FALSE;
char *dupe = cloneString(line);
char *row[ENCODE_PEAK_KNOWN_FIELDS+1];
int wordCount = chopLine(dupe, row);
pt = encodePeakInferType(wordCount, type);
if (pt)
    track->fieldCount = wordCount;
freeMem(dupe);
customPpReuse(cpp, line);
return (pt != 0);
}

static struct pipeline *encodePeakLoaderPipe(struct customTrack *track)
/* Set up pipeline that will load the encodePeak into database. */
{
/* running the single command:
 *	hgLoadBed -customTrackLoader -sqlTable=loader/encodePeak.sql -renameSqlTable
 *                -trimSqlTable -notItemRgb -tmpDir=/data/tmp
 *		-maxChromNameLength=${nameLength} customTrash tableName stdin
 */
struct dyString *tmpDy = newDyString(0);
char *cmd1[] = {"loader/hgLoadBed", "-customTrackLoader",
	"-sqlTable=loader/encodePeak.sql", "-renameSqlTable", "-trimSqlTable", "-notItemRgb", NULL, NULL, NULL, NULL, NULL, NULL};
char *tmpDir = cfgOptionDefault("customTracks.tmpdir", "/data/tmp");
struct stat statBuf;
int index = 6;

if (stat(tmpDir,&statBuf))
    errAbort("can not find custom track tmp load directory: '%s'<BR>\n"
	"create directory or specify in hg.conf customTracks.tmpdir", tmpDir);
dyStringPrintf(tmpDy, "-tmpDir=%s", tmpDir);
cmd1[index++] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-maxChromNameLength=%d", track->maxChromName);
cmd1[index++] = dyStringCannibalize(&tmpDy);
cmd1[index++] = CUSTOM_TRASH;
cmd1[index++] = track->dbTableName;
cmd1[index++] = "stdin";
assert(index <= ArraySize(cmd1));

/* the "/dev/null" file isn't actually used for anything, but it is used
 * in the pipeLineOpen to properly get a pipe started that isn't simply
 * to STDOUT which is what a NULL would do here instead of this name.
 *	This function exits if it can't get the pipe created
 *	The dbStderrFile will get stderr messages from hgLoadBed into the
 *	our private error log so we can send it back to the user
 */
return pipelineOpen1(cmd1, pipelineWrite | pipelineNoAbort,
	"/dev/null", track->dbStderrFile);
}

/* Need customTrackEncodePeak */
static struct encodePeak *customTrackEncodePeak(char *db, char **row, enum encodePeakType pt,
	struct hash *chromHash, struct lineFile *lf)
/* Convert a row of strings to a bed. */
{
struct encodePeak *peak = encodePeakLineFileLoad(row, pt, lf);
hashStoreName(chromHash, peak->chrom);
customFactoryCheckChromNameDb(db, peak->chrom, lf);
return peak;
}

/*   remember to set all the custom track settings necessary */
static struct customTrack *encodePeakFinish(struct customTrack *track, struct encodePeak *peakList, enum encodePeakType pt)
/* Finish up bed tracks (and others that create track->bedList). */
{
struct encodePeak *peak;
char buf[20];
track->fieldCount = ENCODEPEAK_NUM_COLS;
if ((pt == narrowPeak) || (pt == broadPeak))
    track->fieldCount = ENCODE_PEAK_NARROW_PEAK_FIELDS;
track->tdb->type = cloneString("encodePeak");
track->dbTrackType = cloneString("encodePeak");
safef(buf, sizeof(buf), "%d", track->fieldCount);
ctAddToSettings(track, "fieldCount", cloneString(buf));

/* If necessary add track offsets. */
int offset = track->offset;
if (offset != 0)
    {
    /* Add track offsets if any */
    for (peak = peakList; peak != NULL; peak = peak->next)
	{
	peak->chromStart += offset;
	peak->chromEnd += offset;
	}
    track->offset = 0;	/*	so DB load later won't do this again */
    hashMayRemove(track->tdb->settingsHash, "offset"); /* nor the file reader*/
    }

/* If necessary load database */
customFactorySetupDbTrack(track);
struct pipeline *dataPipe = encodePeakLoaderPipe(track);
FILE *out = pipelineFile(dataPipe);
for (peak = peakList; peak != NULL; peak = peak->next)
    encodePeakOutputWithType(peak, encodePeak, out);
fflush(out);		/* help see error from loader failure */
if(ferror(out) || pipelineWait(dataPipe))
    pipelineFailExit(track);	/* prints error and exits */
unlink(track->dbStderrFile);	/* no errors, not used */
pipelineFree(&dataPipe);
return track;
}

static struct customTrack *encodePeakLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up encodePeak data until get next track line. */
{
char *line;
char *db = ctGenomeOrCurrent(track);
struct encodePeak *peakList = NULL;
enum encodePeakType pt = encodePeakInferType(track->fieldCount, track->tdb->type);
if (!dbRequested)
    errAbort("encodePeak custom track type unavailable without custom trash database. Please set that up in your hg.conf");
while ((line = customFactoryNextRealTilTrack(cpp)) != NULL)
    {
    char *row[ENCODE_PEAK_KNOWN_FIELDS];
    int wordCount = chopLine(line, row);
    struct lineFile *lf = cpp->fileStack;
    lineFileExpectAtLeast(lf, track->fieldCount, wordCount);
    struct encodePeak *peak = customTrackEncodePeak(db, row, pt, chromHash, lf);
    slAddHead(&peakList, peak);
    }
slReverse(&peakList);
return encodePeakFinish(track, peakList, pt);
}

static struct customFactory encodePeakFactory =
/* Factory for bed tracks */
    {
    NULL,
    "encodePeak",
    encodePeakRecognizer,
    encodePeakLoader,
    };

/*** bedDetail Factory - close to bed but added fields for detail page ***/

static boolean bedDetailRecognizer(struct customFactory *fac,
        struct customPp *cpp, char *type,
        struct customTrack *track)
/* Return TRUE if looks like we're handling an bedDetail track */
{
if (type != NULL && !sameType(type, fac->name) &&
    !sameString(type, "bedDetail") )
    return FALSE;
if (type == NULL)
    return FALSE; /* just in case gets past bed etc. */
char *line = customFactoryNextRealTilTrack(cpp);
if (line == NULL)
    return FALSE;
char *dupe = cloneString(line);
char *row[14+3];
int wordCount = chopTabs(dupe, row);
if (wordCount > 14 || wordCount < 5)
    return FALSE;
track->fieldCount = wordCount;
/* bed 4 + so is first part bed? */
char *ctDb = ctGenomeOrCurrent(track);
int bedCount = wordCount -2;
boolean isBed = rowIsBed(row, bedCount, ctDb);
freeMem(dupe);
customPpReuse(cpp, line);
return (isBed);
}

static struct pipeline *bedDetailLoaderPipe(struct customTrack *track)
/* Set up pipeline that will load the bedDetail into database. */
/* Must be tab separated file, so that can have spaces in description */
{
/* running the single command:
 *	hgLoadBed -customTrackLoader -sqlTable=loader/bedDetail.sql -renameSqlTable
 *                -trimSqlTable -notItemRgb -tmpDir=/data/tmp
 *		-maxChromNameLength=${nameLength} customTrash tableName stdin
 */
struct dyString *tmpDy = newDyString(0);
//bed size can vary
char *cmd1[] = {"loader/hgLoadBed", "-customTrackLoader", "-tab", "-noBin",
	"-sqlTable=loader/bedDetail.sql", "-renameSqlTable", "-trimSqlTable", "-bedDetail", NULL, NULL, NULL, NULL, NULL, NULL};
char *tmpDir = cfgOptionDefault("customTracks.tmpdir", "/data/tmp");
struct stat statBuf;
int index = 8;

if (stat(tmpDir,&statBuf))
    errAbort("can not find custom track tmp load directory: '%s'<BR>\n"
	"create directory or specify in hg.conf customTracks.tmpdir", tmpDir);
dyStringPrintf(tmpDy, "-tmpDir=%s", tmpDir);
cmd1[index++] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-maxChromNameLength=%d", track->maxChromName);
cmd1[index++] = dyStringCannibalize(&tmpDy);
cmd1[index++] = CUSTOM_TRASH;
cmd1[index++] = track->dbTableName;
cmd1[index++] = "stdin";
assert(index <= ArraySize(cmd1));

/* the "/dev/null" file isn't actually used for anything, but it is used
 * in the pipeLineOpen to properly get a pipe started that isn't simply
 * to STDOUT which is what a NULL would do here instead of this name.
 *	This function exits if it can't get the pipe created
 *	The dbStderrFile will get stderr messages from hgLoadBed into the
 *	our private error log so we can send it back to the user
 */
return pipelineOpen1(cmd1, pipelineWrite | pipelineNoAbort,
	"/dev/null", track->dbStderrFile);
}

/* customTrackBedDetail load item */
static struct bedDetail *customTrackBedDetail(char *db, char **row,
        struct hash *chromHash, struct lineFile *lf, int size)
/* Convert a row of strings to a bed 4 + for bedDetail. */
{
struct bedDetail *item = bedDetailLineFileLoad(row, size, lf);
hashStoreName(chromHash, item->chrom);
customFactoryCheckChromNameDb(db, item->chrom, lf);
int chromSize = hChromSize(db, item->chrom);
if (item->chromEnd > chromSize)
    lineFileAbort(lf, "chromEnd larger than chrom %s size (%d > %d)",
        item->chrom, item->chromEnd, chromSize);
return item;
}

/*   remember to set all the custom track settings necessary */
static struct customTrack *bedDetailFinish(struct customTrack *track, struct bedDetail *itemList)
/* Finish up bedDetail tracks (and others that create track->bedList). */
{
struct bedDetail *item;
char buf[50];
track->tdb->type = cloneString("bedDetail");
track->dbTrackType = cloneString("bedDetail");
safef(buf, sizeof(buf), "%d", track->fieldCount);
ctAddToSettings(track, "fieldCount", cloneString(buf)); /* set by recognizer? */
safef(buf, sizeof(buf), "%d", slCount(itemList));
ctAddToSettings(track, "itemCount", cloneString(buf));
safef(buf, sizeof(buf), "%s:%u-%u", itemList->chrom,
                itemList->chromStart, itemList->chromEnd);
ctAddToSettings(track, "firstItemPos", cloneString(buf));

/* If necessary add track offsets. */
int offset = track->offset;
if (offset != 0)
    {
    /* Add track offsets if any */
    for (item = itemList; item != NULL; item = item->next)
	{
	item->chromStart += offset;
	item->chromEnd += offset;
	}
    track->offset = 0;	/*	so DB load later won't do this again */
    hashMayRemove(track->tdb->settingsHash, "offset"); /* nor the file reader*/
    }

/* If necessary load database */
customFactorySetupDbTrack(track);
struct pipeline *dataPipe = bedDetailLoaderPipe(track);
FILE *out = pipelineFile(dataPipe);
for (item = itemList; item != NULL; item = item->next)
    bedDetailOutput(item, out, '\t', '\n', track->fieldCount);
fflush(out);		/* help see error from loader failure */
if(ferror(out) || pipelineWait(dataPipe))
    pipelineFailExit(track);	/* prints error and exits */
unlink(track->dbStderrFile);	/* no errors, not used */
pipelineFree(&dataPipe);
return track;
}

static struct customTrack *bedDetailLoader(struct customFactory *fac,
        struct hash *chromHash,
        struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up bedDetail data until get next track line. */
{
char *line;
char *db = ctGenomeOrCurrent(track);
struct bedDetail *itemList = NULL;
if (!dbRequested)
    errAbort("bedDetail custom track type unavailable without custom trash database. Please set that up in your hg.conf");
while ((line = customFactoryNextRealTilTrack(cpp)) != NULL)
    {
    char *row[15];
    int wordCount = chopTabs(line, row);
    struct lineFile *lf = cpp->fileStack;
    lineFileExpectAtLeast(lf, track->fieldCount, wordCount);
    struct bedDetail *item = customTrackBedDetail(db, row, chromHash, lf, wordCount);
    slAddHead(&itemList, item);
    }
slReverse(&itemList);
return bedDetailFinish(track, itemList);
}

static struct customFactory bedDetailFactory =
/* Factory for bedDetail tracks */
    {
    NULL,
    "bedDetail",
    bedDetailRecognizer,
    bedDetailLoader,
    };

/*** pgSnp Factory - allow pgSnp(personal genome SNP) custom tracks ***/

static boolean rowIsPgSnp (char **row, char *db, char *type)
/* return TRUE if row looks like a pgSnp row */
{
boolean isPgSnp = rowIsBed(row, 3, db);
if (type != NULL && !sameWord(type, "pgSnp"))
    return FALSE;
if (!isPgSnp && type == NULL)
    return FALSE;
else if (!isPgSnp)
    errAbort("Error line 1 of custom track, type is pgSnp but first 3 fields are not BED");
if (!isdigit(row[4][0]) && type == NULL)
    return FALSE;
else if (!isdigit(row[4][0]))
    errAbort("Error line 1 of custom track, type is pgSnp but count is not an integer (%s)", row[4]);
int count = atoi(row[4]);
if (count < 1 && type == NULL)
    return FALSE;
else if (count < 1)
    errAbort("Error line 1 of custom track, type is pgSnp but count is less than 1");
char pattern[128]; /* include count in pattern */
safef(pattern, sizeof(pattern), "^[ACTG-]+(\\/[ACTG-]+){%d}$", count - 1);
if (! regexMatchNoCase(row[3], pattern) && type == NULL)
    return FALSE;
else if (! regexMatchNoCase(row[3], pattern))
    errAbort("Error line 1 of custom track, type is pgSnp with a count of %d but allele is invalid %s", count, row[3]);
safef(pattern, sizeof(pattern), "^[0-9]+(,[0-9]+){%d}$", count - 1);
if (! regexMatchNoCase(row[5], pattern) && type == NULL)
    return FALSE;
else if (! regexMatchNoCase(row[5], pattern))
    errAbort("Error line 1 of custom track, type is pgSnp with a count of %d but frequency is invalid (%s)", count, row[5]);
safef(pattern, sizeof(pattern), "^[0-9.]+(,[0-9.]+){%d}$", count - 1);
if (! regexMatchNoCase(row[6], pattern) && type == NULL)
    return FALSE;
else if (! regexMatchNoCase(row[6], pattern))
    errAbort("Error line 1 of custom track, type is pgSnp with a count of %d but score is invalid (%s)", count, row[6]);
/* if get here must be pgSnp format */
return TRUE;
}

static boolean pgSnpRecognizer(struct customFactory *fac,
        struct customPp *cpp, char *type,
        struct customTrack *track)
/* Return TRUE if looks like we're handling an pgSnp track */
{
if (type != NULL && !sameType(type, fac->name))
    return FALSE;
char *line = customFactoryNextRealTilTrack(cpp);
if (line == NULL)
    return FALSE;
char *dupe = cloneString(line);
char *row[7+3];
int wordCount = chopLine(dupe, row);
boolean isPgSnp = FALSE;
if (wordCount == 7)
    {
    track->fieldCount = wordCount;
    char *ctDb = ctGenomeOrCurrent(track);
    isPgSnp = rowIsPgSnp(row, ctDb, type);
    }
freeMem(dupe);
customPpReuse(cpp, line);
return (isPgSnp);
}

static struct pipeline *pgSnpLoaderPipe(struct customTrack *track)
/* Set up pipeline that will load the pgSnp into database. */
{
/* running the single command:
 *	hgLoadBed -customTrackLoader -sqlTable=loader/pgSnp.sql -renameSqlTable
 *                -trimSqlTable -notItemRgb -tmpDir=/data/tmp
 *		-maxChromNameLength=${nameLength} customTrash tableName stdin
 */
struct dyString *tmpDy = newDyString(0);
char *cmd1[] = {"loader/hgLoadBed", "-customTrackLoader",
	"-sqlTable=loader/pgSnp.sql", "-renameSqlTable", "-trimSqlTable", "-notItemRgb", NULL, NULL, NULL, NULL, NULL, NULL};
char *tmpDir = cfgOptionDefault("customTracks.tmpdir", "/data/tmp");
struct stat statBuf;
int index = 6;

if (stat(tmpDir,&statBuf))
    errAbort("can not find custom track tmp load directory: '%s'<BR>\n"
	"create directory or specify in hg.conf customTracks.tmpdir", tmpDir);
dyStringPrintf(tmpDy, "-tmpDir=%s", tmpDir);
cmd1[index++] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-maxChromNameLength=%d", track->maxChromName);
cmd1[index++] = dyStringCannibalize(&tmpDy);
cmd1[index++] = CUSTOM_TRASH;
cmd1[index++] = track->dbTableName;
cmd1[index++] = "stdin";
assert(index <= ArraySize(cmd1));

/* the "/dev/null" file isn't actually used for anything, but it is used
 * in the pipeLineOpen to properly get a pipe started that isn't simply
 * to STDOUT which is what a NULL would do here instead of this name.
 *	This function exits if it can't get the pipe created
 *	The dbStderrFile will get stderr messages from hgLoadBed into the
 *	our private error log so we can send it back to the user
 */
return pipelineOpen1(cmd1, pipelineWrite | pipelineNoAbort,
	"/dev/null", track->dbStderrFile);
}

/* customTrackPgSnp load item */
static struct pgSnp *customTrackPgSnp(char *db, char **row,
        struct hash *chromHash, struct lineFile *lf)
/* Convert a row of strings to pgSnp. */
{
struct pgSnp *item = pgSnpLineFileLoad(row, lf);
hashStoreName(chromHash, item->chrom);
customFactoryCheckChromNameDb(db, item->chrom, lf);
int chromSize = hChromSize(db, item->chrom);
if (item->chromEnd > chromSize)
    lineFileAbort(lf, "chromEnd larger than chrom %s size (%d > %d)",
        item->chrom, item->chromEnd, chromSize);
return item;
}

/*   remember to set all the custom track settings necessary */
static struct customTrack *pgSnpFinish(struct customTrack *track, struct pgSnp *itemList)
/* Finish up pgSnp tracks. */
{
struct pgSnp *item;
char buf[50];
track->tdb->type = cloneString("pgSnp");
track->dbTrackType = cloneString("pgSnp");
safef(buf, sizeof(buf), "%d", track->fieldCount);
ctAddToSettings(track, "fieldCount", cloneString(buf));
safef(buf, sizeof(buf), "%d", slCount(itemList));
ctAddToSettings(track, "itemCount", cloneString(buf));
safef(buf, sizeof(buf), "%s:%u-%u", itemList->chrom,
                itemList->chromStart, itemList->chromEnd);
ctAddToSettings(track, "firstItemPos", cloneString(buf));

/* If necessary add track offsets. */
int offset = track->offset;
if (offset != 0)
    {
    /* Add track offsets if any */
    for (item = itemList; item != NULL; item = item->next)
	{
	item->chromStart += offset;
	item->chromEnd += offset;
	}
    track->offset = 0;	/*	so DB load later won't do this again */
    hashMayRemove(track->tdb->settingsHash, "offset"); /* nor the file reader*/
    }

/* If necessary load database */
customFactorySetupDbTrack(track);
struct pipeline *dataPipe = pgSnpLoaderPipe(track);
FILE *out = pipelineFile(dataPipe);
for (item = itemList; item != NULL; item = item->next)
    pgSnpOutput(item, out, '\t', '\n');
fflush(out);		/* help see error from loader failure */
if(ferror(out) || pipelineWait(dataPipe))
    pipelineFailExit(track);	/* prints error and exits */
unlink(track->dbStderrFile);	/* no errors, not used */
pipelineFree(&dataPipe);
return track;
}

static struct customTrack *pgSnpLoader(struct customFactory *fac,
        struct hash *chromHash,
        struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up pgSnp data until get next track line. */
{
char *line;
char *db = ctGenomeOrCurrent(track);
struct pgSnp *itemList = NULL;
if (!dbRequested)
    errAbort("pgSnp custom track type unavailable without custom trash database. Please set that up in your hg.conf");
while ((line = customFactoryNextRealTilTrack(cpp)) != NULL)
    {
    char *row[7];
    int wordCount = chopLine(line, row);
    struct lineFile *lf = cpp->fileStack;
    lineFileExpectAtLeast(lf, track->fieldCount, wordCount);
    struct pgSnp *item = customTrackPgSnp(db, row, chromHash, lf);
    slAddHead(&itemList, item);
    }
slReverse(&itemList);
return pgSnpFinish(track, itemList);
}

static struct customFactory pgSnpFactory =
/* Factory for pgSnp tracks */
    {
    NULL,
    "pgSnp",
    pgSnpRecognizer,
    pgSnpLoader,
    };

/*** GFF/GTF Factory - converts to BED ***/

static boolean rowIsGff(char *db, char **row, int wordCount)
/* Return TRUE if format of this row is consistent with being a .gff */
{
boolean isGff = FALSE;
if (wordCount >= 8 && wordCount <= 9)
    {
    /* Check that strand is + - or . */
    char *strand = row[6];
    char c = strand[0];
    if (c == '.' || c == '+' || c == '-')
        {
	if (strand[1] == 0)
	    {
	    if (hgIsOfficialChromName(db, row[0]))
	        {
		if (isdigit(row[3][0]) && isdigit(row[4][0]))
		    isGff = TRUE;
		}
	    }
	}
    }
return isGff;
}

static boolean gffRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a gff track */
{
if (type != NULL && !sameType(type, fac->name))
    return FALSE;
char *line = customPpNextReal(cpp);
if (line == NULL)
    return FALSE;
char *dupe = cloneString(line);
char *row[10];
int wordCount = chopTabs(dupe, row);
boolean isGff = rowIsGff(track->genomeDb, row, wordCount);
if (isGff)
    track->gffHelper = gffFileNew("custom input");
freeMem(dupe);
customPpReuse(cpp, line);
return isGff;
}

static boolean gtfRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a gtf track.
   First run the GFF recognizer, then check for GTF group syntax */
{
boolean isGtf = FALSE;
if (type != NULL && !sameType(type, fac->name))
    return FALSE;
/* GTF is an extension of GFF, so run the GFF recognizer first.
 * This will also create a GFF file handle for the track */
if (!gffRecognizer(fac, cpp, type, track))
    return FALSE;
char *line = customPpNextReal(cpp);
if (gffHasGtfGroup(line))
    isGtf = TRUE;
customPpReuse(cpp, line);
return isGtf;
}

static double gffGroupAverageScore(struct gffGroup *group, double defaultScore)
/* Return average score of GFF group, or average if none. */
{
double cumScore = 0;
int count = 0;
struct gffLine *line;

/* First see if any non-zero */
for (line = group->lineList; line != NULL; line = line->next)
    {
    if (line->score != 0.0)
        ++count;
    }
if (count <= 0)
    return defaultScore;

/* Calculate and return average score. */
for (line = group->lineList; line != NULL; line = line->next)
    cumScore += line->score;
return cumScore / count;
}

static char *niceGeneName(char *name)
/* Return a nice version of name. */
{
static char buf[128];
char *e;

strncpy(buf, name, sizeof(buf));
if ((e = strchr(buf, ';')) != NULL)
    *e = 0;
eraseWhiteSpace(buf);
stripChar(buf, '%');
stripChar(buf, '\'');
stripChar(buf, '"');
return buf;
}

static struct bed *gffHelperFinish(struct gffFile *gff, struct hash *chromHash)
/* Create bedList from gffHelper. */
{
struct genePred *gp;
struct bed *bedList = NULL, *bed;
struct gffGroup *group;
int i, blockCount, chromStart, exonStart;

gffGroupLines(gff);

for (group = gff->groupList; group != NULL; group = group->next)
    {
    /* First convert to gene-predictions since this is almost what we want. */
    if (gff->isGtf)
        gp = genePredFromGroupedGtf(gff, group, niceGeneName(group->name),
                                    genePredNoOptFld, genePredGxfDefaults);
    else
        gp = genePredFromGroupedGff(gff, group, niceGeneName(group->name),
                                NULL, genePredNoOptFld, genePredGxfDefaults);
    if (gp != NULL)
        {
	/* Make a bed out of the gp. */
	AllocVar(bed);
	bed->chrom = hashStoreName(chromHash, gp->chrom);
	bed->chromStart = chromStart = gp->txStart;
	bed->chromEnd = gp->txEnd;
	bed->name = cloneString(gp->name);
	bed->score = gffGroupAverageScore(group, 1000);
	bed->strand[0] = gp->strand[0];
	bed->thickStart = gp->cdsStart;
	bed->thickEnd = gp->cdsEnd;
	bed->blockCount = blockCount = gp->exonCount;
	AllocArray(bed->blockSizes, blockCount);
	AllocArray(bed->chromStarts, blockCount);
	for (i=0; i<blockCount; ++i)
	    {
	    exonStart = gp->exonStarts[i];
	    bed->chromStarts[i] = exonStart - chromStart;
	    bed->blockSizes[i] = gp->exonEnds[i] - exonStart;
	    }
	genePredFree(&gp);
	slAddHead(&bedList, bed);
	}
    }
return bedList;
}

static struct customTrack *gffLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up gff data until get next track line. */
{
char *line;
char *ctDb = ctGenomeOrCurrent(track);
while ((line = customFactoryNextRealTilTrack(cpp)) != NULL)
    {
    char *row[9];
    int wordCount = chopTabs(line, row);
    struct lineFile *lf = cpp->fileStack;
    customFactoryCheckChromNameDb(ctDb, row[0], lf);
    gffFileAddRow(track->gffHelper, 0, row, wordCount, lf->fileName,
    	lf->lineIx);
    }
track->bedList = gffHelperFinish(track->gffHelper, chromHash);
gffFileFree(&track->gffHelper);
track->fieldCount = 12;
return bedFinish(track, dbRequested);
}

static struct customFactory gffFactory =
/* Factory for gff tracks */
    {
    NULL,
    "gff",
    gffRecognizer,
    gffLoader,
    };

static struct customFactory gtfFactory =
/* Factory for gtf tracks. Shares loader with gffFactory */
    {
    NULL,
    "gtf",
    gtfRecognizer,
    gffLoader,
    };

/*** PSL Factory - converts to BED ***/

static boolean checkStartEnd(char *sSize, char *sStart, char *sEnd)
/* Make sure start < end <= size */
{
int start, end, size;
start = atoi(sStart);
end = atoi(sEnd);
size = atoi(sSize);
return start < end && end <= size;
}

static boolean rowIsPsl(char **row, int wordCount)
/* Return TRUE if format of this row is consistent with being a .psl */
{
int i, len;
char *s, c;
int blockCount;
if (wordCount != PSL_NUM_COLS)
    return FALSE;
for (i=0; i<=7; ++i)
   if (!isdigit(row[i][0]))
       return FALSE;
s = row[8];
len = strlen(s);
if (len < 1 || len > 2)
    return FALSE;
for (i=0; i<len; ++i)
    {
    c = s[i];
    if (c != '+' && c != '-')
        return FALSE;
    }
if (!checkStartEnd(row[10], row[11], row[12]))
    return FALSE;
if (!checkStartEnd(row[14], row[15], row[16]))
    return FALSE;
if (!isdigit(row[17][0]))
   return FALSE;
blockCount = atoi(row[17]);
for (i=18; i<=20; ++i)
    if (countChars(row[i], ',') != blockCount)
        return FALSE;
return TRUE;
}

static boolean pslRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a bed track */
{
if (type != NULL && !sameType(type, fac->name))
    return FALSE;
char *line = customPpNextReal(cpp);
if (line == NULL)
    return FALSE;
boolean isPsl = FALSE;
if (startsWith("psLayout version", line))
    {
    isPsl = TRUE;
    }
else
    {
    char *dupe = cloneString(line);
    char *row[PSL_NUM_COLS+1];
    int wordCount = chopLine(dupe, row);
    isPsl = rowIsPsl(row, wordCount);
    freeMem(dupe);
    }
customPpReuse(cpp, line);
return isPsl;
}

static struct bed *customTrackPsl(char *db, boolean isProt, char **row,
	int wordCount, struct hash *chromHash, struct lineFile *lf)
/* Convert a psl format row of strings to a bed. */
{
struct psl *psl = pslLoad(row);
struct bed *bed;
int i, blockCount, *chromStarts, chromStart, *blockSizes;

/* A tiny bit of error checking on the psl. */
if (psl->qStart >= psl->qEnd || psl->qEnd > psl->qSize
    || psl->tStart >= psl->tEnd || psl->tEnd > psl->tSize)
    {
    lineFileAbort(lf, "mangled psl format");
    }
customFactoryCheckChromNameDb(db, psl->tName, lf);

if (!isProt)
    {
    isProt = pslIsProtein(psl);
    }

/* Allocate bed and fill in from psl. */
AllocVar(bed);
bed->chrom = hashStoreName(chromHash, psl->tName);

bed->score = 1000 - 2*pslCalcMilliBad(psl, TRUE);
if (bed->score < 0) bed->score = 0;
bed->strand[0] = psl->strand[0];
bed->strand[1] = 0;
bed->blockCount = blockCount = psl->blockCount;
bed->blockSizes = blockSizes = (int *)psl->blockSizes;
psl->blockSizes = NULL;
bed->chromStarts = chromStarts = (int *)psl->tStarts;
psl->tStarts = NULL;
bed->name = psl->qName;
psl->qName = NULL;

if (isProt)
    {
    for (i=0; i<blockCount; ++i)
        {
	blockSizes[i] *= 3;
	}
    /* Do a little trimming here.  Arguably blat should do it
     * instead. */
    for (i=1; i<blockCount; ++i)
	{
	int lastEnd = blockSizes[i-1] + chromStarts[i-1];
	if (chromStarts[i] < lastEnd)
	    chromStarts[i] = lastEnd;
	}
    }


/* Switch minus target strand to plus strand. */
if (psl->strand[1] == '-')
    {
    int chromSize = psl->tSize;
    reverseInts(bed->blockSizes, blockCount);
    reverseInts(chromStarts, blockCount);
    for (i=0; i<blockCount; ++i)
	{
	chromStarts[i] = chromSize - chromStarts[i] - blockSizes[i];
	}
    if (bed->strand[0] == '-')
        bed->strand[0] = '+';
    else
        bed->strand[0] = '-';
    }

bed->thickStart = bed->chromStart = chromStart = chromStarts[0];
bed->thickEnd = bed->chromEnd = chromStarts[blockCount-1] + blockSizes[blockCount-1];

/* Convert coordinates to relative. */
for (i=0; i<blockCount; ++i)
    chromStarts[i] -= chromStart;
pslFree(&psl);
return bed;
}

static struct customTrack *pslLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up psl data until get next track line. */
{
char *line;
boolean pslIsProt = FALSE;
char *ctDb = ctGenomeOrCurrent(track);
while ((line = customFactoryNextRealTilTrack(cpp)) != NULL)
    {
    /* Skip over pslLayout version lines noting if they are
     * protein though */
    if (startsWith("psLayout version", line))
        {
	pslIsProt = (stringIn("protein", line) != NULL);
	int i;
	for (i=0; i<3; ++i)
	    customFactoryNextRealTilTrack(cpp);
	continue;
	}
    char *row[PSL_NUM_COLS];
    int wordCount = chopLine(line, row);
    struct lineFile *lf = cpp->fileStack;
    lineFileExpectAtLeast(lf, PSL_NUM_COLS, wordCount);
    struct bed *bed = customTrackPsl(ctDb, pslIsProt, row,
    	wordCount, chromHash, lf);
    slAddHead(&track->bedList, bed);
    }
slReverse(&track->bedList);
track->fieldCount = 12;
return bedFinish(track, dbRequested);
}

static struct customFactory pslFactory =
/* Factory for psl tracks */
    {
    NULL,
    "psl",
    pslRecognizer,
    pslLoader,
    };

static boolean mafRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a maf track */
{
if (type != NULL && !sameType(type, fac->name))
    return FALSE;
char *line = customPpNext(cpp);
if (line == NULL)
    return FALSE;

boolean isMaf = FALSE;
if (startsWith("##maf version", line))
    {
    isMaf = TRUE;
    }

customPpReuse(cpp, line);

return isMaf;
}

static void mafLoaderBuildTab(struct customTrack *track, char *mafFile)
/* build maf tab file and load in database */
{
customFactorySetupDbTrack(track);

struct dyString *tmpDy = newDyString(0);
char *cmd1[] = {"loader/hgLoadMaf", "-verbose=0", "-custom",  NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL};
char **cmds[] = {cmd1, NULL};
char *tmpDir = cfgOptionDefault("customTracks.tmpdir", "/data/tmp");
struct stat statBuf;
struct tempName tn;

trashDirFile(&tn, "ct", "ct", ".pos");

if (stat(tmpDir,&statBuf))
    errAbort("can not find custom track tmp load directory: '%s'<BR>\n"
	"create directory or specify in hg.conf customTracks.tmpdir", tmpDir);
dyStringPrintf(tmpDy, "-tmpDir=%s", tmpDir);
cmd1[3] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-loadFile=%s", mafFile);
cmd1[4] = dyStringCannibalize(&tmpDy);  tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-refDb=%s", track->genomeDb);
cmd1[5] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-maxNameLen=%d", track->maxChromName);
cmd1[6] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-defPos=%s", tn.forCgi);
cmd1[7] = dyStringCannibalize(&tmpDy);
cmd1[8] = CUSTOM_TRASH;
cmd1[9] = track->dbTableName;

struct pipeline *dataPipe =  pipelineOpen(cmds,
    pipelineWrite | pipelineNoAbort, "/dev/null", track->dbStderrFile);
if(pipelineWait(dataPipe))
    pipelineFailExit(track);	/* prints error and exits */

pipelineFree(&dataPipe);
unlink(track->dbStderrFile);	/* no errors, not used */
track->wigFile = NULL;

struct lineFile *lf = lineFileOpen(tn.forCgi, TRUE);
char *line;
int size;

lineFileNeedNext(lf, &line, &size);
lineFileClose(&lf);
unlink(tn.forCgi);
ctAddToSettings(track, "firstItemPos", cloneString(line));
}

static struct customTrack *mafLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
{
FILE *f;
char *line;
struct tempName tn;
struct hash *settings = track->tdb->settingsHash;

if (!dbRequested)
    errAbort("Maf files have to be in database");

track->dbTrackType = cloneString(fac->name);
track->wiggle = TRUE;

/* If mafFile setting already exists, then we are reloading, not loading.
 * Just make sure files still exist. */
if (hashFindVal(settings, "mafFile"))
    {
    track->wibFile = hashFindVal(settings, "mafFile");
    if (!fileExists(track->wibFile))
        return FALSE;
    }
/* MafFile setting doesn't exist, so we are loading from stream. */
else
    {
    char *maxByteStr = cfgOption("customTracks.maxBytes");

    /* Make up wib file name and add to settings. */
    trashDirFile(&tn, "ct", "ct", ".maf");
    track->wibFile = cloneString(tn.forCgi);
    ctAddToSettings(track, "mafFile", track->wibFile);

    char *mafFile = cloneString(track->wibFile);

    /* Actually create maf file. */
    f = mustOpen(mafFile, "w");
    if (maxByteStr != NULL)
	{
	long maxBytes = sqlUnsignedLong(maxByteStr);
	long numBytesLeft = maxBytes;

	while ((line = customFactoryNextTilTrack(cpp)) != NULL)
	    {
	    numBytesLeft -= strlen(line);

	    if (numBytesLeft < 0)
		{
		unlink(mafFile);
		errAbort("exceeded upload limit of %ld bytes\n", maxBytes);
		}

	    fprintf(f, "%s\n", line);
	    }
	}
    else
	while ((line = customFactoryNextTilTrack(cpp)) != NULL)
	    fprintf(f, "%s\n", line);

    carefulClose(&f);

    mafLoaderBuildTab(track, mafFile);
    }
char tdbType[256];
safef(tdbType, sizeof(tdbType), "maf");
track->tdb->type = cloneString(tdbType);

return track;
}

static struct customFactory mafFactory =
/* Factory for maf tracks */
    {
    NULL,
    "maf",
    mafRecognizer,
    mafLoader,
    };

/*** Wig Factory - for wiggle tracks ***/

static boolean wigRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a wig track */
{
return (sameOk(type, fac->name) || sameType(type, "wig"));
}

static struct pipeline *wigLoaderPipe(struct customTrack *track)
/* Set up pipeline that will load wig into database. */
{
/*	Run the two commands in a pipeline:
 *	loader/wigEncode -verbose=0 -wibSizeLimit=300000000 stdin stdout \
 *	    ${wibFile} | \
 *	loader/hgLoadWiggle -verbose=0 -noHistory -tmpDir=/data/tmp \
 *	    -maxChromNameLength=${nameLength} -chromInfoDb=${database} \
 *	    -pathPrefix=[.|/] ${db} ${table} stdin
 */
struct dyString *tmpDy = newDyString(0);
char *cmd1[] = {"loader/wigEncode", "-verbose=0", "-wibSizeLimit=300000000",
	"stdin", "stdout", NULL, NULL};
char *cmd2[] = {"loader/hgLoadWiggle", "-verbose=0", "-noHistory", NULL, NULL,
	NULL, NULL, NULL, NULL, "stdin", NULL};
char **cmds[] = {cmd1, cmd2, NULL};
char *tmpDir = cfgOptionDefault("customTracks.tmpdir", "/data/tmp");
struct stat statBuf;

cmd1[5] = track->wibFile;

if (stat(tmpDir,&statBuf))
    errAbort("can not find custom track tmp load directory: '%s'<BR>\n"
	"create directory or specify in hg.conf customTracks.tmpdir", tmpDir);
dyStringPrintf(tmpDy, "-tmpDir=%s", tmpDir);
cmd2[3] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-maxChromNameLength=%d", track->maxChromName);
cmd2[4] = dyStringCannibalize(&tmpDy); tmpDy = newDyString(0);
dyStringPrintf(tmpDy, "-chromInfoDb=%s", track->genomeDb);
cmd2[5] = dyStringCannibalize(&tmpDy);
/* a system could be using /trash/ absolute reference, and nothing to do with
 *	local references, so don't confuse it with ./ a double // will work
 */
if (startsWith("/", trashDir()))
    cmd2[6] = "-pathPrefix=/";
else
    cmd2[6] = "-pathPrefix=.";
cmd2[7] = CUSTOM_TRASH;
cmd2[8] = track->dbTableName;
/* the "/dev/null" file isn't actually used for anything, but it is used
 * in the pipeLineOpen to properly get a pipe started that isn't simply
 * to STDOUT which is what a NULL would do here instead of this name.
 *	This function exits if it can't get the pipe created
 *	The dbStderrFile will get stderr messages from this pipeline into the
 *	our private error file so we can return the errors to the user.
 */
return pipelineOpen(cmds, pipelineWrite | pipelineNoAbort,
	"/dev/null", track->dbStderrFile);
}

static void wigDbGetLimits(struct sqlConnection *conn, char *tableName,
	double *retUpperLimit, double *retLowerLimit, int *retSpan)
/* Figure out upper/lower limits of wiggle table. */
{
char query[512];
sqlSafef(query,sizeof(query),
 "select min(lowerLimit),max(lowerLimit+dataRange) from %s",
    tableName);
struct sqlResult *sr = sqlGetResult(conn, query);
char **row;
if ((row = sqlNextRow(sr)) != NULL);
    {
    if (row[0]) *retLowerLimit = sqlDouble(row[0]);
    if (row[1]) *retUpperLimit = sqlDouble(row[1]);
    }
sqlFreeResult(&sr);
sqlSafef(query,sizeof(query),
     "select span from %s group by span", tableName);
int span = sqlQuickNum(conn, query);
if (span == 0)
    span = 1;
*retSpan = span;
}

#ifdef PROGRESS_METER
static void progressWrite(char *label, FILE *progressFH, off_t seekTo,
    long msStart, unsigned long long bytesWritten,
    unsigned long long expectedBytes)
/* write to given file the current progress situation */
{
if (seekTo)
    fseeko(progressFH, seekTo, SEEK_SET);
long msNow = clock1000();
int percentDone = 0;
if (expectedBytes > 0)
    percentDone = (100 * bytesWritten) / expectedBytes;
fprintf(progressFH, "%s:\t%llu\t%llu\t%ld\t%% %d\n", label, bytesWritten,
    (unsigned long long)expectedBytes, msNow - msStart, percentDone);
fflush(progressFH);
}
#endif

/*  HACK ALERT - The table browser needs to be able to encode its wiggle
 *	data.  This function is temporarily global until a proper method
 *	is used to work this business into the table browser custom
 *	tracks.  Currently this is also called from customSaveTracks()
 *	in customTrack.c in violation of this object's hidden methods.
 */
void wigLoaderEncoding(struct customTrack *track, char *wigAscii,
	boolean dbRequested)
/* encode wigAscii file into .wig and .wib files */
{
/* Need to figure upper and lower limits. */
double lowerLimit = 0.0;
double upperLimit = 100.0;
int span = 1;

/* Load database if requested */
if (dbRequested)
    {
    /* TODO: see if can avoid extra file copy in this case. */
    customFactorySetupDbTrack(track);

    /* Load ascii file into database via pipeline. */
    struct pipeline *dataPipe = wigLoaderPipe(track);
    FILE *in = mustOpen(wigAscii, "r");
    FILE *out = pipelineFile(dataPipe);
    int c;  /* fgetc and fputc work with int. char type cannot represent EOF properly. */
    int fputcErr = 0;

#ifdef PROGRESS_METER
/* don't forget: http://www.redips.net/javascript/ajax-progress-bar/ */
    struct stat localStat;
    ZeroVar(&localStat);
    off_t wigAsciiSize = 0;
    if (stat(wigAscii,&localStat)==0)
	wigAsciiSize = localStat.st_size;
    FILE *progress = 0;
    long msStart = clock1000();
    unsigned long long bytesWritten = 0;
    off_t progressSeek = 0;
    if (track->progressFile)
	{
	ZeroVar(&localStat);
	if (stat(track->progressFile,&localStat)==0)
	    progressSeek = localStat.st_size;
	progress = mustOpen(track->progressFile, "r+");
	fseeko(progress, progressSeek, SEEK_SET);
	}
#endif
    unlink(wigAscii);/* stays open, disappears when close or pipe fail */
    while ((c = fgetc(in)) != EOF && fputcErr != EOF)
	{
	fputcErr = fputc(c, out);
#ifdef PROGRESS_METER
	++bytesWritten;
	if (0 == (bytesWritten % 100000))
	    {
	    progressWrite("encoding", progress, progressSeek, msStart, bytesWritten, wigAsciiSize);
	    }
#endif
	}
    carefulClose(&in);
#ifdef PROGRESS_METER
    progressWrite("encoding_done", progress, (off_t)0, msStart, bytesWritten, wigAsciiSize);
    carefulClose(&progress);
#endif
    fflush(out);		/* help see error from loader failure */
#if 0  // enable this for help debugging
    fprintf(stderr, "%s\n", pipelineDesc(dataPipe));
#endif
    if(ferror(out) || pipelineWait(dataPipe))
	pipelineFailExit(track);	/* prints error and exits */
    unlink(track->dbStderrFile);	/* no errors, not used */
    pipelineFree(&dataPipe);
    track->wigFile = NULL;

    /* Figure out lower and upper limits with db query */
    struct sqlConnection *ctConn = hAllocConn(CUSTOM_TRASH);
    char buf[64];
    wigDbGetLimits(ctConn, track->dbTableName,
	    &upperLimit, &lowerLimit, &span);
    hFreeConn(&ctConn);
    safef(buf, sizeof(buf), "%d", span);
    ctAddToSettings(track, "spanList", cloneString(buf));
    }
else
    {
    /* Make up wig file name (by replacing suffix of wib file name)
     * and add to settings. */
    track->wigFile = cloneString(track->wibFile);
    chopSuffix(track->wigFile);
    strcat(track->wigFile, ".wig");
    ctAddToSettings(track, "wigFile", track->wigFile);

    struct wigEncodeOptions options;
    ZeroVar(&options);	/*	all is zero	*/
    options.lift = 0;
    options.noOverlap = FALSE;
    options.flagOverlapSpanData = TRUE;
    options.wibSizeLimit = 300000000; /* 300Mb limit*/
    wigAsciiToBinary(wigAscii, track->wigFile,
	track->wibFile, &upperLimit, &lowerLimit, &options);
    if (options.wibSizeLimit >= 300000000)
	warn("warning: reached data limit for wiggle track '%s' "
	     "%lld >= 300,000,000<BR>\n",
	     track->tdb->shortLabel, options.wibSizeLimit);
    unlink(wigAscii);/* done with this, remove it */
    }
freeMem(track->wigAscii);
char tdbType[256];
safef(tdbType, sizeof(tdbType), "wig %g %g", lowerLimit, upperLimit);
track->tdb->type = cloneString(tdbType);
}

static struct customTrack *wigLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up wiggle data until get next track line. */
{
FILE *f;
char *line;
struct tempName tn;
struct hash *settings = track->tdb->settingsHash;

track->dbTrackType = cloneString(fac->name);
track->wiggle = TRUE;
#ifdef PROGRESS_METER
track->progressFile = 0;
#endif

/* If wibFile setting already exists, then we are reloading, not loading.
 * Just make sure files still exist. */
if (hashFindVal(settings, "wibFile"))
    {
    track->wibFile = hashFindVal(settings, "wibFile");
    if (!fileExists(track->wibFile))
        return FALSE;

    /* In database case wigFile is in database table, and that
     * table's existence is checked by framework, so we need not
     * bother. */
    track->wigFile = hashFindVal(settings, "wigFile");
    if (track->wigFile != NULL)
	{
        if (!fileExists(track->wigFile))
	    {
	    return FALSE;
	    }
	}
    }
/* WibFile setting doesn't exist, so we are loading from ascii stream. */
else
    {

    /* Make up wib file name and add to settings. */
    trashDirFile(&tn, "ct", "ct", ".wib");
    track->wibFile = cloneString(tn.forCgi);
    ctAddToSettings(track, "wibFile", track->wibFile);

    /* Don't add wigAscii to settings - not needed. */
    char *wigAscii = cloneString(track->wibFile);
    chopSuffix(wigAscii);
    strcat(wigAscii, ".wia");

#ifdef PROGRESS_METER
    /* Don't add progressFile to settings - not needed. */
    trashDirFile(&tn, "progress", "wig", ".txt");
    track->progressFile = cloneString(tn.forCgi);
fprintf(stderr, "DBG: setting progressFile: '%s'\n", track->progressFile);
FILE *progress = mustOpen(track->progressFile, "w");
fprintf(progress, "progressFile: %s\n", track->progressFile);
fprintf(progress, "shortLabel: %s\n", track->tdb->shortLabel);
fprintf(progress, "forHtml: %s\n", tn.forHtml);
fprintf(progress, "remote size: %llu\n", (unsigned long long)cpp->remoteFileSize);
    fflush(progress);
    off_t remoteSize = cpp->remoteFileSize;
//    off_t progressSeek = ftello(progress);
    unsigned long long bytesWritten = 0;
    unsigned long long linesRead = 0;
    unsigned long long nextInterval = bytesWritten + 100000;
    long msStart = clock1000();
#endif

    /* Actually create wigAscii file. */
    f = mustOpen(wigAscii, "w");
    while ((line = customFactoryNextRealTilTrack(cpp)) != NULL)
	{
#ifdef PROGRESS_METER
	linesRead++;
	bytesWritten += strlen(line);
	if (bytesWritten > nextInterval)
	    {
	    progressWrite("incoming", progress, (off_t)0, msStart, bytesWritten, remoteSize);
    	    nextInterval = bytesWritten + 100000;
	    }
#endif
	fprintf(f, "%s\n", line);
	}
    carefulClose(&f);
#ifdef PROGRESS_METER
    progressWrite("incoming_done", progress, (off_t)0, msStart, bytesWritten, remoteSize);
fprintf(progress, "lines_read: %llu\n", linesRead);
fflush(progress);
    carefulClose(&progress);
#endif

    wigLoaderEncoding(track, wigAscii, dbRequested);
    }
return track;
}

static struct customFactory wigFactory =
/* Factory for wiggle tracks */
    {
    NULL,
    "wiggle_0",
    wigRecognizer,
    wigLoader,
    };

/*** Big Wig Factory - for big client-side wiggle tracks ***/

static boolean bigWigRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a wig track */
{
return (sameType(type, "bigWig"));
}

void setBbiViewLimits(struct customTrack *track)
/* If there are no viewLimits sets, set them from bbiFile info. */
{
struct hash *settings = track->tdb->settingsHash;
if (hashLookup(settings, "viewLimits") == NULL)
    {
    struct bbiSummaryElement sum = bbiTotalSummary(track->bbiFile);
    if (sum.minVal == sum.maxVal)
	{
	sum.minVal += 1;
	sum.maxVal -= 1;
	}
    char text[1024];
    safef(text, sizeof(text), "%f:%f", sum.minVal, sum.maxVal);
    hashAdd(settings, "viewLimits", cloneString(text));
    }
}

static void checkAllowedBigDataUrlProtocols(char *url)
/* Abort if url is not using one of the allowed bigDataUrl network protocols.
 * In particular, do not allow a local file reference. */
{
if (!(startsWith("http://", url)
   || startsWith("https://", url)
   || startsWith("ftp://", url)
))
    errAbort("only network protocols http, https, or ftp allowed in bigDataUrl: '%s'", url);
}


static struct customTrack *bigWigLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up wiggle data until get next track line. */
{
/* Not much to this.  A bigWig has nothing here but a track line. */
struct hash *settings = track->tdb->settingsHash;
char *bigDataUrl = hashFindVal(settings, "bigDataUrl");
if (bigDataUrl == NULL)
    errAbort("Missing bigDataUrl setting from track of type=bigWig.  Please check for case and spelling and that there is no new-line between the 'track' and the 'bigDataUrl' if you think the bigDataUrl is there.");
checkAllowedBigDataUrlProtocols(bigDataUrl);

/* protect against temporary network error */
struct errCatch *errCatch = errCatchNew();
if (errCatchStart(errCatch))
    {
    track->bbiFile = bigWigFileOpen(bigDataUrl);
    setBbiViewLimits(track);
    }
errCatchEnd(errCatch);
if (errCatch->gotError)
    {
    track->networkErrMsg = cloneString(errCatch->message->string);
    }
errCatchFree(&errCatch);

return track;
}

static struct customFactory bigWigFactory =
/* Factory for wiggle tracks */
    {
    NULL,
    "bigWig",
    bigWigRecognizer,
    bigWigLoader,
    };

/*** Big Bed Factory - for big client-side BED tracks ***/

static boolean bigBedRecognizer(struct customFactory *fac,
	struct customPp *cpp, char *type,
    	struct customTrack *track)
/* Return TRUE if looks like we're handling a wig track */
{
return (sameType(type, "bigBed"));
}

static struct customTrack *bigBedLoader(struct customFactory *fac,
	struct hash *chromHash,
    	struct customPp *cpp, struct customTrack *track, boolean dbRequested)
/* Load up big bed data until get next track line. */
{
/* Not much to this.  A bigBed has nothing here but a track line. */
struct hash *settings = track->tdb->settingsHash;
char *bigDataUrl = hashFindVal(settings, "bigDataUrl");
if (bigDataUrl == NULL)
    errAbort("Missing bigDataUrl setting from track of type=bigBed");
checkAllowedBigDataUrlProtocols(bigDataUrl);

/* protect against temporary network error */
struct errCatch *errCatch = errCatchNew();
if (errCatchStart(errCatch))
    {
    track->bbiFile = bigBedFileOpen(bigDataUrl);
    }
errCatchEnd(errCatch);
if (errCatch->gotError)
    {
    track->networkErrMsg = cloneString(errCatch->message->string);
    return track;
    }
errCatchFree(&errCatch);

setBbiViewLimits(track);
return track;
}

static struct customFactory bigBedFactory =
/* Factory for bigBed tracks */
    {
    NULL,
    "bigBed",
    bigBedRecognizer,
    bigBedLoader,
    };


#ifdef USE_BAM
/*** BAM Factory - for client-side BAM alignment files ***/

static boolean bamRecognizer(struct customFactory *fac,	struct customPp *cpp, char *type,
			     struct customTrack *track)
/* Return TRUE if looks like we're handling a bam track */
{
return (sameType(type, "bam"));
}

static struct customTrack *bamLoader(struct customFactory *fac, struct hash *chromHash,
				     struct customPp *cpp, struct customTrack *track,
				     boolean dbRequested)
/* Process the bam track line. */
{
struct hash *settings = track->tdb->settingsHash;
char *bigDataUrl = hashFindVal(settings, "bigDataUrl");
struct dyString *dyErr = dyStringNew(0);
if (bigDataUrl == NULL)
    errAbort("Missing bigDataUrl setting from track of type=bam (%s)", track->tdb->shortLabel);
checkAllowedBigDataUrlProtocols(bigDataUrl);
if (doExtraChecking)
    {
    /* protect against temporary network error */
    struct errCatch *errCatch = errCatchNew();
    if (errCatchStart(errCatch))
	{
        if (!bamFileExists(bigDataUrl))
	    {
            dyStringPrintf(dyErr,
		       "Can't access %s's bigDataUrl %s and/or the associated index file %s.bai",
			   track->tdb->shortLabel, bigDataUrl, bigDataUrl);
	    }
	}
    errCatchEnd(errCatch);
    if (isNotEmpty(errCatch->message->string))
	dyStringPrintf(dyErr, ": %s", errCatch->message->string);
    errCatchFree(&errCatch);

    }
if (isNotEmpty(dyErr->string))
    track->networkErrMsg = dyStringCannibalize(&dyErr);
track->dbTrackType = cloneString("bam");
return track;
}

static struct customFactory bamFactory =
/* Factory for bam tracks */
    {
    NULL,
    "bam",
    bamRecognizer,
    bamLoader,
    };
#endif//def USE_BAM

#ifdef USE_TABIX
/*** VCF+tabix Factory - client-side Variant Call Format files compressed & indexed by tabix ***/

static boolean vcfTabixRecognizer(struct customFactory *fac, struct customPp *cpp, char *type,
				  struct customTrack *track)
/* Return TRUE if looks like we're handling a vcfTabix track */
{
return (sameType(type, "vcfTabix"));
}

static struct customTrack *vcfTabixLoader(struct customFactory *fac, struct hash *chromHash,
					  struct customPp *cpp, struct customTrack *track,
					  boolean dbRequested)
/* Process the vcfTabix track line. */
{
struct hash *settings = track->tdb->settingsHash;
char *bigDataUrl = hashFindVal(settings, "bigDataUrl");
struct dyString *dyErr = dyStringNew(0);
if (bigDataUrl == NULL)
    errAbort("Missing bigDataUrl setting from track of type=vcfTabix (%s)",
	     track->tdb->shortLabel);
checkAllowedBigDataUrlProtocols(bigDataUrl);
if (doExtraChecking)
    {
    /* protect against temporary network error */
    int vcfMaxErr = 100;
    struct errCatch *errCatch = errCatchNew();
    if (errCatchStart(errCatch))
	{
	struct vcfFile *vcff = vcfTabixFileMayOpen(bigDataUrl, NULL, 0, 0, vcfMaxErr, -1);
	if (vcff == NULL)
	    {
            dyStringPrintf(dyErr, "Unable to load and/or parse %s's bigDataUrl %s",
			   track->tdb->shortLabel, bigDataUrl);
	    }
	vcfFileFree(&vcff);
	}
    errCatchEnd(errCatch);
    if (isNotEmpty(errCatch->message->string))
	dyStringPrintf(dyErr, ": %s", errCatch->message->string);
    errCatchFree(&errCatch);
    }
if (isNotEmpty(dyErr->string))
    track->networkErrMsg = dyStringCannibalize(&dyErr);
track->dbTrackType = cloneString("vcfTabix");
return track;
}

static struct customFactory vcfTabixFactory =
/* Factory for vcfTabix tracks */
    {
    NULL,
    "vcfTabix",
    vcfTabixRecognizer,
    vcfTabixLoader,
    };
#endif//def USE_TABIX

/*** makeItems Factory - for track where user interactively creates items. ***/

static boolean makeItemsRecognizer(struct customFactory *fac,	struct customPp *cpp, char *type,
			     struct customTrack *track)
/* Return TRUE if looks like we're handling a makeItems track */
{
return (sameType(type, "makeItems"));
}

struct makeItemsItem *makeItemsItemFromRow(char **row, int rowSize)
/* Create a makeItemsItem from a row of uncertain length. */
{
if (rowSize < 3)
    errAbort("err: need at least %d fields in a makeItems row, got %d", 3, rowSize);
struct makeItemsItem *item;
AllocVar(item);
item->chrom = cloneString(row[0]);
item->chromStart = sqlUnsigned(row[1]);
item->chromEnd = sqlUnsigned(row[2]);
item->bin = binFromRange(item->chromStart, item->chromEnd);
if (rowSize > 3)
    item->name = cloneString(row[3]);
else
    item->name = cloneString("");
if (rowSize > 4)
    item->score = sqlSigned(row[4]);
if (rowSize > 5)
    item->strand[0] = row[5][0];
else
    item->strand[0] = '.';
if (rowSize > 6)
    item->thickStart = sqlUnsigned(row[6]);
else
    item->thickStart = item->chromStart;
if (rowSize > 7)
    item->thickEnd = sqlUnsigned(row[7]);
else
    item->thickEnd = item->chromEnd;
if (rowSize > 8)
    item->itemRgb = bedParseRgb(row[8]);
if (rowSize > 9)
    item->description = cloneString(row[9]);
else
    item->description = cloneString("");
return item;
}

static struct customTrack *makeItemsLoader(struct customFactory *fac, struct hash *chromHash,
				     struct customPp *cpp, struct customTrack *track,
				     boolean dbRequested)
/* Process the makeItems track line. */
{
char *ctDb = ctGenomeOrCurrent(track);
struct makeItemsItem *list = NULL;
int fieldCount = 0;
char *line;
while ((line = customFactoryNextRealTilTrack(cpp)) != NULL)
    {
    char *row[8];
    int wordCount = chopLine(line, row);
    struct lineFile *lf = cpp->fileStack;
    lineFileExpectAtLeast(lf, 3, wordCount);
    if (fieldCount == 0)
        fieldCount = wordCount;
    else if (fieldCount != wordCount)
        {
	errAbort("error: some lines in makeItems type custom track have %d fields, others have %d",
		fieldCount, wordCount);
	}
    struct makeItemsItem *item = makeItemsItemFromRow(row, wordCount);
    customFactoryCheckChromNameDb(ctDb, item->chrom, lf);
    slAddHead(&list, item);
    }
track->dbTrackType = cloneString("makeItems");
track->tdb->type = cloneString("makeItems");
if (fieldCount != 0)
    {
    char buf[16];
    safef(buf, sizeof(buf), "%d", fieldCount);
    ctAddToSettings(track, "fieldCount", cloneString(buf));
    }

/* If necessary add track offsets. */
int offset = track->offset;
if (offset != 0)
    {
    /* Add track offsets if any */
    struct makeItemsItem *item;
    for (item = list; item != NULL; item = item->next)
	{
	item->chromStart += offset;
	item->chromEnd += offset;
	}
    track->offset = 0;	/*	so DB load later won't do this again */
    hashMayRemove(track->tdb->settingsHash, "offset"); /* nor the file reader*/
    }

/* Load database */
customFactorySetupDbTrack(track);
char *tableName = track->dbTableName;
char *tableFormat =
"CREATE TABLE %s (\n"
"    bin int unsigned not null,	# Bin for range index\n"
"    chrom varchar(255) not null,	# Reference sequence chromosome or scaffold\n"
"    chromStart int unsigned not null,	# Start position in chromosome\n"
"    chromEnd int unsigned not null,	# End position in chromosome\n"
"    name varchar(255) not null,	# Name of item - up to 16 chars\n"
"    score int unsigned not null,	# 0-1000.  Higher numbers are darker.\n"
"    strand char(1) not null,	# + or - for strand\n"
"    thickStart int unsigned not null,	# Start of thick part\n"
"    thickEnd int unsigned not null,	# End position of thick part\n"
"    itemRgb int unsigned not null,	# RGB 8 bits each as in bed\n"
"    description longblob not null,	# Longer item description\n"
"    id int auto_increment,\n"
"              #Indices\n"
"    PRIMARY KEY(id),\n"
"    INDEX(chrom(16),bin)\n"
")";
struct dyString *createSql = dyStringNew(0);
sqlDyStringPrintf(createSql, tableFormat, tableName);
struct sqlConnection *conn = hAllocConn(CUSTOM_TRASH);
if (sqlMaybeMakeTable(conn, tableName, createSql->string))
    {
    struct makeItemsItem *item;
    for (item = list; item != NULL; item = item->next)
	makeItemsItemSaveToDb(conn, item, tableName, 1000+strlen(item->description));
    }
dyStringFree(&createSql);
hFreeConn(&conn);
return track;
}

static struct customFactory makeItemsFactory =
/* Factory for makeItems tracks */
    {
    NULL,
    "makeItems",
    makeItemsRecognizer,
    makeItemsLoader,
    };

/*** Framework for custom factories. ***/

static struct customFactory *factoryList;

static void customFactoryInit()
/* Initialize custom track factory system. */
{
if (factoryList == NULL)
    {
    /* mafFactory has to be first because it
     * doesn't strip comments
     */
    slAddHead(&factoryList, &mafFactory);
    slAddTail(&factoryList, &wigFactory);
    slAddTail(&factoryList, &bigWigFactory);
    slAddTail(&factoryList, &chromGraphFactory);
    slAddTail(&factoryList, &pslFactory);
    slAddTail(&factoryList, &gtfFactory);
    slAddTail(&factoryList, &gffFactory);
    slAddTail(&factoryList, &pgSnpFactory);
    slAddTail(&factoryList, &bedFactory);
    slAddTail(&factoryList, &bigBedFactory);
    slAddTail(&factoryList, &bedGraphFactory);
    slAddTail(&factoryList, &microarrayFactory);
    slAddTail(&factoryList, &coloredExonFactory);
    slAddTail(&factoryList, &encodePeakFactory);
    slAddTail(&factoryList, &bedDetailFactory);
#ifdef USE_BAM
    slAddTail(&factoryList, &bamFactory);
#endif//def USE_BAM
#ifdef USE_TABIX
    slAddTail(&factoryList, &vcfTabixFactory);
#endif//def USE_TABIX
    slAddTail(&factoryList, &makeItemsFactory);
    }
}

struct customFactory *customFactoryFind(char *genomeDb, struct customPp *cpp,
	char *type, struct customTrack *track)
/* Figure out factory that can handle this track.  The track is
 * loaded from the track line if any, and type is the type element
 * if any from that track. */
{
struct customFactory *fac;
customFactoryInit();
for (fac = factoryList; fac != NULL; fac = fac->next)
    if (fac->recognizer(fac, cpp, type, track))
	break;
return fac;
}

void customFactoryAdd(struct customFactory *fac)
/* Add factory to global custom track factory list. */
{
slAddTail(&factoryList, fac);
}

static void parseRgb(char *s, int lineIx,
	unsigned char *retR, unsigned char *retG, unsigned char *retB)
/* Turn comma separated list to RGB vals. */
{
int wordCount;
char *row[4];
wordCount = chopString(s, ",", row, ArraySize(row));
if ((wordCount != 3) || (!isdigit(row[0][0]) || !isdigit(row[1][0]) || !isdigit(row[2][0])))
    errAbort("line %d of custom input, Expecting 3 comma separated numbers in color definition.", lineIx);
*retR = atoi(row[0]);
*retG = atoi(row[1]);
*retB = atoi(row[2]);
}

static void stripJavascript(char **pString)
/* Replace *pString (which is dynamically allocated) with javascript free version of itself. */
{
char *tmp = *pString;
*pString = jsStripJavascript(tmp);
freeMem(tmp);
}

static void customTrackUpdateFromSettings(struct customTrack *track,
                                          char *genomeDb,
					  char *line, int lineIx)
/* replace settings in track with those from new track line */
{
char *pLine = line;
nextWord(&pLine);
line = skipLeadingSpaces(pLine);
struct hash *newSettings = hashVarLine(line, lineIx);
struct hashCookie hc = hashFirst(newSettings);
struct hashEl *hel = NULL;

/* there is a memory leak in this business because these values in the
 * existing settings hash were maybe cloned strings and if they get replaced
 * those previous strings are leaking.  We can't fix this because we don't
 * know which values in the hash are cloned strings or not.
 */
while ((hel = hashNext(&hc)) != NULL)
    ctAddToSettings(track, hel->name, hel->val);
freeHash(&newSettings);

struct trackDb *tdb = track->tdb;
struct hash *hash = tdb->settingsHash;
if (hash == NULL) // make sure we have a settings hash
    hash = tdb->settingsHash = newHash(7);

char *val;
if ((val = hashFindVal(hash, "name")) != NULL)
    {
    freeMem(tdb->shortLabel);  // already set by customTrackTdbDefault()
    if (*val)  /* limit shortLabel to 128 characters to avoid problems */
        tdb->shortLabel = cloneStringZ(val,128);
    else
        tdb->shortLabel = cloneString("My Track");
    stripChar(tdb->shortLabel,'"');	/*	no quotes please	*/
    stripChar(tdb->shortLabel,'\'');	/*	no quotes please	*/
    freeMem(tdb->table);  // already set by customTrackTdbDefault()
    tdb->table = customTrackTableFromLabel(tdb->shortLabel);
    freeMem(tdb->track);  // already set by customTrackTdbDefault()
    tdb->track = cloneString(tdb->table);
    freeMem(tdb->longLabel);  // already set by customTrackTdbDefault()
    /* also use name for description, if not specified */
    tdb->longLabel = cloneString(tdb->shortLabel);
    }
if ((val = hashFindVal(hash, "description")) != NULL)
    {
    freeMem(tdb->longLabel);  // already set by customTrackTdbDefault() or name
    if (*val)
        tdb->longLabel = cloneString(val);
    else
        tdb->longLabel = cloneString("My Custom Track");
    stripChar(tdb->longLabel,'"');	/*	no quotes please	*/
    stripChar(tdb->longLabel,'\'');	/*	no quotes please	*/
    }
tdb->type = hashFindVal(hash, "tdbType");
/* might be an old-style wigType track */
if (NULL == tdb->type)
    tdb->type = hashFindVal(hash, "wigType");
/* might be a user-submitted CT that we're reading for the first time */
if (NULL == tdb->type)
    tdb->type = hashFindVal(hash, "type");
track->genomeDb = cloneString(genomeDb);
track->dbTrackType = hashFindVal(hash, "dbTrackType");
track->dbTableName = hashFindVal(hash, "dbTableName");
#ifdef PROGRESS_METER
track->progressFile = 0;
#endif
if (track->dbTableName)
    {
    track->dbDataLoad = TRUE;
    track->dbTrack = TRUE;
    }
if ((val = hashFindVal(hash, "fieldCount")) != NULL)
    track->fieldCount = sqlUnsigned(val);
if ((val = hashFindVal(hash, "htmlFile")) != NULL)
    {
    if (fileExists(val))
        {
	readInGulp(val, &track->tdb->html, NULL);
        if (val != track->htmlFile)
	    freeMem(track->htmlFile);
        track->htmlFile = cloneString(val);
        }
    }
if ((val = hashFindVal(hash, "chromosomes")) != NULL)
    sqlStringDynamicArray(val, &track->tdb->restrictList, &track->tdb->restrictCount);

if ((val = hashFindVal(hash, "htmlUrl")) != NULL)
    {
    /* adding error trapping because various net.c functions can errAbort */
    struct errCatch *errCatch = errCatchNew();
    if (errCatchStart(errCatch))
	{
	struct dyString *ds = NULL;
	int sd = netUrlOpen(val);
	if (sd >= 0)
	    {
	    char *newUrl = NULL;
	    int newSd = 0;
	    if (netSkipHttpHeaderLinesHandlingRedirect(sd, val, &newSd, &newUrl))
		/* redirect can modify the url */
		{
		if (newUrl)
		    {
		    freeMem(newUrl);
		    sd = newSd;
		    }
		ds = netSlurpFile(sd);
		close(sd);
		track->tdb->html = dyStringCannibalize(&ds);
		}
	    }
	}
    errCatchEnd(errCatch);
    if (errCatch->gotError)
	warn("%s", errCatch->message->string);
    errCatchFree(&errCatch);
    }

tdb->url = hashFindVal(hash, "url");

if ((val = hashFindVal(hash, "visibility")) != NULL)
    {
    if (isdigit(val[0]))
	{
	tdb->visibility = atoi(val);
	if (tdb->visibility > tvSquish)
	    errAbort("line %d of custom input: Expecting visibility 0 to 4 got %s", lineIx, val);
	}
    else
        {
	tdb->visibility = hTvFromString(val);
	}
    }
if ((val = hashFindVal(hash, "group")) != NULL)
    tdb->grp = val;
if ((val = hashFindVal(hash, "useScore")) != NULL)
    tdb->useScore = !sameString(val, "0");
if ((val = hashFindVal(hash, "priority")) != NULL)
    tdb->priority = atof(val);
if ((val = hashFindVal(hash, "color")) != NULL)
    {
    char *c = cloneString(val);
    parseRgb(c, lineIx, &tdb->colorR, &tdb->colorG, &tdb->colorB);
    freeMem(c);
    }
if ((val = hashFindVal(hash, "altColor")) != NULL)
    {
    char *c = cloneString(val);
    parseRgb(c, lineIx, &tdb->altColorR, &tdb->altColorG, &tdb->altColorB);
    freeMem(c);
    }
else
    {
    /* If they don't explicitly set the alt color make it a lighter version
     * of color. */
    tdb->altColorR = (tdb->colorR + 255)/2;
    tdb->altColorG = (tdb->colorG + 255)/2;
    tdb->altColorB = (tdb->colorB + 255)/2;
    }
if ((val = hashFindVal(hash, "offset")) != NULL)
    track->offset = atoi(val);
if ((val = hashFindVal(hash, "maxChromName")) != NULL)
    track->maxChromName = sqlSigned(val);
else
    {
    char *ctDb = ctGenomeOrCurrent(track);
    track->maxChromName = hGetMinIndexLength(ctDb);
    }
if ((line != NULL) && !strstr(line, "tdbType"))
    {
    /* for "external" (user-typed) track lines, save for later display
     * in the manager CGI */
    struct dyString *ds = dyStringNew(0);

    /* exclude special setting used by table browser to indicate file needs
     * to be parsed by the factory */
    char *unparsed;
    if ((unparsed = stringIn(CT_UNPARSED, line)) != NULL)
        {
        char *nextTok = skipToSpaces(unparsed);
        if (!nextTok)
            nextTok = "";
        *unparsed = 0;
        dyStringPrintf(ds, "track %s %s", line, nextTok);
        }
    else
        dyStringPrintf(ds, "track %s", line);
    ctAddToSettings(track, "origTrackLine", dyStringCannibalize(&ds));
    }
stripJavascript(&tdb->track);
stripJavascript(&tdb->table);
stripJavascript(&tdb->shortLabel);
stripJavascript(&tdb->longLabel);
}

char *browserLinesToSetting(struct slName *browserLines)
/* Create a setting with browser lines separated by semi-colons */
{
if (!browserLines)
    return NULL;
struct dyString *ds = dyStringNew(0);
struct slName *bl = NULL;
for (bl = browserLines; bl != NULL; bl = bl->next)
    {
    dyStringAppend(ds, bl->name);
    dyStringAppend(ds, ";");
    }
return dyStringCannibalize(&ds);
}

struct slName *ctBrowserLines(struct customTrack *ct)
/* retrieve browser lines from setting */
{
char *setting;
if ((setting = trackDbSetting(ct->tdb, "browserLines")) == NULL)
    return NULL;
return slNameListFromString(setting, ';');
}

static char *browserLinePosition(struct slName *browserLines)
/* return position from browser lines, or NULL if not found */
{
struct slName *bl;
int wordCt;
char *words[64];
for (bl = browserLines; bl != NULL; bl = bl->next)
    {
    wordCt = chopLine(cloneString(bl->name), words);
    if (wordCt == 3 && sameString("position", words[1]))
        return words[2];
    }
return NULL;
}

void customTrackUpdateFromConfig(struct customTrack *ct, char *genomeDb,
                                 char *config, struct slName **retBrowserLines)
/* update custom track from config containing track line and browser lines
 * Return browser lines */
{
if (!config)
    return;
struct lineFile *lf = lineFileOnString("config", TRUE, config);
char *line;
struct slName *browserLines = NULL;
while (lineFileNextReal(lf, &line))
    if (startsWithWord("track", line))
        customTrackUpdateFromSettings(ct, genomeDb, line, 1);
    else if (startsWithWord("browser", line))
        slNameAddTail(&browserLines, line);
    else
        errAbort("expecting track or browser line, got: %s", line);
char *setting = browserLinesToSetting(browserLines);
if (setting)
    {
    ctAddToSettings(ct, "browserLines", setting);
    char *initialPos = browserLinePosition(browserLines);
    if (initialPos)
        ctAddToSettings(ct, "initialPos", initialPos);
    }
lineFileClose(&lf);
if (retBrowserLines)
    *retBrowserLines = browserLines;
}

char *customTrackUserConfig(struct customTrack *ct)
/* return user-defined track line and browser lines */
{
struct dyString *ds = dyStringNew(0);
char *userTrackLine = ctOrigTrackLine(ct);
if (userTrackLine)
    {
    dyStringAppend(ds, userTrackLine);
    dyStringAppend(ds, "\n");
    }
struct slName *bl = NULL;
for (bl = ctBrowserLines(ct); bl != NULL; bl = bl->next)
    {
    dyStringAppend(ds, bl->name);
    dyStringAppend(ds, "\n");
    }
return (dyStringCannibalize(&ds));
}

static struct customTrack *trackLineToTrack(char *genomeDb, char *line, int lineIx)
/* Convert a track specification line to a custom track structure. */
{
/* Make up basic track with associated tdb.  Fill in settings
 * from var=val pairs in line. */
struct customTrack *track;
AllocVar(track);
struct trackDb *tdb = customTrackTdbDefault();
track->tdb = tdb;
customTrackUpdateFromSettings(track, genomeDb, line, lineIx);
return track;
}

static struct lineFile *customLineFile(char *text, boolean isFile)
/* Figure out input source, handling URL's and compression */
{
if (!text)
    return NULL;

struct lineFile *lf = NULL;
if (isFile)
    {
    if (stringIn("://", text))
        lf = netLineFileOpen(text);
    else
	lf = lineFileOpen(text, TRUE);
    }
else
    {
    if (startsWith("compressed://",text))
	{
	char *words[3];
	char *mem;
        unsigned long size;
	chopByWhite(text,words,3);
    	mem = (char *)sqlUnsignedLong(words[1]);
        size = sqlUnsignedLong(words[2]);
	lf = lineFileDecompressMem(TRUE, mem, size);
	}
    else
        {
	lf = lineFileOnString("custom track", TRUE, text);
	}
    }
return lf;
}

char *customDocParse(char *text)
/* Return description text, expanding URLs as for custom track data */
{
char *line;
struct lineFile *lf = customLineFile(text, FALSE);
if (!lf)
    return NULL;

/* wrap a doc customPp object around it. */
struct customPp *cpp = customDocPpNew(lf);

/* extract doc */
struct dyString *ds = dyStringNew(0);
while ((line = customPpNextReal(cpp)) != NULL)
    {
    dyStringAppend(ds, line);
    dyStringAppend(ds, "\n");
    }
return dyStringCannibalize(&ds);
}

char *ctGenome(struct customTrack *ct)
/* return database setting, using old-style var name if present*/
{
char *setting = trackDbSetting(ct->tdb, "db");
if (setting)
    return setting;
return trackDbSetting(ct->tdb, "genome");
}

struct paraFetchData
    {
    struct paraFetchData *next;
    struct customTrack *track;
    struct customFactory *fac;
    boolean done;
    };

static pthread_mutex_t pfdMutex = PTHREAD_MUTEX_INITIALIZER;
static struct paraFetchData *pfdList = NULL, *pfdRunning = NULL, *pfdDone = NULL, *pfdNeverStarted = NULL;

static void *remoteParallelLoad(void *threadParam)
/* Each thread loads tracks in parallel until all work is done. */
{
pthread_t *pthread = threadParam;
struct paraFetchData *pfd = NULL;
pthread_detach(*pthread);  // this thread will never join back with it's progenitor
    // Canceled threads that might leave locks behind,
    // so the theads are detached and will be neither joined nor canceled.
boolean allDone = FALSE;
while(1)
    {
    pthread_mutex_lock( &pfdMutex );
    if (!pfdList)
	{
	allDone = TRUE;
	}
    else
	{  // move it from the waiting queue to the running queue
	pfd = slPopHead(&pfdList);
	slAddHead(&pfdRunning, pfd);
        }
    pthread_mutex_unlock( &pfdMutex );
    if (allDone)
	return NULL;

    /* protect against errAbort */
    struct errCatch *errCatch = errCatchNew();
    if (errCatchStart(errCatch))
	{
	pfd->done = FALSE;
	pfd->fac->loader(pfd->fac, NULL, NULL, pfd->track, FALSE);
	pfd->done = TRUE;
	}
    errCatchEnd(errCatch);
    if (errCatch->gotError)
	{
	pfd->track->networkErrMsg = cloneString(errCatch->message->string);
	pfd->done = TRUE;
	}
    errCatchFree(&errCatch);

    pthread_mutex_lock( &pfdMutex );
    slRemoveEl(&pfdRunning, pfd);  // this list will not be huge
    slAddHead(&pfdDone, pfd);
    pthread_mutex_unlock( &pfdMutex );

    }
}

static int remoteParallelLoadWait(int maxTimeInSeconds)
/* Wait, checking to see if finished (completed or errAborted).
 * If timed-out or never-ran, record error status.
 * Return error count. */
{
int maxTimeInMilliseconds = 1000 * maxTimeInSeconds;
struct paraFetchData *pfd;
int errCount = 0;
int waitTime = 0;
while(1)
    {
    sleep1000(50); // milliseconds
    waitTime += 50;
    boolean done = TRUE;
    pthread_mutex_lock( &pfdMutex );
    if (pfdList || pfdRunning)
	done = FALSE;
    pthread_mutex_unlock( &pfdMutex );
    if (done)
        break;
    if (waitTime >= maxTimeInMilliseconds)
        break;
    }
pthread_mutex_lock( &pfdMutex );
pfdNeverStarted = pfdList;
pfdList = NULL;  // stop the workers from starting any more waiting track loads
for (pfd = pfdNeverStarted; pfd; pfd = pfd->next)
    {
    // track was never even started
    char temp[256];
    safef(temp, sizeof temp, "Ran out of time (%d milliseconds) unable to process  %s", maxTimeInMilliseconds, pfd->track->tdb->track);
    pfd->track->networkErrMsg = cloneString(temp);
    ++errCount;
    }
for (pfd = pfdRunning; pfd; pfd = pfd->next)
    {
    // unfinished track
    char temp[256];
    safef(temp, sizeof temp, "Timeout %d milliseconds exceeded processing %s", maxTimeInMilliseconds, pfd->track->tdb->track);
    pfd->track->networkErrMsg = cloneString(temp);
    ++errCount;
    }
for (pfd = pfdDone; pfd; pfd = pfd->next)
    {
    // some done tracks may have errors
    if (pfd->track->networkErrMsg)
        ++errCount;
    }
pthread_mutex_unlock( &pfdMutex );
return errCount;
}

static struct customTrack *customFactoryParseOptionalDb(char *genomeDb, char *text,
	boolean isFile, struct slName **retBrowserLines,
	boolean mustBeCurrentDb)
/* Parse text into a custom set of tracks.  Text parameter is a
 * file name if 'isFile' is set.  If mustBeCurrentDb, die if custom track
 * is for some database other than genomeDb. */
{
struct customTrack *trackList = NULL, *track = NULL;
char *line = NULL;
struct hash *chromHash = newHash(8);
float prio = 0.0;
struct sqlConnection *ctConn = NULL;
char *ctConnErrMsg = NULL;
boolean dbTrack = ctDbUseAll();
if (dbTrack)
    {
    /* protect against temporarily offline CT trash server */
    struct errCatch *errCatch = errCatchNew();
    if (errCatchStart(errCatch))
        {
	ctConn = hAllocConn(CUSTOM_TRASH);
        }
    errCatchEnd(errCatch);
    if (errCatch->gotError)
        {
	ctConnErrMsg = cloneString(errCatch->message->string);
        }
    errCatchFree(&errCatch);
    /* warnings from here are not visible to user because of higher-level catching
     * if we want to make the warning visible, have to extend behavior of customTrack.c */
    }

setUdcCacheDir();  // Need to set udc cache dir here because this whole cust trk parse routine 
                   // gets called very early in CGI life by cart.c when processing saved-sessions. 
                   // It is not specific to just hgCustom and hgTracks since any CGI that uses the cart 
                   // may need this.

int ptMax = atoi(cfgOptionDefault("parallelFetch.threads", "20"));  // default number of threads for parallel fetch.

struct lineFile *lf = customLineFile(text, isFile);

/* wrap a customPp object around it. */
struct customPp *cpp = customPpNew(lf);
lf = NULL;

/* Loop through this once for each track. */
while ((line = customPpNextReal(cpp)) != NULL)
    {
    /* Parse out track line and save it in track var.
     * First time through make up track var from thin air
     * if no track line. Find out explicit type setting if any.
     * Also make sure settingsHash is set up. */
    lf = cpp->fileStack;
    if (startsWithWord("track", line))
        {
	track = trackLineToTrack(genomeDb, line, cpp->fileStack->lineIx);
        }
    else if (trackList == NULL)
    /* In this case we handle simple files with a single track
     * and no track line. */
        {
        char defaultLine[256];
        safef(defaultLine, sizeof defaultLine,
                        "track name='%s' description='%s'",
                        CT_DEFAULT_TRACK_NAME, CT_DEFAULT_TRACK_DESCR);
        track = trackLineToTrack(genomeDb, defaultLine, 1);
        customPpReuse(cpp, line);
	}
    else
        {
	errAbort("Expecting 'track' line, got %s\nline %d of %s",
		line, lf->lineIx, lf->fileName);
	}
    if (!track)
        continue;

    /* verify database for custom track */
    char *ctDb = ctGenome(track);
    if (mustBeCurrentDb)
	{
	if (ctDb == NULL)
	    ctDb = genomeDb;
	else if (differentString(ctDb, genomeDb))
	    errAbort("can't load %s data into %s custom tracks",
		     ctDb, genomeDb);
	}
    struct customTrack *oneList = NULL, *oneTrack;

    if (track->dbDataLoad)
    /* Database tracks already mostly loaded, just check that table
     * still exists (they are removed when not accessed for a while). */
        {
	if (ctConn && !ctDbTableExists(ctConn, track->dbTableName))
	    continue;
	if (!ctConn && ctConnErrMsg)
	    track->networkErrMsg = ctConnErrMsg;
	if ( startsWith("maf", track->tdb->type))
	    {
	    struct hash *settings = track->tdb->settingsHash;
	    char *fileName;
	    if ((fileName = hashFindVal(settings, "mafFile")) == NULL)
		continue;
	    if (!fileExists(fileName))
		continue;
	    }

	track->wiggle = startsWith("wig ", track->tdb->type);
	if (track->wiggle)
	    {
	    if (!verifyWibExists(ctConn, track->dbTableName))
		continue;
	    }
	oneList = track;
	if (!ctGenome(track) && ctDb)
	    ctAddToSettings(track, "db", ctDb);
	}
    else
    /* Main case - we have to find a track factory that recognizes
     * this track, and call it.  Also we may need to do some work
     * to load track into database. */
        {
	/* Load track from appropriate factory */
        char *type = track->tdb->type;
	struct customFactory *fac = customFactoryFind(genomeDb, cpp, type, track);
	if (fac == NULL)
	    {
	    struct lineFile *lf = cpp->fileStack;
	    /* Check for case of empty track with no type.  This
	     * is silently ignored for backwards compatibility */
	    if (type == NULL)
	        {
		char *line = customFactoryNextRealTilTrack(cpp);
		customPpReuse(cpp, line);
		if (line == NULL)
		    continue;
		else
		    {
		    errAbort("Unrecognized format line %d of %s:\n\t%s (note: chrom names are case sensitive)",
			lf->lineIx, lf->fileName, emptyForNull(line));
		    }
		}
	    else
		{
		errAbort("Unrecognized format type=%s line %d of %s",
			 (type? type : "NULL"),
			 (lf ? lf->lineIx : 0), (lf ? lf->fileName : "NULL file"));
		}
	    }
        char *dataUrl = NULL;
        if (lf->fileName && (
                startsWith("http://" , lf->fileName) ||
                startsWith("https://", lf->fileName) ||
                startsWith("ftp://"  , lf->fileName)
                ))
            dataUrl = cloneString(lf->fileName);
	char *bigDataUrl = hashFindVal(track->tdb->settingsHash, "bigDataUrl");
	if (bigDataUrl && (ptMax > 0)) // handle separately in parallel so long timeouts don't accrue serially
                                       //  (unless ptMax == 0 which means turn parallel loading off)
	    { 
	    struct paraFetchData *pfd;
	    AllocVar(pfd);
	    pfd->track = track;  // need pointer to be stable
	    pfd->fac = fac;
	    slAddHead(&pfdList, pfd);
    	    oneList = track;
	    }
	else
    	    oneList = fac->loader(fac, chromHash, cpp, track, dbTrack);

	/* Save a few more settings. */
	for (oneTrack = oneList; oneTrack != NULL; oneTrack = oneTrack->next)
	    {
	    ctAddToSettings(track, "tdbType", oneTrack->tdb->type);
	    if (dbTrack && oneTrack->dbTrackType != NULL)
		ctAddToSettings(track, "dbTrackType", oneTrack->dbTrackType);
            if (!trackDbSetting(track->tdb, "inputType"))
                ctAddToSettings(track, "inputType", fac->name);
            if (dataUrl)
		ctAddToSettings(track, "dataUrl", dataUrl);
            if (!ctGenome(track) && ctDb)
                ctAddToSettings(track, "db", ctDb);
	    }
	}
    trackList = slCat(trackList, oneList);
    }

// Call the fac loader in parallel on all the bigDataUrl custom tracks
// using pthreads to avoid long serial timeouts 
pthread_t *threads = NULL;
if (ptMax > 0)     // parallelFetch.threads=0 to disable parallel fetch
    {
    /* launch parallel threads */
    ptMax = min(ptMax, slCount(pfdList));
    if (ptMax > 0)
	{
	AllocArray(threads, ptMax);
	int pt;
	for (pt = 0; pt < ptMax; ++pt)
	    {
	    int rc = pthread_create(&threads[pt], NULL, remoteParallelLoad, &threads[pt]);
	    if (rc)
		{
		errAbort("Unexpected error %d from pthread_create(): %s",rc,strerror(rc));
		}
	    }
	}
    }
if (ptMax > 0)
    {
    /* wait for remote parallel load to finish */
    remoteParallelLoadWait(atoi(cfgOptionDefault("parallelFetch.timeout", "90")));  // wait up to default 90 seconds.
    }


struct slName *browserLines = customPpTakeBrowserLines(cpp);
char *initialPos = browserLinePosition(browserLines);

/* Finish off tracks -- add auxiliary settings, fill in some defaults,
 * and adjust priorities so tracks are not all on top of each other
 * if no priority given. */
for (track = trackList; track != NULL; track = track->next)
    {
    if (track->tdb->priority == 0)
	{
	prio += 0.001;
	track->tdb->priority = prio;
	}
    if (initialPos)
	ctAddToSettings(track, "initialPos", initialPos);
    if (track->bedList)
	{
	/* save item count and first item because if track is
	 * loaded to the database, the bedList will be available next time */
	struct dyString *ds = dyStringNew(0);
	dyStringPrintf(ds, "%d", slCount(track->bedList));
	ctAddToSettings(track, "itemCount", cloneString(ds->string));
	dyStringClear(ds);
	dyStringPrintf(ds, "%s:%d-%d", track->bedList->chrom,
		track->bedList->chromStart, track->bedList->chromEnd);
	ctAddToSettings(track, "firstItemPos", cloneString(ds->string));
	dyStringFree(&ds);
	}
    char *setting = browserLinesToSetting(browserLines);
    if (setting)
	ctAddToSettings(track, "browserLines", setting);
    trackDbPolish(track->tdb);
    }

/* Save return variables, clean up,  and go home. */
if (retBrowserLines != NULL)
    *retBrowserLines = browserLines;
customPpFree(&cpp);
hFreeConn(&ctConn);
return trackList;
}

struct customTrack *customFactoryParse(char *genomeDb, char *text, boolean isFile,
	struct slName **retBrowserLines)
/* Parse text into a custom set of tracks.  Text parameter is a
 * file name if 'isFile' is set.  Die if the track is not for genomeDb. */
{
return customFactoryParseOptionalDb(genomeDb, text, isFile, retBrowserLines, TRUE);
}

struct customTrack *customFactoryParseAnyDb(char *genomeDb, char *text, boolean isFile,
					    struct slName **retBrowserLines)
/* Parse text into a custom set of tracks.  Text parameter is a
 * file name if 'isFile' is set.  Track does not have to be for hGetDb(). */
{
return customFactoryParseOptionalDb(genomeDb, text, isFile, retBrowserLines, FALSE);
}

static boolean readAndIgnore(char *fileName)
/* Read a byte from fileName, so its access time is updated. */
{
boolean ret = FALSE;
char buf[256];
FILE *f = fopen(fileName, "r");
if ( f && (fread(buf, 1, 1, f) == 1 ) )
    ret = TRUE;
if (f)
    fclose(f);
return ret;
}

static boolean testFileSettings(struct trackDb *tdb, char *ctFileName)
/* Return TRUE unless tdb has a setting that ends in File but doesn't
 * specify an existing local file.  */
{
boolean isLive = TRUE;
struct hashEl *fileSettings = trackDbSettingsLike(tdb, "*File");
struct hashEl *s;
for (s = fileSettings;  s != NULL;  s = s->next)
    {
    char *fileName = (char *)(s->val);
    if (fileExists(fileName) && readAndIgnore(fileName))
	{
	verbose(4, "setting %s: %s\n", s->name, fileName);
	}
    else
	{
	isLive = FALSE;
	verbose(3, "Custom track %s setting-file %s=%s does not exist\n",
		ctFileName, s->name, fileName);
	break;
	}
    }
hashElFreeList(&fileSettings);
return isLive;
}

static void touchUdcSettings(struct trackDb *tdb)
/* Touch existing local udcCache bitmap and sparse files.  */
{
char *url = trackDbSetting(tdb, "bigDataUrl");
if (url)
    {
    struct slName *el, *list = udcFileCacheFiles(url, udcDefaultDir());
    for (el = list; el; el = el->next)
	{
	if (fileExists(el->name) && readAndIgnore(el->name))
	    {
	    verbose(4, "setting bigDataUrl: %s\n", el->name);
	    }
	}
    slFreeList(&list);
    }
}

static void freeCustomTrack(struct customTrack **pTrack)
{
if (NULL == pTrack)
    return;
struct customTrack *track = *pTrack;
if (NULL == track)
    return;
struct trackDb *tdb = track->tdb;
freeMem(tdb->shortLabel);
freeMem(tdb->longLabel);
freeMem(tdb->table);
freeMem(tdb->track);
freeMem(tdb->grp);
freeMem(tdb->type);
freeMem(tdb->settings);
if (tdb->restrictList)
    {
    freeMem(tdb->restrictList[0]);
    freeMem(tdb->restrictList);
    }
hashFree(&tdb->settingsHash);
hashFree(&tdb->overrides);
tdbExtrasFree(&tdb->tdbExtras);
freeMem(tdb);
freeMem(track->genomeDb);
if (track->bedList)
    bedFreeList(&track->bedList);
freeMem(track->dbTableName);
freeMem(track->dbTrackType);
freeMem(track->dbStderrFile);
freeMem(track->wigFile);
freeMem(track->wibFile);
freeMem(track->wigAscii);
freeMem(track->htmlFile);
if (track->gffHelper)
    gffFileFree(&track->gffHelper);
if (track->bbiFile)
    bbiFileClose(&track->bbiFile);
freeMem(track->groupName);
freeMem(track->networkErrMsg);
freez(pTrack);
}

void customFactoryTestExistence(char *genomeDb, char *fileName, boolean *retGotLive,
				boolean *retGotExpired)
/* Test existence of custom track fileName.  If it exists, parse it just
 * enough to tell whether it refers to database tables and if so, whether
 * they are alive or have expired.  If they are live, touch them to keep
 * them active. */
{
boolean trackNotFound = TRUE;
char *line = NULL;
struct sqlConnection *ctConn = NULL;
boolean dbTrack = ctDbUseAll();

if (!fileExists(fileName))
    {
    if (retGotExpired)
	*retGotExpired = TRUE;
    return;
    }

struct lineFile *lf = customLineFile(fileName, TRUE);
#ifdef PROGRESS_METER
off_t remoteSize = 0;
if (stringIn("://", fileName))
    remoteSize = remoteFileSize(fileName);
#endif

/* wrap a customPp object around it. */
struct customPp *cpp = customPpNew(lf);
#ifdef PROGRESS_METER
cpp->remoteFileSize = remoteSize;
#endif
lf = NULL;

if (dbTrack)
    ctConn = hAllocConn(CUSTOM_TRASH);

/* Loop through this once for each track. */
while ((line = customPpNextReal(cpp)) != NULL)
    {
    struct customTrack *track = NULL;
    boolean isLive = TRUE;
    /* Parse out track line and save it in track var.
     * First time through make up track var from thin air
     * if no track line. Find out explicit type setting if any.
     * Also make sure settingsHash is set up. */
    lf = cpp->fileStack;

    if (startsWithWord("track", line))
        {
	track = trackLineToTrack(genomeDb, line, cpp->fileStack->lineIx);
        }
    else if (trackNotFound)
	/* In this case we handle simple files with a single track
	 * and no track line. */
        {
        char defaultLine[256];
        safef(defaultLine, sizeof defaultLine,
	      "track name='%s' description='%s'",
	      CT_DEFAULT_TRACK_NAME, CT_DEFAULT_TRACK_DESCR);
        track = trackLineToTrack(genomeDb, defaultLine, 1);
        customPpReuse(cpp, line);
	}
    else
        {
	errAbort("Expecting 'track' line, got %s\nline %d of %s",
		 line, lf->lineIx, lf->fileName);
	}
    assert(track);
    assert(track->tdb);

    /* don't verify database for custom track -- we might be testing existence
     * for another database. */


    /* Handle File* settings */
    isLive = (isLive && testFileSettings(track->tdb, fileName));


    /* Handle bigDataUrl udc settings */
    touchUdcSettings(track->tdb);

    if (track->dbDataLoad)
	/* Track was loaded into the database -- check if it still exists. */
        {
	if (ctConn && ctDbTableExists(ctConn, track->dbTableName))
	    {
	    if (isLive)
		{
		/* Touch database table to keep it alive. */
		ctTouchLastUse(ctConn, track->dbTableName, TRUE);
		verbose(4, "customTrash db table: %s\n", track->dbTableName);
		}
	    }
	else
	    {
	    isLive = FALSE;
	    verbose(3, "Custom track %s dbTableName %s does not exist\n",
		    fileName, track->dbTableName);
	    }
	}
    else
	/* Track data in this file. */
	{
	while (customFactoryNextRealTilTrack(cpp))
	    /* Skip data lines until we get to next track or EOF. */
	    ;
	}
    if (isLive)
	{
	if (retGotLive)
	    *retGotLive = TRUE;
	}
    else
	{
	if (retGotExpired)
	    *retGotExpired = TRUE;
	}
    freeCustomTrack(&track);
    trackNotFound = FALSE;
    }
customPpFree(&cpp);
freez(&cpp);
hFreeConn(&ctConn);
}

char *ctInitialPosition(struct customTrack *ct)
/* return initial position plucked from browser lines,
 * or position of first element if none specified */
{
char buf[128];
char *pos = trackDbSetting(ct->tdb, "initialPos");
if (!pos && ct->bedList)
    {
    safef(buf, sizeof(buf), "%s:%d-%d", ct->bedList->chrom,
                ct->bedList->chromStart, ct->bedList->chromEnd);
    pos = buf;
    }
if (pos)
    return cloneString(pos);
return NULL;
}

char *ctDataUrl(struct customTrack *ct)
/* return URL where data can be reloaded, if any */
{
return trackDbSetting(ct->tdb, "dataUrl");
}

char *ctHtmlUrl(struct customTrack *ct)
/* return URL where doc can be reloaded, if any */
{
return trackDbSetting(ct->tdb, "htmlUrl");
}

char *ctInputType(struct customTrack *ct)
/* return type of input */
{
char *type = trackDbSetting(ct->tdb, "inputType");
if (type == NULL)
    type = trackDbSetting(ct->tdb, "tdbType");
return type;
}

int ctItemCount(struct customTrack *ct)
/* return number of 'items' in track, or -1 if unknown */
{
if (ct->bedList)
    {
    return (slCount(ct->bedList));
    }
char *val = trackDbSetting(ct->tdb, "itemCount");
if (val)
    return (sqlUnsigned(val));
return -1;
}

char *ctFirstItemPos(struct customTrack *ct)
/* return position of first item in track, or NULL if wiggle or
 * other "non-item" track */
{
return trackDbSetting(ct->tdb, "firstItemPos");
}

char *ctOrigTrackLine(struct customTrack *ct)
/* return initial setting by user for track line */
{
return trackDbSetting(ct->tdb, "origTrackLine");
}

void customFactoryEnableExtraChecking(boolean enable)
/* Enable/disable extra checking of custom tracks in customFactoryParse.
 * E.g. extra checking is great the first time we read in a custom track,
 * but we don't need it for every hgTracks call. */
{
doExtraChecking = enable;
}

