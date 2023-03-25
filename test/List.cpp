#include "list.h"

int InitList (List *lst) {
    assert (lst);

    if (lst->capacity != 0) {
        return RECREATING_LIST;
    }

    lst->list = (ListElement *)calloc (INITCAPACITY + 1, sizeof (ListElement));
    ASSERT_OK (lst->list == nullptr, return NULLPTR;);

    (lst->list + 0)->prev = 0;
    (lst->list + 0)->next = 0;
    (lst->list + 0)->value = 0;
    lst->head = 0;
    lst->tail = 0;
    lst->capacity = INITCAPACITY;

    lst->ifSorted = 0;

    for (int i = 1; i <= INITCAPACITY; i++) {
        (lst->list + i)->prev = -1;
        
        if (i == INITCAPACITY) {
            (lst->list + i)->next = 0;
            continue;
        }

        (lst->list + i)->next = i + 1;
    }

    lst->freeHead = 1;

    lst->ifSorted = 1;

    return OK;
}

static void FillListAfterAlloc (List *lst, const int free) {
    assert (lst);

    for (int i = free; i <= lst->capacity; i++) {
        if (i == lst->capacity) {
            (lst->list + i)->next = 0;
            (lst->list + i)->prev = -1;
            (lst->list + i)->value = 0;

            break;
        }

        (lst->list + i)->next = i + 1;
        (lst->list + i)->prev = -1;
        (lst->list + i)->value = 0;
    }
}

static int ResizeListUp (List *lst) {
    assert (lst);

    lst->capacity = lst->capacity * 2;

    int newCap = (lst->capacity + 1) * sizeof (ListElement);

    void *ptr = realloc (lst->list, newCap);
    assert (ptr);

    lst->list = (ListElement *)ptr;

    FillListAfterAlloc (lst, lst->freeHead);

    return OK;
}

int PushBack (List *lst, const DATA value) {
    assert (lst);

    if ((lst->list + lst->freeHead)->next == 0 && (lst->list + lst->freeHead)->prev == -1) {
        int res = ResizeListUp (lst);
        ASSERT_OK (res != OK, return NULLPTR; );
    }

    int insIndex = lst->freeHead;

    if (lst->head == 0) {
        lst->head++;

        (lst->list + insIndex)->next = 0;
        (lst->list + insIndex)->prev = lst->tail;
        (lst->list + insIndex)->value = value;

        lst->freeHead++;
        lst->tail++;
        lst->size++;

        return OK;
    }

    lst->freeHead = (lst->list + lst->freeHead)->next;

    (lst->list + insIndex)->next = 0;
    (lst->list + insIndex)->prev = lst->tail;
    (lst->list + insIndex)->value = value;

    (lst->list + lst->tail)->next = insIndex;

    lst->tail = insIndex;

    lst->size++;

    return OK;
}

void PrintList (const List *lst) {
    assert (lst);

    int index = lst->head;

    if (index == 0) {
        printf ("List is empty!\n");
        return;
    }

    while (1) {
        printf ("%d ", (lst->list + index)->value);
        index = (lst->list + index)->next;
        
        if (index == 0) break;
    }

    printf ("\n");
}

int Insert (List *lst, const int index, const DATA value) {
    assert (lst);

    if (lst->size == 0 && index != 1) {
        return WRONGINDEX;
    }

    if (lst->size < 1 || lst->size > lst->capacity) {
        return WRONGINDEX;
    }

    if ((lst->list + index)->prev == -1 && (lst->list + index)->value == 0) {
        return WRONGINDEX;
    }

    if ((lst->list + lst->freeHead)->next == 0 && (lst->list + lst->freeHead)->prev == -1) {
        int res = ResizeListUp (lst);
        ASSERT_OK (res != OK, return NULLPTR; );
    }

    int insIndex = lst->freeHead;

    (lst->list + insIndex)->value = value;

    if (lst->ifSorted) {
        int tmpIndex = (lst->list + index)->next;

        (lst->list + index)->next = insIndex;

        (lst->list + insIndex)->next = tmpIndex;
        (lst->list + insIndex)->prev = index;
    } 
    else {
        int tmpHead = lst->head;

        for (int i = 1; i < index; i++) {
            tmpHead = (lst->list + tmpHead)->next;
        }

        int tmpNext = (lst->list + tmpHead)->next;

        (lst->list + tmpNext)->prev = insIndex;

        lst->freeHead = (lst->list + lst->freeHead)->next;

        (lst->list + tmpHead)->next = insIndex;

        (lst->list + insIndex)->prev = tmpHead;
        (lst->list + insIndex)->next = tmpNext;
    }

    lst->size++;

    return OK;

}

int Remove (List *lst, const int index) {
    assert (lst);

    (lst->list + index)->value = 0;

    int tmpPrev = (lst->list + index)->prev;
    int tmpNext = (lst->list + index)->next;

    (lst->list + tmpPrev)->next = tmpNext;
    
    (lst->list + tmpNext)->prev = tmpPrev;

    (lst->list + index)->next = lst->freeHead;
    (lst->list + index)->prev = -1;

    lst->freeHead = index;

    lst->ifSorted = 0;

    lst->size--;

    return OK;
}

