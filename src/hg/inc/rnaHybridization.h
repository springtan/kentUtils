/* rnaHybridization.h was originally generated by the autoSql program, which also 
 * generated rnaHybridization.c and rnaHybridization.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef RNAHYBRIDIZATION_H
#define RNAHYBRIDIZATION_H

#ifndef JKSQL_H
#include "jksql.h"
#endif

#define RNAHYBRIDIZATION_NUM_COLS 20

struct rnaHybridization
/* perfect hybridizations on RNA level */
    {
    struct rnaHybridization *next;  /* Next in singly linked list. */
    char *chrom;	/* Reference sequence chromosome or scaffold */
    unsigned chromStart;	/* Start position in chromosome for pattern region */
    unsigned chromEnd;	/* End position in chromosome for pattern region */
    char *name;	/* match name */
    unsigned dummy;	/* dummy field to align with bed 6 structure */
    char strand[2];	/* strand for pattern region */
    char *chromTarget;	/* Reference sequence chromosome for target region */
    unsigned chromStartTarget;	/* Start position in chromosome for target region */
    unsigned chromEndTarget;	/* End position in chromosome for target region */
    char strandTarget[2];	/* strand for target region */
    char *refSeqTarget;	/* refSeq gene name for target or empty */
    char *aorfTarget;	/* AORF gene name for target or empty */
    char *igenicsTarget;	/* intergenics name for target or empty */
    char *trnaTarget;	/*  */
    char *JGITarget;	/*  */
    char *patternSeq;	/* Sequence of pattern region 5'-3' */
    char *targetSeq;	/* Sequence of target region 3'-5' */
    float gcContent;	/* GC content ranging from 0 to 1 */
    unsigned matchLength;	/* Length of hybridization site */
    unsigned targetAnnotation;	/* 1 if target is annotated on same strand, zero otherwise */
    };

void rnaHybridizationStaticLoad(char **row, struct rnaHybridization *ret);
/* Load a row from rnaHybridization table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct rnaHybridization *rnaHybridizationLoad(char **row);
/* Load a rnaHybridization from row fetched with select * from rnaHybridization
 * from database.  Dispose of this with rnaHybridizationFree(). */

struct rnaHybridization *rnaHybridizationLoadAll(char *fileName);
/* Load all rnaHybridization from whitespace-separated file.
 * Dispose of this with rnaHybridizationFreeList(). */

struct rnaHybridization *rnaHybridizationLoadAllByChar(char *fileName, char chopper);
/* Load all rnaHybridization from chopper separated file.
 * Dispose of this with rnaHybridizationFreeList(). */

#define rnaHybridizationLoadAllByTab(a) rnaHybridizationLoadAllByChar(a, '\t');
/* Load all rnaHybridization from tab separated file.
 * Dispose of this with rnaHybridizationFreeList(). */

struct rnaHybridization *rnaHybridizationLoadByQuery(struct sqlConnection *conn, char *query);
/* Load all rnaHybridization from table that satisfy the query given.  
 * Where query is of the form 'select * from example where something=something'
 * or 'select example.* from example, anotherTable where example.something = 
 * anotherTable.something'.
 * Dispose of this with rnaHybridizationFreeList(). */

void rnaHybridizationSaveToDb(struct sqlConnection *conn, struct rnaHybridization *el, char *tableName, int updateSize);
/* Save rnaHybridization as a row to the table specified by tableName. 
 * As blob fields may be arbitrary size updateSize specifies the approx size
 * of a string that would contain the entire query. Arrays of native types are
 * converted to comma separated strings and loaded as such, User defined types are
 * inserted as NULL. Strings are automatically escaped to allow insertion into the database. */

struct rnaHybridization *rnaHybridizationCommaIn(char **pS, struct rnaHybridization *ret);
/* Create a rnaHybridization out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new rnaHybridization */

void rnaHybridizationFree(struct rnaHybridization **pEl);
/* Free a single dynamically allocated rnaHybridization such as created
 * with rnaHybridizationLoad(). */

void rnaHybridizationFreeList(struct rnaHybridization **pList);
/* Free a list of dynamically allocated rnaHybridization's */

void rnaHybridizationOutput(struct rnaHybridization *el, FILE *f, char sep, char lastSep);
/* Print out rnaHybridization.  Separate fields with sep. Follow last field with lastSep. */

#define rnaHybridizationTabOut(el,f) rnaHybridizationOutput(el,f,'\t','\n');
/* Print out rnaHybridization as a line in a tab-separated file. */

#define rnaHybridizationCommaOut(el,f) rnaHybridizationOutput(el,f,',',',');
/* Print out rnaHybridization as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#endif /* RNAHYBRIDIZATION_H */

