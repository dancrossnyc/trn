/* util2.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

char* savestr (char*);
char* safecpy (char*,char*,int);
char* cpytill (char*,char*,int);
char* filexp (char*);
char* in_str (char*,char*,bool_int);
#ifndef HAS_STRCASECMP
int trn_casecmp (char*,char*);
int trn_ncasecmp (char*,char*,int);
#endif
char* read_auth_file (char*,char**);
