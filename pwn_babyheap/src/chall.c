#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

char *chunks[16]={0};
int sizes[16]={0};

//magic mallet
void view();
void cancel();
void create();
void print_menu();
int read_int();
void modify();
int setup();

int main(){
	setup();
	int choice;
	while(1){
		print_menu();
		choice = read_int();
		switch(choice){
			case 1:
				create();
				break;

			case 2:
				cancel();
				break;

			case 3:
				view();
				break;

			case 4:
				modify();
				break;

			case 5:
				printf("bye bye");
				return 0;
				break;

			default:
				printf("invalid choice");
				break;
		}
		puts("");
	}
	return 0;
}

int setup(){
	setvbuf(stdin, NULL, 2, NULL);
	setvbuf(stdout, NULL, 2, NULL);
	setvbuf(stderr, NULL, 2, NULL);
	return 0;
}
	

int read_int(){
	char s[20];
	fgets(s, 20, stdin);
	return atoi(s);
}

void modify(){
	int index;
	int wrote;
	int size;

	puts("insert index");
	printf("> ");
	index = read_int();

	if(index<0 || index > 15){
		puts("invalid index");
		return;
	}

	if(chunks[index]==0){
		puts("this chunk isn't allocated");
		return;
	}

	puts("how much do you want to write?");
	printf("> ");
	size=read_int();
	if(size > sizes[index]){
		puts("invalid size");
		return;
	}

	puts("insert content");
	printf("> ");
	wrote = read(0, chunks[index], size);
	chunks[index][wrote]=0;
	return;
}

void view(){
	int index;
	int size;

	puts("insert index");
	printf("> ");
	index = read_int();
	if(index<0 || index > 15){
		puts("invalid index");
		return;
	}

	if(chunks[index]==0){
		puts("this chunk isn't allocated");
		return;
	}

	puts("how much do you want to read?");
	printf("> ");
	size = read_int();
	if(size>sizes[index]){
		puts("invalid size");
		return;
	}

	printf("content: ");
	write(1, chunks[index], size);
	return;
}

void cancel(){
	int index;

	puts("insert index");
	printf("> ");
	index = read_int();

	if(index <0 || index > 15){
		puts("invalid index");
		return;
	}
	if(chunks[index]==0){
		puts("this chunk isn't allocated");
		return;
	}

	free(chunks[index]);	
	chunks[index]=0;
	sizes[index]=0;
	return; 
}

void print_menu(){
	puts("1) create");
	puts("2) remove");
	puts("3) view");
	puts("4) modify");
	puts("5) exit");
	printf("> ");
	return;
}

void create(){
	int size;
	char *ptr;
	int index;
	
	puts("insert index");
	printf("> ");
	index = read_int();
	if(index <0 || index > 15){
		puts("invalid index");
		return;
	}

	puts("insert size");
	printf("> ");
	size = read_int();
	if(size > 0xffff){
		puts("invalid size");
		return;
	}

	ptr = malloc(size);
	if(ptr == NULL){
		puts("failed malloc");
		return;
	}
	chunks[index] = ptr;
	sizes[index]=size;

	
	return;
}
