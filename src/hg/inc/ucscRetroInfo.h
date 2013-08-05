/* ucscRetroInfo.h was originally generated by the autoSql program, which also 
 * generated ucscRetroInfo.c and ucscRetroInfo.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef UCSCRETROINFO_H
#define UCSCRETROINFO_H

#include "jksql.h"
#define UCSCRETROINFO_NUM_COLS 53

struct ucscRetroInfo
/* Retrogenes based on cDNA alignments that are expressed or processed pseudogenes. */
    {
    struct ucscRetroInfo *next;  /* Next in singly linked list. */
    char *chrom;	/* Reference sequence chromosome or scaffold col1 */
    unsigned chromStart;	/* pseudogene alignment start position col2 */
    unsigned chromEnd;	/* pseudogene alignment end position col3 */
    char *name;	/* Name of pseudogene col4 */
    unsigned score;	/* score of pseudogene with gene col5 */
    char strand[3];	/* + or - */
    unsigned thickStart;	/* Start of where display should be thick (start codon) */
    unsigned thickEnd;	/* End of where display should be thick (stop codon) */
    unsigned reserved;	/* Always zero for now */
    int blockCount;	/* Number of blocks */
    int *blockSizes;	/* Comma separated list of block sizes */
    int *chromStarts;	/* Start positions relative to chromStart */
    int retroExonCount;	/* number of exons in retroGene col13 */
    int axtScore;	/* blastz score, parent mrna aligned to pseudogene col14 */
    char *type;	/* type of evidence col15 */
    char *gChrom;	/* Chromosome name col16 */
    int gStart;	/* gene alignment start position col17 */
    int gEnd;	/* gene alignment end position col18 */
    char gStrand[3];	/* strand of gene col19 */
    unsigned parentSpliceCount;	/* # of splice sites in parent gene col20 */
    unsigned geneOverlap;	/* bases overlapping col21 */
    unsigned polyA;	/* count of As in polyA col22 */
    int polyAstart;	/* start of polyA, relative to end of pseudogene col23 */
    int exonCover;	/* number of exons in Gene covered col24 */
    unsigned intronCount;	/* number of introns in pseudogene col25 */
    unsigned bestAliCount;	/* number of good mrnas aligning col26 */
    unsigned matches;	/* matches + repMatches col27 */
    unsigned qSize;	/* aligning bases in pseudogene col28 */
    unsigned qEnd;	/* end of cdna alignment col29 */
    unsigned tReps;	/* repeats in gene col30 */
    unsigned coverage;	/* % of bases that align to gene col31 */
    int label;	/* 1=pseudogene,-1 not pseudogene -2 expressed retroGene col32 */
    unsigned milliBad;	/* milliBad score, pseudogene aligned to genome col33 */
    int alignGapCount;	/* simple intron count col34 */
    int processedIntrons;	/* count of introns removed via retrotransposition col35 */
    int conservedSpliceSites;	/* conserved splice site count col36 */
    int maxOverlap;	/* largest overlap with another mrna col37 */
    char *refSeq;	/* Name of closest regSeq to gene col38 */
    int rStart;	/* refSeq alignment start position col39 */
    int rEnd;	/* refSeq alignment end position col40 */
    char *mgc;	/* Name of closest mgc to gene col41 */
    int mStart;	/* mgc alignment start position col42 */
    int mEnd;	/* mgc alignment end position col43 */
    char *kgName;	/* Name of closest knownGene to gene col44 */
    int kStart;	/* kg alignment start position col45 */
    int kEnd;	/* kg alignment end position col46 */
    char *overName;	/* name of overlapping mrna col47 */
    int overStart;	/* overlapping mrna start position col48 */
    int overExonCover;	/* count of overlapping mrna exons col49 */
    char overStrand[3];	/* strand of overlapping mrna col50 */
    float posConf;	/* pvalue for positive col51 */
    unsigned polyAlen;	/* length of polyA col52 */
    };

struct ucscRetroInfo *ucscRetroInfoLoadByQuery(struct sqlConnection *conn, char *query);
/* Load all ucscRetroInfo from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with ucscRetroInfoFreeList(). */

void ucscRetroInfoSaveToDb(struct sqlConnection *conn, struct ucscRetroInfo *el, char *tableName, int updateSize);
/* Save ucscRetroInfo as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */

struct ucscRetroInfo *ucscRetroInfoLoad(char **row);
/* Load a ucscRetroInfo from row fetched with select * from ucscRetroInfo
 * from database.  Dispose of this with ucscRetroInfoFree(). */

struct ucscRetroInfo *ucscRetroInfoLoadAll(char *fileName);
/* Load all ucscRetroInfo from whitespace-separated file.
 * Dispose of this with ucscRetroInfoFreeList(). */

struct ucscRetroInfo *ucscRetroInfoLoadAllByChar(char *fileName, char chopper);
/* Load all ucscRetroInfo from chopper separated file.
 * Dispose of this with ucscRetroInfoFreeList(). */

#define ucscRetroInfoLoadAllByTab(a) ucscRetroInfoLoadAllByChar(a, '\t');
/* Load all ucscRetroInfo from tab separated file.
 * Dispose of this with ucscRetroInfoFreeList(). */

struct ucscRetroInfo *ucscRetroInfoCommaIn(char **pS, struct ucscRetroInfo *ret);
/* Create a ucscRetroInfo out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new ucscRetroInfo */

void ucscRetroInfoFree(struct ucscRetroInfo **pEl);
/* Free a single dynamically allocated ucscRetroInfo such as created
 * with ucscRetroInfoLoad(). */

void ucscRetroInfoFreeList(struct ucscRetroInfo **pList);
/* Free a list of dynamically allocated ucscRetroInfo's */

void ucscRetroInfoOutput(struct ucscRetroInfo *el, FILE *f, char sep, char lastSep);
/* Print out ucscRetroInfo.  Separate fields with sep. Follow last field with lastSep. */

#define ucscRetroInfoTabOut(el,f) ucscRetroInfoOutput(el,f,'\t','\n');
/* Print out ucscRetroInfo as a line in a tab-separated file. */

#define ucscRetroInfoCommaOut(el,f) ucscRetroInfoOutput(el,f,',',',');
/* Print out ucscRetroInfo as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

struct ucscRetroInfo *ucscRetroInfoQuery(struct sqlConnection *conn,
                                       char *table, char *mappedId);
/* load a single retro object for an mapped id from a table or error if not found */
#endif /* UCSCRETROINFO_H */

