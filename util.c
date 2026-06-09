/* util.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include <sys/wait.h>
#include <time.h>

#include "EXTERN.h"
#include "common.h"
#include "final.h"
#include "term.h"
#include "list.h"
#include "hash.h"
#include "ngdata.h"
#include "nntpclient.h"
#include "datasrc.h"
#include "nntp.h"
#include "nntpauth.h"
#include "intrp.h"
#include "env.h"
#include "util2.h"
#include "only.h"
#include "search.h"
#ifdef SCAN
#include "scan.h"
#include "smisc.h"	/* s_default_cmd */
#endif
#include "univ.h"
#include "INTERN.h"
#include "util.h"

bool waiting = false;  	/* waiting for subprocess (in doshell)? */
bool nowait_fork = false;
bool export_nntp_fds = false;
size_t len_last_line_got = 0;
size_t buflen_last_line_got = 0;

static const char* newsactive_export = "NEWSACTIVE";
static const char* grpdesc_export = "NEWSDESCRIPTIONS";
static const char* quotechars_export = "QUOTECHARS";
static const char* nntpserver_export = "NNTPSERVER";
static const char* nntpfds_export = "NNTPFDS";
#ifdef USE_GENAUTH
static const char* nntpauth_export = "NNTP_AUTH_FDS";
#endif
static const char* nntpforce_export = "NNTP_FORCE_AUTH";

void
util_init (void)
{
    extern char patchlevel[];
    char* cp;
    int i;
    for (i = 0, cp = buf; i < 512; i++)
	*cp++ = 'X';
    *cp = '\0';
    export("NEWSACTIVE", buf);
    grpdesc_export = export("NEWSDESCRIPTIONS", buf);
    nntpserver_export = export("NNTPSERVER", buf);
    buf[64] = '\0';
    quotechars_export = export("QUOTECHARS",buf);
    nntpfds_export = export("NNTPFDS", buf);
#ifdef USE_GENAUTH
    nntpauth_export = export("NNTP_AUTH_FDS", buf);
#endif
    buf[3] = '\0';
    nntpforce_export = export("NNTP_FORCE_AUTH", buf);

    for (cp = patchlevel; isspace(*cp); cp++) ;
    export("TRN_VERSION", cp);
}

