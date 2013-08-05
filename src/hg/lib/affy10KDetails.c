/* affy10KDetails.c was originally generated by the autoSql program, which also 
 * generated affy10KDetails.h and affy10KDetails.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "affy10KDetails.h"


void affy10KDetailsStaticLoad(char **row, struct affy10KDetails *ret)
/* Load a row from affy10KDetails table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{
ret->affyId = row[0];
ret->rsId = row[1];
ret->tscId = row[2];
strcpy(ret->baseA, row[3]);
strcpy(ret->baseB, row[4]);
strcpy(ret->sequenceA, row[5]);
strcpy(ret->sequenceB, row[6]);
strcpy(ret->enzyme, row[7]);
}

struct affy10KDetails *affy10KDetailsLoad(char **row)
/* Load a affy10KDetails from row fetched with select * from affy10KDetails
 * from database.  Dispose of this with affy10KDetailsFree(). */
{
struct affy10KDetails *ret;

AllocVar(ret);
ret->affyId = cloneString(row[0]);
ret->rsId = cloneString(row[1]);
ret->tscId = cloneString(row[2]);
strcpy(ret->baseA, row[3]);
strcpy(ret->baseB, row[4]);
strcpy(ret->sequenceA, row[5]);
strcpy(ret->sequenceB, row[6]);
strcpy(ret->enzyme, row[7]);
return ret;
}

struct affy10KDetails *affy10KDetailsLoadAll(char *fileName) 
/* Load all affy10KDetails from a whitespace-separated file.
 * Dispose of this with affy10KDetailsFreeList(). */
{
struct affy10KDetails *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[8];

while (lineFileRow(lf, row))
    {
    el = affy10KDetailsLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct affy10KDetails *affy10KDetailsLoadAllByChar(char *fileName, char chopper) 
/* Load all affy10KDetails from a chopper separated file.
 * Dispose of this with affy10KDetailsFreeList(). */
{
struct affy10KDetails *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[8];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = affy10KDetailsLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct affy10KDetails *affy10KDetailsLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all affy10KDetails from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with affy10KDetailsFreeList(). */
{
struct affy10KDetails *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = affy10KDetailsLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void affy10KDetailsSaveToDb(struct sqlConnection *conn, struct affy10KDetails *el, char *tableName, int updateSize)
/* Save affy10KDetails as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s','%s','%s','%s','%s','%s','%s','%s')", 
	tableName,  el->affyId,  el->rsId,  el->tscId,  el->baseA,  el->baseB,  el->sequenceA,  el->sequenceB,  el->enzyme);
sqlUpdate(conn, update->string);
freeDyString(&update);
}

struct affy10KDetails *affy10KDetailsCommaIn(char **pS, struct affy10KDetails *ret)
/* Create a affy10KDetails out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new affy10KDetails */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->affyId = sqlStringComma(&s);
ret->rsId = sqlStringComma(&s);
ret->tscId = sqlStringComma(&s);
sqlFixedStringComma(&s, ret->baseA, sizeof(ret->baseA));
sqlFixedStringComma(&s, ret->baseB, sizeof(ret->baseB));
sqlFixedStringComma(&s, ret->sequenceA, sizeof(ret->sequenceA));
sqlFixedStringComma(&s, ret->sequenceB, sizeof(ret->sequenceB));
sqlFixedStringComma(&s, ret->enzyme, sizeof(ret->enzyme));
*pS = s;
return ret;
}

void affy10KDetailsFree(struct affy10KDetails **pEl)
/* Free a single dynamically allocated affy10KDetails such as created
 * with affy10KDetailsLoad(). */
{
struct affy10KDetails *el;

if ((el = *pEl) == NULL) return;
freeMem(el->affyId);
freeMem(el->rsId);
freeMem(el->tscId);
freez(pEl);
}

void affy10KDetailsFreeList(struct affy10KDetails **pList)
/* Free a list of dynamically allocated affy10KDetails's */
{
struct affy10KDetails *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    affy10KDetailsFree(&el);
    }
*pList = NULL;
}

void affy10KDetailsOutput(struct affy10KDetails *el, FILE *f, char sep, char lastSep) 
/* Print out affy10KDetails.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->affyId);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->rsId);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->tscId);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->baseA);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->baseB);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->sequenceA);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->sequenceB);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->enzyme);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

