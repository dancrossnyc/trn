/* search.c
 */

/* string search routines */

/*              Copyright (c) 1981,1980 James Gosling           */

/* Modified Aug. 12, 1981 by Tom London to include regular expressions
   as in ed.  RE stuff hacked over by jag to correct a few major problems,
   mainly dealing with searching within the buffer rather than copying
   each line to a separate array.  Newlines can now appear in RE's */

/* Ripped to shreds and glued back together to make a search package,
 * July 6, 1984, by Larry Wall. (If it doesn't work, it's probably my fault.)
 * Changes include:
 *      Buffer, window, and mlisp stuff gone.
 *      Translation tables reduced to 1 table.
 *      Expression buffer is now dynamically allocated.
 *      Character classes now implemented with a bitmap.
 * Modified by David Canzi, Apr 1997:
 *      Check bounds on alternatives array.
 *      Correct spurious matching, eg. /: re: .*\bfoo/ matched ": re: bar".
 */

#include "common.h"
#include "util.h"
#include "util2.h"
#include "search.h"
#include "search.ih"

#ifndef BITSPERBYTE
#define BITSPERBYTE 8
#endif

#define BMAPSIZ (127 / BITSPERBYTE + 1)

#define MNULL   64              /* Bit is set in a meta-character defn to
                                   indicate that the metacharacter can match
                                   a null string.  advance() uses this. */

/* meta characters in the "compiled" form of a regular expression */
#define CBRA    (2|MNULL)       /* \( -- begin bracket */
#define CCHR    4               /* a vanilla character */
#define CDOT    6               /* . -- match anything except a newline */
#define CCL     8               /* [...] -- character class */
#define NCCL    10              /* [^...] -- negated character class */
#define CDOL    (12|MNULL)      /* $ -- matches the end of a line */
#define CEND    (14|MNULL)      /* The end of the pattern */
#define CKET    (16|MNULL)      /* \) -- close bracket */
#define CBACK   (18|MNULL)      /* \N -- backreference to the Nth bracketed
                                   string */
#define CIRC    (20|MNULL)      /* ^ matches the beginning of a line */

#define WORD    32              /* matches word character \w */
#define NWORD   34              /* matches non-word characer \W */
#define WBOUND  (36|MNULL)      /* matches word boundary \b */
#define NWBOUND (38|MNULL)      /* matches non-(word boundary) \B */

#define STAR    01              /* * -- Kleene star, repeats the previous
                                   REas many times as possible; the value
                                   ORs with the other operator types */

#define ASCSIZ 256
typedef unsigned char TRANSTABLE[ASCSIZ];

static TRANSTABLE trans = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
     16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
     32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
     48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
     64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
     80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
     96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};
static bool folding = false;

static int err;
static const char* FirstCharacter;

void
init_compex (COMPEX *ocompex)
{
    /* the following must start off zeroed */
    struct xcompex *compex = (struct xcompex *)ocompex;

    compex->eblen = 0;
    compex->brastr = NULL;
}

void
free_compex (COMPEX *ocompex)
{
    struct xcompex *compex = (struct xcompex *)ocompex;
    if (compex->eblen) {
        safefree(compex->expbuf);
        compex->eblen = 0;
    }
    if (compex->brastr) {
        safefree(compex->brastr);
        compex->brastr = NULL;
    }
}

static char* gbr_str = NULL;
static int gbr_siz = 0;

char *
getbracket (COMPEX *ocompex, int n)
{
    struct xcompex *compex = (struct xcompex *)ocompex;
    int length = compex->braelist[n] - compex->braslist[n];

    if (!compex->nbra)
        return NULL;
    if (n > compex->nbra || !compex->braelist[n] || length < 0)
        return nullstr;
    growstr(&gbr_str, &gbr_siz, length+1);
    safecpy(gbr_str, compex->braslist[n], length+1);
    return gbr_str;
}

