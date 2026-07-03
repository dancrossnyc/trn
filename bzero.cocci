@@
expression P;
expression L;
@@
- bzero(P, L)
+ memset(P, 0, L)

@@
expression S;
expression D;
expression L;
@@
- bcopy(S, D, L)
+ memmove(D, S, L)