List *LinearList (List *lst) {
    assert (lst);

    if (lst->ifSorted == 1) {
        return lst;
    }

    List *newList = (List *)calloc (1, sizeof (List));

    int resOfAct = 0;
    resOfAct = InitList (newList);

    int tmpHead = lst->head;

    DATA value = 0;

    for (int i = 0; i < lst->size; i++) {
        value = (lst->list + tmpHead)->value;
        tmpHead = (lst->list + tmpHead)->next;

        resOfAct = PushBack (newList, value);

        if (resOfAct != OK) {
            printf ("Error!\n");
        }
    }

    free (lst->list);
    free (lst);

    newList->ifSorted = 1;

    return newList;
}

int PopFront (List *lst) {
    assert (lst);

    if (lst->size == 0) {
        return NOTHINGTOREMOVE;
    }

    int tmpHead = (lst->list + lst->head)->next;

    (lst->list + tmpHead)->prev = 0;

    (lst->list + lst->head)->value = 0;
    (lst->list + lst->head)->next = lst->freeHead;
    (lst->list + lst->head)->prev = -1;

    lst->freeHead = lst->head;

    lst->head = tmpHead;

    lst->size--;

    return OK;
}

int PopBack (List *lst) {
    assert (lst);

    if (lst->size == 0) {
        return NOTHINGTOREMOVE;
    }

    int tmpTail = (lst->list + lst->tail)->prev;

    (lst->list + tmpTail)->next = 0;

    (lst->list + lst->tail)->value = 0;
    (lst->list + lst->tail)->next = lst->freeHead;
    (lst->list + lst->tail)->prev = -1;

    lst->freeHead = lst->tail;

    lst->tail = tmpTail;

    lst->size--;

    return OK;
}

void DumpList (List *lst, FILE *listDump) {
    assert (lst);

    fprintf (listDump, "digraph G {\n   concentrate=True;\n"
                                        "   rankdir=LR;\n"
                                        "   node [shape=record,color=blue];\n");


    int tmpNext = lst->head;

    for (int i = 0; i < lst->size; i++) {
        fprintf (listDump, "   %d [fillcolor=red, style=\"rounded,filled\" label=\"data: %d|{next: %d|prev: %d}\"];\n\n",
                i, (lst->list + tmpNext)->value, (lst->list + tmpNext)->next, (lst->list + tmpNext)->prev);

        tmpNext = (lst->list + tmpNext)->next;
    }

    int tmpFreeHead = lst->freeHead;

    for (int i = lst->size; i < lst->capacity; i++) {
        fprintf (listDump, "   %d [fillcolor=green, style=\"rounded,filled\" label=\"data: %d|{next: %d|prev: %d}\"];\n\n",
                i, (lst->list + tmpFreeHead)->value, (lst->list + tmpFreeHead)->next, (lst->list + tmpFreeHead)->prev);

        tmpFreeHead = (lst->list + tmpFreeHead)->next;
    }

    fprintf (listDump, "-1 [fillcolor=green, style=\"rounded,filled\" label=\"size: %d\"];", lst->size);
    fprintf (listDump, "-2 [fillcolor=blue, style=\"rounded,filled\" label=\"capacity: %d\"];", lst->capacity);
    fprintf (listDump, "-3 [fillcolor=brown, style=\"rounded,filled\" label=\"sorted: %d\"];", lst->ifSorted);
    fprintf (listDump, "-4 [fillcolor=pink, style=\"rounded,filled\" label=\"head: %d\"];", lst->head);
    fprintf (listDump, "-5 [fillcolor=yellow, style=\"rounded,filled\" label=\"tail: %d\"];", lst->tail);
    fprintf (listDump, "-6 [fillcolor=orange, style=\"rounded,filled\" label=\"freeHead: %d\"];", lst->freeHead);

    for (int i = 0; i < lst->size - 1; i++) {
        fprintf (listDump, "%d -> %d\n", i, i + 1);
    }

    for (int i = 1; i < lst->size; i++) {
        fprintf (listDump, "%d -> %d\n", i, i - 1);
    }

    for (int i = lst->size; i < lst->capacity - 1; i++) {
        fprintf (listDump, "%d -> %d\n", i, i + 1);
    }

    for (int i = lst->size + 1; i < lst->capacity; i++) {
        fprintf (listDump, "%d -> %d\n", i, i - 1);
    }

    fprintf (listDump, "}");

    fclose (listDump);

    system ("dot -Tpng ListDump.txt -O\n"
            "xdg-open ListDump.txt.png");

}

int VerifyList (List *lst) {
    assert (lst);

    if (lst->size >= lst->capacity || lst->size < 0) {
        return WRONGSIZE;
    }

    if (lst->tail > lst->capacity || lst->tail < 0) {
        return WRONGTAIL;
    }

    if (lst->head > lst->capacity || lst->head < 0) {
        return WRONGHEAD;
    }

    if (lst->freeHead > lst->capacity || lst->freeHead < 0) {
        return WRONGFREEHEAD;
    }

    if (lst->list == nullptr) {
        return NULLPTR;
    }

    int tmpHead = lst->head;
    int tmpTail = 0;

    for (int i = 0; i < lst->size; i++) {
        tmpTail = (lst->list + tmpHead)->next;    
    }

    if (lst->tail != tmpTail) {
        return LISTISDAMAGED;
    }

    return OK;
}

void PrintErrors (const int err) {
    switch (err) {
        case RECREATING_LIST: {
            printf ("Trying to recreate list!\nError code: %d\n", err);
            break;
        }
        
        case NULLPTR: {
            printf ("Addressing nullptr!\nError code: %d\n", err);
            break;
        }

        case WRONGINDEX: {
            printf ("Wrong index!\nError code: %d\n", err);
            break;
        }
        
    }
}