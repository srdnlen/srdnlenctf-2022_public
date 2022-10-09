// COOKB00K - SrdnlenCTF 2022

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


const char* TODOs = "TODO:\nFix some security issues with buffers and unsecure functions\nRemove function pointers feature by calling functions directly\nTo implement later: new admin shell with /bin/sh";

typedef struct Node {
	struct Node* next;
	void* data;
} LinkedList, *PLinkedList;

typedef struct {
	char* name;
	float salt;
	int time;
	char difficulty;
	char rating;
	PLinkedList ingredients;
} RECIPE, *PRECIPE;

PRECIPE recipes[7] = {};

// this function is vulnerable as it increments the remote char* directly
void take_n_chars(char** str, size_t amount) {
	do {
		**str = getchar();
		// if ((*str)[0] == '\n')
		// 	**str = '\0';
	} while (*((*str)++) != '\n' && --amount);
	(*str)[-1] = '\0';
}

char* take_chars(char** s, size_t amount) {
	char* old = *s;
	take_n_chars(s, 512);
	return old;
}

void take_input_str(char** str) {
	char* buff = malloc(256);
	fgets(buff, 256, stdin);
	*str = strdup(buff);
	free(buff);
}

void take_smol_number(char* c) {
	char num, *buf;
	while (1) {
		take_input_str(&buf);
		int v = atoi(buf);
		num = *buf;
		free(buf);
		if (v > 0 && v < 10)
			break;
	}
	*c = num;
}

void take_smol_smol_number(char* c) {
	char num, *buf;
	while (1) {
		take_input_str(&buf);
		int v = atoi(buf);
		num = *buf;
		free(buf);
		if (v > 0 && v < 6)
			break;
	}
	*c = num;
}

void take_number(int* c) {
	int num;
	char* buf;
	while (1) {
		take_input_str(&buf);
		int v = atoi(buf);
		free(buf);
		num = v;
		if (v > 0 && v < INT_MAX)
			break;
	}
	*c = num;
}

void take_floating_number(double* c) {
	double num;
	char* buf;
	while (1) {
		take_input_str(&buf);
		double v = atof(buf);
		free(buf);
		num = v;
		if (v != 0.0)
			break;
	}
	*c = num;
}

// this function contains a vulnerability (buffer overrun of 1 byte, user can write @ buf[8])
// it also has a bug: the old buffer on each iteration is kept.
char pick_choice_7(char* buf) {
	*(long long*)&buf[0] = 0LL;
	buf[0] = '0';
	//*(long long*)&buf[1] = 0LL; // move this inside while loop to fix the canary last 00 byte
	int choice_i;
	while (1) {
		for (int i = 0; i < 8; ) {
			i++;
			char c = getc(stdin);
			if (c == '\n')
				break;
			buf[i] = c;
		}
		__fpurge(stdin);
		choice_i = atoi(buf);
		if (choice_i <= 0 || choice_i > 7) {
			puts("You provided an invalid index value:");
			puts(buf);
		}
		else break;
	}
	return choice_i;
}

// kinda easy win but it's fine
void print_ascii_art() {
	system("cat banner_updated.txt");
	asm("jmp print_ascii_art+0x2c");
	asm("push %rdi");
	asm("pop %rdi");
	asm("ret");
}

void print_menu() {
	// COOKB00K
	print_ascii_art();
	puts("");
	puts("  1) View recipes list");
	puts("  2) Add recipe");
	puts("  3) Remove recipe");
	puts("  4) Edit recipe");
	puts("  5) View recipe");
	puts("  6) Quit");
	puts("");

}

void free_ingredients(PLinkedList* p) {
	free((*p)->data);
	if ((*p)->next)
		free_ingredients(&(*p)->next);
	free(*p);
}

void load_ingredients(PLinkedList* p) {
	PLinkedList last_node = NULL;
	printf("Insert your recipe's ingredients!\nYou can keep adding as many ingredients as you want.\nWhen you are done, just press ENTER to stop adding new ingredients.\n");
	char buf[1024], *mem = buf, *tmp;
	int count = 0, len;
	*p = calloc(1, sizeof(LinkedList));
	do {
		printf("Ingredient: ");
		tmp = take_chars(&mem, 256);
		if (!strcmp(tmp, ""))
			break;
		len = strlen(tmp);
		char* ptr = malloc(len+1);
		strncpy(ptr, tmp, len);
		ptr[len] = '\0';
		(*p)->data = ptr;
		(*p)->next = calloc(1, sizeof(LinkedList));		
		last_node = *p;
	} while ((p = &(last_node)->next) && ++count);
	if (last_node) { // dirty fix
		free(last_node->next);
		last_node->next = NULL;
	}
	printf("Your %d ingredients have been successfully added to your recipe!\n", count);
	
}

