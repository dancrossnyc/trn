/* config2.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#define Ctl(ch) (ch & 037)

#define strNE(s1,s2) (strcmp(s1,s2))
#define strEQ(s1,s2) (!strcmp(s1,s2))
#define strnNE(s1,s2,l) (strncmp(s1,s2,l))
#define strnEQ(s1,s2,l) (!strncmp(s1,s2,l))

#define strcaseCMP(s1,s2) strcasecmp(s1,s2)
#define strcaseNE(s1,s2) (strcasecmp(s1,s2))
#define strcaseEQ(s1,s2) (!strcasecmp(s1,s2))
#define strncaseCMP(s1,s2,l) strncasecmp(s1,s2,l)
#define strncaseNE(s1,s2,l) (strncasecmp(s1,s2,l))
#define strncaseEQ(s1,s2,l) (!strncasecmp(s1,s2,l))

/* some slight-of-hand for compatibility issues */

#ifndef FILE_REF
#   define FILE_REF(s) (*(s) == '/' ? '/' : 0)
#endif
