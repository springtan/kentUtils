/* gv.c was originally generated by the autoSql program, which also 
 * generated gv.h and gv.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "gv.h"


void gvStaticLoad(char **row, struct gv *ret)
/* Load a row from gv table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->id = row[0];
ret->name = row[1];
ret->srcId = row[2];
ret->baseChangeType = row[3];
ret->location = row[4];
ret->coordinateAccuracy = sqlUnsigned(row[5]);
}

struct gv *gvLoad(char **row)
/* Load a gv from row fetched with select * from gv
 * from database.  Dispose of this with gvFree(). */
{
struct gv *ret;

AllocVar(ret);
ret->id = cloneString(row[0]);
ret->name = cloneString(row[1]);
ret->srcId = cloneString(row[2]);
ret->baseChangeType = cloneString(row[3]);
ret->location = cloneString(row[4]);
ret->coordinateAccuracy = sqlUnsigned(row[5]);
return ret;
}

struct gv *gvLoadAll(char *fileName) 
/* Load all gv from a whitespace-separated file.
 * Dispose of this with gvFreeList(). */
{
struct gv *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[6];

while (lineFileRow(lf, row))
    {
    el = gvLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gv *gvLoadAllByChar(char *fileName, char chopper) 
/* Load all gv from a chopper separated file.
 * Dispose of this with gvFreeList(). */
{
struct gv *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[6];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = gvLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gv *gvLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all gv from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with gvFreeList(). */
{
struct gv *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = gvLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void gvSaveToDb(struct sqlConnection *conn, struct gv *el, char *tableName, int updateSize)
/* Save gv as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s','%s','%s','%s','%s',%u)", 
	tableName,  el->id,  el->name,  el->srcId,  el->baseChangeType,  el->location,  el->coordinateAccuracy);
sqlUpdate(conn, update->string);
freeDyString(&update);
}


struct gv *gvCommaIn(char **pS, struct gv *ret)
/* Create a gv out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new gv */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->id = sqlStringComma(&s);
ret->name = sqlStringComma(&s);
ret->srcId = sqlStringComma(&s);
ret->baseChangeType = sqlStringComma(&s);
ret->location = sqlStringComma(&s);
ret->coordinateAccuracy = sqlUnsignedComma(&s);
*pS = s;
return ret;
}

void gvFree(struct gv **pEl)
/* Free a single dynamically allocated gv such as created
 * with gvLoad(). */
{
struct gv *el;

if ((el = *pEl) == NULL) return;
freeMem(el->id);
freeMem(el->name);
freeMem(el->srcId);
freeMem(el->baseChangeType);
freeMem(el->location);
freez(pEl);
}

void gvFreeList(struct gv **pList)
/* Free a list of dynamically allocated gv's */
{
struct gv *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    gvFree(&el);
    }
*pList = NULL;
}

void gvOutput(struct gv *el, FILE *f, char sep, char lastSep) 
/* Print out gv.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->id);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->name);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->srcId);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->baseChangeType);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->location);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%u", el->coordinateAccuracy);
fputc(lastSep,f);
}

void gvPosStaticLoad(char **row, struct gvPos *ret)
/* Load a row from gvPos table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->bin = sqlUnsigned(row[0]);
ret->chrom = row[1];
ret->chromStart = sqlUnsigned(row[2]);
ret->chromEnd = sqlUnsigned(row[3]);
ret->name = row[4];
ret->id = NULL;
strcpy(ret->strand, row[5]);
ret->label = row[6];
}

struct gvPos *gvPosLoad(char **row)
/* Load a gvPos from row fetched with select * from gvPos
 * from database.  Dispose of this with gvPosFree(). */
{
struct gvPos *ret;

AllocVar(ret);
ret->bin = sqlUnsigned(row[0]);
ret->chrom = cloneString(row[1]);
ret->chromStart = sqlUnsigned(row[2]);
ret->chromEnd = sqlUnsigned(row[3]);
ret->name = cloneString(row[4]);
ret->id = NULL;
strcpy(ret->strand, row[5]);
ret->label = cloneString(row[6]);
return ret;
}

struct gvPos *gvPosLoadAll(char *fileName) 
/* Load all gvPos from a whitespace-separated file.
 * Dispose of this with gvPosFreeList(). */
{
struct gvPos *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[7];

while (lineFileRow(lf, row))
    {
    el = gvPosLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvPos *gvPosLoadAllByChar(char *fileName, char chopper) 
/* Load all gvPos from a chopper separated file.
 * Dispose of this with gvPosFreeList(). */
{
struct gvPos *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[7];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = gvPosLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvPos *gvPosLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all gvPos from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with gvPosFreeList(). */
{
struct gvPos *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = gvPosLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void gvPosSaveToDb(struct sqlConnection *conn, struct gvPos *el, char *tableName, int updateSize)
/* Save gvPos as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( %u,'%s',%u,%u,'%s','%s','%s')", 
	tableName,  el->bin,  el->chrom,  el->chromStart,  el->chromEnd,  el->name,  el->strand,  el->label);
sqlUpdate(conn, update->string);
freeDyString(&update);
}


struct gvPos *gvPosCommaIn(char **pS, struct gvPos *ret)
/* Create a gvPos out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new gvPos */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->bin = sqlUnsignedComma(&s);
ret->chrom = sqlStringComma(&s);
ret->chromStart = sqlUnsignedComma(&s);
ret->chromEnd = sqlUnsignedComma(&s);
ret->name = sqlStringComma(&s);
sqlFixedStringComma(&s, ret->strand, sizeof(ret->strand));
ret->label = sqlStringComma(&s);
*pS = s;
return ret;
}

void gvPosFree(struct gvPos **pEl)
/* Free a single dynamically allocated gvPos such as created
 * with gvPosLoad(). */
{
struct gvPos *el;

if ((el = *pEl) == NULL) return;
freeMem(el->chrom);
freeMem(el->name);
freeMem(el->label);
freeMem(el->id);
freez(pEl);
}

void gvPosFreeList(struct gvPos **pList)
/* Free a list of dynamically allocated gvPos's */
{
struct gvPos *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    gvPosFree(&el);
    }
*pList = NULL;
}

void gvPosOutput(struct gvPos *el, FILE *f, char sep, char lastSep) 
/* Print out gvPos.  Separate fields with sep. Follow last field with lastSep. */
{
fprintf(f, "%u", el->bin);
fputc(sep,f);
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
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->strand);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->label);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

void gvSrcStaticLoad(char **row, struct gvSrc *ret)
/* Load a row from gvSrc table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->srcId = row[0];
ret->src = row[1];
ret->lsdb = row[2];
}

struct gvSrc *gvSrcLoad(char **row)
/* Load a gvSrc from row fetched with select * from gvSrc
 * from database.  Dispose of this with gvSrcFree(). */
{
struct gvSrc *ret;

AllocVar(ret);
ret->srcId = cloneString(row[0]);
ret->src = cloneString(row[1]);
ret->lsdb = cloneString(row[2]);
return ret;
}

struct gvSrc *gvSrcLoadAll(char *fileName) 
/* Load all gvSrc from a whitespace-separated file.
 * Dispose of this with gvSrcFreeList(). */
{
struct gvSrc *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[3];

while (lineFileRow(lf, row))
    {
    el = gvSrcLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvSrc *gvSrcLoadAllByChar(char *fileName, char chopper) 
/* Load all gvSrc from a chopper separated file.
 * Dispose of this with gvSrcFreeList(). */
{
struct gvSrc *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[3];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = gvSrcLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvSrc *gvSrcLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all gvSrc from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with gvSrcFreeList(). */
{
struct gvSrc *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = gvSrcLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void gvSrcSaveToDb(struct sqlConnection *conn, struct gvSrc *el, char *tableName, int updateSize)
/* Save gvSrc as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s','%s','%s')", 
	tableName,  el->srcId,  el->src,  el->lsdb);
sqlUpdate(conn, update->string);
freeDyString(&update);
}


struct gvSrc *gvSrcCommaIn(char **pS, struct gvSrc *ret)
/* Create a gvSrc out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new gvSrc */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->srcId = sqlStringComma(&s);
ret->src = sqlStringComma(&s);
ret->lsdb = sqlStringComma(&s);
*pS = s;
return ret;
}

void gvSrcFree(struct gvSrc **pEl)
/* Free a single dynamically allocated gvSrc such as created
 * with gvSrcLoad(). */
{
struct gvSrc *el;

if ((el = *pEl) == NULL) return;
freeMem(el->srcId);
freeMem(el->src);
freeMem(el->lsdb);
freez(pEl);
}

void gvSrcFreeList(struct gvSrc **pList)
/* Free a list of dynamically allocated gvSrc's */
{
struct gvSrc *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    gvSrcFree(&el);
    }
*pList = NULL;
}

void gvSrcOutput(struct gvSrc *el, FILE *f, char sep, char lastSep) 
/* Print out gvSrc.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->srcId);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->src);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->lsdb);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

void gvAttrStaticLoad(char **row, struct gvAttr *ret)
/* Load a row from gvAttr table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->id = row[0];
ret->attrType = row[1];
ret->attrVal = row[2];
}

struct gvAttr *gvAttrLoad(char **row)
/* Load a gvAttr from row fetched with select * from gvAttr
 * from database.  Dispose of this with gvAttrFree(). */
{
struct gvAttr *ret;

AllocVar(ret);
ret->id = cloneString(row[0]);
ret->attrType = cloneString(row[1]);
ret->attrVal = cloneString(row[2]);
return ret;
}

struct gvAttr *gvAttrLoadAll(char *fileName) 
/* Load all gvAttr from a whitespace-separated file.
 * Dispose of this with gvAttrFreeList(). */
{
struct gvAttr *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[3];

while (lineFileRow(lf, row))
    {
    el = gvAttrLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvAttr *gvAttrLoadAllByChar(char *fileName, char chopper) 
/* Load all gvAttr from a chopper separated file.
 * Dispose of this with gvAttrFreeList(). */
{
struct gvAttr *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[3];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = gvAttrLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvAttr *gvAttrLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all gvAttr from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with gvAttrFreeList(). */
{
struct gvAttr *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = gvAttrLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void gvAttrSaveToDb(struct sqlConnection *conn, struct gvAttr *el, char *tableName, int updateSize)
/* Save gvAttr as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s','%s','%s')", 
	tableName,  el->id,  el->attrType,  el->attrVal);
sqlUpdate(conn, update->string);
freeDyString(&update);
}


struct gvAttr *gvAttrCommaIn(char **pS, struct gvAttr *ret)
/* Create a gvAttr out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new gvAttr */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->id = sqlStringComma(&s);
ret->attrType = sqlStringComma(&s);
ret->attrVal = sqlStringComma(&s);
*pS = s;
return ret;
}

void gvAttrFree(struct gvAttr **pEl)
/* Free a single dynamically allocated gvAttr such as created
 * with gvAttrLoad(). */
{
struct gvAttr *el;

if ((el = *pEl) == NULL) return;
freeMem(el->id);
freeMem(el->attrType);
freeMem(el->attrVal);
freez(pEl);
}

void gvAttrFreeList(struct gvAttr **pList)
/* Free a list of dynamically allocated gvAttr's */
{
struct gvAttr *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    gvAttrFree(&el);
    }
*pList = NULL;
}

void gvAttrOutput(struct gvAttr *el, FILE *f, char sep, char lastSep) 
/* Print out gvAttr.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->id);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->attrType);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->attrVal);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

void gvLinkStaticLoad(char **row, struct gvLink *ret)
/* Load a row from gvLink table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->id = row[0];
ret->attrType = row[1];
ret->raKey = row[2];
ret->acc = row[3];
ret->displayVal = row[4];
}

struct gvLink *gvLinkLoad(char **row)
/* Load a gvLink from row fetched with select * from gvLink
 * from database.  Dispose of this with gvLinkFree(). */
{
struct gvLink *ret;

AllocVar(ret);
ret->id = cloneString(row[0]);
ret->attrType = cloneString(row[1]);
ret->raKey = cloneString(row[2]);
ret->acc = cloneString(row[3]);
ret->displayVal = cloneString(row[4]);
return ret;
}

struct gvLink *gvLinkLoadAll(char *fileName) 
/* Load all gvLink from a whitespace-separated file.
 * Dispose of this with gvLinkFreeList(). */
{
struct gvLink *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[5];

while (lineFileRow(lf, row))
    {
    el = gvLinkLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvLink *gvLinkLoadAllByChar(char *fileName, char chopper) 
/* Load all gvLink from a chopper separated file.
 * Dispose of this with gvLinkFreeList(). */
{
struct gvLink *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[5];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = gvLinkLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvLink *gvLinkLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all gvLink from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with gvLinkFreeList(). */
{
struct gvLink *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = gvLinkLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void gvLinkSaveToDb(struct sqlConnection *conn, struct gvLink *el, char *tableName, int updateSize)
/* Save gvLink as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s','%s','%s','%s','%s')", 
	tableName,  el->id,  el->attrType,  el->raKey,  el->acc,  el->displayVal);
sqlUpdate(conn, update->string);
freeDyString(&update);
}


struct gvLink *gvLinkCommaIn(char **pS, struct gvLink *ret)
/* Create a gvLink out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new gvLink */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->id = sqlStringComma(&s);
ret->attrType = sqlStringComma(&s);
ret->raKey = sqlStringComma(&s);
ret->acc = sqlStringComma(&s);
ret->displayVal = sqlStringComma(&s);
*pS = s;
return ret;
}

void gvLinkFree(struct gvLink **pEl)
/* Free a single dynamically allocated gvLink such as created
 * with gvLinkLoad(). */
{
struct gvLink *el;

if ((el = *pEl) == NULL) return;
freeMem(el->id);
freeMem(el->attrType);
freeMem(el->raKey);
freeMem(el->acc);
freeMem(el->displayVal);
freez(pEl);
}

void gvLinkFreeList(struct gvLink **pList)
/* Free a list of dynamically allocated gvLink's */
{
struct gvLink *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    gvLinkFree(&el);
    }
*pList = NULL;
}

void gvLinkOutput(struct gvLink *el, FILE *f, char sep, char lastSep) 
/* Print out gvLink.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->id);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->attrType);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->raKey);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->acc);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->displayVal);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

void gvAttrLongStaticLoad(char **row, struct gvAttrLong *ret)
/* Load a row from gvAttrLong table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->id = row[0];
ret->attrType = row[1];
ret->attrVal = row[2];
}

struct gvAttrLong *gvAttrLongLoad(char **row)
/* Load a gvAttrLong from row fetched with select * from gvAttrLong
 * from database.  Dispose of this with gvAttrLongFree(). */
{
struct gvAttrLong *ret;

AllocVar(ret);
ret->id = cloneString(row[0]);
ret->attrType = cloneString(row[1]);
ret->attrVal = cloneString(row[2]);
return ret;
}

struct gvAttrLong *gvAttrLongLoadAll(char *fileName) 
/* Load all gvAttrLong from a whitespace-separated file.
 * Dispose of this with gvAttrLongFreeList(). */
{
struct gvAttrLong *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[3];

while (lineFileRow(lf, row))
    {
    el = gvAttrLongLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvAttrLong *gvAttrLongLoadAllByChar(char *fileName, char chopper) 
/* Load all gvAttrLong from a chopper separated file.
 * Dispose of this with gvAttrLongFreeList(). */
{
struct gvAttrLong *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[3];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = gvAttrLongLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct gvAttrLong *gvAttrLongLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all gvAttrLong from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with gvAttrLongFreeList(). */
{
struct gvAttrLong *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = gvAttrLongLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void gvAttrLongSaveToDb(struct sqlConnection *conn, struct gvAttrLong *el, char *tableName, int updateSize)
/* Save gvAttrLong as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s','%s',%s)", 
	tableName,  el->id,  el->attrType,  el->attrVal);
sqlUpdate(conn, update->string);
freeDyString(&update);
}


struct gvAttrLong *gvAttrLongCommaIn(char **pS, struct gvAttrLong *ret)
/* Create a gvAttrLong out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new gvAttrLong */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->id = sqlStringComma(&s);
ret->attrType = sqlStringComma(&s);
ret->attrVal = sqlStringComma(&s);
*pS = s;
return ret;
}

void gvAttrLongFree(struct gvAttrLong **pEl)
/* Free a single dynamically allocated gvAttrLong such as created
 * with gvAttrLongLoad(). */
{
struct gvAttrLong *el;

if ((el = *pEl) == NULL) return;
freeMem(el->id);
freeMem(el->attrType);
freeMem(el->attrVal);
freez(pEl);
}

void gvAttrLongFreeList(struct gvAttrLong **pList)
/* Free a list of dynamically allocated gvAttrLong's */
{
struct gvAttrLong *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    gvAttrLongFree(&el);
    }
*pList = NULL;
}

void gvAttrLongOutput(struct gvAttrLong *el, FILE *f, char sep, char lastSep) 
/* Print out gvAttrLong.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->id);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->attrType);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->attrVal);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

