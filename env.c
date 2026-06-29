/* env.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include <unistd.h>

#include "common.h"
#include "init.h"
#include "final.h"
#include "util.h"
#include "util2.h"
#include "env.h"
#include "env.ih"

bool
env_init (char *tcbuf, bool_int lax)
{
    bool fully_successful = TRUE;

    if ((homedir = getenv("HOME")) == NULL)
	homedir = getenv("LOGDIR");

    if ((tmpdir = getenv("TMPDIR")) == NULL)
	tmpdir = getval("TMP","/tmp");

    /* try to set loginName */
    if (lax) {
	loginName = getenv("USER");
	if (!loginName)
	    loginName = getenv("LOGNAME");
    }
    if (!lax || !loginName) {
	loginName = getlogin();
	if (loginName)
	    loginName = estrdup(loginName);
    }

    /* Set realName, and maybe set loginName and homedir (if NULL). */
    if (!setusername(tcbuf)) {
	if (!loginName)
	    loginName = nullstr;
	if (!realName)
	    realName = nullstr;
	fully_successful = FALSE;
    }
    env_init2();

    /* set phostname to the hostname of our local machine */
    if (!setphostname(tcbuf))
	fully_successful = FALSE;

    return fully_successful;
}

static void
env_init2 (void)
{
    if (dotdir)		/* Avoid running multiple times. */
	return;
    if (!homedir)
	homedir = "/";
    dotdir = getval("DOTDIR",homedir);
    trndir = estrdup(filexp(getval("TRNDIR",TRNDIR)));
    newslib = estrdup(filexp(NEWSLIB));
    rnlib = estrdup(filexp(PRIVLIB));
}

/* Set loginName to the user's login name and realName to the user's
** real name.
*/
bool
setusername (char *tmpbuf)
{
    char* s;
    char* c;

    struct passwd* pwd;

    if (loginName == NULL)
	pwd = getpwuid(getuid());
    else
	pwd = getpwnam(loginName);
    if (!pwd)
	return 0;
    if (!loginName)
	loginName = estrdup(pwd->pw_name);
    if (!homedir)
	homedir = estrdup(pwd->pw_dir);
    s = pwd->pw_gecos;
#ifdef PASSNAMES
#ifdef BERKNAMES
#ifdef BERKJUNK
    while (*s && !isalnum(*s) && *s != '&') s++;
#endif
    if ((c = index(s, ',')) != NULL)
	*c = '\0';
    if ((c = index(s, ';')) != NULL)
	*c = '\0';
    s = cpytill(buf,s,'&');
    if (*s == '&') {			/* whoever thought this one up was */
	c = buf + strlen(buf);		/* in the middle of the night */
	strcat(c,loginName);		/* before the morning after */
	strcat(c,s+1);
	if (islower(*c))
	    *c = toupper(*c);		/* gack and double gack */
    }
    realName = estrdup(buf);
#else /* !BERKNAMES */
    if ((c = index(s, '(')) != NULL)
	*c = '\0';
    if ((c = index(s, '-')) != NULL)
	s = c;
    realName = estrdup(s);
#endif /* !BERKNAMES */
#else /* !PASSNAMES */
    {
	FILE* fp;
	env_init2(); /* Make sure homedir/dotdir/etc. are set. */
	if ((fp = fopen(filexp(FULLNAMEFILE),"r")) != NULL) {
	    fgets(buf,sizeof buf,fp);
	    fclose(fp);
	    buf[strlen(buf)-1] = '\0';
	    realName = estrdup(buf);
	}
	else
	    s = "PUT_YOUR_NAME_HERE";
    }
#endif /* !PASSNAMES */
    endpwent();
    return 1;
}

bool
setphostname (char *tmpbuf)
{
    FILE* fp;
    bool hostname_ok = TRUE;

    /* Find the local hostname */

    gethostname(tmpbuf,TCBUF_SIZE);
    localhost = estrdup(tmpbuf);

    /* Build the host name that goes in postings */

    phostname = PHOSTNAME;
    if (FILE_REF(phostname) || *phostname == '~') {
	phostname = filexp(phostname);
	if ((fp = fopen(phostname,"r")) == NULL)
	    strcpy(tmpbuf,".");
	else {
	    fgets(tmpbuf,TCBUF_SIZE,fp);
	    fclose(fp);
	    phostname = tmpbuf + strlen(tmpbuf) - 1;
	    if (*phostname == '\n')
		*phostname = '\0';
	}
    }
    else
	strcpy(tmpbuf,phostname);

    if (*tmpbuf == '.') {
	if (tmpbuf[1] != '\0')
	    strcpy(buf,tmpbuf);
	else
	    *buf = '\0';
	strcpy(tmpbuf,localhost);
	strcat(tmpbuf,buf);
    }

    if (!index(tmpbuf,'.')) {
	if (*tmpbuf)
	    strcat(tmpbuf, ".");
	{
	    strcat(tmpbuf,"UNKNOWN.HOST");
	    hostname_ok = FALSE;
	}
    }
    phostname = estrdup(tmpbuf);
    return hostname_ok;
}

char *
getval (char *nam, char *def)
{
    char* val;

    if ((val = getenv(nam)) == NULL || !*val)
	return def;
    return val;
}

static bool firstexport = TRUE;
extern char** environ;

const char *
export(const char *name, const char *value)
{
    setenv(name, value, 1);
    return name;
}

void
unexport(const char *name)
{
    unsetenv(name);
}
