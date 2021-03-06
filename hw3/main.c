/*
  Ryan Edelstein, Hyungho Seo
  I pledge my honor that I have abided by the Stevens Honor System.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <locale.h>
/* todo:
    parse file_list.txt for name and siz of each file.
    parse dir_list.txt
*/

/*
  Linked List "Ldisk" of Disk Blocks -
  (1) set of blockId's (not physical address of the blocks)
  (2) The status ("free" or "used")
  (3) the pointer to the next node in Ldisk;
*/
typedef struct ldisk{
  unsigned long blockId;
  bool status; // used == true, free == false
  unsigned long numBlocks;
  struct ldisk *next;
} ldisk;

/*
  linked list "Lfile" for storing disk block addresses for a file -
  (1) starting physical address of block
  (2) pointer to next block
  Note: starting physical address of block of ID k can be calculated as
        k * (blockSize). blockSize is specified by parameter -b.
*/
typedef struct lfile{
  unsigned long blockAddress;
  struct lfile *next;
} lfile;

/*
  Each node in the directory tree G is associated with either a regular file or
  a directory.

  Enum: easier than macros for constant type values.
*/
typedef enum{
  DIR_NODE; //directory node;
  FILE_NODE; //file node;
} nodeType; //node in the directory



/*
  A tree node represented by the data provided by parsing the files.
*/
typedef struct node{
  nodeType nt;
  char *file; //file names --  root
  struct node *left;
  struct node *right;
} node;
/*
  Returns the current time_stamp
*/
char *timeStamp(){
char *timestamp = (char *)malloc(sizeof(char) * 16);
time_t ltime;
ltime=time(NULL);
struct tm *tm;
tm=localtime(&ltime);
sprintf(timestamp,"%04d%02d%02d%02d%02d%02d", tm->tm_year+1900, tm->tm_mon,
    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
return timestamp;
}


/*
Each node in G that corresponds to a regular file, stores the attributes (name,
size, and time-stamp) of the file and a pointer to Lfile, the linked-list data
structure storing the disk block addresses of the file. An example of the
directory tree (and other two data structures) are shown below.
*/
// A utility function to create a new tree node for the directory node type.
struct node* newNode(char *file, node *parent)
{
    struct node* temp = (struct node*) malloc(sizeof(struct node));
    temp->type = DIR_NODE;
    temp->name = name;
    temp->parent = parent;
    return temp;
}
/*
  parse the file_list.txt file.
  The input is the file_list.txt, root is the root node representing the root
  directory. Each node in the directory tree G is associated with either a regular
  file or directory.
  ldisk-program should travers Ldisk starting from the first notde looking for the
  free blocks that can be allocated to the files.
  blocksize - the blocksize of blocks allocated to the file.

  Need to parse the file_list.txt to retrieve the name and size of each file.
*/
void parseFileList(FILE* fl, node *root, ldisk *disk, int blockSize){
  int status1;
  unsigned long size; //file size;
  char month[4] = {0}; //integer array
  char dateAndTime[10];
  if(fl == NULL){
    printf("Cannot open file!\n");
  }
  else{
      do{
        status1 = fscanf(file,  " %*d %*d %*s %*d %*s %*s %lu %s %d %s %[^\n]\n", &size, month, &day, dateAndTime,fileName);
        printf("%d %s %d %d:%d",month, day, dateAndTime);
    }while(status1 != -1);
  }
  fclose(fl);

}
int main(int argc, char *argv[]){
  /*
    Your program must accept the following parameters at the command prompt:
    -f [input files storing information on files]
    -d [input files storing information on directories]
    -s [disk size]
    -b [block size]
  */
  unsigned long diskSize = 0;
  unsigned long blockSize = 0;
  FILE *fl; //file_list
  FILE *dl; //dir_list
  int c;
  // Using the getopt argument parsing documentation
  while((c = getopt(argc,argv, "f:d:s:b:")) != -1)
    switch(c){
      // in the case of f, open up existing file for reading purposes
      case 'f':
        fl = fopen(optarg, "r");
        if(fl == NULL){
          printf("Error opening file, exiting\n");
          exit(-1);
        }
        break;
      // in the case of d, open up existing file for reading purposes
      case 'd':
        dl = fopen(optarg, "r");
        if(dl == NULL){
          printf("Error opening file, exiting\n");
          exit(-1);
        }
      case 'b':
        //block size
      case 's':
        //diskSize;
    }
    if(!fl){
      printf(argv[0]);
      return 1;
    }
    else if(!dl) {
      printf(argv[0]);
      return 1;
    }
    else if(!diskSize) {
      printf(argv[0]);
      return 1;
    }
    else if(!blockSize) {
      printf(argv[0]);
      return 1;
    }

}
