/* rt-ov.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */


/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

bool ov_init (void);
int ov_num (char*,char*);
bool ov_data (ART_NUM,ART_NUM,bool);
void ov_close (void);
char* ov_fieldname (int);
char* ov_field (ARTICLE*,int);
