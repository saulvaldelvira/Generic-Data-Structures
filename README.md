# Generic-Data-Structures
A set of generic Data Structures implemented in C. <br>
It includes:
* Lists: [ArrayList](/src/ArrayList), [LinkedList](/src/LinkedList)
* [Stack](src/Stack)
* [Queue](src/Queue)
* Trees: [BSTree](src/BSTree)

## How it works
These structures are "generic" in the way that the only information they need about the data type stored is its size. When creating 
one of these structures, you'll have to pass a parameter specifying the size in bytes of the data type stored. To add, get or remove 
elements you just have to pass a void pointer to that element. 
It's important to note that these structures store the VALUE of the elements, not their references, so even if you pass a void pointer to an element, these structures will copy the value INSIDE this adress into itself. This makes the structures more memory safe and efficient.

```c
int main(){
    ArrayList *list = arrlist_empty(sizeof(int), compare_int);
    int tmp = 12;
    arrlist_append(list, &tmp);
    tmp = 3;
    arrlist_append(list, &tmp);

    arrlist_free(list);
    return 0;
}
```

In the example above, both 12 and 3 are added into the list, since every call to the append function just copies 4 bytes (sizeof(int)) from the adress of tmp into the list. To acess the elements into the structures, pointer arithmetic is used.

## How are elements compared?
This is an important issue. Since we store "generic" data, we must have a way to compare that data. <br>
That's why all this structures require a function pointer to be passed as a parameter when they are constructed. This function must be like this <br>
```c
int func_name (void* param_1, void* param_2);
```
And it must return <br>
* <b> 1</b> if param_1 is > than param_2
* <b>-1</b> if param_1 is < than param_2
* <b> 0</b> if param_1 is == than param_2
  
For example:<br>
```c
int compare_int(void* param_1, void* param_2){
    // Take the int values
    int i_1 = * (int*) param_1;
    int i_2 = * (int*) param_2;
    // Compare them
    if (i_1 > i_2){
        return 1;
    }else if(i_1 < i_2){
        return -1;
    }else{
        return 0;
    }
}

int main(){
    LinkedList *list = lnkd_list_init(compare_int); // Pass compare_int as a parameter
    int one = 1;
    lnkd_list_push_back(list, &one);
    assert(lnkd_list_exists(list, &one)); // This will return true.
}
```
<b>NOTE:</b> The header file [comparator.h](src/Util/comparator.h) defines functions to compare the most common data types (compare_int, compare_float, etc.).

```c
#include "comparator.h"

int main(){
    LinkedList *list = lnkd_list_init(Comparator.integer); // To compare ints
}
```
