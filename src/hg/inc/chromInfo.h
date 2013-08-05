/* chromInfo.h was originally generated by the autoSql program, which also 
 * generated chromInfo.c and chromInfo.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef CHROMINFO_H
#define CHROMINFO_H

#define CHROMINFO_NUM_COLS 3

struct chromInfo
/* Chromosome names and sizes */
    {
    struct chromInfo *next;  /* Next in singly linked list. */
    char *chrom;	/* Reference sequence chromosome or scaffold */
    unsigned size;	/* Chromosome size */
    char *fileName;	/* Chromosome file (raw one byte per base) */
    };

void chromInfoStaticLoad(char **row, struct chromInfo *ret);
/* Load a row from chromInfo table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct chromInfo *chromInfoLoad(char **row);
/* Load a chromInfo from row fetched with select * from chromInfo
 * from database.  Dispose of this with chromInfoFree(). */

struct chromInfo *chromInfoLoadAll(char *fileName);
/* Load all chromInfo from whitespace-separated file.
 * Dispose of this with chromInfoFreeList(). */

struct chromInfo *chromInfoLoadAllByChar(char *fileName, char chopper);
/* Load all chromInfo from chopper separated file.
 * Dispose of this with chromInfoFreeList(). */

#define chromInfoLoadAllByTab(a) chromInfoLoadAllByChar(a, '\t');
/* Load all chromInfo from tab separated file.
 * Dispose of this with chromInfoFreeList(). */

struct chromInfo *chromInfoCommaIn(char **pS, struct chromInfo *ret);
/* Create a chromInfo out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new chromInfo */

void chromInfoFree(struct chromInfo **pEl);
/* Free a single dynamically allocated chromInfo such as created
 * with chromInfoLoad(). */

void chromInfoFreeList(struct chromInfo **pList);
/* Free a list of dynamically allocated chromInfo's */

void chromInfoOutput(struct chromInfo *el, FILE *f, char sep, char lastSep);
/* Print out chromInfo.  Separate fields with sep. Follow last field with lastSep. */

#define chromInfoTabOut(el,f) chromInfoOutput(el,f,'\t','\n');
/* Print out chromInfo as a line in a tab-separated file. */

#define chromInfoCommaOut(el,f) chromInfoOutput(el,f,',',',');
/* Print out chromInfo as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

boolean chromSeqFileExists(char *db, char *chrom);
/* Check whether chromInfo exists for a database, find the path of the */
/* sequence file for this chromosome and check if the file exists. */
#endif /* CHROMINFO_H */

struct chromInfo *createChromInfoList(char *name, char *database);
/* Load up chromosome information for chrom 'name'.
 * If name is NULL or "all" then load all chroms.
 * Similar to featureBits.c - could be moved to library */

struct hash *chromHashFromDatabase(char *db);
/* read chrom info from database and return hash of name and size */

struct hash *chromHashFromFile(char *fileName);
/* read chrom info from file and return hash of name and size */
