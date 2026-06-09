/* nntpinit.h
 */

#include <stdint.h>

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

int init_nntp (void);
int server_init (char*);
void cleanup_nntp (void);
int get_tcp_socket (const char*, uint16_t, const char*);
