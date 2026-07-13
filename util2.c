/* util2.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


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
#include "util2.h"

#ifdef TILDENAME
static char* tildename = NULL;
static char* tildedir = NULL;
#endif

/* safe version of string copy */
char *
safecpy(char *to, const char *from, size_t len)
{
    size_t n = strlcpy(to, from, len);
    assert(n < len);
    (void)n;
    return to;
}

/* copy a string up to some (non-backslashed) delimiter, if any */
char *
cpytill(char *to, char *from, int delim)
{
    while (*from) {
        if (*from == '\\' && from[1] == delim)
            from++;
        else if (*from == delim)
            break;
        *to++ = *from++;
    }
    *to = '\0';
    return (char *)from;
}

/* expand filename via %, ~, and $ interpretation */
/* returns pointer to static area */
/* Note that there is a 1-deep cache of ~name interpretation */
char *
filexp(char *s)
{
    static char filename[CBUFLEN];
    char scrbuf[CBUFLEN];
    char* d;

    if (debug & DEB_FILEXP)
        printf("< %s\n",s);
    /* interpret any % escapes */
    dointerp(filename,sizeof filename,s,(char*)NULL,(char*)NULL);
    if (debug & DEB_FILEXP)
        printf("%% %s\n",filename);
    s = filename;
    if (*s == '~') {    /* does destination start with ~? */
        if (!*(++s) || *s == '/') {
            snprintf(scrbuf,sizeof(scrbuf),"%s%s",homedir,s);
                                /* swap $HOME for it */
            if (debug & DEB_FILEXP)
                printf("~ %s\n",scrbuf);
            strlcpy(filename,scrbuf,sizeof(filename));
        }
        else if (*s == '~' && (!s[1] || s[1] == '/')) {
            d = getenv("TRNPREFIX");
            if (!d)
                d = INSTALLPREFIX;
            snprintf(scrbuf,sizeof(scrbuf),"%s%s",d,s+1);
            if (debug & DEB_FILEXP)
                printf("~~ %s\n",scrbuf);
        }
        else {
#ifdef TILDENAME
            d = scrbuf;
            size_t dsize = sizeof(scrbuf) - 1;
            for (size_t k = 0; isalnum(*s) && k < dsize; k++)
                *d = *s;
            *d = '\0';
            if (tildedir && strEQ(tildename, scrbuf)) {
                strlcpy(scrbuf, tildedir, sizeof(scrbuf));
                strlcat(scrbuf, s, sizeof(scrbuf));
                strlcpy(filename, scrbuf, sizeof(filename));
                if (debug & DEB_FILEXP)
                    printf("r %s %s\n",tildename,tildedir);
            }
            else {
                if (tildename)
                    safefree(tildename);
                if (tildedir)
                    safefree(tildedir);
                tildedir = NULL;
                tildename = estrdup(scrbuf);
                {
                    struct passwd* pwd = getpwnam(tildename);
                    if (pwd == NULL) {
                        printf("%s is an unknown user. Using default.\n",tildename);
                        return NULL;
                    }
                    sprintf(scrbuf,"%s%s",pwd->pw_dir,s);
                    tildedir = estrdup(pwd->pw_dir);
                    strcpy(filename,scrbuf);
                    endpwent();
                }
            }
#else /* !TILDENAME */
#ifdef VERBOSE
            IF(verbose)
                fputs("~loginname not implemented.\n",stdout);
            ELSE
#endif
#ifdef TERSE
                fputs("~login not impl.\n",stdout);
#endif
#endif
        }
    }
    else if (*s == '$') {       /* starts with some env variable? */
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
        if (debug & DEB_FILEXP)
            printf("$ %s\n",scrbuf);
        /* this might do some extra '%'s, but that's how the Mercedes Benz */
        dointerp(filename,sizeof filename,scrbuf,(char*)NULL,(char*)NULL);
    }
    if (debug & DEB_FILEXP)
        printf("> %s\n",filename);
    return filename;
}

/* return ptr to little string in big string, NULL if not found */

char *
in_str(const char *big, const char *little, bool case_matters)
{
    return case_matters ? strstr(big, little) : strcasestr(big, little);
}

char *
read_auth_file(const char *file, char **pass_ptr)
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
                strptr[i] = estrdup(buf);
            }
        }
        fclose(fp);
    }
    *pass_ptr = strptr[1];
    return strptr[0];
}
