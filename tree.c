#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "tree.h"


//the two functions below are for testing purpose
//the user need to define a compare and print funtion on his own
static int mycompar(const void *a, const void *b) {
  return (*(int*)a) - *((int*)b);
}
static void myprint(const void *a) {
  printf("%d", *((int*)a));
}

typedef struct leaf {
  void *data;
  int bf;//balanced factor
  int level;
  struct leaf *left;
  struct leaf *right;
} Leaf;

Leaf *getLeaf(void *data) {
  Leaf *l = (Leaf*)malloc(sizeof(Leaf));
  l->data = data;
  l->left = NULL;
  l->right = NULL;
  return l;
}

void delLeaf(Leaf *l) {
  if(l) {
    if(l->data) {
      free(l->data);
    }
    free(l);
  }
}

typedef struct _Tree {
  Leaf *root;
  int (*compar)(const void*, const void*);
  //0: left == right, >0: left > right, <0: right > left
  //<=0: goto left subtree >0: goto right subtree
  
  void (*print)(const void*);//a function used to print data pointed by void*
} Tree;

HTree getTree(int (*compar)(const void*, const void*),
            void (*print)(const void*)) {
  Tree *t = (Tree*)malloc(sizeof(Tree));
  t->root = NULL;
  t->compar = compar;
  t->print = print;
  return ((void*)t);
}

void calBf(Leaf *l) {
  if(!l)
    return;
  
  if(l->left) {
    calBf(l->left);
    l->level = l->left->level + 1;
    if(l->right) {
      calBf(l->right);
      if(l->right->level > l->left-> level) {
        l->level = l->right->level + 1;
      }
      l->bf = l->left->level - l->right->level;
    } else {//has left subtree but no right subtree
      l->level = l->left->level + 1;
      l->bf = l->left->level;
    }

  } else if(l->right) {//has right subtree but no left subtree
    calBf(l->right);
    l->level = l->right->level + 1;
    l->bf = -(l->right->level);
  } else {//has no subtree, terminal condition
    l->level = 0;
    l->bf = 0;
  }
  return;
}


void addLeafToTree(HTree t, void *data) {
  if(!t || !data) 
    return;

  Tree *tptr = (Tree*)t;
  Leaf **find = &(tptr->root);
  while( (*find ) != NULL) {
    if( tptr->compar(data, (*find)->data) > 0 ) {
      find = &((*find)->right);
    } else {
      find = &((*find)->left);
    }
  }
  *find = getLeaf(data);
  return;
}

void inOrder(Tree *tptr, Leaf *l) {
  if(!l)
    return;
  
  if(l->left) 
    inOrder(tptr, l->left);
  tptr->print(l->data);
  printf(",");
  if(l->right) 
    inOrder(tptr, l->right);

  return;
}

void preOrder(Tree *tptr, Leaf *l) {
  if(!l)
    return;
  
  tptr->print(l->data);
  printf(",");
  if(l->left) 
    preOrder(tptr, l->left);
  if(l->right) 
    preOrder(tptr, l->right);

  return;
}

void printTree(HTree t) {
  if(!t)
    return;

  Tree *tptr = (Tree*)t;
  printf("InOrder Seq: ");
  inOrder(tptr, tptr->root);
  printf("\n");
  printf("preOrder Seq: ");
  preOrder(tptr, tptr->root);
  printf("\n");
}

void delLeafFromTree(HTree t, void *del_data) {
  if(!t)
    return;
  
  Tree* tptr = (Tree*)t;
  Leaf **find = &(tptr->root);
  while((*find) != NULL) {
    int comp_result = tptr->compar(del_data, (*find)->data);
    if(comp_result == 0)  {
      break;
    } else if(comp_result > 0) {
      find = &((*find)->right);
    } else {
      find = &((*find)->left);
    }
  }
  if(!(*find)) {
    printf("Sorry, can't find your data!\n");
    return;
  }
  
  if( (*find)->left) {
    if( !((*find)->right)) {
      //has left subtree, but no right subtree
      Leaf *temp = *find;
      *find = (*find)->left;
      delLeaf(temp);
    } else {
      //has left and right subtree
      //find the biggest one in left subtree
      Leaf **replaced = &((*find)->left);
      while((*replaced)->right != NULL) {
        replaced = &((*replaced)->right);
      }
      (*find)->data = (*replaced)->data;
      (*replaced)->data = NULL;

      Leaf *temp = *replaced;
      (*replaced) = (*replaced)->left;
      delLeaf(temp);
    }
  } else {
    //if there is no left subtree
    Leaf *temp = *find;
    *find = (*find)->right;
    delLeaf(temp);
  }

}


void recurDel(Leaf *l) {
  if(!l)
    return;
  
  if(l->left)
    recurDel(l->left);
  if(l->right)
    recurDel(l->right);
  printf("delete:%d, level: %d, bf: %d\n", 
      *((int*)l->data), l->level, l->bf);
  delLeaf(l);
  l = NULL;
  return;
}

void delTree(HTree t) {
  if(!t)
    return;
  Tree *tptr = (Tree*)t;
  calBf(tptr->root);

  recurDel(tptr->root);
  tptr->compar = NULL;
  tptr->print = NULL;
  t = NULL;
  return;
}

//for demo purpose
int main() {
  void *t = getTree(mycompar, myprint);
  int *data1 = (int*)malloc(sizeof(int));
  *data1 = 7;
  int *data2 = (int*)malloc(sizeof(int));
  *data2 = 5;
  int *data3 = (int*)malloc(sizeof(int));
  *data3 = 4;
  int *data4 = (int*)malloc(sizeof(int));
  *data4 = 3;
  int *data5 = (int*)malloc(sizeof(int));
  *data5 = 2;
  int *data6 = (int*)malloc(sizeof(int));
  *data6 = 1;
  int *data7 = (int*)malloc(sizeof(int));
  *data7 = 10;
  int *data8 = (int*)malloc(sizeof(int));
  *data8 = 6;
  int *data9 = (int*)malloc(sizeof(int));
  *data9 = 8;
  int *data10 = (int*)malloc(sizeof(int));
  *data10 = 9;
  int *data11 = (int*)malloc(sizeof(int));
  *data11 = 11;
  int *data12 = (int*)malloc(sizeof(int));
  *data12 = 12;
  int *data13 = (int*)malloc(sizeof(int));
  *data13 = 6;
  int *data14 = (int*)malloc(sizeof(int));
  *data14 = 6;
  int *data15 = (int*)malloc(sizeof(int));
  *data15 = 6;

  addLeafToTree(t, data1);
  addLeafToTree(t, data2);
  addLeafToTree(t, data3);
  addLeafToTree(t, data4);
  addLeafToTree(t, data5);
  addLeafToTree(t, data6);
  addLeafToTree(t, data7);
  addLeafToTree(t, data8);
  addLeafToTree(t, data9);
  addLeafToTree(t, data10);
  addLeafToTree(t, data11);
  addLeafToTree(t, data12);
  addLeafToTree(t, data13);
  addLeafToTree(t, data14);
  addLeafToTree(t, data15);
  printTree(t);
  
  int *data16 = (int*)malloc(sizeof(int));
  *data16 = 1;
  //delLeafFromTree(t, data16);
  printTree(t);
  delTree(t);
  return 0;
}
