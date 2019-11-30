#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define FILE_SIZE 20000
#define LINE_LENGTH 1024

//online material
//https://stackoverflow.com/questions/12911299/read-csv-file-in-c
// read tokens splitted by comma in a line
int getField(char* line, int num, char* content){
    char* tok;
    for (tok = strtok(line,",");tok && *tok;tok=strtok(NULL,",\n")){
        if(!num--){
            strcpy(content,tok);
            return 1;
        }
    }
    return 0;
}


//find the colunm of name that acturally contains name or "name";
//which could use fgets() later to read the first line as header to check
int findName(char* line){
    char* tok;
    int colm = 0;

    for(tok = strtok(line,",");tok && *tok;tok=strtok(NULL,",\n")){
        if(!strcmp(tok, "name")){
            /*isValid = true;
            isQuated = false;*/
            return colm;
        }else if(!strcmp(tok, "\"name\"")){
            /*isValid = true;
            isQuated = true;*/
            return colm;
        }else{
            colm ++;
        }
    }
    return -1; //there no column called name; NEED ERROR MESSAGE??????
}


struct DataItem {
    char name[100];
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
        printf("There is/are only %d element(s)\n", numOfElement);
        for(int i=0;i<numOfElement;i++){
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
    char name[100];

    int column;
    int clk = 1;
    int numOfitem=0;

    bool isValid = false;
    bool isQuated = false;

    struct DataItem* tweeter = (struct DataItem*)malloc(sizeof(struct DataItem));
    //int* test = (int*)malloc(5* sizeof(int));
    //test[3]=1;
    // argc will be 1 + command line argument, in this case, should be 2
    //printf("%d\n",test[3]);
    if(argc != 2){
        printf("Invalid Input Format1\n");
        return -1;
    }

    FILE* fp = fopen(argv[1], "r");//read only

    //check if the first line is header which contains name or "name"
    if(fgets(line,LINE_LENGTH,fp)){
        //printf("%s\n",line);
        lineRead = strdup(line);

        char* tok;
        int headerCheck = 0;

        for(tok = strtok(line,",");tok && *tok;tok=strtok(NULL,",\n")){
            if(!strcmp(tok, "name")){
                isValid = true;
                isQuated = false;
                headerCheck ++;
            }else if(!strcmp(tok, "\"name\"")){
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

        column = findName(lineRead);
        if(column == -1){
            printf("Invalid Input Format2\n");
            return -1;
        }
    }else{
        printf("Invalid Input Format3");
        return -1;
    }
    //printf("1 %d \n",column);
    while(fgets(line,LINE_LENGTH,fp)){
        lineRead = strdup(line);
        int tempVaild = 0;
        tempVaild = getField(lineRead,column,name);
        //printf("%s\n",name);

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


        if(clk > FILE_SIZE){
            printf("Invalid Input Format5\n");
            return -1;
        }

    }


    getTopTweeters(10,tweeter,numOfitem);
    free(tweeter);
    free(lineRead);
    return 0;

}
