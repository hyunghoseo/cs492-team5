/*
  Hyungho Seo, Ryan Edelstein
  We pledge our honor that we have abided by the Stevens Honor System.
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
int MAIN_MEMORY_SIZE = 512;

// Struct for the page
typedef struct Page {
    int page_id;
    int valid;
    int referenced;
    unsigned long counter; // In FIFO, counter is the relative load time; in LRU, the relative use time
} page;

typedef struct Program {
    int pid;
    page* pt;
    int num_pages;
    int hand; // Hand variable for Clock algorithm
} program;

program* programs;

int* pages_in_memory;

unsigned long global_counter = 0; // counter to keep track of relative memory access time

// Loads the page at the index page_num of process pid's page table
void load_page(int pid, int page_num, int replace_alg, int prepaging) {
    int swap_target; // Variable to keep track of which page will be replaced
    int num_pages = programs[pid].num_pages; // Number of pages in the program's page table
    page* page_table = programs[pid].pt; // The program's page table

    int i;
    for (i = 0; i < num_pages; i++) {
        if (page_table[i].valid) {
            swap_target = i;
            break;
        }
    }

    if (replace_alg == 0 || replace_alg == 1) { /* FIFO or LRU */
        for (i = 0; i < num_pages; i++) {
            if (page_table[i].valid && (page_table[i].counter < page_table[swap_target].counter)) {
                swap_target = i;
            }
        }
    }
    else if (replace_alg == 2) { /* Clock */
        // Advance the clock's hand through the valid pages until it finds one where
        // the referenced bit is 0, then set it as the swap_target
        for (i = programs[pid].hand; ; i++) {
            int hand = i % num_pages;
            if (page_table[hand].valid) {
                if (page_table[hand].referenced) {
                    programs[pid].pt[hand].referenced = 0;
                }
                else {
                    swap_target = hand;
                    // Advance the hand before the swap
                    programs[pid].hand = (++hand) % num_pages;
                    break;
                }
            }
        } 
    }

    // Perform the swap
    programs[pid].pt[swap_target].valid = 0;
    programs[pid].pt[page_num].valid = 1;
                
    // For FIFO, update counter of the loaded page
    // to indicate relative load time
    if (replace_alg == 0) {
        programs[pid].pt[page_num].counter = ++global_counter;
    }
    
    // These values are not updated for the contiguous page in pre-paging
    if (!prepaging) {
        // For LRU, update counter of the page
        // to indicate relative use time
        if (replace_alg == 1) {
            programs[pid].pt[page_num].counter = ++global_counter;
        }
        // For Clock, set the referenced bit to 1
        else if (replace_alg == 2) {
            programs[pid].pt[page_num].referenced = 1;
        }
    }
    
    printf("Process %d: Swap page %d for page %d\n", pid, page_table[swap_target].page_id, page_table[page_num].page_id);
}

