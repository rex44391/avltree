#ifndef TREE_H
#define TREE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void* HTree;

HTree getTree(int (*compar)(const void*, const void*),
              void (*print)(const void*));
void addLeafToTree(HTree, void *);
void printTree(HTree); 

#ifdef __cplusplus
}
#endif

#endif
