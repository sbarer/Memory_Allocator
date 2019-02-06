#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "kallocator.h"

#define EMPTY 0
#define LARGE 10000000

typedef struct nodeStruct {
    int size;
    void* pointer;
    struct nodeStruct *next;
} nodeStruct;

struct KAllocator {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    nodeStruct* freeLL;
    nodeStruct* fullLL;
};

struct KAllocator kallocator;

struct nodeStruct* List_createNode(int item)
{
    struct nodeStruct *pNode = malloc(sizeof(struct nodeStruct));
    if (pNode != NULL) {
        pNode->size = item;
        pNode->pointer = NULL;
        pNode->next = NULL;
    }
    return pNode;
}


void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    kallocator.aalgorithm = _aalgorithm;
    kallocator.size = _size;
    kallocator.memory = malloc((size_t)kallocator.size);
    // Add some other initialization
    kallocator.freeLL = List_createNode(_size);
    kallocator.freeLL->pointer = kallocator.memory;
    kallocator.fullLL = NULL;
}

void destroy_allocator() {
    // delete freeLL
    while (kallocator.freeLL != NULL) {
        nodeStruct* dNode = kallocator.freeLL;
        kallocator.freeLL = kallocator.freeLL->next;
        free(dNode);
    }
    
    // delete fullLL
    while (kallocator.fullLL != NULL) {
        nodeStruct* dNode = kallocator.fullLL->next;
        kallocator.fullLL = kallocator.fullLL->next;
        free(dNode);
    }
    
    // delete kallocator
    free(kallocator.memory);
}


void* kalloc(int _size) {
    void* ptr = NULL;
    nodeStruct* newNode = List_createNode(_size);
    nodeStruct* currNodeFree = NULL;
    nodeStruct* prevCurrNode = NULL;
    nodeStruct* findNodeFree = kallocator.freeLL;
    nodeStruct* prevNodeFree = kallocator.freeLL;
    nodeStruct* findNodeFull = kallocator.fullLL;
    nodeStruct* prevNodeFull = kallocator.fullLL;
    _Bool assigned = false;
    // Allocate memory from kallocator.memory 
    // ptr = address of allocated memory

    // allocate memory based on kAllocator_algorithm:
        // FIRST_FIT, BEST_FIT, WORST_FIT
    if (kallocator.aalgorithm == FIRST_FIT) {
        while (findNodeFree != NULL) {
            if (_size <= findNodeFree->size) {
                // allocate the required memory to newNode
                newNode->pointer = findNodeFree->pointer;
                // Update findNode to new size & pointer
                findNodeFree->size -= _size;
                findNodeFree->pointer += newNode->size;
                
                break;
            } else {
                prevNodeFree = findNodeFree;
                findNodeFree = findNodeFree->next;
            }
        }
        
        if (findNodeFree == NULL) {
            return NULL;
        }
        
    } else if (kallocator.aalgorithm == BEST_FIT) {
        double minRemaining = LARGE;
        
        // find correct node for kallocation
        while (findNodeFree != NULL) {
            if (_size <= findNodeFree->size &&
                (findNodeFree->size - _size) < minRemaining)
            {
                minRemaining = findNodeFree->size - _size;
                // set node to keep track of best_fit node
                currNodeFree = findNodeFree;
                prevCurrNode = prevNodeFree;
            }
            prevNodeFree = findNodeFree;
            findNodeFree = findNodeFree->next;
        }
        
        // check to make sure ptr can be allocated
        if (minRemaining == LARGE) {
            printf("There is no block of free memory in the allocated memory space that can fit a kalloc of size %d.\n", _size);
            return NULL;
        }
        // if taking from first block of free memory
        else if (kallocator.freeLL->next == NULL) {
            findNodeFree = kallocator.freeLL;
        } else {
            findNodeFree = currNodeFree;
            prevNodeFree = prevCurrNode;
        }
        
        // allocate the required memory to newNode
        newNode->pointer = findNodeFree->pointer;
        // resize findNode to new size & pointer
        findNodeFree->size -= _size;
        findNodeFree->pointer += newNode->size;
        
    } else if (kallocator.aalgorithm == WORST_FIT) {
        double maxRemaining = EMPTY;
        
        // find correct node for kallocation
        while (findNodeFree != NULL) {
            if (_size <= findNodeFree->size &&
                findNodeFree->size - _size > maxRemaining)
            {
                maxRemaining = findNodeFree->size - _size;
                // set node to keep track of best_fit node
                currNodeFree = findNodeFree;
                prevCurrNode = prevNodeFree;
            }
            prevNodeFree = findNodeFree;
            findNodeFree = findNodeFree->next;
        }
        
        // check to make sure ptr can be allocated
        if (maxRemaining == EMPTY && currNodeFree == NULL) {
            printf("There is no block of free memory in the allocated memory space that can fit a kalloc of size %d.\n", _size);
            return NULL;
        }
        // if taking from first block of free memory
        else if (kallocator.freeLL->next == NULL) {
            findNodeFree = kallocator.freeLL;
        } else {
            findNodeFree = currNodeFree;
            prevNodeFree = prevCurrNode;
        }
        
        // allocate the required memory to newNode
        newNode->pointer = findNodeFree->pointer;
        // resize findNode to new size & pointer
        findNodeFree->size -= _size;
        findNodeFree->pointer += newNode->size;
    }
    
    
    // Insert newNode into fullLL
    // if fullLL is empty
    if (kallocator.fullLL == NULL) {
        kallocator.fullLL = newNode;
        assigned = true;
    }
    // else if newNode belongs at front of fullLL
    else if (newNode->pointer < kallocator.fullLL->pointer) {
        newNode->next = kallocator.fullLL;
        kallocator.fullLL = newNode;
        assigned = true;
    } else {
        while (newNode->pointer > findNodeFull->pointer) {
            prevNodeFull = findNodeFull;
            findNodeFull = findNodeFull->next;
            if (findNodeFull == NULL) { break; }
        }
        newNode->next = findNodeFull;
        prevNodeFull->next = newNode;
        
        assigned = true;
    }
    
    // set ptr to newNode
    if (assigned == true) {
        ptr = newNode->pointer;
    }
    
    // remove node if empty
    if (findNodeFree->size == 0) {
        if (kallocator.freeLL->next == NULL) {
            kallocator.freeLL = NULL;
        } else if (kallocator.freeLL->size == 0) {
            kallocator.freeLL = findNodeFree->next;
        } else {
            prevNodeFree->next = findNodeFree->next;
        }
        
        free(findNodeFree);
    }
    
    return ptr;
}

