/* cutter.c was originally generated by the autoSql program, which also 
 * generated cutter.h and cutter.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "dnaseq.h"
#include "dnautil.h"
#include "bed.h"
#include "cutter.h"


struct cutter *cutterLoad(char **row)
/* Load a cutter from row fetched with select * from cutter
 * from database.  Dispose of this with cutterFree(). */
{
struct cutter *ret;

AllocVar(ret);
ret->numSciz = sqlUnsigned(row[8]);
ret->numCompanies = sqlUnsigned(row[10]);
ret->numRefs = sqlUnsigned(row[12]);
ret->name = cloneString(row[0]);
ret->size = sqlUnsigned(row[1]);
ret->matchSize = sqlUnsigned(row[2]);
ret->seq = cloneString(row[3]);
ret->cut = sqlUnsigned(row[4]);
ret->overhang = sqlSigned(row[5]);
ret->palindromic = sqlUnsigned(row[6]);
ret->semicolon = sqlUnsigned(row[7]);
{
int sizeOne;
sqlStringDynamicArray(row[9], &ret->scizs, &sizeOne);
assert(sizeOne == ret->numSciz);
}
{
int sizeOne;
sqlCharDynamicArray(row[11], &ret->companies, &sizeOne);
assert(sizeOne == ret->numCompanies);
}
{
int sizeOne;
sqlUnsignedDynamicArray(row[13], &ret->refs, &sizeOne);
assert(sizeOne == ret->numRefs);
}
return ret;
}

