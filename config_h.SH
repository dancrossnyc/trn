case $CONFIG in
'') . ./config.sh ;;
esac
echo "Extracting config.h (with variable substitutions)"
sed <<!GROK!THIS! >config.h -e 's!^#undef\(.*\)/\*!/\*#define\1\*//\*!' -e 's!^#un-def!#undef!'
/*
 * This file was produced by running the config_h.SH script, which
 * gets its values from config.sh, which is generally produced by
 * running Configure.
 *
 * Feel free to modify any of this as the need arises.  Note, however,
 * that running config_h.SH again will wipe out any changes you've made.
 * For a more permanent change edit config.sh and rerun config_h.SH.
 *
 * \$Id: Config_h.U,v 3.0.1.5 1997/02/28 14:57:43 ram Exp $
 */

/*
 * Package name      : $package
 * Source directory  : $src
 * Configuration time: $cf_time
 * Configured by     : $cf_by
 * Target system     : $myuname
 */

#ifndef _config_h_
#define _config_h_

/* INTERNET:
 *	This symbol, if defined, indicates that there is a mailer available
 *	which supports internet-style addresses (user@site.domain).
 */
#$d_internet	INTERNET	/**/

/* NEWS_ADMIN:
 *	This symbol, if defined, contains the login name of the news
 *	administrator.
 */
#$d_newsadm NEWS_ADMIN "$newsadmin"		/**/

/* NOLINEBUF:
 *	This symbol, if defined, indicates that stdout is not buffered, so that
 *	the program can call setbuf() or setlinebuf() for efficiency.
 */
#$d_nolnbuf	NOLINEBUF	/**/

/* NORMSIG:
 *	This symbol, if defined, indicates that normal signal handling routines
 *	should be used, as opposed to the ones in 4.1bsd (sigset, etc.).
 */
#$d_normsig NORMSIG		/**/

/* HAS_SIGBLOCK:
 *	This symbol, if defined, indicates that the sigblock routine is
 *	available to block signal reception.
 */
#$d_sigblock HAS_SIGBLOCK	/**/

/* HAS_SIGHOLD:
 *	This symbol, if defined, indicates that the sighold routine is
 *	available to hold signals.
 */
#$d_sighold HAS_SIGHOLD	/**/

/* DEFEDITOR:
 *	This symbol contains the full pathname of the default editor.
 */
#define DEFEDITOR "$defeditor"		/**/

/* MAILFILE:
 *	This symbol contains the interpretable name of the mail spool file
 *	for the current user.  The program must be prepared to substitute
 *	the HOME directory for %~, and the login id for %L.
 */
#define MAILFILE "$mailfile"		/**/

/* MBOXCHAR:
 *	This symbol contains a character which will match the beginning
 *	of a mailbox file.
 */
#define MBOXCHAR '$mboxchar'		/**/

/* PASSNAMES:
 *	This symbol, if defined, indicates that full names are stored in
 *	the /etc/passwd file.
 */
/* BERKNAMES:
 *	This symbol, if defined, indicates that full names are stored in
 *	the /etc/passwd file in Berkeley format (name first thing, everything
 *	up to first comma, with & replaced by capitalized login id, yuck).
 */
#$d_passnames PASSNAMES /*  (undef to take name from ~/.fullname) */
#$d_berknames BERKNAMES /* (that is, ":name,stuff:") */

/* INSTALLPREFIX:
 *	This symbol contains the name of the install prefix for this package.
 */
#define INSTALLPREFIX "$prefix"		/**/

/* PREFSHELL:
 *	This symbol contains the full name of the preferred user shell on this
 *	system.  Usual values are /bin/csh, /bin/ksh, /bin/sh.
 */
#define PREFSHELL "$prefshell"		/**/

/* ROOTID:
 *	This symbol contains the uid of root, normally 0.
 */
#define ROOTID $rootid		/**/

/* ACTIVE:
 *	The name of the active file for the news system.  This file contains
 *	the list of active newsgroups.  The name may have ~ on the front.
 */
/* ACTIVE_TIMES:
 *	The name of the active.times file for the news system.
 */
#define ACTIVE "$active"		/**/
#$d_acttimes ACTIVE_TIMES "$acttimes"		/**/

/* HAS_TERMLIB:
 *	This symbol, when defined, indicates that termlib-style routines
 *	are available.  There is nothing to include.
 */
#$d_havetlib	HAS_TERMLIB	/**/

/* IGNOREORG:
 *	This symbol, if defined, indicates that the ORGANIZATION environment
 *	variable does not contain an organization name.
 */
