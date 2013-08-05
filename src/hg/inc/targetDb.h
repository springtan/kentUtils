/* targetDb.h was originally generated by the autoSql program, which also 
 * generated targetDb.c and targetDb.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef TARGETDB_H
#define TARGETDB_H

#define TARGETDB_NUM_COLS 10

struct targetDb
/* Description of non-genomic target sequences (e.g. native mRNAs for PCR) */
    {
    struct targetDb *next;  /* Next in singly linked list. */
    char *name;	/* Identifier for this target */
    char *description;	/* Brief description for select box */
    char *db;	/* Database to which target has been mapped */
    char *pslTable;	/* PSL table in db that maps target coords to db coords */
    char *seqTable;	/* Table in db that has extFileTable indices of target sequences */
    char *extFileTable;	/* Table in db that has .id, .path, and .size of target sequence files */
    char *seqFile;	/* Target sequence file path (typically /gbdb/$db/targetDb/$name.2bit) */
    float priority;	/* Relative priority compared to other targets for same db (smaller numbers are higher priority) */
    char *time;	/* Time at which this record was updated -- should be newer than db tables (so should blat server) */
    char *settings;	/* .ra-formatted metadata */
    struct hash *settingsHash;  /* Hash for settings. Not saved in database.
                                 * Don't use directly, rely on targetDbSetting to access. */
    };

void targetDbStaticLoad(char **row, struct targetDb *ret);
/* Load a row from targetDb table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct targetDb *targetDbLoad(char **row);
/* Load a targetDb from row fetched with select * from targetDb
 * from database.  Dispose of this with targetDbFree(). */

struct targetDb *targetDbLoadAll(char *fileName);
/* Load all targetDb from whitespace-separated file.
 * Dispose of this with targetDbFreeList(). */

struct targetDb *targetDbLoadAllByChar(char *fileName, char chopper);
/* Load all targetDb from chopper separated file.
 * Dispose of this with targetDbFreeList(). */

#define targetDbLoadAllByTab(a) targetDbLoadAllByChar(a, '\t');
/* Load all targetDb from tab separated file.
 * Dispose of this with targetDbFreeList(). */

struct targetDb *targetDbCommaIn(char **pS, struct targetDb *ret);
/* Create a targetDb out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new targetDb */

void targetDbFree(struct targetDb **pEl);
/* Free a single dynamically allocated targetDb such as created
 * with targetDbLoad(). */

void targetDbFreeList(struct targetDb **pList);
/* Free a list of dynamically allocated targetDb's */

void targetDbOutput(struct targetDb *el, FILE *f, char sep, char lastSep);
/* Print out targetDb.  Separate fields with sep. Follow last field with lastSep. */

#define targetDbTabOut(el,f) targetDbOutput(el,f,'\t','\n');
/* Print out targetDb as a line in a tab-separated file. */

#define targetDbCommaOut(el,f) targetDbOutput(el,f,',',',');
/* Print out targetDb as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#include "jksql.h"

struct targetDb *targetDbMaybeLoad(struct sqlConnection *conn, char **row);
/* If row specifies a target whose tables and file exist, and are not newer
 * than target, allocate and return a targetDb; otherwise, return NULL
 * and log a warning to stderr for QA monitoring. */

struct targetDb *targetDbLookup(char *db, char *name);
/* Given the name of a genomic database and the name of a PCR target
 * (or NULL to get all available PCR targets for db), query the
 * central database targetDb table and load the results.  Remove 
 * entries that are out of sync or have missing tables. */

char *targetDbSetting(struct targetDb *tdb, char *name);
/* Return setting string or NULL if none exists. */

#endif /* TARGETDB_H */

