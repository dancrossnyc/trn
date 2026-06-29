/* This file Copyright 1992 by Clifford A. Adams */
/* sdisp.h
 *
 * scan display functions
 */

	/* height of screen in characters */
extern int scr_height;
	/* width of screen in characters */
extern int scr_width;

/* has the window been resized? */
extern bool s_resized;

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void s_goxy (int,int);
void s_mail_and_place (void);
void s_refresh_top (void);
void s_refresh_bot (void);
void s_refresh_entzone (void);
void s_place_ptr (void);
void s_refresh_status (int);
void s_refresh_description (int);
void s_ref_entry (int,int);
void s_rub_ptr (void);
void s_refresh (void);
int s_initscreen (void);
void s_ref_status_onpage (long);
void s_resize_win (void);
