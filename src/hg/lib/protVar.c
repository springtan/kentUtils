/* protVar.c was originally generated by the autoSql program, which also 
 * generated protVar.h and protVar.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "protVar.h"


void protVarStaticLoad(char **row, struct protVar *ret)
/* Load a row from protVar table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->id = row[0];
ret->name = row[1];
ret->srcId = row[2];
ret->baseChangeType = row[3];
ret->location = row[4];
ret->coordinateAccuracy = sqlUnsigned(row[5]);
}

struct protVar *protVarLoad(char **row)
/* Load a protVar from row fetched with select * from protVar
 * from database.  Dispose of this with protVarFree(). */
{
struct protVar *ret;

AllocVar(ret);
ret->id = cloneString(row[0]);
ret->name = cloneString(row[1]);
ret->srcId = cloneString(row[2]);
ret->baseChangeType = cloneString(row[3]);
ret->location = cloneString(row[4]);
ret->coordinateAccuracy = sqlUnsigned(row[5]);
return ret;
}

struct protVar *protVarLoadAll(char *fileName) 
/* Load all protVar from a whitespace-separated file.
 * Dispose of this with protVarFreeList(). */
{
struct protVar *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[6];

while (lineFileRow(lf, row))
    {
    el = protVarLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct protVar *protVarLoadAllByChar(char *fileName, char chopper) 
/* Load all protVar from a chopper separated file.
 * Dispose of this with protVarFreeList(). */
{
struct protVar *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[6];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = protVarLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct protVar *protVarLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all protVar from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with protVarFreeList(). */
{
struct protVar *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = protVarLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void protVarSaveToDb(struct sqlConnection *conn, struct protVar *el, char *tableName, int updateSize)
/* Save protVar as a row to the table specified by tableName. 
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


struct protVar *protVarCommaIn(char **pS, struct protVar *ret)
/* Create a protVar out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new protVar */
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

void protVarFree(struct protVar **pEl)
/* Free a single dynamically allocated protVar such as created
 * with protVarLoad(). */
{
struct protVar *el;

if ((el = *pEl) == NULL) return;
freeMem(el->id);
freeMem(el->name);
freeMem(el->srcId);
freeMem(el->baseChangeType);
freeMem(el->location);
freez(pEl);
}

void protVarFreeList(struct protVar **pList)
/* Free a list of dynamically allocated protVar's */
{
struct protVar *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    protVarFree(&el);
    }
*pList = NULL;
}

void protVarOutput(struct protVar *el, FILE *f, char sep, char lastSep) 
/* Print out protVar.  Separate fields with sep. Follow last field with lastSep. */
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

void protVarPosStaticLoad(char **row, struct protVarPos *ret)
/* Load a row from protVarPos table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->bin = sqlUnsigned(row[0]);
ret->chrom = row[1];
ret->chromStart = sqlUnsigned(row[2]);
ret->chromEnd = sqlUnsigned(row[3]);
ret->name = row[4];
safecpy(ret->strand, sizeof(ret->strand), row[5]);
ret->label = row[6];
}

struct protVarPos *protVarPosLoad(char **row)
/* Load a protVarPos from row fetched with select * from protVarPos
 * from database.  Dispose of this with protVarPosFree(). */
{
struct protVarPos *ret;

AllocVar(ret);
ret->bin = sqlUnsigned(row[0]);
ret->chrom = cloneString(row[1]);
ret->chromStart = sqlUnsigned(row[2]);
ret->chromEnd = sqlUnsigned(row[3]);
ret->name = cloneString(row[4]);
safecpy(ret->strand, sizeof(ret->strand), row[5]);
ret->label = cloneString(row[6]);
return ret;
}

struct protVarPos *protVarPosLoadAll(char *fileName) 
/* Load all protVarPos from a whitespace-separated file.
 * Dispose of this with protVarPosFreeList(). */
{
struct protVarPos *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[7];

while (lineFileRow(lf, row))
    {
    el = protVarPosLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct protVarPos *protVarPosLoadAllByChar(char *fileName, char chopper) 
/* Load all protVarPos from a chopper separated file.
 * Dispose of this with protVarPosFreeList(). */
{
struct protVarPos *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[7];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = protVarPosLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct protVarPos *protVarPosLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all protVarPos from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with protVarPosFreeList(). */
{
struct protVarPos *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = protVarPosLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void protVarPosSaveToDb(struct sqlConnection *conn, struct protVarPos *el, char *tableName, int updateSize)
/* Save protVarPos as a row to the table specified by tableName. 
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


struct protVarPos *protVarPosCommaIn(char **pS, struct protVarPos *ret)
/* Create a protVarPos out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new protVarPos */
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

void protVarPosFree(struct protVarPos **pEl)
/* Free a single dynamically allocated protVarPos such as created
 * with protVarPosLoad(). */
{
struct protVarPos *el;

if ((el = *pEl) == NULL) return;
freeMem(el->chrom);
freeMem(el->name);
freeMem(el->label);
freez(pEl);
}

void protVarPosFreeList(struct protVarPos **pList)
/* Free a list of dynamically allocated protVarPos's */
{
struct protVarPos *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    protVarPosFree(&el);
    }
*pList = NULL;
}

void protVarPosOutput(struct protVarPos *el, FILE *f, char sep, char lastSep) 
/* Print out protVarPos.  Separate fields with sep. Follow last field with lastSep. */
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

void protVarAttrStaticLoad(char **row, struct protVarAttr *ret)
/* Load a row from protVarAttr table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->id = row[0];
ret->attrType = row[1];
ret->attrVal = row[2];
}

struct protVarAttr *protVarAttrLoad(char **row)
/* Load a protVarAttr from row fetched with select * from protVarAttr
 * from database.  Dispose of this with protVarAttrFree(). */
{
struct protVarAttr *ret;

AllocVar(ret);
ret->id = cloneString(row[0]);
ret->attrType = cloneString(row[1]);
ret->attrVal = cloneString(row[2]);
return ret;
}

struct protVarAttr *protVarAttrLoadAll(char *fileName) 
/* Load all protVarAttr from a whitespace-separated file.
 * Dispose of this with protVarAttrFreeList(). */
{
struct protVarAttr *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[3];

while (lineFileRow(lf, row))
    {
    el = protVarAttrLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct protVarAttr *protVarAttrLoadAllByChar(char *fileName, char chopper) 
/* Load all protVarAttr from a chopper separated file.
 * Dispose of this with protVarAttrFreeList(). */
{
struct protVarAttr *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[3];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = protVarAttrLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct protVarAttr *protVarAttrLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all protVarAttr from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with protVarAttrFreeList(). */
{
struct protVarAttr *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = protVarAttrLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void protVarAttrSaveToDb(struct sqlConnection *conn, struct protVarAttr *el, char *tableName, int updateSize)
/* Save protVarAttr as a row to the table specified by tableName. 
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


struct protVarAttr *protVarAttrCommaIn(char **pS, struct protVarAttr *ret)
/* Create a protVarAttr out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new protVarAttr */
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

void protVarAttrFree(struct protVarAttr **pEl)
/* Free a single dynamically allocated protVarAttr such as created
 * with protVarAttrLoad(). */
{
struct protVarAttr *el;

if ((el = *pEl) == NULL) return;
freeMem(el->id);
freeMem(el->attrType);
freeMem(el->attrVal);
freez(pEl);
}

void protVarAttrFreeList(struct protVarAttr **pList)
/* Free a list of dynamically allocated protVarAttr's */
{
struct protVarAttr *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    protVarAttrFree(&el);
    }
*pList = NULL;
}

void protVarAttrOutput(struct protVarAttr *el, FILE *f, char sep, char lastSep) 
/* Print out protVarAttr.  Separate fields with sep. Follow last field with lastSep. */
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

void protVarLinkStaticLoad(char **row, struct protVarLink *ret)
/* Load a row from protVarLink table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->id = row[0];
ret->attrType = row[1];
ret->raKey = row[2];
ret->acc = row[3];
ret->displayVal = row[4];
}

struct protVarLink *protVarLinkLoad(char **row)
/* Load a protVarLink from row fetched with select * from protVarLink
 * from database.  Dispose of this with protVarLinkFree(). */
{
struct protVarLink *ret;

AllocVar(ret);
ret->id = cloneString(row[0]);
ret->attrType = cloneString(row[1]);
ret->raKey = cloneString(row[2]);
ret->acc = cloneString(row[3]);
ret->displayVal = cloneString(row[4]);
return ret;
}

struct protVarLink *protVarLinkLoadAll(char *fileName) 
/* Load all protVarLink from a whitespace-separated file.
 * Dispose of this with protVarLinkFreeList(). */
{
struct protVarLink *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[5];

while (lineFileRow(lf, row))
    {
    el = protVarLinkLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct protVarLink *protVarLinkLoadAllByChar(char *fileName, char chopper) 
/* Load all protVarLink from a chopper separated file.
 * Dispose of this with protVarLinkFreeList(). */
{
struct protVarLink *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[5];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = protVarLinkLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct protVarLink *protVarLinkLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all protVarLink from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with protVarLinkFreeList(). */
{
struct protVarLink *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = protVarLinkLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void protVarLinkSaveToDb(struct sqlConnection *conn, struct protVarLink *el, char *tableName, int updateSize)
/* Save protVarLink as a row to the table specified by tableName. 
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


struct protVarLink *protVarLinkCommaIn(char **pS, struct protVarLink *ret)
/* Create a protVarLink out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new protVarLink */
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

void protVarLinkFree(struct protVarLink **pEl)
/* Free a single dynamically allocated protVarLink such as created
 * with protVarLinkLoad(). */
{
struct protVarLink *el;

if ((el = *pEl) == NULL) return;
freeMem(el->id);
freeMem(el->attrType);
freeMem(el->raKey);
freeMem(el->acc);
freeMem(el->displayVal);
freez(pEl);
}

void protVarLinkFreeList(struct protVarLink **pList)
/* Free a list of dynamically allocated protVarLink's */
{
struct protVarLink *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    protVarLinkFree(&el);
    }
*pList = NULL;
}

void protVarLinkOutput(struct protVarLink *el, FILE *f, char sep, char lastSep) 
/* Print out protVarLink.  Separate fields with sep. Follow last field with lastSep. */
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

/* -------------------------------- End autoSql Generated Code -------------------------------- */