#$d_ignoreorg IGNOREORG		/**/

/* SUPPORT_NNTP:
 *	This symbol, if defined, indicates that NNTP support is possible.
 */
/* USE_GENAUTH:
 *	This symbol, if defined, indicates that authinfo generic
 *	authentication is to be supported.
 */
/* SERVER_NAME:
 *	When using NNTP, this symbol indicates the server name or a
 *	file to open to read the server name.
 */
/* SUPPORT_XTHREAD:
 *	This symbol, if defined, indicates that .thread files can be
 *	grabbed via NNTP.
 */
/* HAS_LOCAL_SPOOL:
 *	This symbol, if defined, indicates that there's a local spool
 *	directory configured into trn.
 */
#$d_nntp SUPPORT_NNTP	/**/
#$d_genauth USE_GENAUTH	/**/
#define SERVER_NAME "$servername"  	/**/
#$d_xthread SUPPORT_XTHREAD  	/**/
#$d_local HAS_LOCAL_SPOOL	/**/

/* MIMECAP:
 *	This symbol contains the pathname of mimecap file, which controls
 *	what programs get run when handling mime articles.  It is often
 *	metamail's "mailcap" file, and is the same format.
 */
#define MIMECAP "%./.mimecap:~/.mailcap:$mimecap"		/**/

/* NEWSLIB:
 *	This symbol contains the name of the directory serving as the news
 *	library.  The program must be prepared to do ~ expansion on it.
 */
/* EXTRAINEWS:
 *	Contains the full path and filename of an inews to use for local
 *	postings, or nothing.
 */
/* GROUPDESC:
 *	Contains the full path and filename of the "newsgroups" file for
 *	group descriptions, or nothing.
 */
/* SUBSCRIPTIONS:
 *	Contains the full path and filename of the list of default
 *	subscriptions, or nothing.
 */
#define NEWSLIB "$newslib"		/**/
#define EXTRAINEWS "$extrainews"	/**/
#define GROUPDESC "$groupdesc"	/**/
#define SUBSCRIPTIONS "$subscriptions"	/**/

/* NEWSSPOOL:
 *	This symbol contains the directory name where news articles are
 *	spooled.  The program must be prepared to do ~ expansion on it.
 */
#define NEWSSPOOL "$newsspool"		/**/

/* ORGNAME:
 *	This symbol contains either the organizaton name or the full pathname
 *	of a file containing the organization name, which the program must
 *	be prepared to open and substitute the contents of.
 */
#define ORGNAME "$orgname"		/**/

/* PHOSTNAME:
 *	This symbol contains the posting host's name or a file from which
 *	to read its name.
 */
/* HOSTBITS:
 *	Set to TRUE if we should compare only the domain portion of the
 *	hostname when looking for local articles.
 */
#define PHOSTNAME "$phost"		/**/
#define HOSTBITS $hostbits	/**/

/* PRIVLIB:
 *	This symbol contains the name of the private library for this package.
 *	The library is private in the sense that it needn't be in anyone's
 *	execution path, but it should be accessible by the world.  The program
 *	should be prepared to do ~ expansion.
 */
#define PRIVLIB "$privlib"		/**/

/* SCAN:
 *	This is defined if you want strn's scan mode.
 */
/* SCORE:
 *	This is defined if you want strn's article scoring.
 */
#$strn SCAN		/**/
#$strn SCORE		/**/

/* THREAD_DIR:
 *	This symbol indicates where the thread files go.
 */
/* OVERVIEW_DIR:
 *	This symbol indicates where the overview files go.
 */
/* OVERVIEW_FMT:
 *	The overview.fmt file.
 */
#define THREAD_DIR	"$threaddir"		/**/
#define OVERVIEW_DIR	"$overviewdir"		/**/
#define OVERVIEW_FMT	"$overviewfmt"		/**/

/* USE_TK:
 *	This is defined if you want to link trn with Tk.
 */
#$tk USE_TK		/**/

/* THREAD_INIT:
 *	This symbol indicates we act like trn no matter what our name is.
 */
/* SELECT_INIT:
 *	This symbol indicates we default to the selector for group entry.
 */
#define THREAD_INIT	$trn_init
#define SELECT_INIT	$trn_select

#define CALL_INEWS "$useinews -h <%h"
#define NEWSPOSTER "$bin/Pnews -h %h"
#define MAILPOSTER "$bin/Rnmail -h %h"
/*#define ANCIENT_NEWS	*//* if your B news system is <= 2.10.1 */

#endif
!GROK!THIS!
