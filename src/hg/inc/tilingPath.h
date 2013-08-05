/* tilingPath.h was originally generated by the autoSql program, which also 
 * generated tilingPath.c and tilingPath.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef TILINGPATH_H
#define TILINGPATH_H

struct tilingPath
/* A tiling path of clones through a chromosome */
    {
    struct tilingPath *next;  /* Next in singly linked list. */
    char *chrom;	/* Chromosome name: chr1, chr2, etc. */
    char *accession;	/* Clone accession or ? or GAP */
    char *clone;	/* Clone name in BAC library */
    char *contig;	/* Contig (or gap size) */
    int chromIx;	/* Number of clone in tiling path starting chrom start */
    };

void tilingPathStaticLoad(char **row, struct tilingPath *ret);
/* Load a row from tilingPath table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct tilingPath *tilingPathLoad(char **row);
/* Load a tilingPath from row fetched with select * from tilingPath
 * from database.  Dispose of this with tilingPathFree(). */

struct tilingPath *tilingPathLoadAll(char *fileName);
/* Load all tilingPath from a tab-separated file.
 * Dispose of this with tilingPathFreeList(). */

struct tilingPath *tilingPathLoadWhere(struct sqlConnection *conn, char *table, char *where);
/* Load all tilingPath from table that satisfy where clause. The
 * where clause may be NULL in which case whole table is loaded
 * Dispose of this with tilingPathFreeList(). */

struct tilingPath *tilingPathCommaIn(char **pS, struct tilingPath *ret);
/* Create a tilingPath out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new tilingPath */

void tilingPathFree(struct tilingPath **pEl);
/* Free a single dynamically allocated tilingPath such as created
 * with tilingPathLoad(). */

void tilingPathFreeList(struct tilingPath **pList);
/* Free a list of dynamically allocated tilingPath's */

void tilingPathOutput(struct tilingPath *el, FILE *f, char sep, char lastSep);
/* Print out tilingPath.  Separate fields with sep. Follow last field with lastSep. */

#define tilingPathTabOut(el,f) tilingPathOutput(el,f,'\t','\n');
/* Print out tilingPath as a line in a tab-separated file. */

#define tilingPathCommaOut(el,f) tilingPathOutput(el,f,',',',');
/* Print out tilingPath as a comma separated list including final comma. */

#endif /* TILINGPATH_H */

