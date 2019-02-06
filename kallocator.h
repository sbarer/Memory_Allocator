#ifndef __KALLOCATOR_H__
#define __KALLOCATOR_H__

enum allocation_algorithm {FIRST_FIT, BEST_FIT, WORST_FIT};

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm);

struct nodeStruct* List_createNode(int item);
void* kalloc(int _size);
void kfree(void* _ptr);
int available_memory(void);
void print_statistics(void);
int compact_allocation(void** _before, void** _after);
void destroy_allocator(void);
void print_lists(void);

#endif
