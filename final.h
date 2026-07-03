/* final.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


/* cleanup status for fast exits */

extern bool panic;              /* we got hung up or something-- */
                                /*  so leave tty alone */
extern bool doing_ng;           /* do we need to reconstitute */
                                /* current rc line? */

extern char int_count;          /* how many interrupts we've had */

extern bool bos_on_stop;        /* set when handling the stop signal */
                                /* would leave the screen a mess */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void final_init (void);
[[noreturn]] void finalize (int);
void int_catcher (int);
void sig_catcher (int);
void pipe_catcher (int);
void stop_catcher (int);
