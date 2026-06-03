/* nntpinit.h
 */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

int init_nntp (void);
int server_init (char*);
void cleanup_nntp (void);
int get_tcp_socket (char*,int,char*);
#ifdef NONETDB
unsigned long inet_addr (char*);
#endif
