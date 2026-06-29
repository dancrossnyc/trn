/* rt-page.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */


#define PRESERVE_PAGE     0
#define FILL_LAST_PAGE    1

extern int sel_total_obj_cnt;
extern int sel_prior_obj_cnt;
extern int sel_page_obj_cnt;
extern int sel_page_item_cnt;
extern int sel_max_per_page;
extern int sel_max_line_cnt;

extern ARTICLE** sel_page_app;
extern ARTICLE** sel_next_app;
extern ARTICLE* sel_last_ap;
extern SUBJECT* sel_page_sp;
extern SUBJECT* sel_next_sp;
extern SUBJECT* sel_last_sp;

extern char* sel_grp_dmode;
extern char* sel_art_dmode;

extern bool group_init_done;

union sel_union {
    ARTICLE* ap;
    SUBJECT* sp;
    ADDGROUP* gp;
    MULTIRC* mp;
    NGDATA* np;
    UNIV_ITEM* un;
    int op;
};

struct sel_item {
    SEL_UNION u;
    int line;
    int sel;
};

#define MAX_SEL 99
extern SEL_ITEM sel_items[MAX_SEL];

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

bool set_sel_mode (char_int);
char* get_sel_order (int);
bool set_sel_order (int,char*);
bool set_sel_sort (int,char_int);
void set_selector (int,int);
void init_pages (bool_int);
bool first_page (void);
bool last_page (void);
bool next_page (void);
bool prev_page (void);
bool calc_page (SEL_UNION);
void display_page_title (bool_int);
void display_page (void);
void update_page (void);
void output_sel (int,int,bool_int);
void display_option (int,int);