void
case_fold (int which)
{
    int i;

    if (which != folding) {
        if (which) {
            for (i = 'A'; i <= 'Z'; i++)
                trans[i] = tolower(i);
        }
        else {
            for (i = 'A'; i <= 'Z'; i++)
                trans[i] = i;
        }
        folding = which;
    }
}

/* Compile the given regular expression into a [secret] internal format */

const char *
compile (COMPEX *ocompex, const char *strp, bool fold)
{
    struct xcompex *compex = (struct xcompex *)ocompex;
    int c;
    char* ep;
    char* lastep;
    char  bracket[NBRA];
    char* bracketp;
    char** alt = compex->alternatives;
    const char* retmes = "Badly formed search string";

    case_fold(compex->do_folding = fold);
    if (!compex->eblen) {
        compex->expbuf = safemalloc(84);
        compex->eblen = 80;
    }
    ep = compex->expbuf;                /* point at expression buffer */
    *alt++ = ep;                        /* first alternative starts here */
    bracketp = bracket;                 /* first bracket goes here */
    if (*strp == 0) {                   /* nothing to compile? */
        if (*ep == 0)                   /* nothing there yet? */
            return "Null search string";
        return NULL;                    /* just keep old expression */
    }
    compex->nbra = 0;                   /* no brackets yet */
    lastep = 0;
    for (;;) {
        if (ep + 4 - compex->expbuf >= compex->eblen)
            ep = grow_eb(compex, ep, alt);
        c = *strp++;                    /* fetch next char of pattern */
        if (c == 0) {                   /* end of pattern? */
            if (bracketp != bracket) {  /* balanced brackets? */
#ifdef VERBOSE
                retmes = "Unbalanced parens";
#endif
                goto cerror;
            }
            *ep++ = CEND;               /* terminate expression */
            *alt++ = 0;                 /* terminal alternative list */
            return NULL;                /* return success */
        }
        if (c != '*')
            lastep = ep;
        switch (c) {
        case '\\':              /* meta something */
            switch (c = *strp++) {
            case '(':
                if (compex->nbra >= NBRA) {
#ifdef VERBOSE
                    retmes = "Too many parens";
#endif
                    goto cerror;
                }
                *bracketp++ = ++compex->nbra;
                *ep++ = CBRA;
                *ep++ = compex->nbra;
                break;
            case '|':
                if (bracketp>bracket) {
#ifdef VERBOSE
                    retmes = "No \\| in parens";        /* Alas! */
#endif
                    goto cerror;
                }
                *ep++ = CEND;
                *alt++ = ep;
                if (alt > compex->alternatives + NALTS) {
#ifdef VERBOSE
                        retmes = "Too many alternatives in reg ex";
#endif
                        goto cerror;
                }
                break;
            case ')':
                if (bracketp <= bracket) {
#ifdef VERBOSE
                    retmes = "Unmatched right paren";
#endif
                    goto cerror;
                }
                *ep++ = CKET;
                *ep++ = *--bracketp;
                break;
            case 'w':
                *ep++ = WORD;
                break;
            case 'W':
                *ep++ = NWORD;
                break;
            case 'b':
                *ep++ = WBOUND;
                break;
            case 'B':
                *ep++ = NWBOUND;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                *ep++ = CBACK;
                *ep++ = c - '0';
                break;
            default:
                *ep++ = CCHR;
                if (c == '\0')
                    goto cerror;
                *ep++ = c;
                break;
            }
            break;
        case '.':
            *ep++ = CDOT;
            continue;

        case '*':
            if (lastep == 0 || *lastep == CBRA || *lastep == CKET
                || *lastep == CIRC
                || (*lastep&STAR)|| *lastep>NWORD)
                goto defchar;
            *lastep |= STAR;
            continue;

        case '^':
            if (ep != compex->expbuf && ep[-1] != CEND)
                goto defchar;
            *ep++ = CIRC;
            continue;

        case '$':
            if (*strp != 0 && (*strp != '\\' || strp[1] != '|'))
                goto defchar;
            *ep++ = CDOL;
            continue;

        case '[': {             /* character class */
            int i;

            if (ep - compex->expbuf >= compex->eblen - BMAPSIZ)
                ep = grow_eb(compex, ep, alt); /* reserve bitmap */

            for (i = BMAPSIZ; i; --i)
                ep[i] = 0;

            if ((c = *strp++) == '^') {
                c = *strp++;
                *ep++ = NCCL;   /* negated */
            }
            else
                *ep++ = CCL;    /* normal */

            i = 0;              /* remember oldchar */
            do {
                if (c == '\0') {
#ifdef VERBOSE
                    retmes = "Missing ]";
#endif
                    goto cerror;
                }
                if (*strp == '-' && *(++strp) != ']' && *strp)
                    i = *strp++;
                else
                    i = c;
                while (c <= i) {
                    ep[c / BITSPERBYTE] |= 1 << (c % BITSPERBYTE);
                    if (fold && isalpha(c))
                        ep[(c ^ 32) / BITSPERBYTE] |=
                            1 << ((c ^ 32) % BITSPERBYTE);
                                /* set the other bit too */
                    c++;
                }
            } while ((c = *strp++) != ']');
            ep += BMAPSIZ;
            continue;
        }

defchar:
        default:
            *ep++ = CCHR;
            *ep++ = c;
        }
    }
cerror:
    compex->expbuf[0] = 0;
    compex->nbra = 0;
    return retmes;
}