void kfree(void* _ptr) {
    assert(_ptr != NULL);
    nodeStruct* freeNode = NULL;
    nodeStruct* findNodeFree = kallocator.freeLL;
    nodeStruct* prevNodeFree = kallocator.freeLL;
    nodeStruct* findNodeFull = kallocator.fullLL;
    nodeStruct* prevNodeFull = kallocator.fullLL;
    
    // find corresponding node for _ptr & remove from fullLL
    
    // check to make sure ptr was valid
    if (findNodeFull == NULL) {
        printf("%p is not within the allocated memory space.\n", _ptr);
        return;
    }
    // if _ptr matches first node in fullLL
    else if (_ptr == findNodeFull->pointer) {
        freeNode = findNodeFull;
        kallocator.fullLL = findNodeFull->next;
    }
    // check if _ptr matches any other nodes in fullLL
    else {
        findNodeFull = findNodeFull->next;
        while (findNodeFull != NULL) {
            if (findNodeFull->pointer == _ptr) {
                freeNode = findNodeFull;
                prevNodeFull->next = findNodeFull->next;
                break;
            } else {
                prevNodeFull = findNodeFull;
                findNodeFull = findNodeFull->next;
            }
        }
    }
    
    
    // find insertion point for freeNode in freeLL
    
    // empty list
    if (kallocator.freeLL == NULL) {
        kallocator.freeLL = freeNode;
    }
    // freeNode belongs at front of list
    else if (freeNode->pointer < findNodeFree->pointer) {
        freeNode->next = kallocator.freeLL;
        kallocator.freeLL = freeNode;
    }
    // freeNode belongs at middle or end of list
    else {
        findNodeFree = findNodeFree->next;
        while (findNodeFree != NULL) {
            if (freeNode->pointer < findNodeFree->pointer) {
                break;
            }
            prevNodeFree = findNodeFree;
            findNodeFree = findNodeFree->next;
        }
        freeNode->next = findNodeFree;
        prevNodeFree->next = freeNode;
    }
    
    // CONTIGUOUS BLOCK CONDITION
    
    // if prevNodeFree & freeNode & findNodeFree need to be merged
    if( prevNodeFree->pointer + prevNodeFree->size == freeNode->pointer
       && freeNode->pointer + freeNode->size == findNodeFree->pointer) {
        // allocate size of freeNode * findNodeFree to prevNodeFree
        prevNodeFree->size += freeNode->size + findNodeFree->size;
        // delink freeNode & findNodeFree
        prevNodeFree = findNodeFree->next;
        // deallocate memory for freeNode & findNodeFree
        free(freeNode);
        free(findNodeFree);
    }
    // if prevNodeFree & freeNode need to be merged
    else if (prevNodeFree->pointer + prevNodeFree->size == freeNode->pointer) {
        // allocate size of freeNode to prevNodeFree
        prevNodeFree->size += freeNode->size;
        // delink freeNode
        prevNodeFree->next = freeNode->next;
        // deallocate memory for freeNode
        free(freeNode);
    }
    // else if freeNode & findNodeFree need to be merged
    else if (freeNode->pointer + freeNode->size == findNodeFree->pointer) {
        // allocate size of of findNodeFree to freeNode
        freeNode->size += findNodeFree->size;
        // delink findNodeFree
        freeNode->next = findNodeFree->next;
        // deallocate memory for freeNode
        free(findNodeFree);
    }

    _ptr = NULL;
}

