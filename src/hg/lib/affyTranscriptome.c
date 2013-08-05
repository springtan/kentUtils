/* affyTranscriptome.c was originally generated by the autoSql program, which also 
 * generated affyTranscriptome.h and affyTranscriptome.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "affyTranscriptome.h"


void affyTranscriptomeStaticLoad(char **row, struct affyTranscriptome *ret)
/* Load a row from affyTranscriptome table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{
int sizeOne,i;
char *s;

ret->chrom = row[0];
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = row[3];
ret->score = sqlUnsigned(row[4]);
strcpy(ret->strand, row[5]);
ret->sampleCount = sqlUnsigned(row[6]);
ret->samplePosition = row[7];
ret->sampleHeight = row[8];
}

struct affyTranscriptome *affyTranscriptomeLoad(char **row)
/* Load a affyTranscriptome from row fetched with select * from affyTranscriptome
 * from database.  Dispose of this with affyTranscriptomeFree(). */
{
struct affyTranscriptome *ret;
int sizeOne,i;
char *s;

AllocVar(ret);
ret->chrom = cloneString(row[0]);
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = cloneString(row[3]);
ret->score = sqlUnsigned(row[4]);
strcpy(ret->strand, row[5]);
ret->sampleCount = sqlUnsigned(row[6]);
ret->samplePosition = cloneString(row[7]);
ret->sampleHeight = cloneString(row[8]);
return ret;
}

struct affyTranscriptome *affyTranscriptomeLoadAll(char *fileName) 
/* Load all affyTranscriptome from a whitespace-separated file.
 * Dispose of this with affyTranscriptomeFreeList(). */
{
struct affyTranscriptome *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[9];

while (lineFileRow(lf, row))
    {
    el = affyTranscriptomeLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct affyTranscriptome *affyTranscriptomeLoadAllByChar(char *fileName, char chopper) 
/* Load all affyTranscriptome from a chopper separated file.
 * Dispose of this with affyTranscriptomeFreeList(). */
{
struct affyTranscriptome *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[9];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = affyTranscriptomeLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct affyTranscriptome *affyTranscriptomeLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all affyTranscriptome from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with affyTranscriptomeFreeList(). */
{
struct affyTranscriptome *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = affyTranscriptomeLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void affyTranscriptomeSaveToDb(struct sqlConnection *conn, struct affyTranscriptome *el, char *tableName, int updateSize)
/* Save affyTranscriptome as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s',%u,%u,'%s',%u,'%s',%u,%s,%s)", 
	tableName,  el->chrom,  el->chromStart,  el->chromEnd,  el->name,  el->score,  el->strand,  el->sampleCount,  el->samplePosition,  el->sampleHeight);
sqlUpdate(conn, update->string);
freeDyString(&update);
}

struct affyTranscriptome *affyTranscriptomeCommaIn(char **pS, struct affyTranscriptome *ret)
/* Create a affyTranscriptome out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new affyTranscriptome */
{
char *s = *pS;
int i;

if (ret == NULL)
    AllocVar(ret);
ret->chrom = sqlStringComma(&s);
ret->chromStart = sqlUnsignedComma(&s);
ret->chromEnd = sqlUnsignedComma(&s);
ret->name = sqlStringComma(&s);
ret->score = sqlUnsignedComma(&s);
sqlFixedStringComma(&s, ret->strand, sizeof(ret->strand));
ret->sampleCount = sqlUnsignedComma(&s);
ret->samplePosition = sqlStringComma(&s);
ret->sampleHeight = sqlStringComma(&s);
*pS = s;
return ret;
}

void affyTranscriptomeFree(struct affyTranscriptome **pEl)
/* Free a single dynamically allocated affyTranscriptome such as created
 * with affyTranscriptomeLoad(). */
{
struct affyTranscriptome *el;

if ((el = *pEl) == NULL) return;
freeMem(el->chrom);
freeMem(el->name);
freeMem(el->samplePosition);
freeMem(el->sampleHeight);
freez(pEl);
}

void affyTranscriptomeFreeList(struct affyTranscriptome **pList)
/* Free a list of dynamically allocated affyTranscriptome's */
{
struct affyTranscriptome *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    affyTranscriptomeFree(&el);
    }
*pList = NULL;
}

void affyTranscriptomeOutput(struct affyTranscriptome *el, FILE *f, char sep, char lastSep) 
/* Print out affyTranscriptome.  Separate fields with sep. Follow last field with lastSep. */
{
int i;
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->chrom);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->chromStart);
fputc(sep,f);
fprintf(f, "%u", el->chromEnd);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->name);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->score);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->strand);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->sampleCount);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->samplePosition);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->sampleHeight);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