/* fork and exec a shell command */
int
doshell (char *shell, char *s)
{
    int status;
    pid_t pid, w;
    int ret;

    xmouse_off();

    sigset(SIGTSTP,SIG_DFL);
    sigset(SIGTTOU,SIG_DFL);
    sigset(SIGTTIN,SIG_DFL);

    if (datasrc && (datasrc->flags & DF_REMOTE)) {
#ifdef USE_GENAUTH
	if (export_nntp_fds) {
	    if (!nntplink.rd_fp) {
		if (nntp_command("DATE") <= 0 || nntp_check() < 0)
		    finalize(1); /*$$*/
	    }
	    sprintf(buf,"%d.%d.%d",(int)fileno(nntplink.rd_fp),
		    (int)fileno(nntplink.wr_fp),nntplink.cookiefd);
	    export(nntpauth_export, buf);
	}
	else
	    unexport(nntpauth_export);
#endif
	if (!export_nntp_fds || !nntplink.rd_fp)
	    unexport(nntpfds_export);
	else {
	    sprintf(buf,"%d.%d",(int)fileno(nntplink.rd_fp),
				(int)fileno(nntplink.wr_fp));
	    export(nntpfds_export, buf);
	}
	export(nntpserver_export, datasrc->newsid);
	if (datasrc->nntplink.flags & NNTP_FORCE_AUTH_NEEDED)
	    export(nntpforce_export,"yes");
	else
	    unexport(nntpforce_export);
	if (datasrc->auth_user) {
	    int fd;
	    if ((fd = open(nntp_auth_file, O_WRONLY|O_CREAT, 0600)) >= 0) {
		write(fd, datasrc->auth_user, strlen(datasrc->auth_user));
		write(fd, "\n", 1);
		if (datasrc->auth_pass) {
		    write(fd, datasrc->auth_pass, strlen(datasrc->auth_pass));
		    write(fd, "\n", 1);
		}
		close(fd);
	    }
	}
	if (nntplink.port_number) {
	    char tbuf[8192];
	    snprintf(tbuf, sizeof(tbuf), "%s;%d",
	        getenv(nntpserver_export), nntplink.port_number);
	    export(nntpserver_export, tbuf);
	}
	if (datasrc->act_sf.fp)
	    export(newsactive_export, datasrc->extra_name);
	else
	    export(newsactive_export, "none");
    } else {
	unexport(nntpfds_export);
#ifdef USE_GENAUTH
	unexport(nntpauth_export);
#endif
	unexport(nntpserver_export);
	unexport(nntpforce_export);
	if (datasrc)
	    export(newsactive_export, datasrc->newsid);
	else
	    unexport(newsactive_export);
    }
    if (datasrc)
	export(grpdesc_export, datasrc->grpdesc);
    else
	unexport(grpdesc_export);
    interp(buf,64-1+2,"%I");
    buf[strlen(buf)-1] = '\0';
    export(quotechars_export, buf+1);
    if (shell == NULL && (shell = getval("SHELL",NULL)) == NULL)
	shell = PREFSHELL;
    termlib_reset();
    if ((pid = fork()) == 0) {
	if (datasrc && (datasrc->flags & DF_REMOTE)) {
	    int i;
	    /* This is necessary to keep the bourne shell from puking */
	    for (i = 3; i < 10; ++i) {
		if (nntplink.rd_fp
		 && (i == fileno(nntplink.rd_fp)
		  || i == fileno(nntplink.wr_fp)))
		    continue;
#ifdef USE_GENAUTH
		if (i == nntplink.cookiefd)
		    continue;
#endif
		close(i);
	    }
	}
	if (nowait_fork) {
	    close(1);
	    close(2);
	    dup(open("/dev/null",1));
	}

	if (*s)
	    execl(shell, shell, "-c", s, (char*)NULL);
	else
	    execl(shell, shell, (char*)NULL, (char*)NULL, (char*)NULL);
	_exit(127);
    }
    sigignore(SIGINT);
    sigignore(SIGQUIT);
    waiting = TRUE;
    while ((w = wait(&status)) != pid)
	if (w == -1 && errno != EINTR)
	    break;
    if (w == -1)
	ret = -1;
    else
	ret = WEXITSTATUS(status);
    termlib_init();
    xmouse_check();
    waiting = FALSE;
    sigset(SIGINT,int_catcher);
    sigset(SIGQUIT,SIG_DFL);
    sigset(SIGTSTP,stop_catcher);
    sigset(SIGTTOU,stop_catcher);
    sigset(SIGTTIN,stop_catcher);
    if (datasrc && datasrc->auth_user)
	UNLINK(nntp_auth_file);
    return ret;
}

/* safe version of string concatenate, with \n deletion and space padding */
char *
safecat(char *to, const char *from, size_t len)
{
    char* dest = to;

    len--;				/* leave room for null */
    if (*dest) {
	while (len && *dest++) len--;
	if (len) {
	    len--;
	    *(dest-1) = ' ';
	}
    }
    if (from)
	while (len && (*dest++ = *from++)) len--;
    if (len)
	dest--;
    if (*(dest-1) == '\n')
	dest--;
    *dest = '\0';
    return to;
}

/* effective access */

#ifdef SETUIDGID
int
eaccess(char* filename, int mod)
{
    int protection, euid;

    mod &= 7;				/* remove extraneous garbage */
    if (stat(filename, &filestat) < 0)
	return -1;
    euid = geteuid();
    if (euid == ROOTID)
	return 0;
    protection = 7 & ( filestat.st_mode >> (filestat.st_uid == euid ?
			6 : (filestat.st_gid == getegid() ? 3 : 0)) );
    if ((mod & protection) == mod)
	return 0;
    errno = EACCES;
    return -1;
}
#endif

/*
 * Get working directory
 */
char *
trn_getwd(char *buf, size_t buflen)
{
    char* ret;

    ret = getcwd(buf, buflen);
    if (!ret) {
	printf("Cannot determine current working directory!\n");
	finalize(1);
    }
    return ret;
}

/* just like fgets but will make bigger buffer as necessary */
char *
get_a_line(char *buffer, size_t buffer_length, bool realloc_ok, FILE *fp)
{
    size_t bufix = 0;
    int nextch;

    if (buffer_length == 0)
        return NULL;

    buffer[0] = '\0';
    do {
	if (bufix >= buffer_length) {
	    buffer_length *= 2;
	    if (realloc_ok) {		/* just grow in place, if possible */
		buffer = saferealloc(buffer, buffer_length + 1);
	    } else {
		char* tmp = safemalloc(buffer_length + 1);
		strlcpy(tmp, buffer, buffer_length / 2);
		buffer = tmp;
		realloc_ok = true;
	    }
	}
	if ((nextch = getc(fp)) == EOF) {
	    if (bufix == 0)
		return NULL;
	    break;
	}
	buffer[bufix++] = nextch;
    } while (nextch != '\0' && nextch != '\n');
    buffer[bufix] = '\0';
    len_last_line_got = bufix;
    buflen_last_line_got = buffer_length;

    return buffer;
}

