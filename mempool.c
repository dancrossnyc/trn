/*
 * mempool.c
 */

#include "common.h"
#include "final.h"
#include "util.h"
#include "util2.h"
#include "mempool.h"
#include "mempool.ih"

/*
 * This was originally implemented, I suspect, for performance:
 * an arena bump allocator is just faster than calling `malloc`
 * over and over again; or at least, that was (likely) the theory.
 * Unfortunately, what we've really got is more like a K&R-style
 * allocator, just done manually.
 *
 * The only reason I don't remove this entirely and replace it
 * with malloc/free is because `mp_free` frees an entire "pool"
 * at once, and tracking down all of the allocations that need
 * to be freed would be a bigger lift.
 */

typedef struct Alloc Alloc;
struct Alloc {
    Alloc *next;
    void *alloc;
};
#define NPOOLS 3
static Alloc *pools[NPOOLS];

void
mp_init (void)
{
}

char *
mp_estrdup(const char *str, Mempool pool)
{
    if (str == NULL) {
        printf("\ntrn: mp_estrdup(NULL,%d) error.\n", pool);
        abort();
    }
    char *s = estrdup(str);
    Alloc *a = safemalloc(sizeof(Alloc));
    a->alloc = s;
    a->next = pools[pool];
    pools[pool] = a;
    return s;
}

/* free a whole memory pool */
void
mp_free(Mempool pool)
{
    for (Alloc *next = NULL, *a = pools[pool]; a != NULL; a = next) {
        next = a->next;
        safefree(a->alloc);
        safefree(a);
    }
    pools[pool] = NULL;
}