void print_ingredients(PLinkedList* p) {
	puts("  Ingredients used in recipe:");
	if (!(*p)->data) {
		puts("    This recipe has no ingredients yet!");
		return;
	}
	do {
		printf("   - %s;\n", (*p)->data);
	} while ((*p)->next && (p = &(*p++)->next));

}

void show_recipe(PRECIPE* ptr) {
	if ((size_t)ptr < (size_t)recipes ||
		(size_t)ptr >= (size_t)recipes+sizeof(recipes)) {
		puts("- Invalid recipe detected!");
		return;
	}
	PRECIPE p = *ptr;
	printf(" Showing recipe #%d\n", ((size_t)ptr-(size_t)&recipes)/sizeof(PRECIPE) + 1);
	printf("  Recipe name: %s", p->name);
	printf("  Difficulty (1-9): %c\n", p->difficulty);
	printf("  Rating (1-5): %c\n", p->rating);
	printf("  Cooking time: %d\n", p->time);
	printf("  Salt amount: %f\n", p->salt);
	print_ingredients(&p->ingredients);
	puts("");

}


void view_recipes() {
	puts("");
	puts(" Current recipes list:");
	puts("__________________________\n");
	for (int i = 0; i < sizeof(recipes) / sizeof(PRECIPE); i++) {
		PRECIPE* r = &recipes[i];
		if (*(size_t*)r)
			show_recipe(r);
	}
	puts("__________________________\n");
}

// this function is vulnerable, it can leak the canary
void view_recipe() {
	int choice_i; // remove unused var maybe
	char choice[8];
	printf("> Which recipe you want to view? (1-7): ");
	int id = pick_choice_7(choice);
	puts("");
	PRECIPE* r = &recipes[id-1];
	if (*(size_t*)r)
		show_recipe(r);
	else
		printf("\nThis recipe (#%d) doesn't exist yet.\n", id);

}

void add_recipe() {
	PRECIPE p = malloc(sizeof(RECIPE));
	
	printf("Recipe name: ");
	take_input_str(&p->name);
	printf("Difficulty (1-9): ");
	take_smol_number(&p->difficulty);
	printf("Rating (1-5): ");
	take_smol_smol_number(&p->rating);
	printf("Cooking time: ");
	take_number(&p->time);
	printf("Salt amount: ");
	double salt;
	take_floating_number(&salt);
	p->salt = (float)salt;
	// load the linkedlist now
	load_ingredients(&p->ingredients);

	// get the first free index and fill it
	int i;
	for (i = 0; i < sizeof(recipes) / sizeof(PRECIPE) && recipes[i]; i++) ;
	if (i == sizeof(recipes) / sizeof(PRECIPE))
		puts("The recipes list is already full, saving has been discarded.");
	else
		recipes[i] = p;

}

void remove_recipe() {
	// this commented code allows leakage of libc printf address
	// struct {
	// 	char buf[8];
	// 	void* pf;
	// } locals;
	// locals.pf = &printf;
	// ((int(*)(const char*, int))locals.pf)("> Which recipe you want to delete? (1-%d): ", sizeof(recipes) / sizeof(PRECIPE));
	// PRECIPE p, *ptr = &recipes[pick_choice_7(locals.buf)-1];
	// p = *ptr;
	
	char buf[8];
	printf("> Which recipe you want to delete? (1-%d): ", sizeof(recipes) / sizeof(PRECIPE));
	PRECIPE p, *ptr = &recipes[pick_choice_7(buf)-1];
	p = *ptr;

	int id = ((size_t)ptr-(size_t)&recipes)/sizeof(PRECIPE) + 1;
	if (p) {
		free(p->name);
		free_ingredients(&p->ingredients);
		free(p);
		*ptr = NULL;
		printf("Recipe #%d has been successfully deleted.\n", id);
	}
	else
		printf("\nThis recipe (#%d) doesn't exist yet.\n", id);
	
}

