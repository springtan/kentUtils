/* cnpSharpSample.h was originally generated by the autoSql program, which also 
 * generated cnpSharpSample.c and cnpSharpSample.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef CNPSHARPSAMPLE_H
#define CNPSHARPSAMPLE_H

#ifndef JKSQL_H
#include "jksql.h"
#endif

#define CNPSHARPSAMPLE_NUM_COLS 5

struct cnpSharpSample
/* CNP sample data from Sharp lab */
    {
    struct cnpSharpSample *next;  /* Next in singly linked list. */
    char *bac;	/* BAC Name */
    char *sample;	/* Sample name */
    unsigned batch;	/* Batch number */
    float value;	/* Data value */
    char *gender;	/* Gender */
    };

void cnpSharpSampleStaticLoad(char **row, struct cnpSharpSample *ret);
/* Load a row from cnpSharpSample table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct cnpSharpSample *cnpSharpSampleLoad(char **row);
/* Load a cnpSharpSample from row fetched with select * from cnpSharpSample
 * from database.  Dispose of this with cnpSharpSampleFree(). */

struct cnpSharpSample *cnpSharpSampleLoadAll(char *fileName);
/* Load all cnpSharpSample from whitespace-separated file.
 * Dispose of this with cnpSharpSampleFreeList(). */

struct cnpSharpSample *cnpSharpSampleLoadAllByChar(char *fileName, char chopper);
/* Load all cnpSharpSample from chopper separated file.
 * Dispose of this with cnpSharpSampleFreeList(). */

#define cnpSharpSampleLoadAllByTab(a) cnpSharpSampleLoadAllByChar(a, '\t');
/* Load all cnpSharpSample from tab separated file.
 * Dispose of this with cnpSharpSampleFreeList(). */

struct cnpSharpSample *cnpSharpSampleLoadByQuery(struct sqlConnection *conn, char *query);
/* Load all cnpSharpSample from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with cnpSharpSampleFreeList(). */

void cnpSharpSampleSaveToDb(struct sqlConnection *conn, struct cnpSharpSample *el, char *tableName, int updateSize);
/* Save cnpSharpSample as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */

struct cnpSharpSample *cnpSharpSampleCommaIn(char **pS, struct cnpSharpSample *ret);
/* Create a cnpSharpSample out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new cnpSharpSample */

void cnpSharpSampleFree(struct cnpSharpSample **pEl);
/* Free a single dynamically allocated cnpSharpSample such as created
 * with cnpSharpSampleLoad(). */

void cnpSharpSampleFreeList(struct cnpSharpSample **pList);
/* Free a list of dynamically allocated cnpSharpSample's */

void cnpSharpSampleOutput(struct cnpSharpSample *el, FILE *f, char sep, char lastSep);
/* Print out cnpSharpSample.  Separate fields with sep. Follow last field with lastSep. */

#define cnpSharpSampleTabOut(el,f) cnpSharpSampleOutput(el,f,'\t','\n');
/* Print out cnpSharpSample as a line in a tab-separated file. */

#define cnpSharpSampleCommaOut(el,f) cnpSharpSampleOutput(el,f,',',',');
/* Print out cnpSharpSample as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#endif /* CNPSHARPSAMPLE_H */

