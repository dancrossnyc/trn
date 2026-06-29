/* util3.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include <stdio.h>
#include "config.h"
#include "config2.h"
#include "typedef.h"
#include "config.h"
#include "config2.h"
#include "nntpclient.h"
#include "util2.h"
#include "util3.h"

char* sh = NULL;
bool export_nntp_fds = false;

char* nntp_password;

int
doshell (char *sh, char *cmd)
{
    return system(cmd);
}

void
finalize (int num)
{
    nntp_close(true);
    exit(num);
}

char *
dointerp (char *dest, int destsize, char *pattern, char *stoppers, char *cmd)
{
    extern char* dotdir;
    if (*pattern == '%' && pattern[1] == '.') {
	int len = strlen(dotdir);
	safecpy(dest, dotdir, destsize);
	if (len < destsize)
	    safecpy(dest+len, pattern+2, destsize - len);
    }
    else
	safecpy(dest, pattern, destsize);
    return stoppers; /* This is wrong on purpose */
}

int
nntp_handle_nested_lists (void)
{
    fputs("Programming error! Nested NNTP calls detected.\n",stderr);
    return -1;
}

char *
get_auth_user (void)
{
    extern char* nntp_auth_file;
    return read_auth_file(nntp_auth_file, &nntp_password);
}

char *
get_auth_pass (void)
{
    return nntp_password;
}

#if defined(USE_GENAUTH) && defined(SUPPORT_NNTP)
char *
get_auth_command (void)
{
    return NULL;
}
#endif
