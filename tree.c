#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
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
      l->bf = l -> level;
    }

  } else if(l->right) {//has right subtree but no left subtree
    calBf(l->right);
    l->level = l->right->level + 1;
    l->bf = -(l->level);
  } else {//has no subtree, terminal condition
    l->level = 0;
    l->bf = 0;
  }
  return;
}

typedef struct {
  int size;
  int count;
  int *direction;
} Route;

Route *getRoute(int size) {
  Route *r = (Route*)malloc(sizeof(Route));
  r->size = size;
  r->count = 0;
  r->direction = (int*)malloc(sizeof(int)*size);
  return r;
}

void addToRoute(Route *r, int in_dir) {
  //in_dir: input direction
  if(r->count == r->size) {
    int newSize = r->size * 2;
    int *newDirection = (int*)malloc(newSize*sizeof(int));
    memcpy(newDirection, r->direction, r->size*sizeof(int));
    free(r->direction);
    r->direction = newDirection;
    r->size= newSize;
  }
  r->direction[r->count++] = in_dir;
  return;
}

void delRoute(Route *r) {
  if(!r)
    return;
  if(r->direction)
    free(r->direction);
  free(r);
  return;
}

void adjustToAvl(Tree *t, Route *r) {
  calBf(t->root);
  
  Leaf **lptr = &(t->root);
  int counter = -1;
  
  Leaf **pivot = NULL;
  int pivot_flag = -1;
  
  do{
    if( (*lptr)->bf > 1 || (*lptr)->bf < -1) {
      pivot = &(*lptr);
      pivot_flag = counter;
    }
    
    counter++;

    if( r->direction[counter] == 0)
      lptr = &((*lptr)->left);
    else
      lptr = &((*lptr)->right);

  }while(counter < r->count);

  
  if(pivot) {
    printf("pivot: %d\n", *((int*)(*pivot)->data) );
    //just for demo, you need to delete this line
    if(pivot_flag + 2 >= r->count) {
      printf("sorry, a bug here!");
      return;
    }
    
    if(r->direction[pivot_flag+1] == 0) {
      if(r->direction[pivot_flag+2] == 0) {
        //LL case
        printf("LL case!\n");
        Leaf *left_temp = (*pivot)->left;
        (*pivot)->left  = ((*pivot)->left)->right;
        left_temp->right = (*pivot);
        (*pivot) = left_temp;
      } else {
        //LR case
        printf("LR case!\n");
        Leaf *left_temp = (*pivot)->left;
        Leaf *left_right_temp = ((*pivot)->left)->right;
        
        if(! (left_right_temp->left) ) {
          left_temp->right = NULL;
        } else {
          left_temp->right = left_right_temp->left;
        }

        if(! (left_right_temp->right) ) {
          (*pivot)->left = NULL;
        } else {
          (*pivot)->left = left_right_temp->right;
        }

        left_right_temp->left = left_temp;
        left_right_temp->right = (*pivot);

        (*pivot) = left_right_temp;
      }
    } else {
      if(r->direction[pivot_flag+2] == 0) {
        //RL case
        printf("RL case!\n");
        Leaf *right_temp = (*pivot)->right;
        Leaf *right_left_temp = ((*pivot)->right)->left;

        if( !(right_left_temp->left) )  {
          (*pivot)->right = NULL;
        } else {
          (*pivot)->right = right_left_temp->left;
        }
        
        if( !(right_left_temp->right) ) {
          right_temp->left = NULL;
        } else {
          right_temp->left = right_left_temp->right;
        }
        
        right_left_temp->right = right_temp;
        right_left_temp->left = (*pivot);
        
        (*pivot) = right_left_temp;
      } else {
        //RR case
        printf("RR case!\n");
        Leaf *right_temp = (*pivot)->right;
        (*pivot)->right  = ((*pivot)->right)->left;
        right_temp->left = (*pivot);
        (*pivot) = right_temp;
      }
    }      
  }//end of if(pivot)

  return;  
}

void addLeafToTree(HTree t, void *data) {
  if(!t || !data) 
    return;

  Tree *tptr = (Tree*)t;

  Route *r = getRoute(2);
  Leaf **find = &(tptr->root);
  while( (*find ) != NULL) {
    if( tptr->compar(data, (*find)->data) > 0 ) {
      addToRoute(r, 1);
      find = &((*find)->right);
    } else {
      addToRoute(r, 0);
      find = &((*find)->left);
    }
  }
  *find = getLeaf(data);
  adjustToAvl(tptr, r); 
  delRoute(r);
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
  //for demotration purpose, need to delete later
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
  *data1 = 111;
  int *data2 = (int*)malloc(sizeof(int));
  *data2 = 15;
  int *data3 = (int*)malloc(sizeof(int));
  *data3 = 24;
  int *data4 = (int*)malloc(sizeof(int));
  *data4 = 55;
  int *data5 = (int*)malloc(sizeof(int));
  *data5 = 89;
  int *data6 = (int*)malloc(sizeof(int));
  *data6 = 99;
  int *data7 = (int*)malloc(sizeof(int));
  *data7 = 12;
  int *data8 = (int*)malloc(sizeof(int));
  *data8 = 1;
  int *data9 = (int*)malloc(sizeof(int));
  *data9 = 8;
  int *data10 = (int*)malloc(sizeof(int));
  *data10 = 7;
  int *data11 = (int*)malloc(sizeof(int));
  *data11 = 9;
  int *data12 = (int*)malloc(sizeof(int));
  *data12 = 121;
  int *data13 = (int*)malloc(sizeof(int));
  *data13 = 34;
  int *data14 = (int*)malloc(sizeof(int));
  *data14 = 31;
  int *data15 = (int*)malloc(sizeof(int));
  *data15 = 22;
  int *data16 = (int*)malloc(sizeof(int));
  *data16 = 3;

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
  addLeafToTree(t, data16);

  printTree(t);
  delTree(t);
  return 0;
}
