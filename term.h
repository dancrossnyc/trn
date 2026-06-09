/* term.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */

/* XXX: #include <term.h> */
int tgetent(char *bp, const char *name);
int tgetflag(const char *id);
int tgetnum(const char *id);
extern char *tgetstr(const char *id, char **area);
extern char *tgoto(const char *cap, int col, int row);
extern int tputs(const char *str, int affcnt, int (*putc)(int));

extern char ERASECH;		/* rubout character */
extern char KILLCH;		/* line delete character */
extern char circlebuf[PUSHSIZE];
extern int nextin;
extern int nextout;
extern unsigned char lastchar;

/* stuff wanted by terminal mode diddling routines */

extern struct termios _tty, _oldtty;

extern int _tty_ch;
extern bool bizarre;		/* do we need to restore terminal? */

/* terminal mode diddling routines */


#define crmode() ((bizarre=1), _tty.c_lflag &= ~ICANON,_tty.c_cc[VMIN]=1,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nocrmode() ((bizarre=1),_tty.c_lflag |= ICANON,_tty.c_cc[VEOF] = CEOF,tcsetattr(_tty_ch, TCSAFLUSH,&_tty))
#define echo()	 ((bizarre=1),_tty.c_lflag |= ECHO, tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define noecho() ((bizarre=1),_tty.c_lflag &=~ECHO, tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nl()	 ((bizarre=1),_tty.c_iflag |= ICRNL,_tty.c_oflag |= ONLCR,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nonl()	 ((bizarre=1),_tty.c_iflag &=~ICRNL,_tty.c_oflag &=~ONLCR,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define	savetty() (tcgetattr(_tty_ch, &_oldtty),tcgetattr(_tty_ch, &_tty))
#define	resetty() ((bizarre=0),tcsetattr(_tty_ch, TCSAFLUSH, &_oldtty))
#define unflush_output()



#ifdef TIOCSTI
#define forceme(c) ioctl(_tty_ch,TIOCSTI,c) /* pass character in " " */
#else
#define forceme(c)
#endif

/* termcap stuff */

/*
 * NOTE: if you don't have termlib you'll either have to define these strings
 *    and the tputs routine, or you'll have to redefine the macros below
 */

#ifdef HAS_TERMLIB
extern int tc_GT;				/* hardware tabs */
extern char* tc_BC;		/* backspace character */
extern char* tc_UP;		/* move cursor up one line */
extern char* tc_CR;		/* get to left margin, somehow */
extern char* tc_VB;		/* visible bell */
extern char* tc_CL;		/* home and clear screen */
extern char* tc_CE;		/* clear to end of line */
extern char* tc_TI;		/* initialize terminal */
extern char* tc_TE;		/* reset terminal */
extern char* tc_KS;		/* enter `keypad transmit' mode */
extern char* tc_KE;		/* exit `keypad transmit' mode */
extern char* tc_CM;		/* cursor motion */
extern char* tc_HO;		/* home cursor */
extern char* tc_IL;		/* insert line */
extern char* tc_CD;		/* clear to end of display */
extern char* tc_SO;		/* begin standout mode */
extern char* tc_SE;		/* end standout mode */
extern int tc_SG;		/* blanks left by SO and SE */
extern char* tc_US;		/* start underline mode */
extern char* tc_UE;		/* end underline mode */
extern char* tc_UC;		/* underline a character, if that's how it's done */
extern int tc_UG;		/* blanks left by US and UE */
extern bool tc_AM;		/* does terminal have automatic margins? */
extern bool tc_XN;		/* does it eat 1st newline after automatic wrap? */
extern char tc_PC;		/* pad character for use by tputs() */

extern speed_t outspeed;	/* terminal output speed, */

extern int fire_is_out;
extern int tc_LINES, tc_COLS;	/* size of screen */
extern int term_line, term_col;	/* position of cursor */
extern int term_scrolled;	/* how many lines scrolled away */
extern int just_a_sec;		/* 1 sec at current baud rate */
				/* (number of nulls) */

/* define a few handy macros */

#define termdown(x) term_line+=(x), term_col=0
#define newline() term_line++, term_col=0, putchar('\n')
#define backspace() tputs(tc_BC,0,putchr)
#define erase_eol() tputs(tc_CE,1,putchr)
#define clear_rest() tputs(tc_CD,tc_LINES,putchr)
#define maybe_eol() if(erase_screen&&erase_each_line)tputs(tc_CE,1,putchr)
#define underline() tputs(tc_US,1,putchr)
#define un_underline() fire_is_out|=UNDERLINE, tputs(tc_UE,1,putchr)
#define underchar() tputs(tc_UC,0,putchr)
#define standout() tputs(tc_SO,1,putchr)
#define un_standout() fire_is_out|=STANDOUT, tputs(tc_SE,1,putchr)
#define up_line() term_line--, tputs(tc_UP,1,putchr)
#define insert_line() tputs(tc_IL,1,putchr)
#define carriage_return() term_col=0, tputs(tc_CR,1,putchr)
#define dingaling() tputs(tc_VB,1,putchr)
#else /* !HAS_TERMLIB */
..."Don't know how to define the term macros!"
#endif /* !HAS_TERMLIB */

#define input_pending() finput_pending(TRUE)
#define macro_pending() finput_pending(FALSE)

extern int page_line;	/* line number for paging in
				 print_line (origin 1) */
extern bool error_occurred;

extern char* mousebar_btns;
extern int mousebar_cnt;
extern int mousebar_start;
extern int mousebar_width;
extern bool xmouse_is_on;
extern bool mouse_is_down;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void term_init (void);
void term_set (char*);
void set_macro (char*,char*);
void arrow_macros (char*);
void mac_line (char*,char*,int);
void show_macros (void);
void set_mode (char_int,char_int);
int putchr (char_int);
void hide_pending (void);
bool finput_pending (bool_int);
bool finish_command (int);
char* edit_buf (char*,char*);
bool finish_dblchar (void);
void eat_typeahead (void);
void save_typeahead (char*,int);
void settle_down (void);
void alarm_catcher (int);
int read_tty (char*,int);
#if !defined(FIONREAD) && !defined(HAS_RDCHK) && !defined(MSDOS)
int circfill (void);
#endif
void pushchar (char_int);
void underprint (char*);
#ifdef NOFIREWORKS
void no_sofire (void);
void no_ulfire (void);
#endif
void getcmd (char*);
void pushstring (char*,char_int);
int get_anything (void);
int pause_getcmd (void);
void in_char (char*,char_int,char*);
void in_answer (char*,char_int);
bool in_choice (char*,char*,char*,char_int);
int print_lines (char*,int);
int check_page_line (void);
void page_start (void);
void errormsg (char*);
void warnmsg (char*);
void pad (int);
#ifdef VERIFY
void printcmd (void);
#endif
void rubout (void);
void reprint (void);
void erase_line (bool_int);
void clear (void);
void home_cursor (void);
void goto_xy (int,int);
#ifdef SIGWINCH
void winch_catcher (int);
#endif
void termlib_init (void);
void termlib_reset (void);
#ifdef NBG_SIGIO
void waitkey_sig_handler (int);
#endif
bool wait_key_pause (int);
void xmouse_init (char*);
void xmouse_check (void);
void xmouse_on (void);
void xmouse_off (void);
void draw_mousebar (int,bool_int);
bool check_mousebar (int,int,int,int,int,int);
void add_tc_string (char*,char*);
char* tc_color_capability (char*);
