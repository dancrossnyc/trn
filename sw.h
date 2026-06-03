/* sw.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


EXT char** init_environment_strings INIT(NULL);
EXT int init_environment_cnt INIT(0);
EXT int init_environment_max INIT(0);

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

void sw_file (char**);
void sw_list (char*);
void decode_switch (char*);
void save_init_environment (char*);
void write_init_environment (FILE*);
