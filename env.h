
/* env.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


extern char* homedir;		/* login directory */
extern char* dotdir;		/* where . files go */
extern char* trndir;		/* usually %./.trn */
extern char* newslib;		/* news library */
extern char* rnlib;		/* private news program library */
extern char* tmpdir;		/* where tmp files go */
extern char* loginName;		/* login id of user */
extern char* realName;		/* real name of user */
extern char* phostname;		/* host name in a posting */
extern char* localhost;		/* local host name */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

bool env_init (char*,bool);
bool setusername (char*);
bool setphostname (char*);
char* getval (char*,char*);
const char* export (const char*, const char*);
void unexport (const char*);