int compact_allocation(void** _before, void** _after) {
    int compacted_size = 0;
    
    nodeStruct* findNodeFree = kallocator.freeLL;
    nodeStruct* findNodeFull = kallocator.fullLL;

    // while there are free blocks to compact into
    while (findNodeFree != NULL) {
        // while there are fullNodes left to be compacted left
        while (findNodeFull != NULL) {
            // swap places of fullNode and freeNode
            if (findNodeFull->pointer > findNodeFree->pointer) {
                // assign _before pointer
                _before[compacted_size] = findNodeFull->pointer;
                
                // swap places of nodes
                findNodeFull->pointer = findNodeFree->pointer;
                findNodeFree->pointer = findNodeFree->pointer + findNodeFull->size;
                
                // increment compacted_size and assign _after pointer
                compacted_size++;
                _after[compacted_size] = findNodeFull->pointer;

            }
            
            findNodeFull = findNodeFull->next;
        }
        
        findNodeFree = findNodeFree->next;
    }
    
    // merge all freeNodes into one large freeNode
    // ensure there are free nodes to merge
    if (kallocator.freeLL != NULL) {
        nodeStruct* mergeNodeFree = kallocator.freeLL->next;
        while (mergeNodeFree != NULL) {
            kallocator.freeLL->size += mergeNodeFree->size;
            nodeStruct* deleteNode = mergeNodeFree;
            mergeNodeFree = mergeNodeFree->next;
            free(deleteNode);
        }
        kallocator.freeLL->next = NULL;
    }
    
    return compacted_size;
}

int available_memory() {
    int available_memory_size = 0;
    nodeStruct* findNodeFree = kallocator.freeLL;
    
    // Calculate available memory size
    while (findNodeFree != NULL) {
        available_memory_size += findNodeFree->size;
        findNodeFree = findNodeFree->next;
    }
    
    return available_memory_size;
}

void print_statistics() {
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = kallocator.size;
    int largest_free_chunk_size = 0;
    nodeStruct* findNodeFree = kallocator.freeLL;
    nodeStruct* findNodeFull = kallocator.fullLL;
    
    // Calculate the statistics
    while (findNodeFull != NULL) {
        // allocated_size
        allocated_size += findNodeFull->size;
        // allocated_chunks
        allocated_chunks++;
        // iterate through list
        findNodeFull = findNodeFull->next;
    }
    
    while (findNodeFree != NULL) {
        // free_size
        free_size += findNodeFree->size;
        // free_chunks
        free_chunks++;
        // smallest_free_chunk_size
        if (findNodeFree->size < smallest_free_chunk_size) {
            smallest_free_chunk_size = findNodeFree->size;
        }
        // largest_free_chunk_size
        if (findNodeFree->size > largest_free_chunk_size) {
            largest_free_chunk_size = findNodeFree->size;
        }
        // iterate through list
        findNodeFree = findNodeFree->next;
    }

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}

void print_lists() {
    printf("freeLL:");
    nodeStruct* node = kallocator.freeLL;
    while (node != NULL) {
        printf("{%d, %p} -> ", node->size, node->pointer);
        node = node->next;
    }
    printf ("{NULL}\n");
    
    printf("fullLL:");
    node = kallocator.fullLL;
    while (node != NULL) {
        printf("[%d, %p] -> ", node->size, node->pointer);
        node = node->next;
    }
    printf ("[NULL]\n\n");
}

