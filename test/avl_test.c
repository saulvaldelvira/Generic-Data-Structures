#define QUIET_DISABLE
#include "test.h"
#undef QUIET_DISABLE

#include "../src/AVLTree/AVLTree.h"

void print_preord(AVLTree *tree){
        void *preord = avl_preorder(tree);
        for (size_t i = 0; i < avl_size(tree); i++){
                void *tmp = void_offset(preord, i * sizeof(int));
                printf("%d-",* (int*) tmp);
        }
        printf("\n");
        free(preord);
}

void assert_preord(AVLTree *tree, int exp[]){
        void *preord = avl_preorder(tree);
        for (size_t i = 0; i < avl_size(tree); i++){
                void *tmp = void_offset(preord, i * sizeof(int));
                assert(exp[i] == * (int*) tmp);
        }
        free(preord);
}

int main(){
        printf("[AVLTree Test]\n");
        TIMESTAMP_START
        AVLTree *t = avl_init(sizeof(int), compare_int);
        assert(avl_size(t) == 0UL);
        assert(avl_isempty(t));
        assert(-1 == avl_height(t));

        int nums[] = {10, 6, 15, 3, 9, 14, 20, 2, 4, 7, 12};

        assert(avl_add_array(t, nums, 7UL));

        int exp1 [] = {10, 6, 3, 9, 15, 14, 20};
        assert_preord(t, exp1);

        assert(!avl_isempty(t));

        assert(avl_add_array(t, &nums[7], 4UL));

        int exp2 [] = {10, 6, 3, 2, 4, 9, 7, 15, 14, 12, 20};
        assert_preord(t, exp2);

        assert(3 == avl_height(t));

        assert(avl_add(t, cast_int(1)));
        int exp3 [] = {10, 6, 3, 2, 1, 4, 9, 7, 15, 14, 12, 20};
        assert_preord(t, exp3);

        assert(4 == avl_height(t));

        assert(avl_remove(t, cast_int(20)));
        int exp4 [] = {6, 3, 2, 1, 4, 10, 9, 7, 14, 12, 15};
        assert_preord(t, exp4);

        assert(avl_remove(t, cast_int(4)));
        int exp5 [] = {6, 2, 1, 3, 10, 9, 7, 14, 12, 15};
        assert_preord(t, exp5);

        assert(avl_remove(t, cast_int(10)));
        int exp6 [] = {6, 2, 1, 3, 9, 7, 14, 12, 15};
        assert_preord(t, exp6);

        assert(avl_remove(t, cast_int(9)));
        int exp7 [] = {6, 2, 1, 3, 14, 7, 12, 15};
        assert_preord(t, exp7);

        assert(avl_remove(t, cast_int(6)));
        int exp8 [] = {3, 2, 1, 14, 7, 12, 15};
        assert_preord(t, exp8);

        assert(avl_remove(t, cast_int(3)));
        int exp9 [] = {7, 2, 1, 14, 12, 15};
        assert_preord(t, exp9);

        t = avl_reset(t);

        assert(avl_add_array(t, nums, 11UL));
        assert(avl_remove_array(t, nums, 11UL));
        assert(avl_isempty(t));


        avl_free(t);
        TIMESTAMP_STOP
        
        END_MSG(AVLTree);

        return 0;
}