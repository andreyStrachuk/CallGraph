#include "list.h"

int main () {
    List lst = {};

    int resOfAct = 0;

    resOfAct = InitList (&lst);


    for (int i = 0; i < 10; ++i) {
        resOfAct = PushBack (&lst, i + 7);
    }

    for (int i = 9; i >= 0; i--) {
        resOfAct = PopFront (&lst);
    }
    

    free(lst.list);

    return 0;
}
