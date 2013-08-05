/* metaChromGraph.h was originally generated by the autoSql program, which also 
 * generated metaChromGraph.c and metaChromGraph.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef METACHROMGRAPH_H
#define METACHROMGRAPH_H

#define METACHROMGRAPH_NUM_COLS 4

struct metaChromGraph
/* Information about each chromosome graph. */
    {
    struct metaChromGraph *next;  /* Next in singly linked list. */
    char *name;	/* Corresponds to chrom graph table name */
    double minVal;	/* Minimum value observed */
    double maxVal;	/* Maximum value observed */
    char *binaryFile;	/* Location of binary data point file if any */
    };

void metaChromGraphStaticLoad(char **row, struct metaChromGraph *ret);
/* Load a row from metaChromGraph table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct metaChromGraph *metaChromGraphLoad(char **row);
/* Load a metaChromGraph from row fetched with select * from metaChromGraph
 * from database.  Dispose of this with metaChromGraphFree(). */

struct metaChromGraph *metaChromGraphLoadAll(char *fileName);
/* Load all metaChromGraph from whitespace-separated file.
 * Dispose of this with metaChromGraphFreeList(). */

struct metaChromGraph *metaChromGraphLoadAllByChar(char *fileName, char chopper);
/* Load all metaChromGraph from chopper separated file.
 * Dispose of this with metaChromGraphFreeList(). */

#define metaChromGraphLoadAllByTab(a) metaChromGraphLoadAllByChar(a, '\t');
/* Load all metaChromGraph from tab separated file.
 * Dispose of this with metaChromGraphFreeList(). */

struct metaChromGraph *metaChromGraphCommaIn(char **pS, struct metaChromGraph *ret);
/* Create a metaChromGraph out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new metaChromGraph */

void metaChromGraphFree(struct metaChromGraph **pEl);
/* Free a single dynamically allocated metaChromGraph such as created
 * with metaChromGraphLoad(). */

void metaChromGraphFreeList(struct metaChromGraph **pList);
/* Free a list of dynamically allocated metaChromGraph's */

void metaChromGraphOutput(struct metaChromGraph *el, FILE *f, char sep, char lastSep);
/* Print out metaChromGraph.  Separate fields with sep. Follow last field with lastSep. */

#define metaChromGraphTabOut(el,f) metaChromGraphOutput(el,f,'\t','\n');
/* Print out metaChromGraph as a line in a tab-separated file. */

#define metaChromGraphCommaOut(el,f) metaChromGraphOutput(el,f,',',',');
/* Print out metaChromGraph as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#endif /* METACHROMGRAPH_H */