int main(int argc, char* argv[]) {
    int i, j;
    int page_counter = 0; // counter to give pages unique ids
    int total_swaps = 0; // counter to keep track of number of swaps

    // Check that there is a proper amount of parameters
    if (argc != 6) {
        printf("%s [plist] [ptrace] [page size] [FIFO/LRU/Clock] [pre-paging (+/-)]\n", argv[0]);
        return -1;
    }

    // P1: Size of pages, i.e., # of memory locations on each page.
    int page_size = atoi(argv[3]);

    // P2: FIFO, LRU, or Clock for type of page replacement algorithms
    int replace_alg;
    if (strcmp(argv[4], "FIFO") == 0) {
        replace_alg = 0;
    }
    else if (strcmp(argv[4], "LRU") == 0) {
        replace_alg = 1;
    }
    else if (strcmp(argv[4], "Clock") == 0) {
        replace_alg = 2;
    }
    else {
        printf("Invalid page replacement algorithm \"%s\"\n Choose from [FIFO/LRU/Clock]\n", argv[4]);
        return -1;
    }

    // P3: flag to turn on/off pre-paging. If pre-paging is not turned on, we use demand paging by
    // default.
    int prepaging;
    if (strcmp(argv[5], "+") == 0)
        prepaging = 1;
    else if (strcmp(argv[5], "-") == 0)
        prepaging = 0;
    else {
        printf("Prepaging parameter must be + or -\n");
        return -1;
    }

    // Open plist file
    FILE* plist = fopen(argv[1], "r");
    if (plist == NULL) {
        printf("plist file \"%s\" is missing\n", argv[1]);
        return -1;
    }

    int num_programs = 0, pid, memloc;
    while (fscanf(plist, "%d %d\n", &pid, &memloc) != EOF) {
        num_programs++;
    }

    programs = calloc(num_programs, sizeof(program));

    rewind(plist);

    int prog_count = 0;
    while (fscanf(plist, "%d %d\n", &pid, &memloc) != EOF) {
        int num_pages = (int)ceil((double)memloc/page_size);
        programs[prog_count] = (program) {
            .pid = pid,
            .num_pages = num_pages,
            .pt = calloc(num_pages, sizeof(page)),
            .hand = 0
        };
        for (j = 0; j < num_pages; j++) {
            programs[prog_count].pt[j].page_id = ++page_counter;
        }
        prog_count++;
    }

    fclose(plist);


    // Determine the number of available pages
    int available_pages = MAIN_MEMORY_SIZE/page_size;
    
    // Divide the pages equally among the program
    int pages_per_program = available_pages/num_programs;
    
    // Initialize array that will record the pages in memory
    pages_in_memory = calloc(available_pages, sizeof(int));

    // Perform a default loading of memory
    for (i = 0; i < num_programs; i++) {
        // Determine the number of pages each program should get into its assigned memory part
        // Take the min in case program is smaller than default load size
        int initial_num_pages = MIN(pages_per_program, programs[i].num_pages);

        for (j = 0; j < initial_num_pages; j++) {
            programs[i].pt[j].valid = 1;
            pages_in_memory[i*pages_per_program + j] = programs[i].pt[j].page_id;
            if (replace_alg == 0) { /* FIFO */
                programs[i].pt[j].counter = ++global_counter;
            }
            else if (replace_alg == 2) { /* Clock */
                programs[i].pt[j].referenced = 1;
            }
        }
    }
    
    for (i = 0; i < available_pages; i++) {
        printf("%d ", pages_in_memory[i]);
    }

    FILE* ptrace = fopen(argv[2], "r");
    if (ptrace == NULL) {
        printf("ptrace file \"%s\" is missing\n", argv[2]);
        return -1;
    }

    int line = 0;
    while (fscanf(ptrace, "%d %d\n", &pid, &memloc) != EOF) {
        int page_num = (int)ceil((double)memloc/page_size) - 1; 

        // printf("%d: Requesting page %d...", ++line, programs[pid].pt[page_num].page_id);
        // Check if this page is in memory
        if (!programs[pid].pt[page_num].valid) { // There is a page fault, so load the page into the memory    
            printf("not in memory, loading page\n");
            load_page(pid, page_num, replace_alg, 0);
            total_swaps++; // Increment the number of swaps            

            // Demand paging is done by default. If pre-paging is enabled, load the next page not in memory
            if (prepaging && pages_per_program != 1) {
                for (i = (page_num + 1) % programs[pid].num_pages; i != page_num; i = (i+1) % programs[pid].num_pages) {
                    if (!programs[pid].pt[i].valid) {
                        load_page(pid, i, replace_alg, 1);
                        break;
                    }
                }
            }
        }
        else {
            printf("in memory!\n");
            // For LRU, update counter of the page
            // to indicate relative use time
            if (replace_alg == 1) {
                programs[pid].pt[page_num].counter = ++global_counter;
            }
            // For Clock, set the referenced bit to 1
            else if (replace_alg == 2) {
                programs[pid].pt[page_num].referenced = 1;
            }
        }
    }

    fclose(ptrace);


    for (i = 0; i < num_programs; i++) {
        printf("pid %d has %d pages:\n", programs[i].pid, programs[i].num_pages);
        for (j = 0; j < programs[i].num_pages; j++) {
            printf("%d: %d\t", programs[i].pt[j].page_id, programs[i].pt[j].valid);
        }
        printf("\n");
    }
    printf("Total swaps: %d\n", total_swaps);
    printf("Memory: ");
    for (i = 0; i < available_pages; i++) {
        printf("%d ", pages_in_memory[i]);
    }

    for (i = 0; i < num_programs; i++) {
        free(programs[i].pt);
    }

    free(programs);
}
