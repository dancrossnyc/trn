/* tkstuff.h
 */

#ifdef USE_TK
extern char* ttk_keys;

extern int ttk_idle_flag;

/* if true, we are really running Tk */
extern int ttk_running;

/* if true, allow update via ttk_do_waiting_events() */
extern int ttk_do_waiting_flag;
#endif

/* if true, we are really running TCL */
extern int ttcl_running;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

#ifdef USE_TK
void ttk_do_waiting_events (void);
void ttk_wait_for_input (void);
#endif
void ttcl_init (void);
void ttcl_finalize (int);
void ttcl_set_int (char*,int);
void ttcl_set_str (char*,char*);
int ttcl_get_int (char*);
char* ttcl_get_str (char*);
void ttcl_eval (char*);
