/* rt-select.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */


extern bool sel_rereading;
extern char sel_disp_char[];

#define SM_THREAD       1
#define SM_SUBJECT      2
#define SM_ARTICLE      3
#define SM_NEWSGROUP    4
#define SM_ADDGROUP     5
#define SM_MULTIRC      6
#define SM_OPTIONS      7
#define SM_UNIVERSAL    8

extern int sel_mode;
extern int sel_defaultmode;
extern int sel_threadmode;

#define SS_DATE         1
#define SS_STRING       2
#define SS_AUTHOR       3
#define SS_COUNT        4
#define SS_NATURAL      5
#define SS_GROUPS       6
#define SS_LINES        7
/* NOTE: The score order is still valid even without scoring enabled. */
/*       (The real order is then something like natural or date.) */
#define SS_SCORE        8

extern char* sel_mode_string;
extern int sel_sort;
extern int sel_artsort;
extern int sel_threadsort;
extern int sel_newsgroupsort;
extern int sel_addgroupsort;
extern int sel_univsort;

extern char* sel_sort_string;
extern int sel_direction;
extern bool sel_exclusive;
extern int sel_mask;

extern bool selected_only;
extern ART_UNREAD selected_count;
extern int selected_subj_cnt;
extern int added_articles;

extern char* sel_chars;
extern int sel_item_index;
extern int sel_last_line;
extern bool sel_at_end;
extern bool art_sel_ilock;

#define DS_ASK          1
#define DS_UPDATE       2
#define DS_DISPLAY      3
#define DS_RESTART      4
#define DS_STATUS       5
#define DS_QUIT         6
#define DS_DOCOMMAND    7
#define DS_ERROR        8


#define UR_NORM         1
#define UR_BREAK        2       /* request return to selector */
#define UR_ERROR        3       /* non-normal return */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

char article_selector (int);
char multirc_selector (void);
char newsgroup_selector (void);
char addgroup_selector (int);
char option_selector (void);
char universal_selector (void);
void selector_mouse (int,int,int,int,int,int);
int univ_visit_group (char*);
void univ_visit_help (int);
