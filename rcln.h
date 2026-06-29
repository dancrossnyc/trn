/* rcln.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


/* if true, silence is golden (universal scan mode) */
extern bool toread_quiet;

#define ST_STRICT	0
#define ST_LAX		1

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void rcln_init (void);
#ifdef CATCHUP
void catch_up (NGDATA*,int,int);
#endif
int addartnum (DATASRC*,ART_NUM,char*);
#ifdef MCHASE
void subartnum (DATASRC*,ART_NUM,char*);
#endif
void prange (char*,ART_NUM,ART_NUM);
void set_toread (NGDATA*,bool);
void checkexpired (NGDATA*,ART_NUM);
bool was_read_group (DATASRC*,ART_NUM,char*);