char *
grow_eb (struct xcompex *compex, char *epp, char **alt)
{
    char* oldbuf = compex->expbuf;
    char** altlist = compex->alternatives;

    compex->eblen += 80;
    compex->expbuf = saferealloc(compex->expbuf, (size_t)compex->eblen + 4);
    if (compex->expbuf != oldbuf) {     /* realloc can change expbuf! */
        epp += compex->expbuf - oldbuf;
        while (altlist != alt)
            *altlist++ += compex->expbuf - oldbuf;
    }
    return epp;
}

const char *
execute (COMPEX *ocompex, const char *addr)
{
    struct xcompex *compex = (struct xcompex *)ocompex;
    const char* p1 = addr;
    Uchar* trt = trans;
    int c;

    if (addr == NULL || compex->expbuf == NULL)
        return NULL;
    if (compex->nbra) {                 /* any brackets? */
        for (c = 0; c <= compex->nbra; c++)
            compex->braslist[c] = compex->braelist[c] = NULL;
        if (compex->brastr)
            safefree(compex->brastr);
        compex->brastr = estrdup(p1);   /* in case p1 is not static */
        p1 = compex->brastr;            /* ! */
    }
    case_fold(compex->do_folding);      /* make sure table is correct */
    FirstCharacter = p1;                /* for ^ tests */
    if (compex->expbuf[0] == CCHR && !compex->alternatives[1]) {
        c = trt[*(Uchar*)(compex->expbuf+1)]; /* fast check for first char */
        do {
            if (trt[*(Uchar*)p1] == c && advance(compex, p1, compex->expbuf))
                return p1;
            p1++;
        } while (*p1 && !err);
        if (err) err = 0;
        return NULL;
    }
    else {                      /* regular algorithm */
        do {
            char** alt = compex->alternatives;
            while (*alt) {
                if (advance(compex, p1, *alt++))
                    return p1;
            }
            p1++;
        } while (*p1 && !err);
        if (err) err = 0;
        return NULL;
    }
   /*NOTREACHED*/
}

/* advance the match of the regular expression starting at ep along the
   string lp, simulates an NDFSA */
