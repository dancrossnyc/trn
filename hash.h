/*
 * general-purpose in-core hashing
 */
/* This file is an altered version of a set of hash routines by
 * Geoffrey Collyer.  See hash.c for his copyright.
 */

 #include <stddef.h>

struct hashdatum {
    void *dat_ptr;
    size_t dat_len;
};

#define HASH_DEFCMPFUNC (int(*)(const void *, size_t,HASHDATUM))NULL

/* DON'T EDIT BELOW THIS LINE OR YOUR CHANGES WILL BE LOST! */

HASHTABLE* hashcreate (unsigned int, int (*)(const void *, size_t, HASHDATUM));
void hashdestroy (HASHTABLE*);
void hashstore (HASHTABLE*,char*,int,HASHDATUM);
void hashdelete (HASHTABLE*,char*,int);
HASHDATUM hashfetch (HASHTABLE*,char*,int);
void hashstorelast (HASHDATUM);
void hashwalk (HASHTABLE*,int(*) (int,HASHDATUM*,int),int);
