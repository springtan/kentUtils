/* encodeIndels.c was originally generated by the autoSql program, which also 
 * generated encodeIndels.h and encodeIndels.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "encode/encodeIndels.h"


void encodeIndelsStaticLoad(char **row, struct encodeIndels *ret)
/* Load a row from encodeIndels table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->chrom = row[0];
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = row[3];
ret->score = sqlUnsigned(row[4]);
strcpy(ret->strand, row[5]);
ret->thickStart = sqlUnsigned(row[6]);
ret->thickEnd = sqlUnsigned(row[7]);
ret->reserved = sqlUnsigned(row[8]);
ret->traceName = row[9];
ret->traceId = row[10];
ret->tracePos = sqlUnsigned(row[11]);
strcpy(ret->traceStrand, row[12]);
ret->variant = row[13];
ret->reference = row[14];
}

struct encodeIndels *encodeIndelsLoad(char **row)
/* Load a encodeIndels from row fetched with select * from encodeIndels
 * from database.  Dispose of this with encodeIndelsFree(). */
{
struct encodeIndels *ret;

AllocVar(ret);
ret->chrom = cloneString(row[0]);
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = cloneString(row[3]);
ret->score = sqlUnsigned(row[4]);
strcpy(ret->strand, row[5]);
ret->thickStart = sqlUnsigned(row[6]);
ret->thickEnd = sqlUnsigned(row[7]);
ret->reserved = sqlUnsigned(row[8]);
ret->traceName = cloneString(row[9]);
ret->traceId = cloneString(row[10]);
ret->tracePos = sqlUnsigned(row[11]);
strcpy(ret->traceStrand, row[12]);
ret->variant = cloneString(row[13]);
ret->reference = cloneString(row[14]);
return ret;
}

struct encodeIndels *encodeIndelsLoadAll(char *fileName) 
/* Load all encodeIndels from a whitespace-separated file.
 * Dispose of this with encodeIndelsFreeList(). */
{
struct encodeIndels *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[15];

while (lineFileRow(lf, row))
    {
    el = encodeIndelsLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct encodeIndels *encodeIndelsLoadAllByChar(char *fileName, char chopper) 
/* Load all encodeIndels from a chopper separated file.
 * Dispose of this with encodeIndelsFreeList(). */
{
struct encodeIndels *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[15];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = encodeIndelsLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct encodeIndels *encodeIndelsCommaIn(char **pS, struct encodeIndels *ret)
/* Create a encodeIndels out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new encodeIndels */
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
ret->thickStart = sqlUnsignedComma(&s);
ret->thickEnd = sqlUnsignedComma(&s);
ret->reserved = sqlUnsignedComma(&s);
ret->traceName = sqlStringComma(&s);
ret->traceId = sqlStringComma(&s);
ret->tracePos = sqlUnsignedComma(&s);
sqlFixedStringComma(&s, ret->traceStrand, sizeof(ret->traceStrand));
ret->variant = sqlStringComma(&s);
ret->reference = sqlStringComma(&s);
*pS = s;
return ret;
}

void encodeIndelsFree(struct encodeIndels **pEl)
/* Free a single dynamically allocated encodeIndels such as created
 * with encodeIndelsLoad(). */
{
struct encodeIndels *el;

if ((el = *pEl) == NULL) return;
freeMem(el->chrom);
freeMem(el->name);
freeMem(el->traceName);
freeMem(el->traceId);
freeMem(el->variant);
freeMem(el->reference);
freez(pEl);
}

void encodeIndelsFreeList(struct encodeIndels **pList)
/* Free a list of dynamically allocated encodeIndels's */
{
struct encodeIndels *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    encodeIndelsFree(&el);
    }
*pList = NULL;
}

void encodeIndelsOutput(struct encodeIndels *el, FILE *f, char sep, char lastSep) 
/* Print out encodeIndels.  Separate fields with sep. Follow last field with lastSep. */
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
fprintf(f, "%u", el->thickStart);
fputc(sep,f);
fprintf(f, "%u", el->thickEnd);
fputc(sep,f);
fprintf(f, "%u", el->reserved);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->traceName);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->traceId);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->tracePos);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->traceStrand);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->variant);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->reference);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

