/* This file is Copyright 1993 by Clifford A. Adams */
/* smisc.h
 */

/* true if the last command (run through setdef()) was the default */
extern bool s_default_cmd;

/* explicitly follow until end of thread */
extern bool s_follow_temp;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

bool s_eligible (long);
void s_beep (void);
char* s_get_statchars (long,int);
char* s_get_desc (long,int,bool);
int s_ent_lines (long);
