#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

char sourceword[100];  //global source word to check
char sourcesx[4];	   //global source word soundex value

typedef struct lexword{  //list of words and corresponding soundex values
	char word[100];		 //obtained from reading dictionary file "simple.txt"
	char soundex[100];
	struct lexword *next; //ptr to next word
} lexword;

lexword * lexicon = NULL; //head of dictionary word list

void add(char w[]); 
int checkDict();
void findSuggestions();
int sameness(char w[]);
int buildLexicon();
void soundexEncode(char *sx, char *sw);

int main(void){
	printf("ECE368H: Phonetic Spell Checker -- Matt Swanson -- Fall 2008\n");
	printf("Constructing lexicon...\n");
	if (buildLexicon() == 1){  //if problem, check that you have "simple.txt" in pwd
		printf("Error! Lexicon could not be built!\n");
		return 1;
	}
	int i;
	while(1){  //main loop
		printf("Please enter a word: ");
		scanf("%s", sourceword);  //get user input
		for (i=0; i<100; i++){ //convert word to all lowercase
			sourceword[i]=tolower(sourceword[i]);
		}
		soundexEncode(sourcesx, sourceword); //obtain soundex value
		if (strcmp(sourceword,"quit") == 0){ //exit program on "quit"
			printf("\nBye!\n");
			return 0;
		}
		printf("Checking if \"%s\" is a word...\n",sourceword);
		if (checkDict() == 1){  //word found, do nothing
			printf("\"%s\" is a valid word.\n\n",sourceword);
		} else { 				//not found, suggest corrections
			printf("\"%s\" is not a valid word...\n",sourceword);
			findSuggestions();
		}
	}
	return 0;
}

//encodes sw into soundex value and stores in sx
void soundexEncode(char *sx, char *sw){
	int counter = 1, i, len = strlen(sw);
	char t1[4], last, w[100];
	strcpy(w,sw);  //copy source word contents
	t1[0]=w[0];
	for (i=1; i<len; i++){
		last = w[i-1];
		if (last == w[i]) continue;  //skip double letters
		if (counter > 3) break; //only encode three digits
		//determine soundex value based on characters
		if (w[i] == 'b' || w[i] == 'f' || w[i] == 'p' || w[i] == 'v'){
			t1[counter] = '1';
			counter++;
		}
		if (w[i] == 'c' || w[i] == 'g' || w[i] == 'j' || w[i] == 'k' ||
			w[i] == 'q' || w[i] == 's' || w[i] == 'x' || w[i] == 'z'){
			t1[counter] = '2';
			counter++;
		}
		else if (w[i] == 'd' || w[i] == 't'){
			t1[counter] = '3';
			counter++;
		}
		else if (w[i] == 'l'){
			t1[counter] = '4';
			counter++;
		}
		else if (w[i] == 'm' || w[i] == 'n'){
			t1[counter] = '5';
			counter++;
		}
		else if (w[i] == 'r'){
			t1[counter] = '6';
			counter++;
		}
	}
	for (i=counter;i<4;i++){
		t1[i] = '0';  //pad with 0's to reach 4 digits, if necessary
	}
	strncpy(sx, t1, 4);  //store soundex value in sx
	return;
}

//linear search of lexicon list, returns 1 for match, 0 for no match
int checkDict(){
	lexword * p;
	for (p=lexicon; p!=NULL;p=p->next){  //step through lexicon list
		if (strcmp(sourceword,p->word) == 0){
			return 1;
		}
	}
	return 0;
}

//suggest words that have same soundex value and "sameness", prints results
void findSuggestions(){
	lexword * p;
	int count=0;
	printf("Did you mean:\n");
	for (p=lexicon; p!=NULL;p=p->next){
		if (strncmp(sourcesx,p->soundex,4) == 0){  //soundex values match
			if(sameness(p->word) >= floor(strlen(sourceword) * 0.9) ){
				//"sameness" is over threshold, valid suggestion
				printf("\t%s\n",p->word);
				count++;
			}
		}
	}
	if (count == 0){ //if no suggestions are found print this msg
		printf("No suggestions found.\n\n");
		return;
	}
	printf("\n");
	return;
}

//determine how many characters in given word are common to source word
//used to filter out "bad" suggestions, useful for longer words
int sameness(char w[]){
	int i, j, s=0;
	for (i=0;i<strlen(w);i++){
		for (j=0;j<strlen(sourceword);j++){	
			if (w[i] == sourceword[j]){
				s++;
				break;
			}
		}
	}
	return s;
}

//construct lexicon by reading dictionary file and adding to list
int buildLexicon(){
	FILE *fp = NULL;
	char temp[100];
	fp=fopen("simple.txt", "r"); //dictionary file, **MUST BE IN PWD!**
	while (fscanf(fp,"%s", temp) == 1){
		add(temp);
	}
	fclose(fp);
	return 0;
}

//add word and soundex value to list, adds in reverse order
void add(char w[]){
	lexword *lw = (lexword *)malloc(sizeof(lexword));  //allocate memory
	if (lw == NULL){
		return;
	}
	strcpy(lw->word, w);
	soundexEncode(lw->soundex,lw->word);  //encode word
	lw->next = lexicon;  //set list link
	lexicon = lw; 		 //change head to point to new node
	return;
}
