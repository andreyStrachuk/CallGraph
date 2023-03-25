#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define INITCAPACITY 5 // do not do!

#define ASSERT_OK(smth, do)         if (smth) { do }

typedef int DATA;

enum ERRORS {
    RECREATING_LIST,
    NULLPTR,
    OK,
    WRONGINDEX,
    NOTHINGTOREMOVE,
    WRONGSIZE,
    WRONGTAIL,
    WRONGHEAD,
    WRONGFREEHEAD,
    LISTISDAMAGED
};

struct ListElement {
    DATA value;

    int next;
    int prev;
};

struct List {
    ListElement *list;

    int size;
    int capacity;
    int tail;
    int head;
    int ifSorted;

    int freeHead;
};

int InitList (List *lst);

int PushBack (List *lst, const DATA value);

int Insert (List *lst, const int index, const DATA value);

int Remove (List *lst, const int index);

void PrintList (const List *lst);

int PopFront (List *lst);

int PopBack (List *lst);

List  *LinearList (List *lst);

void DumpList (List *lst, FILE *listDump);

int VerifyList (List *lst);

void PrintErrors (const int err);

#endif