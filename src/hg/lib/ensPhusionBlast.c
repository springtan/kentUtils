/* ensPhusionBlast.c was originally generated by the autoSql program, which also 
 * generated ensPhusionBlast.h and ensPhusionBlast.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "ensPhusionBlast.h"


void ensPhusionBlastStaticLoad(char **row, struct ensPhusionBlast *ret)
/* Load a row from ensPhusionBlast table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->chrom = row[0];
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = row[3];
ret->score = sqlUnsigned(row[4]);
strcpy(ret->strand, row[5]);
ret->xenoStart = sqlUnsigned(row[6]);
ret->xenoEnd = sqlUnsigned(row[7]);
}

struct ensPhusionBlast *ensPhusionBlastLoad(char **row)
/* Load a ensPhusionBlast from row fetched with select * from ensPhusionBlast
 * from database.  Dispose of this with ensPhusionBlastFree(). */
{
struct ensPhusionBlast *ret;

AllocVar(ret);
ret->chrom = cloneString(row[0]);
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = cloneString(row[3]);
ret->score = sqlUnsigned(row[4]);
strcpy(ret->strand, row[5]);
ret->xenoStart = sqlUnsigned(row[6]);
ret->xenoEnd = sqlUnsigned(row[7]);
return ret;
}

struct ensPhusionBlast *ensPhusionBlastLoadAll(char *fileName) 
/* Load all ensPhusionBlast from a tab-separated file.
 * Dispose of this with ensPhusionBlastFreeList(). */
{
struct ensPhusionBlast *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[8];

while (lineFileRow(lf, row))
    {
    el = ensPhusionBlastLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct ensPhusionBlast *ensPhusionBlastLoadWhere(struct sqlConnection *conn, char *table, char *where)
/* Load all ensPhusionBlast from table that satisfy where clause. The
 * where clause may be NULL in which case whole table is loaded
 * Dispose of this with ensPhusionBlastFreeList(). */
{
struct ensPhusionBlast *list = NULL, *el;
struct dyString *query = dyStringNew(256);
struct sqlResult *sr;
char **row;

sqlDyStringPrintf(query, "select * from %s", table);
if (where != NULL)
    dyStringPrintf(query, " where %s", where);
sr = sqlGetResult(conn, query->string);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = ensPhusionBlastLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
dyStringFree(&query);
return list;
}

struct ensPhusionBlast *ensPhusionBlastCommaIn(char **pS, struct ensPhusionBlast *ret)
/* Create a ensPhusionBlast out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new ensPhusionBlast */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->chrom = sqlStringComma(&s);
ret->chromStart = sqlUnsignedComma(&s);
ret->chromEnd = sqlUnsignedComma(&s);
ret->name = sqlStringComma(&s);
ret->score = sqlUnsignedComma(&s);
sqlFixedStringComma(&s, ret->strand, sizeof(ret->strand));
ret->xenoStart = sqlUnsignedComma(&s);
ret->xenoEnd = sqlUnsignedComma(&s);
*pS = s;
return ret;
}

void ensPhusionBlastFree(struct ensPhusionBlast **pEl)
/* Free a single dynamically allocated ensPhusionBlast such as created
 * with ensPhusionBlastLoad(). */
{
struct ensPhusionBlast *el;

if ((el = *pEl) == NULL) return;
freeMem(el->chrom);
freeMem(el->name);
freez(pEl);
}

void ensPhusionBlastFreeList(struct ensPhusionBlast **pList)
/* Free a list of dynamically allocated ensPhusionBlast's */
{
struct ensPhusionBlast *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    ensPhusionBlastFree(&el);
    }
*pList = NULL;
}

void ensPhusionBlastOutput(struct ensPhusionBlast *el, FILE *f, char sep, char lastSep) 
/* Print out ensPhusionBlast.  Separate fields with sep. Follow last field with lastSep. */
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
fprintf(f, "%u", el->xenoStart);
fputc(sep,f);
fprintf(f, "%u", el->xenoEnd);
fputc(lastSep,f);
}

