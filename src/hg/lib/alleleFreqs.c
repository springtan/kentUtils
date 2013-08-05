/* alleleFreqs.c was originally generated by the autoSql program, which also 
 * generated alleleFreqs.h and alleleFreqs.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "alleleFreqs.h"


void alleleFreqsStaticLoad(char **row, struct alleleFreqs *ret)
/* Load a row from alleleFreqs table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{
int sizeOne,i;
char *s;

ret->rsId = row[0];
ret->chrom = row[1];
ret->chromStart = sqlSigned(row[2]);
ret->strand = row[3][0];
ret->assembly = row[4];
ret->center = row[5];
ret->protLSID = row[6];
ret->assayLSID = row[7];
ret->panelLSID = row[8];
ret->majAllele = row[9][0];
ret->majCount = sqlSigned(row[10]);
ret->majFreq = atof(row[11]);
ret->minAllele = row[12][0];
ret->minCount = sqlSigned(row[13]);
ret->minFreq = atof(row[14]);
ret->total = sqlSigned(row[15]);
}

struct alleleFreqs *alleleFreqsLoad(char **row)
/* Load a alleleFreqs from row fetched with select * from alleleFreqs
 * from database.  Dispose of this with alleleFreqsFree(). */
{
struct alleleFreqs *ret;
int sizeOne,i;
char *s;

AllocVar(ret);
ret->rsId = cloneString(row[0]);
ret->chrom = cloneString(row[1]);
ret->chromStart = sqlSigned(row[2]);
ret->strand = row[3][0];
ret->assembly = cloneString(row[4]);
ret->center = cloneString(row[5]);
ret->protLSID = cloneString(row[6]);
ret->assayLSID = cloneString(row[7]);
ret->panelLSID = cloneString(row[8]);
ret->majAllele = row[9][0];
ret->majCount = sqlSigned(row[10]);
ret->majFreq = atof(row[11]);
ret->minAllele = row[12][0];
ret->minCount = sqlSigned(row[13]);
ret->minFreq = atof(row[14]);
ret->total = sqlSigned(row[15]);
return ret;
}

struct alleleFreqs *alleleFreqsLoadAll(char *fileName) 
/* Load all alleleFreqs from a whitespace-separated file.
 * Dispose of this with alleleFreqsFreeList(). */
{
struct alleleFreqs *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[16];

while (lineFileRow(lf, row))
    {
    el = alleleFreqsLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct alleleFreqs *alleleFreqsLoadAllByChar(char *fileName, char chopper) 
/* Load all alleleFreqs from a chopper separated file.
 * Dispose of this with alleleFreqsFreeList(). */
{
struct alleleFreqs *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[16];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = alleleFreqsLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct alleleFreqs *alleleFreqsLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all alleleFreqs from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with alleleFreqsFreeList(). */
{
struct alleleFreqs *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = alleleFreqsLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void alleleFreqsSaveToDb(struct sqlConnection *conn, struct alleleFreqs *el, char *tableName, int updateSize)
/* Save alleleFreqs as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s','%s',%d,'%c','%s','%s','%s','%s','%s','%c',%d,%f,'%c',%d,%f,%d)", 
	tableName,  el->rsId,  el->chrom,  el->chromStart,  el->strand,  el->assembly,  el->center,  el->protLSID,  el->assayLSID,  el->panelLSID,  el->majAllele,  el->majCount,  el->majFreq,  el->minAllele,  el->minCount,  el->minFreq,  el->total);
sqlUpdate(conn, update->string);
freeDyString(&update);
}


struct alleleFreqs *alleleFreqsCommaIn(char **pS, struct alleleFreqs *ret)
/* Create a alleleFreqs out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new alleleFreqs */
{
char *s = *pS;
int i;

if (ret == NULL)
    AllocVar(ret);
ret->rsId = sqlStringComma(&s);
ret->chrom = sqlStringComma(&s);
ret->chromStart = sqlSignedComma(&s);
sqlFixedStringComma(&s, &(ret->strand), sizeof(ret->strand));
ret->assembly = sqlStringComma(&s);
ret->center = sqlStringComma(&s);
ret->protLSID = sqlStringComma(&s);
ret->assayLSID = sqlStringComma(&s);
ret->panelLSID = sqlStringComma(&s);
sqlFixedStringComma(&s, &(ret->majAllele), sizeof(ret->majAllele));
ret->majCount = sqlSignedComma(&s);
ret->majFreq = sqlFloatComma(&s);
sqlFixedStringComma(&s, &(ret->minAllele), sizeof(ret->minAllele));
ret->minCount = sqlSignedComma(&s);
ret->minFreq = sqlFloatComma(&s);
ret->total = sqlSignedComma(&s);
*pS = s;
return ret;
}

void alleleFreqsFree(struct alleleFreqs **pEl)
/* Free a single dynamically allocated alleleFreqs such as created
 * with alleleFreqsLoad(). */
{
struct alleleFreqs *el;

if ((el = *pEl) == NULL) return;
freeMem(el->rsId);
freeMem(el->chrom);
freeMem(el->assembly);
freeMem(el->center);
freeMem(el->protLSID);
freeMem(el->assayLSID);
freeMem(el->panelLSID);
freez(pEl);
}

void alleleFreqsFreeList(struct alleleFreqs **pList)
/* Free a list of dynamically allocated alleleFreqs's */
{
struct alleleFreqs *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    alleleFreqsFree(&el);
    }
*pList = NULL;
}

void alleleFreqsOutput(struct alleleFreqs *el, FILE *f, char sep, char lastSep) 
/* Print out alleleFreqs.  Separate fields with sep. Follow last field with lastSep. */
{
int i;
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->rsId);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->chrom);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%d", el->chromStart);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%c", el->strand);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->assembly);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->center);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->protLSID);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->assayLSID);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->panelLSID);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%c", el->majAllele);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%d", el->majCount);
fputc(sep,f);
fprintf(f, "%f", el->majFreq);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%c", el->minAllele);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%d", el->minCount);
fputc(sep,f);
fprintf(f, "%f", el->minFreq);
fputc(sep,f);
fprintf(f, "%d", el->total);
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