int
makedir (char *dirname, int nametype)
{
    char* end;
    char* s;
    int status = 0;

    for (end = dirname; *end; end++) ;	/* find the end */
    if (nametype == MD_FILE) {		/* not to create last component? */
	for (--end; end != dirname && *end != '/'; --end) ;
	if (*end != '/')
	    return 0;			/* nothing to make */
	*end = '\0';			/* isolate file name */
    }

    s = end;
    for (;;) {
	if (stat(dirname,&filestat) >= 0 && S_ISDIR(filestat.st_mode)) {
					/* does this much exist as a dir? */
	    *s = '/';			/* mark this as existing */
	    break;
	}
	s = rindex(dirname,'/');	/* shorten name */
	if (!s)				/* relative path! */
	    break;			/* hope they know what they are doing */
	*s = '\0';			/* mark as not existing */
    }

    for (s=dirname; s <= end; s++) {	/* this is grody but efficient */
	if (!*s) {			/* something to make? */
	    status = status || mkdir(dirname,0777);
	    *s = '/';			/* mark it made */
	}
    }
    if (nametype == MD_DIR)		/* don't need final slash unless */
	*end = '\0';			/*  a filename follows the dir name */

    return status;
}

void
notincl (char *feature)
{
    printf("\nNo room for feature \"%s\" on this machine.\n",feature);
}

/* grow a static string to at least a certain length */

void
growstr (char **strptr, int *curlen, int newlen)
{
    if (newlen > *curlen) {		/* need more room? */
	if (*curlen)
	    *strptr = saferealloc(*strptr,(size_t)newlen);
	else
	    *strptr = safemalloc(newlen);
	*curlen = newlen;
    }
}

void
setdef (char *buffer, char *dflt)
{
#ifdef SCAN
    s_default_cmd = FALSE;
#endif
    univ_default_cmd = FALSE;
    if (*buffer == ' '
#ifndef STRICTCR
     || *buffer == '\n' || *buffer == '\r'
#endif
    ) {
#ifdef SCAN
	s_default_cmd = TRUE;
#endif
	univ_default_cmd = TRUE;
	if (*dflt == '^' && isupper(dflt[1]))
	    pushchar(Ctl(dflt[1]));
	else
	    pushchar(*dflt);
	getcmd(buffer);
    }
}

void
safelink (char *old, char *new)
{
    if (link(old,new)) {
	printf("Can't link backup (%s) to .newsrc (%s)\n", old, new);
	finalize(1);
    }
}

/* attempts to verify a cryptographic signature. */
void
verify_sig (void)
{
    int i;

    printf("\n");
    /* RIPEM */
    i = doshell(sh,filexp("grep -s \"BEGIN PRIVACY-ENHANCED MESSAGE\" %A"));
    if (!i) {	/* found RIPEM */
	i = doshell(sh,filexp(getval("VERIFY_RIPEM",VERIFY_RIPEM)));
	printf("\nReturned value: %d\n",i);
	return;
    }
    /* PGP */
    i = doshell(sh,filexp("grep -s \"BEGIN PGP\" %A"));
    if (!i) {	/* found PGP */
	i = doshell(sh,filexp(getval("VERIFY_PGP",VERIFY_PGP)));
	printf("\nReturned value: %d\n",i);
	return;
    }
    printf("No PGP/RIPEM signatures detected.\n");
}

time_t
text2secs (char *s, time_t defSecs)
{
    time_t secs = 0;
    time_t item;

    if (!isdigit(*s)) {
	if (*s == 'm' || *s == 'M')	/* "missing" */
	    return 2;
	if (*s == 'y' || *s == 'Y')	/* "yes" */
	    return defSecs;
	return secs;			/* "never" */
    }
    do {
	item = atol(s);
	while (isdigit(*s)) s++;
	while (isspace(*s)) s++;
	if (isalpha(*s)) {
	    switch (*s) {
	      case 'd': case 'D':
		item *= 24 * 60L;
		break;
	      case 'h': case 'H':
		item *= 60L;
		break;
	      case 'm': case 'M':
		break;
	      default:
		item = 0;
		break;
	    }
	    while (isalpha(*s)) s++;
	    if (*s == ',') s++;
	    while (isspace(*s)) s++;
	}
	secs += item;
    } while (isdigit(*s));

    return secs * 60;
}

