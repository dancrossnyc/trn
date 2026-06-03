/* This file is Copyright 1993 by Clifford A. Adams */
/* smisc.h
 */

/* TRUE if the last command (run through setdef()) was the default */
EXT bool s_default_cmd INIT(FALSE);

/* explicitly follow until end of thread */
EXT bool s_follow_temp INIT(FALSE);

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

bool s_eligible (long);
void s_beep (void);
char* s_get_statchars (long,int);
char* s_get_desc (long,int,bool_int);
int s_ent_lines (long);
