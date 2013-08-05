/* putaInfo.c was originally generated by the autoSql program, which also 
 * generated putaInfo.h and putaInfo.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "putaInfo.h"


struct putaInfo *putaInfoLoad(char **row)
/* Load a putaInfo from row fetched with select * from putaInfo
 * from database.  Dispose of this with putaInfoFree(). */
{
struct putaInfo *ret;

AllocVar(ret);
ret->blockCount = sqlUnsigned(row[9]);
ret->chrom = cloneString(row[0]);
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = cloneString(row[3]);
ret->score = sqlUnsigned(row[4]);
ret->strand = cloneString(row[5]);
ret->oChrom = cloneString(row[6]);
ret->oChromStart = sqlUnsigned(row[7]);
ret->oChromEnd = sqlUnsigned(row[8]);
ret->stop = sqlUnsigned(row[10]);
sqlUnsignedArray(row[11], ret->tExons, 2);
sqlUnsignedArray(row[12], ret->qExons, 4);
sqlUnsignedArray(row[13], ret->qBases, 4);
sqlUnsignedArray(row[14], ret->repeats, 2);
{
int sizeOne;
sqlUnsignedDynamicArray(row[15], &ret->stops, &sizeOne);
assert(sizeOne == ret->blockCount);
}
sqlDoubleArray(row[16], ret->id, 2);
return ret;
}

struct putaInfo *putaInfoLoadAll(char *fileName) 
/* Load all putaInfo from a whitespace-separated file.
 * Dispose of this with putaInfoFreeList(). */
{
struct putaInfo *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[17];

while (lineFileRow(lf, row))
    {
    el = putaInfoLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct putaInfo *putaInfoLoadAllByChar(char *fileName, char chopper) 
/* Load all putaInfo from a chopper separated file.
 * Dispose of this with putaInfoFreeList(). */
{
struct putaInfo *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[17];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = putaInfoLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct putaInfo *putaInfoLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all putaInfo from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with putaInfoFreeList(). */
{
struct putaInfo *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = putaInfoLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void putaInfoSaveToDb(struct sqlConnection *conn, struct putaInfo *el, char *tableName, int updateSize)
/* Save putaInfo as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
char  *tExonsArray, *qExonsArray, *qBasesArray, *repeatsArray, *stopsArray, *idArray;
tExonsArray = sqlUnsignedArrayToString(el->tExons, 2);
qExonsArray = sqlUnsignedArrayToString(el->qExons, 4);
qBasesArray = sqlUnsignedArrayToString(el->qBases, 4);
repeatsArray = sqlUnsignedArrayToString(el->repeats, 2);
stopsArray = sqlUnsignedArrayToString(el->stops, el->blockCount);
idArray = sqlDoubleArrayToString(el->id, 2);
sqlDyStringPrintf(update, "insert into %s values ( '%s',%u,%u,'%s',%u,'%s','%s',%u,%u,%u,%u,'%s','%s','%s','%s','%s','%s')", 
	tableName,  el->chrom,  el->chromStart,  el->chromEnd,  el->name,  el->score,  el->strand,  el->oChrom,  el->oChromStart,  el->oChromEnd,  el->blockCount,  el->stop,  tExonsArray ,  qExonsArray ,  qBasesArray ,  repeatsArray ,  stopsArray ,  idArray );
sqlUpdate(conn, update->string);
freeDyString(&update);
freez(&tExonsArray);
freez(&qExonsArray);
freez(&qBasesArray);
freez(&repeatsArray);
freez(&stopsArray);
freez(&idArray);
}


struct putaInfo *putaInfoCommaIn(char **pS, struct putaInfo *ret)
/* Create a putaInfo out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new putaInfo */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->chrom = sqlStringComma(&s);
ret->chromStart = sqlUnsignedComma(&s);
ret->chromEnd = sqlUnsignedComma(&s);
ret->name = sqlStringComma(&s);
ret->score = sqlUnsignedComma(&s);
ret->strand = sqlStringComma(&s);
ret->oChrom = sqlStringComma(&s);
ret->oChromStart = sqlUnsignedComma(&s);
ret->oChromEnd = sqlUnsignedComma(&s);
ret->blockCount = sqlUnsignedComma(&s);
ret->stop = sqlUnsignedComma(&s);
{
int i;
s = sqlEatChar(s, '{');
for (i=0; i<2; ++i)
    {
    ret->tExons[i] = sqlUnsignedComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
{
int i;
s = sqlEatChar(s, '{');
for (i=0; i<4; ++i)
    {
    ret->qExons[i] = sqlUnsignedComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
{
int i;
s = sqlEatChar(s, '{');
for (i=0; i<4; ++i)
    {
    ret->qBases[i] = sqlUnsignedComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
{
int i;
s = sqlEatChar(s, '{');
for (i=0; i<2; ++i)
    {
    ret->repeats[i] = sqlUnsignedComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
{
int i;
s = sqlEatChar(s, '{');
AllocArray(ret->stops, ret->blockCount);
for (i=0; i<ret->blockCount; ++i)
    {
    ret->stops[i] = sqlUnsignedComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
{
int i;
s = sqlEatChar(s, '{');
for (i=0; i<2; ++i)
    {
    ret->id[i] = sqlDoubleComma(&s);
    }
s = sqlEatChar(s, '}');
s = sqlEatChar(s, ',');
}
*pS = s;
return ret;
}

void putaInfoFree(struct putaInfo **pEl)
/* Free a single dynamically allocated putaInfo such as created
 * with putaInfoLoad(). */
{
struct putaInfo *el;

if ((el = *pEl) == NULL) return;
freeMem(el->chrom);
freeMem(el->name);
freeMem(el->strand);
freeMem(el->oChrom);
freeMem(el->stops);
freez(pEl);
}

void putaInfoFreeList(struct putaInfo **pList)
/* Free a list of dynamically allocated putaInfo's */
{
struct putaInfo *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    putaInfoFree(&el);
    }
*pList = NULL;
}

void putaInfoOutput(struct putaInfo *el, FILE *f, char sep, char lastSep) 
/* Print out putaInfo.  Separate fields with sep. Follow last field with lastSep. */
{
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
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->oChrom);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->oChromStart);
fputc(sep,f);
fprintf(f, "%u", el->oChromEnd);
fputc(sep,f);
fprintf(f, "%u", el->blockCount);
fputc(sep,f);
fprintf(f, "%u", el->stop);
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<2; ++i)
    {
    fprintf(f, "%u", el->tExons[i]);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<4; ++i)
    {
    fprintf(f, "%u", el->qExons[i]);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<4; ++i)
    {
    fprintf(f, "%u", el->qBases[i]);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<2; ++i)
    {
    fprintf(f, "%u", el->repeats[i]);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<el->blockCount; ++i)
    {
    fprintf(f, "%u", el->stops[i]);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(sep,f);
{
int i;
if (sep == ',') fputc('{',f);
for (i=0; i<2; ++i)
    {
    fprintf(f, "%g", el->id[i]);
    fputc(',', f);
    }
if (sep == ',') fputc('}',f);
}
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

