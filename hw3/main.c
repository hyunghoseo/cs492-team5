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

    implement user commands:
      cd - sete specified directory as the current directory
      cd.. - set parent directory as current directory
      ls - list all files and sub-directories in current directory
      mkdir - create a new directory in the current directory
      create - create a new file in the current directory
      append - append a number of bytes to the file
      remove - delete a number of bytes from the file
      delete - delete the file or directory
      exit - deallocate data structures and exit program
      dir - print out director tree branch in breadth-frist order
      prfiles - print out all file information
      prdist - print out disk space info
*/


/*
  Linked List "Ldisk" of Disk Blocks -
  (1) set of blockId's (not physical address of the blocks)
  (2) The status ("free" or "used")
  (3) the pointer to the next node in Ldisk;
*/
typedef struct lDisk{
  unsigned long blockId;
  bool status; // used == true, free == false
  unsigned long numBlocks;
  struct lDisk *next;
} lDisk;

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

int main(int argc, char* argv[]){


}
