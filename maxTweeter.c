#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define FILE_SIZE 20000
#define LINE_LENGTH 1024

//online material
//https://stackoverflow.com/questions/12911299/read-csv-file-in-c
// read tokens splitted by comma in a line
char* getField(char* line, int num){
  char* tok;
  for (tok = strtok(line,",");tok && *tok;tok=strtok(NULL,",\n")){
    if(!--num){
      return tok;
    }
  }
  return NULL;
}

//find the colunm of name that acturally contains name or "name";
//which could use fgets() later to read the first line as header to check
int findName(char* line){
  char* tok;
  int colm = 1;

  for(tok = strtok(line,",");tok && *tok;tok=strtok(NULL,",\n")){
    if(!strcmp(tok, "name")){
      return colm;
    }else if(!strcmp(tok, "\"name\"")){
      return colm;
    }else{
      colm ++;
    }
  }
  return -1; //there no column called name; NEED ERROR MESSAGE??????
}

bool* isValid;
bool* isQuated;

void checkHeaderValid(char* line/*, bool* isValid*/){
  char* tok;

  for(tok = strtok(line,",");tok && *tok;tok=strtok(NULL,",\n")){
    if(!strcmp(tok, "name")){
      *isValid = true;
      *isQuated = false;
    }else if(!strcmp(tok, "\"name\"")){
      *isValid = true;
      *isQuated = true;
    }else{
      *isValid = false;
      *isQuated = false;
    }
  }
}

bool checkContentValid(char* line/*, bool isQuated*/){
  char* tok;

  // check four different cases of invalid line:
  //1.name";
  //2. "name;
  //3, header with "" but content without;
  //4. header without "" but content with.
  for(tok = strtok(line,",");tok && *tok;tok=strtok(NULL,",\n")){
    if((tok[0] != '"' && tok[strlen(tok)-1] == '"')
    || (tok[0] == '"' && tok[strlen(tok)-1] != '"')
    || (*isQuated &&(tok[0]!='"' || tok[strlen(tok)-1]!='"')) || (!*isQuated && (tok[0]=='"'|| tok[strlen(tok)-1]=='"'))){
      return false;
    }
  }

  return true;
}


struct DataItem {
   char* name;
   int count;
};

//struct DataItem tweeter[];


int comparator(const void* p, const void* q){
  int l = ((struct DataItem *)p)->count;
  int r = ((struct DataItem *)q)->count;

  return r-l; // Since we need to sort in decreaseing order
}

void getTopTweeters(int n,struct DataItem tweeterIn[], int numOfElement){//in this case, n = 10; but for further tests, set it as n
  //int numOfElement = sizeof(tweeterIn)/sizeof(tweeterIn[0]);

  qsort((void*)tweeterIn,numOfElement, sizeof(tweeterIn[0]),comparator);

  if(numOfElement < n){
    for(int i=0;i<numOfElement;i++){
      printf("There is/are only %d element(s)", numOfElement);
      printf("Name: %s, Count: %d\n",tweeterIn[i].name,tweeterIn[i].count);
    }
  }else{
    for(int i=0;i<n;i++){
      printf("Name: %s, Count: %d\n",tweeterIn[i].name,tweeterIn[i].count);
    }
  }


}


int main(int argc, char* argv[]){
   char line[LINE_LENGTH];
   char* lineRead;
   char* name;
   int column=0;
   int clk = 1;
   int index = 0;
   int sizeExpand = 0;
   bool tempBool = false;
   struct DataItem* tweeter = malloc(1 * sizeof(struct DataItem));

  // argc will be 1 + command line argument, in this case, should be 2
    if(argc != 2){
      printf("Invalid Input Format\n");
      return -1;
    }

    FILE* fp = fopen(argv[1], "r");//read only

    //check if the first line is header which contains name or "name"
    if(fgets(line,LINE_LENGTH,fp)){
      lineRead = strdup(line);
      checkHeaderValid(lineRead);
      if(*isValid == false){
        printf("Invalid Input Format\n");
        return -1;
      }
      column = findName(lineRead);
      if(column == -1){
        printf("Invalid Input Format\n");
        return -1;
      }

    }else{
      printf("Invalid Input Format");
      return -1;
    }

    while(fgets(line,LINE_LENGTH,fp)){
      lineRead = strdup(line);
      tempBool = checkContentValid(lineRead);
      if(tempBool == false){
        printf("Invalid Input Format\n");
        return -1;
      }

      name = getField(lineRead,column);
      if(name == NULL){
        printf("Invalid Input Format\n");
        return -1;
      }

      if(clk == 1){
        tweeter[index].name = name;
        tweeter[index].count = 1;
      }else{
        for(int m = 0; m < sizeof(tweeter)/sizeof(tweeter[0]);m++){
          if(!strcmp(tweeter[m].name, name)){
            tweeter[m].count += 1;
          }else{
            index += 1;
            sizeExpand = index + 1;
            tweeter = realloc(tweeter,sizeExpand*sizeof(struct DataItem));
            tweeter[index].name = name;
            tweeter[index].count = 1;
          }
        }
      }

      clk++;
      if(clk > FILE_SIZE){
        printf("Invalid Input Format\n");
        return -1;
      }

      free(lineRead);
    }

    int numOfElement = sizeof(tweeter)/sizeof(tweeter[0]);

    getTopTweeters(10,tweeter,numOfElement);
    return 0;

}
