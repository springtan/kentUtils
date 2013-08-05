/* ensPhusionBlast.h was originally generated by the autoSql program, which also 
 * generated ensPhusionBlast.c and ensPhusionBlast.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef ENSPHUSIONBLAST_H
#define ENSPHUSIONBLAST_H

struct ensPhusionBlast
/* Ensembl PhusionBlast Synteny (bed 6 +). */
    {
    struct ensPhusionBlast *next;  /* Next in singly linked list. */
    char *chrom;	/* $Organism chromosome */
    unsigned chromStart;	/* Start position in $Organism chromosome */
    unsigned chromEnd;	/* End position in $Organism chromosome */
    char *name;	/* Name of $Xeno chromosome */
    unsigned score;	/* Integer alignment score */
    char strand[2];	/* Direction of alignment: + or - */
    unsigned xenoStart;	/* Start position in $Xeno chromosome */
    unsigned xenoEnd;	/* End position in $Xeno chromosome */
    };

void ensPhusionBlastStaticLoad(char **row, struct ensPhusionBlast *ret);
/* Load a row from ensPhusionBlast table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct ensPhusionBlast *ensPhusionBlastLoad(char **row);
/* Load a ensPhusionBlast from row fetched with select * from ensPhusionBlast
 * from database.  Dispose of this with ensPhusionBlastFree(). */

struct ensPhusionBlast *ensPhusionBlastLoadAll(char *fileName);
/* Load all ensPhusionBlast from a tab-separated file.
 * Dispose of this with ensPhusionBlastFreeList(). */

struct ensPhusionBlast *ensPhusionBlastLoadWhere(struct sqlConnection *conn, char *table, char *where);
/* Load all ensPhusionBlast from table that satisfy where clause. The
 * where clause may be NULL in which case whole table is loaded
 * Dispose of this with ensPhusionBlastFreeList(). */

struct ensPhusionBlast *ensPhusionBlastCommaIn(char **pS, struct ensPhusionBlast *ret);
/* Create a ensPhusionBlast out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new ensPhusionBlast */

void ensPhusionBlastFree(struct ensPhusionBlast **pEl);
/* Free a single dynamically allocated ensPhusionBlast such as created
 * with ensPhusionBlastLoad(). */

void ensPhusionBlastFreeList(struct ensPhusionBlast **pList);
/* Free a list of dynamically allocated ensPhusionBlast's */

void ensPhusionBlastOutput(struct ensPhusionBlast *el, FILE *f, char sep, char lastSep);
/* Print out ensPhusionBlast.  Separate fields with sep. Follow last field with lastSep. */

#define ensPhusionBlastTabOut(el,f) ensPhusionBlastOutput(el,f,'\t','\n');
/* Print out ensPhusionBlast as a line in a tab-separated file. */

#define ensPhusionBlastCommaOut(el,f) ensPhusionBlastOutput(el,f,',',',');
/* Print out ensPhusionBlast as a comma separated list including final comma. */

#endif /* ENSPHUSIONBLAST_H */

