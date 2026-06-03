/* util2.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include "EXTERN.h"
#include "common.h"
#include "list.h"
#include "hash.h"
#include "ngdata.h"
#include "nntpclient.h"
#include "datasrc.h"
#include "nntp.h"
#include "nntpauth.h"
#include "util.h"
#include "util3.h"
#include "INTERN.h"
#include "util2.h"

#ifdef TILDENAME
static char* tildename = NULL;
static char* tildedir = NULL;
#endif

/* copy a string to a safe spot */

char *
savestr (char *str)
{
    register char* newaddr = safemalloc((MEM_SIZE)(strlen(str)+1));

    strcpy(newaddr,str);
    return newaddr;
}

/* safe version of string copy */

char *
safecpy (char *to, register char *from, register int len)
{
    register char* dest = to;

    if (from) {
	while (--len && *from)
	    *dest++ = *from++;
    }
    *dest = '\0';

    return to;
}

/* copy a string up to some (non-backslashed) delimiter, if any */

char *
cpytill (register char *to, register char *from, register int delim)
{
    while (*from) {
	if (*from == '\\' && from[1] == delim)
	    from++;
	else if (*from == delim)
	    break;
	*to++ = *from++;
    }
    *to = '\0';
    return from;
}

/* expand filename via %, ~, and $ interpretation */
/* returns pointer to static area */
/* Note that there is a 1-deep cache of ~name interpretation */

char *
filexp (register char *s)
{
    static char filename[CBUFLEN];
    char scrbuf[CBUFLEN];
    register char* d;

#ifdef DEBUG
    if (debug & DEB_FILEXP)
	printf("< %s\n",s) FLUSH;
#endif
    /* interpret any % escapes */
    dointerp(filename,sizeof filename,s,(char*)NULL,(char*)NULL);
#ifdef DEBUG
    if (debug & DEB_FILEXP)
	printf("%% %s\n",filename) FLUSH;
#endif
    s = filename;
    if (*s == '~') {	/* does destination start with ~? */
	if (!*(++s) || *s == '/') {
	    sprintf(scrbuf,"%s%s",homedir,s);
				/* swap $HOME for it */
#ifdef DEBUG
	    if (debug & DEB_FILEXP)
		printf("~ %s\n",scrbuf) FLUSH;
#endif
	    strcpy(filename,scrbuf);
	}
	else if (*s == '~' && (!s[1] || s[1] == '/')) {
	    d = getenv("TRNPREFIX");
	    if (!d)
		d = INSTALLPREFIX;
	    sprintf(scrbuf,"%s%s",d,s+1);
#ifdef DEBUG
	    if (debug & DEB_FILEXP)
		printf("~~ %s\n",scrbuf) FLUSH;
#endif
	}
	else {
#ifdef TILDENAME
	    for (d = scrbuf; isalnum(*s); s++, d++) *d = *s;
	    *d = '\0';
	    if (tildedir && strEQ(tildename,scrbuf)) {
		strcpy(scrbuf,tildedir);
		strcat(scrbuf, s);
		strcpy(filename, scrbuf);
#ifdef DEBUG
		if (debug & DEB_FILEXP)
		    printf("r %s %s\n",tildename,tildedir) FLUSH;
#endif
	    }
	    else {
		if (tildename)
		    free(tildename);
		if (tildedir)
		    free(tildedir);
		tildedir = NULL;
		tildename = savestr(scrbuf);
		{
		    struct passwd* pwd = getpwnam(tildename);
		    if (pwd == NULL) {
			printf("%s is an unknown user. Using default.\n",tildename) FLUSH;
			return NULL;
		    }
		    sprintf(scrbuf,"%s%s",pwd->pw_dir,s);
		    tildedir = savestr(pwd->pw_dir);
		    strcpy(filename,scrbuf);
		    endpwent();
		}
	    }
#else /* !TILDENAME */
#ifdef VERBOSE
	    IF(verbose)
		fputs("~loginname not implemented.\n",stdout) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		fputs("~login not impl.\n",stdout) FLUSH;
#endif
#endif
	}
    }
    else if (*s == '$') {	/* starts with some env variable? */
	d = scrbuf;
	*d++ = '%';
	if (s[1] == '{')
	    strcpy(d,s+2);
	else {
	    *d++ = '{';
	    for (s++; isalnum(*s); s++) *d++ = *s;
				/* skip over token */
	    *d++ = '}';
	    strcpy(d,s);
	}
#ifdef DEBUG
	if (debug & DEB_FILEXP)
	    printf("$ %s\n",scrbuf) FLUSH;
#endif
	/* this might do some extra '%'s, but that's how the Mercedes Benz */
	dointerp(filename,sizeof filename,scrbuf,(char*)NULL,(char*)NULL);
    }
#ifdef DEBUG
    if (debug & DEB_FILEXP)
	printf("> %s\n",filename) FLUSH;
#endif
    return filename;
}

/* return ptr to little string in big string, NULL if not found */

char *
in_str (char *big, char *little, bool_int case_matters)
{
    register char* t;
    register char* s;
    register char* x;

    for (t = big; *t; t++) {
	for (x=t,s=little; *s; x++,s++) {
	    if (!*x)
		return NULL;
	    if (case_matters == TRUE) {
		if (*s != *x)
		    break;
	    } else {
		register char c,d;
		if (isupper(*s))
		    c = tolower(*s);
		else
		    c = *s;
		if (isupper(*x))
		    d = tolower(*x);
		else
		    d = *x;
		if ( c != d )
		    break;
	   }
	}
	if (!*s)
	    return t;
    }
    return NULL;
}




#ifdef SUPPORT_NNTP
char *
read_auth_file (char *file, char **pass_ptr)
{
    FILE* fp;
    char* strptr[2];
    char buf[1024];
    strptr[0] = strptr[1] = NULL;
    if ((fp = fopen(file,"r")) != NULL) {
	int i;
	for (i = 0; i < 2; i++) {
	    if (fgets(buf, sizeof buf, fp) != NULL) {
		char* cp = buf + strlen(buf) - 1;
		if (*cp == '\n')
		    *cp = '\0';
		strptr[i] = savestr(buf);
	    }
	}
	fclose(fp);
    }
    *pass_ptr = strptr[1];
    return strptr[0];
}
#endif


