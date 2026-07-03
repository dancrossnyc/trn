/* util.h
 * vi: set sw=4 ts=8 ai sm noet :
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include <stdbool.h>
#include <stdint.h>

#include "utf.h"

extern bool waiting;    /* waiting for subprocess (in doshell)? */
extern bool nowait_fork;
extern bool export_nntp_fds;

/* the strlen and the buffer length of "some_buf" after a call to:
 *     some_buf = get_a_line(bufptr,bufsize,realloc,fp); */
extern size_t len_last_line_got;
extern size_t buflen_last_line_got;

static inline bool
AT_GREY_SPACE(const char *s) {
    return s != NULL && (!at_norm_char(s) || (*s != '\0' && *s == ' '));
}

static inline bool
AT_NORM_CHAR(const char *s) {
    return at_norm_char(s);
}

/* is the string for makedir a directory name or a filename? */
#define MD_DIR  0
#define MD_FILE 1

/* a template for parsing an ini file */
struct ini_words {
    unsigned int checksum;
    char* item;
    char* help_str;
};

#define INI_LEN(words)         (words)[0].checksum
#define INI_VALUES(words)      ((char**)(words)[0].help_str)
#define INI_VALUE(words,num)   INI_VALUES(words)[num]

#define safefree0(ptr)  ptr = safefree(ptr)

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void util_init (void);
int doshell (char*, char*);
void* safemalloc(size_t);
void* saferealloc(void*, size_t);
void *safefree(void*);
char *estrdup(const char *);
char* safecat (char*, const char*, size_t);
#ifdef SETUIDGID
int eaccess (char*,int);
#endif
char* trn_getwd (char*, size_t);
char* get_a_line(char*, size_t, bool, FILE*);
int makedir (char*, int);
void notincl (char*);
void growstr (char**,int*,int);
void setdef (char*,char*);
#ifndef NO_FILELINKS
void safelink (char*,char*);
#endif
void verify_sig (void);
uint64_t current_time_ms (void);
time_t text2secs (char*,time_t);
char* secs2text (time_t);
char* temp_filename (void);
char* get_auth_user (void);
char* get_auth_pass (void);
#if defined(USE_GENAUTH) && defined(SUPPORT_NNTP)
char* get_auth_command (void);
#endif
char** prep_ini_words (INI_WORDS*);
void unprep_ini_words (INI_WORDS*);
void prep_ini_data (char*,char*);
bool parse_string (char**,char**);
char* next_ini_section (char*,char**,char**);
char* parse_ini_section (char*,INI_WORDS*);
bool check_ini_cond (char*);
char menu_get_char (void);
int edit_file (char*);
