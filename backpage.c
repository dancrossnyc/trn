/* backpage.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include "EXTERN.h"
#include "common.h"
#include "intrp.h"
#include "util2.h"
#include "final.h"
#include "backpage.h"

ART_LINE maxindx = -1;

long oldoffset = -1;            /* offset to block currently in window */

int varyfd = 0;                 /* virtual array file for storing  */
                                /* file offsets */
ART_POS varybuf[VARYSIZE];      /* current window onto virtual array */

void
backpage_init (void)
{
    char* varyname;

    varyname = filexp(VARYNAME);
    close(creat(varyname,0600));
    varyfd = open(varyname,2);
    UNLINK(varyname);
    if (varyfd < 0) {
	printf(cantopen,varyname);
	sig_catcher(0);
    }

}

/* virtual array read */

ART_POS
vrdary (ART_LINE indx)
{
    int subindx;
    long offset;

#ifdef DEBUG
    if (indx > maxindx) {
	printf("vrdary(%ld) > %ld\n",(long)indx, (long)maxindx);
	return 0;
    }
#endif
    if (indx < 0)
	return 0;
    subindx = indx % VARYSIZE;
    offset = (indx - subindx) * sizeof(varybuf[0]);
    if (offset != oldoffset) {
	if (oldoffset >= 0) {
	    (void)lseek(varyfd,oldoffset,0);
	    write(varyfd, (char*)varybuf,sizeof(varybuf));
	}
	(void)lseek(varyfd,offset,0);
	read(varyfd,(char*)varybuf,sizeof(varybuf));
	oldoffset = offset;
    }
    return varybuf[subindx];
}

/* write to virtual array */

void
vwtary (ART_LINE indx, ART_POS newvalue)
{
    int subindx;
    long offset;

#ifdef DEBUG
    if (indx < 0)
	printf("vwtary(%ld)\n",(long)indx);
    if (!indx)
	maxindx = 0;
    if (indx > maxindx) {
	if (indx != maxindx + 1)
	    printf("indx skipped %d-%d\n",maxindx+1,indx-1);
	maxindx = indx;
    }
#endif
    subindx = indx % VARYSIZE;
    offset = (indx - subindx) * sizeof(varybuf[0]);
    if (offset != oldoffset) {
	if (oldoffset >= 0) {
	    (void)lseek(varyfd,oldoffset,0);
	    write(varyfd,(char*)varybuf,sizeof(varybuf));
	}
	(void)lseek(varyfd,offset,0);
	read(varyfd,(char*)varybuf,sizeof(varybuf));
	oldoffset = offset;
    }
    varybuf[subindx] = newvalue;
}

