/* arcogdesc.h was originally generated by the autoSql program, which also 
 * generated arcogdesc.c and arcogdesc.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef ARCOGDESC_H
#define ARCOGDESC_H

#define ARCOGDESC_NUM_COLS 3

struct arcogdesc
/* arCOG Description */
    {
    struct arcogdesc *next;  /* Next in singly linked list. */
    char *name;	/* arCOG name */
    char code[2];	/* COG Code */
    char *description;	/* arCOG description */
    };

void arcogdescStaticLoad(char **row, struct arcogdesc *ret);
/* Load a row from arcogdesc table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct arcogdesc *arcogdescLoad(char **row);
/* Load a arcogdesc from row fetched with select * from arcogdesc
 * from database.  Dispose of this with arcogdescFree(). */

struct arcogdesc *arcogdescLoadAll(char *fileName);
/* Load all arcogdesc from whitespace-separated file.
 * Dispose of this with arcogdescFreeList(). */

struct arcogdesc *arcogdescLoadAllByChar(char *fileName, char chopper);
/* Load all arcogdesc from chopper separated file.
 * Dispose of this with arcogdescFreeList(). */

#define arcogdescLoadAllByTab(a) arcogdescLoadAllByChar(a, '\t');
/* Load all arcogdesc from tab separated file.
 * Dispose of this with arcogdescFreeList(). */

struct arcogdesc *arcogdescCommaIn(char **pS, struct arcogdesc *ret);
/* Create a arcogdesc out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new arcogdesc */

void arcogdescFree(struct arcogdesc **pEl);
/* Free a single dynamically allocated arcogdesc such as created
 * with arcogdescLoad(). */

void arcogdescFreeList(struct arcogdesc **pList);
/* Free a list of dynamically allocated arcogdesc's */

void arcogdescOutput(struct arcogdesc *el, FILE *f, char sep, char lastSep);
/* Print out arcogdesc.  Separate fields with sep. Follow last field with lastSep. */

#define arcogdescTabOut(el,f) arcogdescOutput(el,f,'\t','\n');
/* Print out arcogdesc as a line in a tab-separated file. */

#define arcogdescCommaOut(el,f) arcogdescOutput(el,f,',',',');
/* Print out arcogdesc as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#endif /* ARCOGDESC_H */

