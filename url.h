/* This file Copyright 1993 by Clifford A. Adams */
/* url.h
 *
 * Routines for handling WWW URL references.
 */

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

bool fetch_http (char*,int,char*,char*);
bool fetch_ftp (char*,char*,char*);
bool parse_url (char*);
bool url_get (char*,char*);
