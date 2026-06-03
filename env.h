
/* env.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


EXT char* homedir INIT(NULL);		/* login directory */
EXT char* dotdir INIT(NULL);		/* where . files go */
EXT char* trndir INIT(NULL);		/* usually %./.trn */
EXT char* lib INIT(NULL);		/* news library */
EXT char* rnlib INIT(NULL);		/* private news program library */
EXT char* tmpdir INIT(NULL);		/* where tmp files go */
EXT char* loginName INIT(NULL);		/* login id of user */
EXT char* realName INIT(NULL);		/* real name of user */
EXT char* phostname INIT(NULL);		/* host name in a posting */
EXT char* localhost INIT(NULL);		/* local host name */

#ifdef SUPPORT_NNTP
EXT int netspeed INIT(20);		/* how fast our net-connection is */
#endif

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

bool env_init (char*,bool_int);
bool setusername (char*);
bool setphostname (char*);
char* getval (char*,char*);
char* export (char*,char*);
void un_export (char*);
void re_export (char*,char*,int);
#ifdef MSDOS
char* GetEnv (char*);
#endif