void edit_recipe() {
	char buff[32];
	printf("> Which recipe you want to edit now? (1-%d): ", sizeof(recipes) / sizeof(PRECIPE));
	PRECIPE p, *ptr = &recipes[pick_choice_7(buff)-1];
	p = *ptr;
	int id = ((size_t)ptr-(size_t)&recipes)/sizeof(PRECIPE) + 1;
	if (p) {
		free(p->name);
		free_ingredients(&p->ingredients);

		printf("Recipe name: ");
		take_input_str(&p->name);
		printf("Difficulty (1-9): ");
		take_smol_number(&p->difficulty);
		printf("Rating (1-5): ");
		take_smol_smol_number(&p->rating);
		printf("Cooking time: ");
		take_number(&p->time);
		printf("Salt amount: ");
		double salt;
		take_floating_number(&salt);
		p->salt = (float)salt;
		// load the linkedlist now
		load_ingredients(&p->ingredients);

		printf("\nRecipe #%d has been successfully edited.\n", id);
		
	}
	else
		printf("\nThis recipe (#%d) doesn't exist yet.\n", id);

}

void quit() {
	puts("\nThanks for choosing COOKB00K.\nBye!");
	exit(0);
}


typedef enum {
	PRINT_MENU,
	VIEW_RECIPES,
	ADD_RECIPE,
	REMOVE_RECIPE,
	EDIT_RECIPE,
	VIEW_RECIPE,
	QUIT
} FUNCTIONT;

typedef struct {
	void* addr;
	FUNCTIONT t;
} FUNCTION, *PFUNCTION;

typedef void*(*func_t)(void);

func_t find_function(PFUNCTION fs, FUNCTIONT t, int** fs_n) {
	while (fs->addr && (**fs_n)-- > 0) {
		if (fs->t == t)
			return fs->addr;
		fs++;
	}
	return NULL;
}

void* call_function(PFUNCTION fs, int* fs_n, FUNCTIONT t) {
	int fs_n_saved = *fs_n;
	func_t f = find_function(fs, t, &fs_n);
	*fs_n = fs_n_saved;
	return f();
}


/* DESCRIPTION
I coded this program to keep in order my delicious recipes.
During development, I was having fun with function pointers when I started seeing some weird artifacts on my screen!
Can you please check if there is any security issue in my software?
*/

int main(int, char**) {	
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	struct {
		char choice_buf[8]; // this buffer is vulnerable
		FUNCTION functions[7];
		int functions_n;

	} locals;

	locals.functions[0] = (FUNCTION){view_recipes, VIEW_RECIPES};
	locals.functions[1] = (FUNCTION){print_menu, PRINT_MENU};
	locals.functions[2] = (FUNCTION){add_recipe, ADD_RECIPE};
	locals.functions[3] = (FUNCTION){remove_recipe, REMOVE_RECIPE};
	locals.functions[4] = (FUNCTION){edit_recipe, EDIT_RECIPE};
	locals.functions[5] = (FUNCTION){view_recipe, VIEW_RECIPE};
	locals.functions[6] = (FUNCTION){quit, QUIT};
	locals.functions_n = sizeof(locals.functions)/sizeof(FUNCTION);

	while (1) {

		call_function((PFUNCTION)&locals.functions, &locals.functions_n, PRINT_MENU);
		
		//printf("%p %p\n", ((void**)&(locals.functions))[0], ((void**)&(locals.functions))[1]);
		
		// vulnerable, should be able to write 1 byte to functions[0].addr
		printf("pick your option: ");
		call_function((PFUNCTION)&locals.functions, &locals.functions_n, (FUNCTIONT)pick_choice_7(locals.choice_buf));
		
		/*
		char choice = pick_choice_7(locals.choice_buf);
		switch (choice) {
			case 1:
			call_function((PFUNCTION)&locals.functions, &locals.functions_n, VIEW_RECIPES);
			break;
			case 2:
			call_function((PFUNCTION)&locals.functions, &locals.functions_n, ADD_RECIPE);
			break;
			case 3:
			call_function((PFUNCTION)&locals.functions, &locals.functions_n, REMOVE_RECIPE);
			break;
			case 4:
			call_function((PFUNCTION)&locals.functions, &locals.functions_n, EDIT_RECIPE);
			break;
			case 5:
			call_function((PFUNCTION)&locals.functions, &locals.functions_n, VIEW_RECIPE);
			break;
			case 6:
			call_function((PFUNCTION)&locals.functions, &locals.functions_n, QUIT);
			break;	
		}
		*/
		
	}


}



