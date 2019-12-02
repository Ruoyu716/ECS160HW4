#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define FILE_SIZE 20000
#define LINE_LENGTH 1025


// read tokens splitted by comma in a line
bool spilt(char* input, int slot,char* newStr){
    char copy[1025];
    strcpy(copy,input);

    int count = 0;
    int startBit = -1;

    for (int i = 0; i < strlen(copy); ++i) {
        if(copy[i] == ',' || copy[i]=='\n'){
            if(count == slot){
                newStr[i-startBit-1] = '\000';
            }
            count++;
            startBit = i;
            if(copy[i] == ';'){
                return newStr;
            }
        }else{
            if(count==slot){
                newStr[i-startBit-1] = copy[i];
            }
        }
    }
    return newStr;
}

struct DataItem {
    char name[100];
    int count;
};

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
            printf("%s:%d\n",tweeterIn[i].name,tweeterIn[i].count);
        }
    }else{
        for(int i=0;i<n;i++){
            printf("%s:%d\n",tweeterIn[i].name,tweeterIn[i].count);
        }
    }
}

//count how many commas in a line for later checking.
int countCom(char* lineRead){
    int count = 0;
    bool isComplete = false;

    for (int i = 0; i < strlen(lineRead); ++i) {
        if(lineRead[i] == ','){
            count++;
        }
        if(lineRead[i]=='\n'){
            isComplete = true;
        }
    }
    if(isComplete){
        return count;
    }else{
        return -count;
    }
}

int main(int argc, char* argv[]){
    char line[LINE_LENGTH+4];
    char lineRead[1025];
    char name[100];
    int column;
    int clk = 1;

    int numOfitem=0;
    int countComma=0;
    int lastCount;

    bool isValid = false;
    bool isQuated = false;


    struct DataItem* tweeter = (struct DataItem*)malloc(sizeof(struct DataItem));

    if(argc != 2){
        printf("Invalid Input Format1\n");
        return -1;
    }

    FILE* fp = fopen(argv[1], "r");//read only
    if(fp == NULL){
        printf("Invalid Input file\n");
        return -1;
    }
    //check if the first line is a valid header
    if(fgets(line,LINE_LENGTH,fp)){
        strcpy(lineRead,strdup(line));

        int headerCheck = 0;
        countComma = countCom(lineRead);
        if(countComma < 0){
            printf("Invalid Input Format: Header-1\n");
            return -1;
        }
        char buffer[1025];
        for (int i = 0; i <= countComma; ++i) {
            spilt(lineRead,i,buffer);
            if(!strcmp(buffer, "name")){
                column = i;
                isValid = true;
                isQuated = false;
                headerCheck ++;
            }else if(!strcmp(buffer, "\"name\"")){
                column = i;
                isValid = true;
                isQuated = true;
                headerCheck++;
            }
        }

        if(isValid == false){
            printf("Invalid Input Format: Header\n");
            return -1;
        }
        if(headerCheck > 1){
            printf("Invalid Input Format: duplicated name\n");
            return -1;
        }

        if(column == -1){
            printf("Invalid Input Format2\n");
            return -1;
        }
    }else{
        printf("Invalid Input Format3");
        return -1;
    }

    while(fgets(line,LINE_LENGTH,fp)){
        strcpy(lineRead,strdup(line));
        if(lastCount < 0){
            printf("Invalid Input Format6\n");
            return -1;
        }
        lastCount=countCom(lineRead);

        if(!(countComma == abs(lastCount))){
            printf("Invalid Input Format6\n");
            return -1;
        }
        int tempVaild = 0;
        tempVaild = spilt(lineRead,column,name);

        // check four different cases of invalid line:
        //1.name";
        //2. "name;
        //3, header with "" but content without;
        //4. header without "" but content with.
        if(!tempVaild || ((name[0] != '"' && name[strlen(name)-1] == '"')
                          || (name[0] == '"' && name[strlen(name)-1] != '"')
                          || (isQuated &&(name[0]!='"' || name[strlen(name)-1]!='"'))
                          || (!isQuated && (name[0]=='"'|| name[strlen(name)-1]=='"')))){
            printf("Invalid Input Format4\n");
            return -1;
        }
        //remove outermost quotes for output
        if(isQuated){
            int lenth = strlen(name);
            name[lenth-1]='\000';
            strcpy(name,name+1);
        }
        bool exist = false;
        for (int i = 0; i < numOfitem; ++i) {
            if(strcmp(tweeter[i].name,name) == 0){
                tweeter[i].count = tweeter[i].count + 1;
                exist = true;
            }
        }
        if(!exist){
            numOfitem++;
            tweeter = realloc(tweeter, numOfitem*sizeof(struct DataItem));
            strcpy(tweeter[numOfitem-1].name,name);
            tweeter[numOfitem-1].count=1;
        }

        clk++;
        if(clk > FILE_SIZE){
            printf("Invalid Input Format5\n");
            return -1;
        }

    }

    getTopTweeters(10,tweeter,numOfitem);
    free(tweeter);
    //free(lineRead);
    return 0;

}

