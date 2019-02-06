#include <stdio.h>
#include <stdlib.h>
#include "kallocator.h"

int main(int argc, char* argv[]) {
    initialize_allocator(100, WORST_FIT);
    // initialize_allocator(100, BEST_FIT);
    // initialize_allocator(100, WORST_FIT);
    printf("Using best fit algorithm on memory size 100\n");

    int* p[50] = {NULL};
    for(int i=0; i<10; ++i) {
//        p[i] = kalloc((rand() % 6) + 1);
        p[i] = kalloc(sizeof(int));
        if(p[i] == NULL) {
            printf("Allocation failed\n");
            continue;
        }
        *(p[i]) = i;
        printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
    }

    /*
    for(int i=0; i<10; ++i) {
        if(i%2 == 0)
            continue;

        printf("Freeing p[%d]\n", i);
        kfree(p[i]);
        p[i] = NULL;
    } */
    
    print_statistics();
    
    kfree(p[0]);
    p[0] = NULL;
    kfree(p[5]);
    p[5] = NULL;
    kfree(p[7]);
    p[7] = NULL;
    kfree(p[9]);
    p[9] = NULL;
    print_lists();



    printf("AFTER FREE's:\n");
    print_lists();

    for (int i=0; i<10; ++i) {
        int random = (rand() % 6) + 1;
        printf("p[%d] : insert block size: %d\n", i, random);
        p[i] = kalloc(random);
//        p[i] = kalloc(sizeof(int));
        if(p[i] == NULL) {
            printf("Allocation failed\n");
            continue;
        }
        *(p[i]) = i;
        printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
        print_lists();
    }
    
    printf("available_memory %d\n\n", available_memory());

    void* before[100] = {NULL};
    void* after[100] = {NULL};
//    print_lists();
//    printf("\n\n");
    int compact = compact_allocation(before, after);
    printf("compact = %d\n", compact);
    print_lists();

    print_statistics();
    

    // You can assume that the destroy_allocator will always be the 
    // last funciton call of main function to avoid memory leak 
    // before exit

    destroy_allocator();

    return 0;
}