static bool
advance (struct xcompex *compex, const char *lp, const char *ep)
{
    const char* curlp;
    Uchar* trt = trans;
    int i;

    while (*lp || (*ep & (STAR|MNULL))) {
        switch (*ep++) {

            case CCHR:
                if (trt[*(Uchar*)ep++] != trt[*(Uchar*)lp])
                    return false;
                lp++;
                continue;

            case CDOT:
                if (*lp == '\n') return false;
                lp++;
                continue;

            case CDOL:
                if (!*lp || *lp == '\n')
                    continue;
                return false;

            case CIRC:
                if (lp == FirstCharacter || lp[-1]=='\n')
                    continue;
                return false;

            case WORD:
                if (isalnum(*lp)) {
                    lp++;
                    continue;
                }
                return false;

            case NWORD:
                if (!isalnum(*lp)) {
                    lp++;
                    continue;
                }
                return false;

            case WBOUND:
                if ((lp == FirstCharacter || !isalnum(lp[-1])) !=
                        (!*lp || !isalnum(*lp)) )
                    continue;
                return false;

            case NWBOUND:
                if ((lp == FirstCharacter || !isalnum(lp[-1])) ==
                        (!*lp || !isalnum(*lp)))
                    continue;
                return false;

            case CEND:
                return true;

            case CCL:
                if (cclass(ep, *lp, 1)) {
                    ep += BMAPSIZ;
                    lp++;
                    continue;
                }
                return false;

            case NCCL:
                if (cclass(ep, *lp, 0)) {
                    ep += BMAPSIZ;
                    lp++;
                    continue;
                }
                return false;

            case CBRA:
                compex->braslist[(unsigned char)*ep++] = lp;
                continue;

            case CKET:
                i = *ep++;
                compex->braelist[i] = lp;
                compex->braelist[0] = lp;
                compex->braslist[0] = compex->braslist[i];
                continue;

            case CBACK:
                if (compex->braelist[i = *ep++] == 0) {
                    fputs("bad braces\n",stdout);
                    err = true;
                    return false;
                }
                if (backref(compex, i, lp)) {
                    lp += compex->braelist[i] - compex->braslist[i];
                    continue;
                }
                return false;

            case CBACK | STAR:
                if (compex->braelist[i = *ep++] == 0) {
                    fputs("bad braces\n",stdout);
                    err = true;
                    return false;
                }
                curlp = lp;
                while (backref(compex, i, lp)) {
                    lp += compex->braelist[i] - compex->braslist[i];
                }
                while (lp >= curlp) {
                    if (advance(compex, lp, ep))
                        return true;
                    lp -= compex->braelist[i] - compex->braslist[i];
                }
                continue;

            case CDOT | STAR:
                curlp = lp;
                while (*lp++ && lp[-1] != '\n') ;
                goto star;

            case WORD | STAR:
                curlp = lp;
                while (*lp++ && isalnum(lp[-1])) ;
                goto star;

            case NWORD | STAR:
                curlp = lp;
                while (*lp++ && !isalnum(lp[-1])) ;
                goto star;

            case CCHR | STAR:
                curlp = lp;
                while (*lp++ && trt[*(Uchar*)(lp-1)] == trt[*(Uchar*)ep]) ;
                ep++;
                goto star;

            case CCL | STAR:
            case NCCL | STAR:
                curlp = lp;
                while (*lp++ && cclass(ep, lp[-1], ep[-1] == (CCL | STAR))) ;
                ep += BMAPSIZ;
                goto star;

        star:
                do {
                    lp--;
                    if (advance(compex, lp, ep))
                        return true;
                } while (lp > curlp);
                return false;

            default:
                fputs("Badly compiled pattern\n",stdout);
                err = true;
                return -1;
        }
    }
    return false;
}

static bool
backref(struct xcompex *compex, int i, const char *lp)
{
    const char* bp;

    bp = compex->braslist[i];
    while (*lp && *bp == *lp) {
        bp++;
        lp++;
        if (bp >= compex->braelist[i])
            return true;
    }
    return false;
}

static bool
cclass (const char *set, int c, int af)
{
    c &= 0177;
    if (set[c / BITSPERBYTE] & 1 << (c % BITSPERBYTE))
        return af;
    return !af;
}