char *
secs2text (time_t secs)
{
    char* s = buf;
    int items;

    if (!secs || (secs & 1))
	return "never";
    if (secs & 2)
	return "missing";

    secs /= 60;
    if (secs >= 24L * 60) {
	items = (int)(secs / (24*60));
	secs = secs % (24*60);
	sprintf(s, "%d day%s, ", items, PLURAL(items));
	s += strlen(s);
    }
    if (secs >= 60L) {
	items = (int)(secs / 60);
	secs = secs % 60;
	sprintf(s, "%d hour%s, ", items, PLURAL(items));
	s += strlen(s);
    }
    if (secs) {
	sprintf(s, "%d minute%s, ", (int)secs, PLURAL(items));
	s += strlen(s);
    }
    s[-2] = '\0';
    return buf;
}

/* returns a saved string representing a unique temporary filename */
char *
temp_filename (void)
{
    static int tmpfile_num = 0;
    char tmpbuf[CBUFLEN];
    extern long our_pid;
    sprintf(tmpbuf,"%s/trn%d.%ld",tmpdir,tmpfile_num++,our_pid);
    return estrdup(tmpbuf);
}

char *
get_auth_user (void)
{
    return datasrc->auth_user;
}

char *
get_auth_pass (void)
{
    return datasrc->auth_pass;
}

#if defined(USE_GENAUTH) && defined(SUPPORT_NNTP)
char *
get_auth_command (void)
{
    return datasrc->auth_command;
}
#endif

char **
prep_ini_words (INI_WORDS words[])
{
    int checksum;
    char* cp = (char*)INI_VALUES(words);
    if (!cp) {
	int i;
	for (i = 1; words[i].item != NULL; i++) {
	    if (*words[i].item == '*') {
		words[i].checksum = -1;
		continue;
	    }
	    checksum = 0;
	    for (cp = words[i].item; *cp; cp++)
		checksum += (isupper(*cp)? tolower(*cp) : *cp);
	    words[i].checksum = (checksum << 8) + (cp - words[i].item);
	}
	words[0].checksum = i;
	words[0].help_str = cp = safemalloc(i * sizeof (char*));
    }
    bzero(cp, INI_LEN(words) * sizeof (char*));
    return (char**)cp;
}

void
unprep_ini_words (INI_WORDS words[])
{
    safefree((char *)INI_VALUES(words));
    words[0].checksum = 0;
    words[0].help_str = NULL;
}

void
prep_ini_data (char *cp, char *filename)
{
    char* t = cp;

#ifdef DEBUG
    if (debug & DEB_RCFILES)
	printf("Read %d bytes from %s\n",strlen(cp),filename);
#endif

    while (*cp) {
	while (isspace(*cp)) cp++;

	if (*cp == '[') {
	    char* s = t;
	    do {
		*t++ = *cp++;
	    } while (*cp && *cp != ']' && *cp != '\n');
	    if (*cp == ']' && t != s) {
		*t++ = '\0';
		cp++;
		if (parse_string(&t, &cp))
		    cp++;

		while (*cp) {
		    while (isspace(*cp)) cp++;
		    if (*cp == '[')
			break;
		    if (*cp == '#')
			s = cp;
		    else {
			s = t;
			while (*cp && *cp != '\n') {
			    if (*cp == '=')
				break;
			    if (isspace(*cp)) {
				if (s == t || t[-1] != ' ')
				    *t++ = ' ';
				cp++;
			    }
			    else
				*t++ = *cp++;
			}
			if (*cp == '=' && t != s) {
			    while (t != s && isspace(t[-1])) t--;
			    *t++ = '\0';
			    cp++;
			    if (parse_string(&t, &cp))
				s = NULL;
			    else
				s = cp;
			}
			else
			    s = cp;
		    }
		    cp++;
		    if (s)
			for (cp = s; *cp && *cp++ != '\n'; ) ;
		}
	    }
	    else {
		*t = '\0';
		printf("Invalid section in %s: %s\n", filename, s);
		t = s;
		while (*cp && *cp++ != '\n') ;
	    }
	}
	else
	    while (*cp && *cp++ != '\n') ;
    }
    *t = '\0';
}

