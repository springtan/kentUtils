/* dbSnpRs.c was originally generated by the autoSql program, which also 
 * generated dbSnpRs.h and dbSnpRs.sql.  This module links the database and
 * the RAM representation of objects. */

#include "common.h"
#include "linefile.h"
#include "dystring.h"
#include "jksql.h"
#include "dbSnpRs.h"


void dbSnpRsStaticLoad(char **row, struct dbSnpRs *ret)
/* Load a row from dbSnpRs table into ret.  The contents of ret will
 * be replaced at the next call to this function. */
{

ret->rsId = row[0];
ret->avHet = atof(row[1]);
ret->avHetSE = atof(row[2]);
ret->valid = row[3];
ret->allele1 = row[4];
ret->allele2 = row[5];
ret->assembly = row[6];
ret->alternate = row[7];
ret->func = row[8];
}

struct dbSnpRs *dbSnpRsLoad(char **row)
/* Load a dbSnpRs from row fetched with select * from dbSnpRs
 * from database.  Dispose of this with dbSnpRsFree(). */
{
struct dbSnpRs *ret;

AllocVar(ret);
ret->rsId = cloneString(row[0]);
ret->avHet = atof(row[1]);
ret->avHetSE = atof(row[2]);
ret->valid = cloneString(row[3]);
ret->allele1 = cloneString(row[4]);
ret->allele2 = cloneString(row[5]);
ret->assembly = cloneString(row[6]);
ret->alternate = cloneString(row[7]);
ret->func = cloneString(row[8]);
return ret;
}

struct dbSnpRs *dbSnpRsLoadAll(char *fileName) 
/* Load all dbSnpRs from a whitespace-separated file.
 * Dispose of this with dbSnpRsFreeList(). */
{
struct dbSnpRs *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[9];

while (lineFileRow(lf, row))
    {
    el = dbSnpRsLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct dbSnpRs *dbSnpRsLoadAllByChar(char *fileName, char chopper) 
/* Load all dbSnpRs from a chopper separated file.
 * Dispose of this with dbSnpRsFreeList(). */
{
struct dbSnpRs *list = NULL, *el;
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *row[9];

while (lineFileNextCharRow(lf, chopper, row, ArraySize(row)))
    {
    el = dbSnpRsLoad(row);
    slAddHead(&list, el);
    }
lineFileClose(&lf);
slReverse(&list);
return list;
}

struct dbSnpRs *dbSnpRsLoadByQuery(struct sqlConnection *conn, char *query)
/* Load all dbSnpRs from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with dbSnpRsFreeList(). */
{
struct dbSnpRs *list = NULL, *el;
struct sqlResult *sr;
char **row;

sr = sqlGetResult(conn, query);
while ((row = sqlNextRow(sr)) != NULL)
    {
    el = dbSnpRsLoad(row);
    slAddHead(&list, el);
    }
slReverse(&list);
sqlFreeResult(&sr);
return list;
}

void dbSnpRsSaveToDb(struct sqlConnection *conn, struct dbSnpRs *el, char *tableName, int updateSize)
/* Save dbSnpRs as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */
{
struct dyString *update = newDyString(updateSize);
sqlDyStringPrintf(update, "insert into %s values ( '%s',%f,%f,'%s','%s','%s','%s','%s','%s')", 
	tableName,  el->rsId,  el->avHet,  el->avHetSE,  el->valid,  el->allele1,  el->allele2,  el->assembly,  el->alternate,  el->func);
sqlUpdate(conn, update->string);
freeDyString(&update);
}


struct dbSnpRs *dbSnpRsCommaIn(char **pS, struct dbSnpRs *ret)
/* Create a dbSnpRs out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new dbSnpRs */
{
char *s = *pS;

if (ret == NULL)
    AllocVar(ret);
ret->rsId = sqlStringComma(&s);
ret->avHet = sqlFloatComma(&s);
ret->avHetSE = sqlFloatComma(&s);
ret->valid = sqlStringComma(&s);
ret->allele1 = sqlStringComma(&s);
ret->allele2 = sqlStringComma(&s);
ret->assembly = sqlStringComma(&s);
ret->alternate = sqlStringComma(&s);
ret->func = sqlStringComma(&s);
*pS = s;
return ret;
}

void dbSnpRsFree(struct dbSnpRs **pEl)
/* Free a single dynamically allocated dbSnpRs such as created
 * with dbSnpRsLoad(). */
{
struct dbSnpRs *el;

if ((el = *pEl) == NULL) return;
freeMem(el->rsId);
freeMem(el->valid);
freeMem(el->allele1);
freeMem(el->allele2);
freeMem(el->assembly);
freeMem(el->alternate);
freeMem(el->func);
freez(pEl);
}

void dbSnpRsFreeList(struct dbSnpRs **pList)
/* Free a list of dynamically allocated dbSnpRs's */
{
struct dbSnpRs *el, *next;

for (el = *pList; el != NULL; el = next)
    {
    next = el->next;
    dbSnpRsFree(&el);
    }
*pList = NULL;
}

void dbSnpRsOutput(struct dbSnpRs *el, FILE *f, char sep, char lastSep) 
/* Print out dbSnpRs.  Separate fields with sep. Follow last field with lastSep. */
{
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->rsId);
if (sep == ',') fputc('"',f);
fputc(sep,f);
fprintf(f, "%f", el->avHet);
fputc(sep,f);
fprintf(f, "%f", el->avHetSE);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->valid);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->allele1);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->allele2);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->assembly);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->alternate);
if (sep == ',') fputc('"',f);
fputc(sep,f);
if (sep == ',') fputc('"',f);
fprintf(f, "%s", el->func);
if (sep == ',') fputc('"',f);
fputc(lastSep,f);
}

/* -------------------------------- End autoSql Generated Code -------------------------------- */

