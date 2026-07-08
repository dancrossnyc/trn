/*
 * mempool.h
 */

/*
 * memory pool numbers
 */
typedef enum Mempool Mempool;
enum Mempool {
    MP_SCORE1 = 0,    /* scoring rule text */
    MP_SCORE2 = 1,    /* scorefile cache */
    MP_SATHREAD  = 2, /* sathread.c storage */
};

char *mp_estrdup(const char *, Mempool);
void mp_free(Mempool);
