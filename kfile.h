/* kfile.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#define KF_GLOBAL 0
#define KF_LOCAL 1

#define KFS_LOCAL_CHANGES	0x0001
#define KFS_THREAD_CHANGES	0x0002
#define KFS_NORMAL_LINES	0x0010
#define KFS_THREAD_LINES	0x0020
#define KFS_GLOBAL_THREADFILE	0x1000

#define AUTO_KILL_THD	0x8000
#define AUTO_KILL_SBJ	0x4000
#define AUTO_KILL_FOL	0x2000
#define AUTO_KILL_1	0x1000
#define AUTO_SEL_THD	0x0800
#define AUTO_SEL_SBJ	0x0400
#define AUTO_SEL_FOL	0x0200
#define AUTO_SEL_1	0x0100
#define AUTO_OLD	0x0080

#define AUTO_KILLS	0xF000
#define AUTO_SELS	0x0F00

/* The following defines are only valid as flags to function calls, used
 * in combination with the AUTO_* flags above. */
#define AFFECT_UNSEL	0
#define AFFECT_ALL	0x0001
#define ALSO_ECHO	0x0002	/* only works with [un]select_article() */
#define SET_TORETURN	0x0004	/* only works with kill_*() */

#define KF_AGE_MASK	0x003F
#define KF_DAYNUM(x)	((long)time((time_t*)NULL) / 86400 - 10490 - (x))

#define KF_MAXDAYS	30

#ifdef KILLFILES
extern FILE* globkfp;			/* global article killer file */
extern FILE* localkfp;			/* local (for this newsgroup) file */
extern int kf_state;			/* the state of our kill files */
extern int kfs_local_change_clear;	/* bits to clear local changes */
extern int kfs_thread_change_set;	/* bits to set for thread changes */
extern int kf_thread_cnt;		/* # entries in the thread kfile */
extern int kf_changethd_cnt;		/* # entries changed from old to new */
extern long kf_daynum;			/* day number for thread killfile */
extern ART_NUM killfirst;		/* used as firstart when killing */
#endif

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void kfile_init (void);
int do_kfile (FILE*,int);
void kill_unwanted (ART_NUM,char*,int);
void rewrite_kfile (ART_NUM);
void update_thread_kfile (void);
void change_auto_flags (ARTICLE*,int);
void clear_auto_flags (ARTICLE*);
void perform_auto_flags (ARTICLE*,int,int,int);
int edit_kfile (void);
#ifdef KILLFILES
void open_kfile (int);
#endif
void kf_append (char*,bool_int);
