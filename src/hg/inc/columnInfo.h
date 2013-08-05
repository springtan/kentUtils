/* columnInfo.h was originally generated by the autoSql program, which also 
 * generated columnInfo.c and columnInfo.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef COLUMNINFO_H
#define COLUMNINFO_H

#ifndef JKSQL_H
#include "jksql.h"
#endif

struct columnInfo
/* Meta data information about a particular column in the database. */
    {
    struct columnInfo *next;  /* Next in singly linked list. */
    char *name;	/* Column name */
    char *type;	/* Column type, i.e. int, blob, varchar. */
    char *nullAllowed;	/* Are NULL values allowed? */
    char *key;	/* Is this field indexed? If so how. */
    char *defaultVal;	/* Default value filled. */
    char *extra;	/* Any extra information. */
    };

void columnInfoStaticLoad(char **row, struct columnInfo *ret);
/* Load a row from columnInfo table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct columnInfo *columnInfoLoad(char **row);
/* Load a columnInfo from row fetched with select * from columnInfo
 * from database.  Dispose of this with columnInfoFree(). */

struct columnInfo *columnInfoLoadAll(char *fileName);
/* Load all columnInfo from a tab-separated file.
 * Dispose of this with columnInfoFreeList(). */

struct columnInfo *columnInfoLoadByQuery(struct sqlConnection *conn, char *query);
/* Load all columnInfo from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with columnInfoFreeList(). */

void columnInfoSaveToDb(struct sqlConnection *conn, struct columnInfo *el, char *tableName, int updateSize);
/* Save columnInfo as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */

struct columnInfo *columnInfoCommaIn(char **pS, struct columnInfo *ret);
/* Create a columnInfo out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new columnInfo */

void columnInfoFree(struct columnInfo **pEl);
/* Free a single dynamically allocated columnInfo such as created
 * with columnInfoLoad(). */

void columnInfoFreeList(struct columnInfo **pList);
/* Free a list of dynamically allocated columnInfo's */

void columnInfoOutput(struct columnInfo *el, FILE *f, char sep, char lastSep);
/* Print out columnInfo.  Separate fields with sep. Follow last field with lastSep. */

#define columnInfoTabOut(el,f) columnInfoOutput(el,f,'\t','\n');
/* Print out columnInfo as a line in a tab-separated file. */

#define columnInfoCommaOut(el,f) columnInfoOutput(el,f,',',',');
/* Print out columnInfo as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#endif /* COLUMNINFO_H */

