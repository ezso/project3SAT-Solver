#include "list.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"
#include "util.h"

/**
 * Struct for encapsulating a single list element.
 */
typedef struct ListItem {
    struct ListItem* next;  // pointer to the next element (NULL if last)
    void* data;             // pointer to the data
} ListItem;

List mkList(void) {
    List res;
    res.head = NULL;
    return res;
}

void clearList(List* s) {
    while (!isEmpty(s)) {
        pop(s);
    }
}

void push(List* s, void* data) {
    struct ListItem* new_item = malloc(sizeof(ListItem));
    new_item->data = data;
    new_item->next = s->head;
    s->head = new_item;
}

void* peek(List* s) {
    if (isEmpty(s) == 0)
        return s->head->data;
    else
        return NULL;
}

void pop(List* s) {
    if (isEmpty(s) == 0) {
        struct ListItem* temp = s->head;
        s->head = s->head->next;
        free(temp);
    }
}

char isEmpty(List* s) { return s->head == NULL ? 1 : 0; }

ListIterator mkIterator(List* list) {
    ListIterator res;
    res.list = list;
    res.prev = NULL;
    res.current = list->head;

    return res;
}

void* getCurr(ListIterator* it) {
    assert(it->current != NULL);
    return it->current->data;
}

void next(ListIterator* it) {
    assert(isValid(it));
    it->prev = it->current;
    it->current = it->current->next;
}

char isValid(ListIterator* it) { return it->current != NULL; }
