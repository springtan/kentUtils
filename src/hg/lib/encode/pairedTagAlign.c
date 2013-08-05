/* pairedTagAlign.c was originally generated by the autoSql program, which also 
 * generated pairedTagAlign.h and pairedTagAlign.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "pairedTagAlign.h"


void pairedTagAlignStaticLoad(char **row, struct pairedTagAlign *ret)
/* Load a row from pairedTagAlign table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->chrom = row[0];
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = row[3];
ret->score = sqlUnsigned(row[4]);
safecpy(ret->strand, sizeof(ret->strand), row[5]);
ret->seq1 = row[6];
ret->seq2 = row[7];
}

struct pairedTagAlign *pairedTagAlignLoad(char **row)
/* Load a pairedTagAlign from row fetched with select * from pairedTagAlign
 * from database.  Dispose of this with pairedTagAlignFree(). */
{
struct pairedTagAlign *ret;

AllocVar(ret);
ret->chrom = cloneString(row[0]);
ret->chromStart = sqlUnsigned(row[1]);
ret->chromEnd = sqlUnsigned(row[2]);
ret->name = cloneString(row[3]);
ret->score = sqlUnsigned(row[4]);
safecpy(ret->strand, sizeof(ret->strand), row[5]);
ret->seq1 = cloneString(row[6]);
ret->seq2 = cloneString(row[7]);
return ret;
}

struct pairedTagAlign *pairedTagAlignLoadAll(char *fileName) 
/* Load all pairedTagAlign from a whitespace-separated file.
 * Dispose of this with pairedTagAlignFreeList(). */
{
struct pairedTagAlign *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[8];

while (lineFileRow(lf, row))
    {
    el = pairedTagAlignLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct pairedTagAlign *pairedTagAlignLoadAllByChar(char *fileName, char chopper) 
/* Load all pairedTagAlign from a chopper separated file.
 * Dispose of this with pairedTagAlignFreeList(). */
{
struct pairedTagAlign *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[8];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = pairedTagAlignLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct pairedTagAlign *pairedTagAlignCommaIn(char **pS, struct pairedTagAlign *ret)
/* Create a pairedTagAlign out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new pairedTagAlign */
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
ret->seq1 = sqlStringComma(&s);
ret->seq2 = sqlStringComma(&s);
*pS = s;
return ret;
}

void pairedTagAlignFree(struct pairedTagAlign **pEl)
/* Free a single dynamically allocated pairedTagAlign such as created
 * with pairedTagAlignLoad(). */
{
struct pairedTagAlign *el;

if ((el = *pEl) == NULL) return;
freeMem(el->chrom);
freeMem(el->name);
freeMem(el->seq1);
freeMem(el->seq2);
freez(pEl);
}

void pairedTagAlignFreeList(struct pairedTagAlign **pList)
/* Free a list of dynamically allocated pairedTagAlign's */
{
struct pairedTagAlign *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    pairedTagAlignFree(&el);
    }
*pList = NULL;
}

void pairedTagAlignOutput(struct pairedTagAlign *el, FILE *f, char sep, char lastSep) 
/* Print out pairedTagAlign.  Separate fields with sep. Follow last field with lastSep. */
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
fprintf(f, "%s", el->seq1);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->seq2);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