bool
parse_string (char **to, char **from)
{
    char inquote = 0;
    char* t = *to;
    char* f = *from;
    char* s;

    while (isspace(*f) && *f != '\n') f++;

    for (s = t; *f; f++) {
	if (inquote) {
	    if (*f == inquote) {
		inquote = 0;
		s = t;
		continue;
	    }
	}
	else if (*f == '\n')
	    break;
	else if (*f == '\'' || *f == '"') {
	    inquote = *f;
	    continue;
	}
	else if (*f == '#') {
	    while (*++f && *f != '\n') ;
	    break;
	}
	if (*f == '\\') {
	    if (*++f == '\n')
		continue;
	    f = interp_backslash(t, f);
	    t++;
	}
	else
	    *t++ = *f;
    }
#if 0
    if (inquote)
	printf("Unbalanced quotes.\n");
#endif
    inquote = (*f != '\0');

    while (t != s && isspace(t[-1])) t--;
    *t++ = '\0';

    *to = t;
    *from = f;

    return inquote;	/* return TRUE if the string ended with a newline */
}

char *
next_ini_section (char *cp, char **section, char **cond)
{
    while (*cp != '[') {
	if (!*cp)
	    return NULL;
	cp += strlen(cp) + 1;
	cp += strlen(cp) + 1;
    }
    *section = cp+1;
    cp += strlen(cp) + 1;
    *cond = cp;
    cp += strlen(cp) + 1;
#ifdef DEBUG
    if (debug & DEB_RCFILES)
	printf("Section [%s] (condition: %s)\n",*section,
	       **cond? *cond : "<none>");
#endif
    return cp;
}

char *
parse_ini_section (char *cp, INI_WORDS words[])
{
    int checksum;
    char* s;
    char** values = prep_ini_words(words);
    int i;

    if (!*cp)
	return NULL;

    while (*cp && *cp != '[') {
	checksum = 0;
	for (s = cp; *s; s++) {
	    if (isupper(*s))
		*s = tolower(*s);
	    checksum += *s;
	}
	checksum = (checksum << 8) + (s++ - cp);
	if (*s) {
	    for (i = 1; words[i].checksum; i++) {
		if (words[i].checksum == checksum
		 && strcaseEQ(cp,words[i].item)) {
		    values[i] = s;
		    break;
		}
	    }
	    if (!words[i].checksum)
		printf("Unknown option: `%s'.\n",cp);
	    cp = s + strlen(s) + 1;
	}
	else
	    cp = s + 1;
    }

#ifdef DEBUG
    if (debug & DEB_RCFILES) {
	printf("Ini_words: %s\n", words[0].item);
	for (i = 1; words[i].checksum; i++)
	    if (values[i])
		printf("%s=%s\n",words[i].item,values[i]);
    }
#endif

    return cp;
}

bool
check_ini_cond (char *cond)
{
    int not, equal, upordown, num;
    char* s;
    cond = dointerp(buf,sizeof buf,cond,"!=<>",(char*)NULL);
    s = buf + strlen(buf);
    while (s != buf && isspace(s[-1])) s--;
    *s = '\0';
    if ((not = (*cond == '!')) != 0)
	cond++;
    if ((upordown = (*cond=='<'? -1: (*cond=='>'? 1:0))) != 0)
	cond++;
    if ((equal = (*cond == '=')) != 0)
	cond++;
    while (isspace(*cond)) cond++;
    if (upordown) {
	num = atoi(cond) - atoi(buf);
	if (!((equal && !num) || (upordown * num < 0)) ^ not)
	    return FALSE;
    }
    else if (equal) {
	COMPEX condcompex;
	init_compex(&condcompex);
	if ((s = compile(&condcompex,cond,TRUE,TRUE)) != NULL) {
	    /*warning(s)*/;
	    equal = FALSE;
	}
	else
	    equal = execute(&condcompex,buf) != NULL;
	free_compex(&condcompex);
	return equal;
    }
    else
	return FALSE;
    return TRUE;
}

/* $$ might get replaced soonish... */
/* Ask for a single character (improve the prompt?) */
char
menu_get_char (void)
{
    printf("Enter your choice: ");
    fflush(stdout);
    eat_typeahead();
    getcmd(buf);
    printf("%c\n",*buf);
    return(*buf);
}

/* NOTE: kfile.c uses its own editor function */
/* used in a few places, now centralized */
int
edit_file (char *fname)
{
    int r = -1;

    if (!fname || !*fname)
	return r;

    /* XXX paranoia check on length */
    sprintf(cmd_buf,"%s ",
	    filexp(getval("VISUAL",getval("EDITOR",defeditor))));
    strcat(cmd_buf, filexp(fname));
    termdown(3);
    resetty();			/* make sure tty is friendly */
    r = doshell(sh,cmd_buf);/* invoke the shell */
    noecho();			/* and make terminal */
    crmode();			/*   unfriendly again */
    return r;
}

/* Consider a trn_pushdir, trn_popdir pair of functions */
