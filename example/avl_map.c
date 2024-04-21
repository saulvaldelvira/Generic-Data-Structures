/**
 * Example of using an AVL Tree as a map of Key-Value pairs.
*/
#include "../src/AVLTree.h"
#include <stdio.h>

struct Pair{
        int key;
        char value;
};

int compare_pair(const void *e_1, const void *e_2){
	struct Pair n_1 = * (struct Pair *) e_1;
	struct Pair n_2 = * (struct Pair *) e_2;
	if (n_1.key > n_2.key){
		return 1;
	}else if (n_2.key > n_1.key){
		return -1;
	}else{
		return 0;
	}
}

int main(void){
        AVLTree *tree = avl_init(sizeof(struct Pair), compare_pair);

        struct Pair p;
        for (int i=0; i < 50; i++){
                p.key = i;
                p.value = 0x30 + i;
                avl_add(tree, &p);
        }

        for (int i=0; i < 50; i++){
                p.key = i;
                struct Pair result;
                avl_get(tree, &p, &result);
                printf("%d -> %c\n", result.key, result.value);
        }

        avl_free(tree);
        return 0;
}
