/* exonJunctionPrimers.h was originally generated by the autoSql program, which also 
 * generated exonJunctionPrimers.c and exonJunctionPrimers.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef EXONJUNCTIONPRIMERS_H
#define EXONJUNCTIONPRIMERS_H

#define EXONJUNCTIONPRIMERS_NUM_COLS 14

struct exonJunctionPrimers
/* Browser extensible data, with extended fields for detail page */
    {
    struct exonJunctionPrimers *next;  /* Next in singly linked list. */
    char *chrom;	/* Reference sequence chromosome or scaffold */
    unsigned chromStart;	/* Start position in chromosome */
    unsigned chromEnd;	/* End position in chromosome */
    char *name;	/* Short Name of item */
    unsigned score;	/* Score from 0-1000 */
    char strand[2];	/* + or - */
    unsigned thickStart;	/* Start of where display should be thick (start codon) */
    unsigned thickEnd;	/* End of where display should be thick (stop codon) */
    unsigned reserved;	/* Used as itemRgb as of 2004-11-22 */
    int blockCount;	/* Number of blocks */
    int *blockSizes;	/* Comma separated list of block sizes */
    int *chromStarts;	/* Start positions relative to chromStart */
    char *id;	/* ID to bed used in URL to link back */
    char *description;	/* Long description of item for the details page */
    };

struct exonJunctionPrimers *exonJunctionPrimersLoad(char **row);
/* Load a exonJunctionPrimers from row fetched with select * from exonJunctionPrimers
 * from database.  Dispose of this with exonJunctionPrimersFree(). */

struct exonJunctionPrimers *exonJunctionPrimersLoadAll(char *fileName);
/* Load all exonJunctionPrimers from whitespace-separated file.
 * Dispose of this with exonJunctionPrimersFreeList(). */

struct exonJunctionPrimers *exonJunctionPrimersLoadAllByChar(char *fileName, char chopper);
/* Load all exonJunctionPrimers from chopper separated file.
 * Dispose of this with exonJunctionPrimersFreeList(). */

#define exonJunctionPrimersLoadAllByTab(a) exonJunctionPrimersLoadAllByChar(a, '\t');
/* Load all exonJunctionPrimers from tab separated file.
 * Dispose of this with exonJunctionPrimersFreeList(). */

struct exonJunctionPrimers *exonJunctionPrimersCommaIn(char **pS, struct exonJunctionPrimers *ret);
/* Create a exonJunctionPrimers out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new exonJunctionPrimers */

void exonJunctionPrimersFree(struct exonJunctionPrimers **pEl);
/* Free a single dynamically allocated exonJunctionPrimers such as created
 * with exonJunctionPrimersLoad(). */

void exonJunctionPrimersFreeList(struct exonJunctionPrimers **pList);
/* Free a list of dynamically allocated exonJunctionPrimers's */

void exonJunctionPrimersOutput(struct exonJunctionPrimers *el, FILE *f, char sep, char lastSep);
/* Print out exonJunctionPrimers.  Separate fields with sep. Follow last field with lastSep. */

#define exonJunctionPrimersTabOut(el,f) exonJunctionPrimersOutput(el,f,'\t','\n');
/* Print out exonJunctionPrimers as a line in a tab-separated file. */

#define exonJunctionPrimersCommaOut(el,f) exonJunctionPrimersOutput(el,f,',',',');
/* Print out exonJunctionPrimers as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#endif /* EXONJUNCTIONPRIMERS_H */

