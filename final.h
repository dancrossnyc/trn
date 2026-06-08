/* final.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


/* cleanup status for fast exits */

EXT bool panic INIT(FALSE);		/* we got hung up or something-- */
					/*  so leave tty alone */
EXT bool doing_ng INIT(FALSE);		/* do we need to reconstitute */
					/* current rc line? */

EXT char int_count INIT(0);		/* how many interrupts we've had */

EXT bool bos_on_stop INIT(FALSE);	/* set when handling the stop signal */
					/* would leave the screen a mess */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void final_init (void);
[[noreturn]] void finalize (int);
void int_catcher (int);
void sig_catcher (int);
void pipe_catcher (int);
void stop_catcher (int);
