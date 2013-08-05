/* wgEncodeGencodeTranscriptSupport.h was originally generated by the autoSql program, which also 
 * generated wgEncodeGencodeTranscriptSupport.c and wgEncodeGencodeTranscriptSupport.sql.  This header links the database and
 * the RAM representation of objects. */

#ifndef WGENCODEGENCODETRANSCRIPTSUPPORT_H
#define WGENCODEGENCODETRANSCRIPTSUPPORT_H

#define WGENCODEGENCODETRANSCRIPTSUPPORT_NUM_COLS 3

struct wgEncodeGencodeTranscriptSupport
/* GENCODE transcript support from other datasets */
    {
    struct wgEncodeGencodeTranscriptSupport *next;  /* Next in singly linked list. */
    char *transcriptId;	/* GENCODE transcript identifier */
    char *seqId;	/* Identifier of sequence supporting transcript */
    char *seqSrc;	/* Source of supporting sequence */
    };

void wgEncodeGencodeTranscriptSupportStaticLoad(char **row, struct wgEncodeGencodeTranscriptSupport *ret);
/* Load a row from wgEncodeGencodeTranscriptSupport table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct wgEncodeGencodeTranscriptSupport *wgEncodeGencodeTranscriptSupportLoad(char **row);
/* Load a wgEncodeGencodeTranscriptSupport from row fetched with select * from wgEncodeGencodeTranscriptSupport
 * from database.  Dispose of this with wgEncodeGencodeTranscriptSupportFree(). */

struct wgEncodeGencodeTranscriptSupport *wgEncodeGencodeTranscriptSupportLoadAll(char *fileName);
/* Load all wgEncodeGencodeTranscriptSupport from whitespace-separated file.
 * Dispose of this with wgEncodeGencodeTranscriptSupportFreeList(). */

struct wgEncodeGencodeTranscriptSupport *wgEncodeGencodeTranscriptSupportLoadAllByChar(char *fileName, char chopper);
/* Load all wgEncodeGencodeTranscriptSupport from chopper separated file.
 * Dispose of this with wgEncodeGencodeTranscriptSupportFreeList(). */

#define wgEncodeGencodeTranscriptSupportLoadAllByTab(a) wgEncodeGencodeTranscriptSupportLoadAllByChar(a, '\t');
/* Load all wgEncodeGencodeTranscriptSupport from tab separated file.
 * Dispose of this with wgEncodeGencodeTranscriptSupportFreeList(). */

struct wgEncodeGencodeTranscriptSupport *wgEncodeGencodeTranscriptSupportCommaIn(char **pS, struct wgEncodeGencodeTranscriptSupport *ret);
/* Create a wgEncodeGencodeTranscriptSupport out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new wgEncodeGencodeTranscriptSupport */

void wgEncodeGencodeTranscriptSupportFree(struct wgEncodeGencodeTranscriptSupport **pEl);
/* Free a single dynamically allocated wgEncodeGencodeTranscriptSupport such as created
 * with wgEncodeGencodeTranscriptSupportLoad(). */

void wgEncodeGencodeTranscriptSupportFreeList(struct wgEncodeGencodeTranscriptSupport **pList);
/* Free a list of dynamically allocated wgEncodeGencodeTranscriptSupport's */

void wgEncodeGencodeTranscriptSupportOutput(struct wgEncodeGencodeTranscriptSupport *el, FILE *f, char sep, char lastSep);
/* Print out wgEncodeGencodeTranscriptSupport.  Separate fields with sep. Follow last field with lastSep. */

#define wgEncodeGencodeTranscriptSupportTabOut(el,f) wgEncodeGencodeTranscriptSupportOutput(el,f,'\t','\n');
/* Print out wgEncodeGencodeTranscriptSupport as a line in a tab-separated file. */

#define wgEncodeGencodeTranscriptSupportCommaOut(el,f) wgEncodeGencodeTranscriptSupportOutput(el,f,',',',');
/* Print out wgEncodeGencodeTranscriptSupport as a comma separated list including final comma. */

/* -------------------------------- End autoSql Generated Code -------------------------------- */

#endif /* WGENCODEGENCODETRANSCRIPTSUPPORT_H */

