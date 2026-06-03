/* nntpauth.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */


/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

int nntp_handle_auth_err (void);
#ifdef USE_GENAUTH
int nntp_auth (char*);
#endif
