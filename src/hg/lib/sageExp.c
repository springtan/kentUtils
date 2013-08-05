/* sageExp.c was originally generated by the autoSql program, which also 
 * generated sageExp.h and sageExp.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "jksql.h"
#include "sageExp.h"


void sageExpStaticLoad(char **row, struct sageExp *ret)
/* Load a row from sageExp table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->num = sqlSigned(row[0]);
ret->exp = row[1];
ret->totalCount = sqlSigned(row[2]);
ret->tissueType = row[3];
ret->tissueDesc = row[4];
ret->tissueSupplier = row[5];
ret->organism = row[6];
ret->organ = row[7];
ret->producer = row[8];
ret->desription = row[9];
}

struct sageExp *sageExpLoad(char **row)
/* Load a sageExp from row fetched with select * from sageExp
 * from database.  Dispose of this with sageExpFree(). */
{
struct sageExp *ret;

AllocVar(ret);
ret->num = sqlSigned(row[0]);
ret->exp = cloneString(row[1]);
ret->totalCount = sqlSigned(row[2]);
ret->tissueType = cloneString(row[3]);
ret->tissueDesc = cloneString(row[4]);
ret->tissueSupplier = cloneString(row[5]);
ret->organism = cloneString(row[6]);
ret->organ = cloneString(row[7]);
ret->producer = cloneString(row[8]);
ret->desription = cloneString(row[9]);
return ret;
}

struct sageExp *sageExpLoadAll(char *fileName) 
/* Load all sageExp from a tab-separated file.
 * Dispose of this with sageExpFreeList(). */
{
struct sageExp *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[10];

while (lineFileRow(lf, row))
    {
    el = sageExpLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct sageExp *sageExpCommaIn(char **pS, struct sageExp *ret)
/* Create a sageExp out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new sageExp */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->num = sqlSignedComma(&s);
ret->exp = sqlStringComma(&s);
ret->totalCount = sqlSignedComma(&s);
ret->tissueType = sqlStringComma(&s);
ret->tissueDesc = sqlStringComma(&s);
ret->tissueSupplier = sqlStringComma(&s);
ret->organism = sqlStringComma(&s);
ret->organ = sqlStringComma(&s);
ret->producer = sqlStringComma(&s);
ret->desription = sqlStringComma(&s);
*pS = s;
return ret;
}

void sageExpFree(struct sageExp **pEl)
/* Free a single dynamically allocated sageExp such as created
 * with sageExpLoad(). */
{
struct sageExp *el;

if ((el = *pEl) == NULL) return;
freeMem(el->exp);
freeMem(el->tissueType);
freeMem(el->tissueDesc);
freeMem(el->tissueSupplier);
freeMem(el->organism);
freeMem(el->organ);
freeMem(el->producer);
freeMem(el->desription);
freez(pEl);
}

void sageExpFreeList(struct sageExp **pList)
/* Free a list of dynamically allocated sageExp's */
{
struct sageExp *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    sageExpFree(&el);
    }
*pList = NULL;
}

void sageExpOutput(struct sageExp *el, FILE *f, char sep, char lastSep) 
/* Print out sageExp.  Separate fields with sep. Follow last field with lastSep. */
{
fprintf(f, "%d", el->num);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->exp);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%d", el->totalCount);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->tissueType);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->tissueDesc);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->tissueSupplier);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->organism);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->organ);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->producer);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->desription);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

