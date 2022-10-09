#include <cstdio>
#include <stdlib.h>

#define MAX_RATS 16
#define NAME_LEN 16

// Partly based on pwncollege's heap11
// Compile with PIE so that we can run the win function in the base instead of a gadget in the libc with more or less the same difficulty (instead of being trivial without PIE)
// gcc -fPIE -o rats main.cpp

// One winning exploit strategy:
// Allocate 2 blocks. First block will have a name of exactly 15 characters + enter, overwriting length field with 0.
// To retrieve the leaks, you have to: 
// - set the length field buffer to the desired overflow size, 
// - fill it completely, overflowing into the second block's pointers at the start of the struct
// - call dialogue() to read the first byte of the next block's pointer.
// 
// ie. to read the first byte of the dialogue() pointer, you should set the length field to 40, fill it with whatever, 
// and the first byte will be appended to the end of your payload when you call greetRat on your first block.
// 
// Do this 8 times (you'll have to allocate 9 blocks since you're corrupting the second block's header every time and can't just free->alloc) and you get the full pointer leak.
// After you have the base leak, you calculate and write the win function in the second block's dialogue pointer

// Flavor change (but not more difficult): run it with a one_gadget and a libc leak (maybe from a non-tcache free/malloc operation?) instead of a win function.

struct rat {
    struct rat** pack; // Stack pointer (not actually necessary, just put win function in dialogue)
    void(*dialogue)(struct rat*); // Base pointer
    char name[NAME_LEN];
    int maxlen;
};

void dialogue(struct rat* r) {
    printf("Hi! I'm %s.\n", r->name);
}

void name(struct rat* r) {
    int nread;
    int i = 0;
    int maxlen = r->maxlen;
    char c;

    printf("Give your rat a name\n");
    do {
        scanf("%c", &c);
        if (c == 0xa) {
            c = 0;
            break; // Mistake that preserves the last byte in the buffer overflow
        }
        r->name[i] = c;
        i++;
    } while (i != maxlen); // Mistake that allows an overflow after the null byte overwrite
    r->name[i + 1] = 0; // Mistake that creates the off by one null byte overwrite
    //printf("%s, %d, %d\n", r->name, i, r->maxlen); // DEBUG

}

void createRat(struct rat** packaddr) {
    // Allocate and initialize
    struct rat* newRat = (struct rat*) malloc(sizeof(struct rat));
    newRat->maxlen = NAME_LEN;
    newRat->dialogue = dialogue;
    newRat->pack = packaddr;
    name(newRat);

    // Place rat in pack.
    for (int i = 0; i < MAX_RATS; i++) {
        if (packaddr[i] == NULL) {
            packaddr[i] = newRat;
            return;
        }
    }
    printf("You ran out of rat space!\n");
    free(newRat);
}

void renameRat(struct rat** packaddr) {
    int i = 0;
    printf("Select a rat number.\n");
    scanf("%d", &i);
    getchar();
    i %= MAX_RATS;

    if (packaddr[i] != NULL) {
        name(packaddr[i]);
    }
    else {
        printf("There's no rat there.\n");
    }
}

void deleteRat(struct rat** packaddr) {
    int i = 0;
    printf("Select a rat number.\n");
    scanf("%d", &i);
    getchar();
    i %= MAX_RATS;

    if (packaddr[i] != NULL) {
        free(packaddr[i]);
        packaddr[i] = NULL;
    }
    else {
        printf("There's no rat there.\n");
    }
}

void greetRat(struct rat** packaddr) {
    int i = 0;
    printf("Select a rat number.\n");
    scanf("%d", &i);
    getchar();
    i %= MAX_RATS;

    if (packaddr[i] != NULL) {
        packaddr[i]->dialogue(packaddr[i]);
    }
    else {
        printf("There's no rat there.\n");
    }
}


void win() {
    if(!getenv("FLAG")) {
        printf("Error reading flag env. If you see this in the CTF, call an admin.");
        exit(1);
    } else{
        printf("%s", getenv("FLAG"));
    }
}



int main()
{
    struct rat* pack[MAX_RATS] = {0,}; // VERY IMPORTANT THAT THEYRE ALL NULLED
    int c;

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    printf("Rats, we're rats, we're the rats\nWe prey at night, we stalk at night, we're the rats!\n\n");
    // printf("While looking for the ancient Mayan treasure, we've found this: %8p\n\n", stdin); // Simplification for the libc variant

    while (true) {
        printf("1) Create new rat\n2) Greet your rat\n3) Rename the rat\n4) Delete the rat\n5) Quit.\n\n");
        scanf("%d", &c);
        getchar();
        if (c == 1) createRat(pack);
        else if (c == 2) greetRat(pack);
        else if (c == 3) renameRat(pack);
        else if (c == 4) deleteRat(pack);
        else if (c == 5) return 0;
        else printf("Sorry, try again.\n");
    }  

    return 0;
}