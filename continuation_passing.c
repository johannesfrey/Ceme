#include <stdlib.h>
#include <assert.h>

#include "dbg.h"
#include "continuation_passing.h"

cont_list*
cont_list_create()
{
    return calloc(1, sizeof(cont_list));
}

void
cont_list_init(cont_list *list)
{
    for (int i = 0; i < DEFAULT_EXPAND_RATE; i++) {
        cont_p cont = alloc_continuation();
        cont_list_push_back(list, cont);
    }

    list->next = list->first;
}

void
cont_list_push_back(cont_list *list, cont_p cont)
{
    assert(list && "list can't be NULL");

    cont_list_node *node = calloc(1, sizeof(cont_list_node));
    check_mem(node);

    node->cont = cont;

    if (list->last == NULL) {
        list->first = node;
        list->last = node;
        list->next = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;
    assert(list->first && "If a node was added the first can't be NULL.");

error:
    return;
}

void
cont_list_push_front(cont_list *list, cont_p cont)
{
    assert(list && "list can't be NULL");

    cont_list_node *node = calloc(1, sizeof(cont_list_node));
    check_mem(node);

    node->cont = cont;

    if (list->first == NULL) {
        list->first = node;
        list->last = node;
        list->next = node;
    } else {
        list->first->prev = node;
        node->next = list->first;
        list->first = node;
    }

    list->count++;
    assert(list->first && "If a node was added the first can't be NULL.");

error:
    return;
}

cont_p
cont_list_remove(cont_list *list, cont_list_node *node)
{
    assert(list && "list can't be NULL.");

    cont_p cont = NULL;

    check(list->first && list->last, "List is empty.");
    check(node, "node can't be NULL");

    if (node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
        list->next = NULL;
    } else if (node == list->first) {
        list->first = node->next;
        check(list->first != NULL, "Invalid list. First is NULL");
        list->first->prev = NULL;
        if (node == list->next)
            list->next = list->first;
    } else if (node == list->last) {
        list->last = node->prev;
        check(list->last != NULL, "Invalid list. Last is NULL");
        list->last->next = NULL;
        if (node == list->next)
            list->next = list->last;
    } else {
        cont_list_node *after = node->next;
        cont_list_node *before = node->prev;
        after->prev = before;
        before->next = after;
        if (node == list->next)
            list->next = before;
    }
    assert((list->count > 0) && "List count must not become negative.");

    list->count--;
    cont = node->cont;
    free(node);

    // fallthrough
error:
    return cont;
}

cont_p
cont_list_obtain(cont_list *list)
{
    assert(list && "list can't be NULL.");

    cont_list_node *curr_node = list->next;
    cont_p curr_cont = NULL;

    if (curr_node) {
        curr_cont = cont_list_next(list);

        if (curr_node == list->last) {
            // if we are at the end preallocate more
            for (int i = 0; i < DEFAULT_EXPAND_RATE; i ++) {
                cont_p cont = alloc_continuation();
                cont_list_push_back(list, cont);
            }
        }

        list->next = curr_node->next;
    } else {
        cont_list_init(list);

        curr_cont = cont_list_next(list);
    }

    return curr_cont;
}

void
cont_list_release(cont_list *list)
{
    assert(list && "list can't be NULL.");

    cont_list_node *curr_node = list->next;

    if (curr_node && curr_node != list->first)
        list->next = curr_node->prev;
}
