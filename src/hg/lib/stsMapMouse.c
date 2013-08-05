/* stsMapMouse.c was originally generated by the autoSql program, which also 
 * generated stsMapMouse.h and stsMapMouse.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "stsMapMouse.h"


void stsMapMouseStaticLoad(char **row, struct stsMapMouse *ret)
/* Load a row from stsMapMouse table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->chrom = row[0];
ret->chromStart = sqlSigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = row[3];
ret->score = sqlUnsigned(row[4]);
ret->identNo = sqlUnsigned(row[5]);
ret->probeId = sqlUnsigned(row[6]);
ret->markerId = sqlUnsigned(row[7]);
}

struct stsMapMouse *stsMapMouseLoad(char **row)
/* Load a stsMapMouse from row fetched with select * from stsMapMouse
 * from database.  Dispose of this with stsMapMouseFree(). */
{
struct stsMapMouse *ret;

AllocVar(ret);
ret->chrom = cloneString(row[0]);
ret->chromStart = sqlSigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = cloneString(row[3]);
ret->score = sqlUnsigned(row[4]);
ret->identNo = sqlUnsigned(row[5]);
ret->probeId = sqlUnsigned(row[6]);
ret->markerId = sqlUnsigned(row[7]);
return ret;
}

struct stsMapMouse *stsMapMouseLoadAll(char *fileName) 
/* Load all stsMapMouse from a tab-separated file.
 * Dispose of this with stsMapMouseFreeList(). */
{
struct stsMapMouse *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[8];

while (lineFileRow(lf, row))
    {
    el = stsMapMouseLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct stsMapMouse *stsMapMouseLoadWhere(struct sqlConnection *conn, char *table, char *where)
/* Load all stsMapMouse from table that satisfy where clause. The
 * where clause may be NULL in which case whole table is loaded
 * Dispose of this with stsMapMouseFreeList(). */
{
struct stsMapMouse *list = NULL, *el;
struct dyString *query = dyStringNew(256);
struct sqlResult *sr;
char **row;

sqlDyStringPrintf(query, "select * from %s", table);
if (where != NULL)
    dyStringPrintf(query, " where %s", where);
sr = sqlGetResult(conn, query->string);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = stsMapMouseLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
dyStringFree(&query);
return list;
}

struct stsMapMouse *stsMapMouseCommaIn(char **pS, struct stsMapMouse *ret)
/* Create a stsMapMouse out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new stsMapMouse */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->chrom = sqlStringComma(&s);
ret->chromStart = sqlSignedComma(&s);
ret->chromEnd = sqlUnsignedComma(&s);
ret->name = sqlStringComma(&s);
ret->score = sqlUnsignedComma(&s);
ret->identNo = sqlUnsignedComma(&s);
ret->probeId = sqlUnsignedComma(&s);
ret->markerId = sqlUnsignedComma(&s);
*pS = s;
return ret;
}

void stsMapMouseFree(struct stsMapMouse **pEl)
/* Free a single dynamically allocated stsMapMouse such as created
 * with stsMapMouseLoad(). */
{
struct stsMapMouse *el;

if ((el = *pEl) == NULL) return;
freeMem(el->chrom);
freeMem(el->name);
freez(pEl);
}

void stsMapMouseFreeList(struct stsMapMouse **pList)
/* Free a list of dynamically allocated stsMapMouse's */
{
struct stsMapMouse *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    stsMapMouseFree(&el);
    }
*pList = NULL;
}

void stsMapMouseOutput(struct stsMapMouse *el, FILE *f, char sep, char lastSep) 
/* Print out stsMapMouse.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->chrom);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%d", el->chromStart);
fputc(sep,f);
fprintf(f, "%u", el->chromEnd);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->name);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->score);
fputc(sep,f);
fprintf(f, "%u", el->identNo);
fputc(sep,f);
fprintf(f, "%u", el->probeId);
fputc(sep,f);
fprintf(f, "%u", el->markerId);
fputc(lastSep,f);
}