struct cutter *cutterLoadAll(char *fileName) 
/* Load all cutter from a whitespace-separated file.
 * Dispose of this with cutterFreeList(). */
{
struct cutter *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[14];

while (lineFileRow(lf, row))
    {
    el = cutterLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct cutter *cutterLoadAllByChar(char *fileName, char chopper) 
/* Load all cutter from a chopper separated file.
 * Dispose of this with cutterFreeList(). */
{
struct cutter *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[14];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = cutterLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct cutter *cutterLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all cutter from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with cutterFreeList(). */
{
struct cutter *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = cutterLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void cutterSaveToDb(struct sqlConnection *conn, struct cutter *el, char *tableName, int updateSize)
/* Save cutter as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
char  *scizsArray, *companiesArray, *refsArray;
scizsArray = sqlStringArrayToString(el->scizs, el->numSciz);
companiesArray = sqlCharArrayToString(el->companies, el->numCompanies);
refsArray = sqlUnsignedArrayToString(el->refs, el->numRefs);
sqlDyStringPrintf(update, "insert into %s values ( '%s',%u,%u,'%s',%u,%d,%u,%u,%u,'%s',%u,'%s',%u,'%s')", 
	tableName,  el->name,  el->size,  el->matchSize,  el->seq,  el->cut,  el->overhang,  el->palindromic,  el->semicolon,  el->numSciz,  scizsArray ,  el->numCompanies,  companiesArray ,  el->numRefs,  refsArray );
sqlUpdate(conn, update->string);
freeDyString(&update);
freez(&scizsArray);
freez(&companiesArray);
freez(&refsArray);
}


struct cutter *cutterCommaIn(char **pS, struct cutter *ret)
/* Create a cutter out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new cutter */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->name = sqlStringComma(&s);
ret->size = sqlUnsignedComma(&s);
ret->matchSize = sqlUnsignedComma(&s);
ret->seq = sqlStringComma(&s);
ret->cut = sqlUnsignedComma(&s);
ret->overhang = sqlSignedComma(&s);
ret->palindromic = sqlUnsignedComma(&s);
ret->semicolon = sqlUnsignedComma(&s);
ret->numSciz = sqlUnsignedComma(&s);
{
int i;
s = sqlEatChar(s, '{');
AllocArray(ret->scizs, ret->numSciz);
for (i=0; i<ret->numSciz; ++i)
    {
    ret->scizs[i] = sqlStringComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
ret->numCompanies = sqlUnsignedComma(&s);
{
int i;
s = sqlEatChar(s, '{');
AllocArray(ret->companies, ret->numCompanies);
for (i=0; i<ret->numCompanies; ++i)
    {
    ret->companies[i] = sqlCharComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
ret->numRefs = sqlUnsignedComma(&s);
{
int i;
s = sqlEatChar(s, '{');
AllocArray(ret->refs, ret->numRefs);
for (i=0; i<ret->numRefs; ++i)
    {
    ret->refs[i] = sqlUnsignedComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
*pS = s;
return ret;
}

void cutterFree(struct cutter **pEl)
/* Free a single dynamically allocated cutter such as created
 * with cutterLoad(). */
{
struct cutter *el;

if ((el = *pEl) == NULL) return;
freeMem(el->name);
freeMem(el->seq);
/* All strings in scizs are allocated at once, so only need to free first. */
if (el->scizs != NULL)
    freeMem(el->scizs[0]);
freeMem(el->scizs);
freeMem(el->companies);
freeMem(el->refs);
freez(pEl);
}

void cutterFreeList(struct cutter **pList)
/* Free a list of dynamically allocated cutter's */
{
struct cutter *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    cutterFree(&el);
    }
*pList = NULL;
}

void cutterOutput(struct cutter *el, FILE *f, char sep, char lastSep) 
/* Print out cutter.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->name);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->size);
fputc(sep,f);
fprintf(f, "%u", el->matchSize);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->seq);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->cut);
fputc(sep,f);
fprintf(f, "%d", el->overhang);
fputc(sep,f);
fprintf(f, "%u", el->palindromic);
fputc(sep,f);
fprintf(f, "%u", el->semicolon);
fputc(sep,f);
fprintf(f, "%u", el->numSciz);
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<el->numSciz; ++i)
    {
    if (sep == ',') fputc('"',f);
    fprintf(f, "%s", el->scizs[i]);
    if (sep == ',') fputc('"',f);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(sep,f);
fprintf(f, "%u", el->numCompanies);
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<el->numCompanies; ++i)
    {
    if (sep == ',') fputc('"',f);
    fprintf(f, "%c", el->companies[i]);
    if (sep == ',') fputc('"',f);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(sep,f);
fprintf(f, "%u", el->numRefs);
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<el->numRefs; ++i)
    {
    fprintf(f, "%u", el->refs[i]);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

boolean isIsosciz(struct cutter *mainEnz, struct cutter *possibleScizEnz)
/* Is possibleScizEnz an isoscizomer of mainEnz? */
{
int i;
for (i = 0; i < mainEnz->numSciz; i++)
    if (sameString(possibleScizEnz->name, mainEnz->scizs[i]))
	return TRUE;
return FALSE;
}

struct dnaSeq *stickyEnd(struct cutter *enz)
/* Return the sticky end sequence of an enzyme. Strand is unspecified.  Free this. */
{
struct dnaSeq *ret = NULL;
if (!enz)
    return NULL;
if (enz->overhang > 0)
    {
    char *seq = cloneStringZ(enz->seq + enz->cut, enz->overhang);
    ret = newDnaSeq(seq, enz->overhang, "sticky");
    }
else if (enz->overhang < 0)
    {
    int size = intAbs(enz->overhang);
    char *seq = cloneStringZ(enz->seq + enz->cut - size, size);
    complement(seq, strlen(seq));
    ret = newDnaSeq(seq, strlen(seq), "sticky");
    }
return ret;
}

int acgtCount(char *seq)
/* Return the count of canonical bases in a sequence. */
{
return countChars(seq,'A') + countChars(seq,'C') +
    countChars(seq,'G') + countChars(seq,'T');
}

boolean sameStickyEnd(struct cutter *enz1, struct cutter *enz2)
/* Check to see if two enzymes make the same sticky ends.  If either of the
   enzymes have sticky ends that isn't all ACGT, then this returns false. */
{
boolean ret = FALSE;
struct dnaSeq *sticky1 = stickyEnd(enz1);
struct dnaSeq *sticky2 = stickyEnd(enz2);

if (sticky1 && sticky2)
if (sticky1 && sticky2 && (sticky1->size == sticky2->size) &&
    (acgtCount(sticky1->dna) == sticky1->size) && (acgtCount(sticky2->dna) == sticky2->size))
    {
    if (sameString(sticky1->dna, sticky2->dna))
	ret = TRUE;
    else
	{
	reverseComplement(sticky2->dna, sticky2->size);
	if (sameString(sticky1->dna, sticky2->dna))
	    ret = TRUE;
	}
    }
freeDnaSeq(&sticky1);
freeDnaSeq(&sticky2);
return ret;
}

struct slName *findIsoligamers(struct cutter *enz, struct cutter *enzList)
/* Find isoligamers to an enzyme in a list of enzymes.  */
{
struct slName *list = NULL;
struct cutter *cur;
if (!enz || !enzList)
    return NULL;
for (cur = enzList; cur != NULL; cur = cur->next)
    {
    if (!sameString(cur->name, enz->name) && (sameStickyEnd(enz, cur)))
	{
	struct slName *newname = newSlName(cur->name);
	slAddHead(&list, newname);
	}
    }
slReverse(&list);
return list;
}

boolean isPalindrome(char *seq)
/* Determine if a sequence is palindromic. */
{
int i, j, size = strlen(seq);
for (i = 0; i < size; i++)
    switch (seq[i])
	{
	case 'H':
	case 'B':
	case 'V':
	case 'D': return FALSE;
	}
/* Loop from both ends of the sequence: i from the left, j from the right. */
for (i = 0, j = size-1; (i < size/2) && (j >= ((size%2==1) ? size/2+1 : size/2)); i++, j--)
    {
    switch (seq[i])
	{
	case 'A':
	    {
	    if (seq[j] != 'T')
		return FALSE;
	    break;
	    }
	case 'C':
	    {
	    if (seq[j] != 'G')
		return FALSE;
	    break;
	    }
	case 'G':
	    {
	    if (seq[j] != 'C')
		return FALSE;
	    break;
	    }
	case 'T':
	    {
	    if (seq[j] != 'A')
		return FALSE;
	    break;
	    }
	case 'W':
	    {
	    if (seq[j] != 'S')
		return FALSE;
	    break;
	    }
	case 'S':
	    {
	    if (seq[j] != 'S')
		return FALSE;
	    break;
	    }
	case 'Y':
	    {
	    if (seq[j] != 'R')
		return FALSE;
	    break;
	    }
	case 'R':
	    {
	    if (seq[j] != 'Y')
		return FALSE;
	    break;
	    }
	case 'M':
	    {
	    if (seq[j] != 'K')
		return FALSE;
	    break;
	    }
	case 'K':
	    {
	    if (seq[j] != 'M')
		return FALSE;
	    break;
	    }
	case 'N':
	    {
	    if (seq[j] != 'N')
		return FALSE;
	    break;
	    }
	}
    }
return TRUE;
}

struct cutter *readGcg(char *gcgFile)
/* Parse a GCG file and load it into cutter format. */
{
struct lineFile *lf = lineFileOpen(gcgFile,TRUE);
struct cutter *enzList = NULL;
char *line = "whatever", *words[10], numWords;

/* Skip to the right line. */
while (lineFileNext(lf,&line,NULL) && !startsWith("..",line));
/* */
while ((numWords=lineFileChop(lf,words)))
    {
    struct cutter *newone = NULL;
    int comIx = (numWords==7) ? 5 : 6;
    int refIx = (numWords==7) ? 6 : 7;
    int i;
    char *items[100];

    /* Skip ones */
    if (words[4][0] == '?')
	continue;
    AllocVar(newone);
    newone->semicolon = (words[0][0] == ';') ? TRUE : FALSE;
    /* Deal with the first few columns */
    if (!isdigit(words[1][0]))
	errAbort("Error: expecting a number in cut site column on line %d\n", lf->lineIx+1);
    if (!isdigit(words[3][0]) && words[3][0]!='-')
	errAbort("Error: expecting a number in the overhang column on line %d\n", lf->lineIx+1);
    if (words[comIx][0] != '>')
	errAbort("Error: expecting a \'>\' in the commercial sources column of line %d\n", lf->lineIx+1);
    newone->name = (words[0][0] == ';') ? cloneString(words[0]+1) : cloneString(words[0]);
    newone->cut = atoi(words[1]);
    newone->seq = cloneString(words[2]);
    touppers(newone->seq);
    stripChar(newone->seq,'\'');
    stripChar(newone->seq,'_');
    newone->size = strlen(newone->seq);
    newone->matchSize = newone->size - countChars(newone->seq, 'N');
    newone->palindromic = isPalindrome(newone->seq);
    newone->overhang = atoi(words[3]);
    newone->numCompanies = strlen(words[comIx]+1);
    if (newone->numCompanies > 0)
	newone->companies = cloneMem(words[comIx]+1, newone->numCompanies*sizeof(char));
    newone->numRefs = chopString(words[refIx], ",", items, ArraySize(items));
    AllocArray(newone->refs, newone->numRefs);
    for (i = 0; i < newone->numRefs; i++) 
	{
	if (i == 100)
	    errAbort("Error: Andy didn't make the array for holding references big enough\n");
	if (!isdigit(items[i][0]))
	    errAbort("Error: expecting number in references column in line %d\n", lf->lineIx+1);
	newone->refs[i] = atoi(items[i]);
	}
    /* Deal with isoscizomers. */
    if (numWords == 8)
	{
	newone->numSciz = chopString(words[5], ",", items, ArraySize(items));
	AllocArray(newone->scizs, newone->numSciz*sizeof(int));
	for (i = 0; i < newone->numSciz; i++)
	    {
	    if (i == 100)
		errAbort("Error: Andy didn't make the array for having isoscizomers big enough\n");
	    newone->scizs[i] = cloneString(items[i]);
	    }
	}
    else 
	newone->numSciz = 0;
    slAddHead(&enzList, newone);
    }
slReverse(&enzList);
lineFileClose(&lf);
return enzList;
}

boolean matchingBase(char enzBase, char seqBase)
/* An enzyme matching */
{
switch (seqBase)
    {
    /* Ignore hard-masked and gaps for now. */
    case 'A': 
	{
	switch (enzBase)
	    {
	    case 'A': 
	    case 'R':
	    case 'M':
	    case 'W':
	    case 'H':
	    case 'V':
	    case 'D': 
	    case 'N': return TRUE;
	    default: return FALSE;
	    }
	}
    case 'C': 
	{
	switch (enzBase)
	    {
	    case 'C': 
	    case 'Y':
	    case 'M':
	    case 'S':
	    case 'H':
	    case 'B':
	    case 'V': 
	    case 'N': return TRUE;
	    default: return FALSE;
	    }
	}
    case 'G':
	{
	switch (enzBase)
	    {
	    case 'G': 
	    case 'R':
	    case 'K':
	    case 'S':
	    case 'B':
	    case 'V':
	    case 'D': 
	    case 'N': return TRUE;
	    default: return FALSE;
	    }
	}
    case 'T':
	{
	switch (enzBase)
	    {
	    case 'T': 
	    case 'Y':
	    case 'K':
	    case 'W':
	    case 'H':
	    case 'B':
	    case 'D': 
	    case 'N': return TRUE;
	    default: return FALSE;
	    }
	}
    }
return FALSE;
}

struct bed *allocBedEnz(struct cutter *enz, char *seqName, int seqPos, char strand)
{
struct bed *newbed = NULL;
AllocVar(newbed);
newbed->chrom = cloneString(seqName);
newbed->chromStart = (strand == '-') ? seqPos - enz->size : seqPos;
newbed->chromEnd = (strand == '-') ? seqPos : seqPos + enz->size;
newbed->name = cloneString(enz->name);
newbed->score = 1000;
newbed->strand[0] = strand;
return newbed;
}

struct bed *searchStrand(struct hash *sixers, struct cutter *ACGTo[], struct dnaSeq *seq, int startOffset, char strand)
/* Cheesey function that checks a strand for the enzymes after they're put in the hash/array structures.  
   This used to be a part of the matchEnzymes function but I do it twice now. */
{
struct cutter *enz;
struct bed *bedList = NULL;
int seqPos;

if (ACGTo[0] || ACGTo[1] || ACGTo[2] || ACGTo[3] || (sixers->elCount > 0)) 
    {
    for (seqPos = 0; seqPos < seq->size; seqPos++)
	{
	struct cutter *enzList = NULL;
	char sixer[7];
	int bedPos = (strand == '-') ? (seq->size - seqPos) : seqPos;
	if (seq->size - seqPos >= 6)
	    {
	    struct hashEl *el = NULL;
	    sixer[6] = '\0';
	    memcpy(sixer, seq->dna+seqPos, 6);
	    el = hashLookup(sixers, sixer);
	    if (el)
		{	    
		struct bed *add;
		enz = el->val;
		add = allocBedEnz(enz, seq->name, bedPos + startOffset, strand);
		slAddHead(&bedList, add);
		/* Just in case there's another one with the same sequence. */
		while ((el = hashLookupNext(el)))
		    {
		    enz = el->val;
		    add = allocBedEnz(enz, seq->name, bedPos + startOffset, strand);
		    slAddHead(&bedList, add);
		    }
		}
	    }
	/* Use a certain list depending on which letter we're on in the sequence. */
	if (seq->dna[seqPos] == 'A')
	    enzList = ACGTo[0];
	else if (seq->dna[seqPos] == 'C')
	    enzList = ACGTo[1];
	else if (seq->dna[seqPos] == 'G')
	    enzList = ACGTo[2];
	else if (seq->dna[seqPos] == 'T')
	    enzList = ACGTo[3];
	for (enz = enzList; enz != NULL; enz = enz->next)
	    {
	    int enzPos = 0;
	    int seqCurPos = seqPos;	
	    while (enzPos < enz->size && seqCurPos < seq->size && matchingBase(enz->seq[enzPos],seq->dna[seqCurPos]))
		{
		enzPos++; 
		seqCurPos++;
		}
	    if (enzPos == enz->size)
		{
		struct bed *add = allocBedEnz(enz, seq->name, bedPos + startOffset, strand);
		slAddHead(&bedList, add);
		}
	    }
	}
    }
return bedList;
}

struct bed *matchEnzymes(struct cutter *cutters, struct dnaSeq *seq, int startOffset)
/* Match the enzymes to sequence and return a bed list in all cases. */
{
struct hash *sixers = newHash(8), *palinSixers = newHash(8);
struct cutter *enz;
struct cutter *ACGTo[5], *palinACGTo[5];
struct bed *bedList = NULL, *tmp;
int i;
if (!cutters)
    return NULL;
for (i = 0; i < 5; i++)
    ACGTo[i] = palinACGTo[i] = NULL;
/* Put each of the enzymes in either a hash table of six-cutters or */
enz = cutters;
while (enz != NULL)
    {
    int acgtCount = 0;
    struct cutter *next = enz->next;
    acgtCount = countChars(enz->seq,'A') + countChars(enz->seq,'C') + 
                countChars(enz->seq,'G') + countChars(enz->seq,'T');
    /* Super dumb coding here but it's quick. */
     if (enz->palindromic)
	{
	if (enz->size==6 && acgtCount==6)
	    hashAdd(palinSixers, enz->seq, enz);
	else
	    {
	    if (enz->seq[0] == 'A')
		slAddHead(&palinACGTo[0], enz);
	    else if (enz->seq[0] == 'C')
		slAddHead(&palinACGTo[1], enz);
	    else if (enz->seq[0] == 'G')
		slAddHead(&palinACGTo[2], enz);
	    else if (enz->seq[0] == 'T')
		slAddHead(&palinACGTo[3], enz);
	    else 
		{
		slAddHead(&palinACGTo[4], enz);
		}
	    }
	}
    else
	{
	if (enz->size==6 && acgtCount==6)
	    hashAdd(sixers, enz->seq, enz);
	else
	    {
	    if (enz->seq[0] == 'A')
		slAddHead(&ACGTo[0], enz);
	    else if (enz->seq[0] == 'C')
		slAddHead(&ACGTo[1], enz);
	    else if (enz->seq[0] == 'G')
		slAddHead(&ACGTo[2], enz);
	    else if (enz->seq[0] == 'T')
		slAddHead(&ACGTo[3], enz);
	    else 
		slAddHead(&ACGTo[4], enz);
	    }	
	}
    enz = next;
    }
/* At this point we got a hash for the palindromes and non-palindromic six-cutters, 
   plus an array for each too.  The array is set up so the enzymes starting with 'A' go into [0], 'C'
   into [1], 'G' into [2], 'T' into [3], and other bases into [4].  */
if (ACGTo[4])
    {
    ACGTo[0] = slCat(ACGTo[0], ACGTo[4]);
    ACGTo[1] = slCat(ACGTo[1], ACGTo[4]);
    ACGTo[2] = slCat(ACGTo[2], ACGTo[4]);
    ACGTo[3] = slCat(ACGTo[3], ACGTo[4]);
    }
if (palinACGTo[4])
    {
    palinACGTo[0] = slCat(palinACGTo[0], palinACGTo[4]);
    palinACGTo[1] = slCat(palinACGTo[1], palinACGTo[4]);
    palinACGTo[2] = slCat(palinACGTo[2], palinACGTo[4]);
    palinACGTo[3] = slCat(palinACGTo[3], palinACGTo[4]);
    }
/* Search the DNA in three ways: on the plus strand for both palindromes and nonpalindromes, and then
   just nonpalindromes on the minus strand. */
bedList = searchStrand(palinSixers, palinACGTo, seq, startOffset, '+');
tmp = searchStrand(sixers, ACGTo, seq, startOffset, '+');
bedList = slCat(bedList, tmp);
reverseComplement(seq->dna, seq->size);
tmp = searchStrand(sixers, ACGTo, seq, startOffset, '-');
bedList = slCat(bedList, tmp);
if (bedList)
    slReverse(&bedList);
return bedList;
}

void cullCutters(struct cutter **enzList, boolean semicolon, struct slName *justThese, int matchSize)
/* Reduce the list of enzymes based on different options. */
{
struct cutter *enz = *enzList, *next;
while (enz != NULL)
    {
    next = enz->next;
    if ((justThese && !slNameInList(justThese, enz->name)) ||  
	(enz->matchSize < matchSize) || (!semicolon && enz->semicolon))
	{	
	slRemoveEl(enzList, enz);
	cutterFree(&enz);
	}
    enz = next;
    }
}
