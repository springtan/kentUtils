/* rnaSecStr.h was originally generated by the autoSql program, which also 
 * generated rnaSecStr.c and rnaSecStr.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef RNASECSTR_H
#define RNASECSTR_H

#define RNASECSTR_NUM_COLS 9

struct rnaSecStr
/* Browser extensible data */
    {
    struct rnaSecStr *next;  /* Next in singly linked list. */
    char *chrom;	/* Chromosome or FPC contig */
    unsigned chromStart;	/* Start position in chromosome */
    unsigned chromEnd;	/* End position in chromosome */
    char *name;	/* Name of item */
    unsigned score;	/* Score from 0-1000 */
    char strand[2];	/* + or - */
    unsigned size;	/* Size of element. */
    char *secStr;	/* Parentheses and '.'s which define the secondary structure */
    double *conf;	/* Confidence of secondary-structure annotation per position (0.0-1.0). */
    };

struct rnaSecStr *rnaSecStrLoad(char **row);
/* Load a rnaSecStr from row fetched with select * from rnaSecStr
 * from database.  Dispose of this with rnaSecStrFree(). */

struct rnaSecStr *rnaSecStrLoadAll(char *fileName);
/* Load all rnaSecStr from whitespace-separated file.
 * Dispose of this with rnaSecStrFreeList(). */

struct rnaSecStr *rnaSecStrLoadAllByChar(char *fileName, char chopper);
/* Load all rnaSecStr from chopper separated file.
 * Dispose of this with rnaSecStrFreeList(). */

#define rnaSecStrLoadAllByTab(a) rnaSecStrLoadAllByChar(a, '\t');
/* Load all rnaSecStr from tab separated file.
 * Dispose of this with rnaSecStrFreeList(). */

struct rnaSecStr *rnaSecStrCommaIn(char **pS, struct rnaSecStr *ret);
/* Create a rnaSecStr out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new rnaSecStr */

void rnaSecStrFree(struct rnaSecStr **pEl);
/* Free a single dynamically allocated rnaSecStr such as created
 * with rnaSecStrLoad(). */

void rnaSecStrFreeList(struct rnaSecStr **pList);
/* Free a list of dynamically allocated rnaSecStr's */

void rnaSecStrOutput(struct rnaSecStr *el, FILE *f, char sep, char lastSep);
/* Print out rnaSecStr.  Separate fields with sep. Follow last field with lastSep. */

#define rnaSecStrTabOut(el,f) rnaSecStrOutput(el,f,'\t','\n');
/* Print out rnaSecStr as a line in a tab-separated file. */

#define rnaSecStrCommaOut(el,f) rnaSecStrOutput(el,f,',',',');
/* Print out rnaSecStr as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#endif /* RNASECSTR_H */

