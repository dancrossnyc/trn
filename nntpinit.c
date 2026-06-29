/* nntpinit.c
*/
/* This software is copyrighted as detailed in the LICENSE file. */


/*#define DECNET	*//* If you want decnet support */
/*#define EXCELAN	*//* Excelan EXOS 205 support */
/*#define NONETD	*//* Define if you're missing netdb.h */

#include "common.h"
#include "nntpclient.h"
#include "nntpinit.h"
#include "nntpinit.ih"


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>



int
init_nntp (void)
{
    return 1;
}

int
server_init (char *machine)
{
    int sockt_rd, sockt_wr;

    sockt_rd = get_tcp_socket(machine, nntplink.port_number, "nntp");

    if (sockt_rd < 0)
	return -1;

    sockt_wr = dup(sockt_rd);

    /* Now we'll make file pointers (i.e., buffered I/O) out of
    ** the socket file descriptor.  Note that we can't just
    ** open a fp for reading and writing -- we have to open
    ** up two separate fp's, one for reading, one for writing. */
    if ((nntplink.rd_fp = fdopen(sockt_rd, "r")) == NULL) {
	perror("server_init: fdopen #1");
	return -1;
    }
    if ((nntplink.wr_fp = fdopen(sockt_wr, "w")) == NULL) {
	perror("server_init: fdopen #2");
	nntplink.rd_fp = NULL;
	return -1;
    }

    /* Now get the server's signon message */
    nntp_check();

    if (*ser_line == NNTP_CLASS_OK) {
	char save_line[NNTP_STRLEN];
	strcpy(save_line, ser_line);
	/* Try MODE READER just in case we're talking to innd.
	** If it is not an invalid command, use the new reply. */
	if (nntp_command("MODE READER") <= 0)
	    sprintf(ser_line, "%d failed to send MODE READER\n", NNTP_ACCESS_VAL);
	else if (nntp_check() <= 0 && atoi(ser_line) == NNTP_BAD_COMMAND_VAL)
	    strcpy(ser_line, save_line);
    }
    return atoi(ser_line);
}

void
cleanup_nntp (void)
{
}
