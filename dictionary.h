#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

struct nlist { /* table entry: */
    struct nlist *next; /* next entry in chain */
    unsigned char hash[32]; /* hash key */
    uint64_t result; /* result value */
};

#define HASHSIZE 1001

/* hash: form hash value for unsigned char string s */
unsigned hash_fn(unsigned char s[])
{
    unsigned hashval = 0;
    for (int i = 0; i != 32; i++)
      hashval = s[i] + 31 * hashval;
    return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
struct nlist *lookup(struct nlist * hashtab[], unsigned char s[])
{
    struct nlist *np;
    for (np = hashtab[hash_fn(s)]; np != 0; np = np->next)
        if (memcmp(s, np->hash,32) == 0)
          return np; /* found */
    return 0; /* not found */
}

/* install: put (name, defn) in hashtab */
int install(struct nlist * hashtab[], unsigned char key[], uint64_t val)
{
    struct nlist *np;
    unsigned hashval;
    if ((np = lookup(hashtab,key)) == 0) { /* not found */
        np = (struct nlist *) malloc(sizeof(struct nlist));
  	np->result = val;
	memcpy(np->hash,key,32);
        hashval = hash_fn(key);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    } else /* already there */
        return 1;
    return 0;
}
/*
void main(){
    unsigned char key1[32] = "test1";
    unsigned char key2[32] = "test2";
    uint64_t result1 = 20,result2 = 30;
    install(key1,result1);
    install(key2,result2);
    unsigned char key1lookup[32] = "test1";
    unsigned char key2lookup[32] = "test2";
    struct nlist* x1 = lookup(key1lookup);
    struct nlist* x2 = lookup("test3");
    if(x2==NULL)
	    printf("Result2: not found\n");
    printf("resul1: %" PRIu64 "\n", x1->result);
}*/
