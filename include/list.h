/*   include/list.h   */

/* Circular doubly linked-list data structure implementation. */

#ifndef KERNEL_API_LIST_H
#define KERNEL_API_LIST_H

#include <stddef.h>

/* Intrusive list node. */
typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

/*
 * @brief Obtain a pointer to the struct that contains a list node.
 * 
 * @param ptr  A pointer to the list node.
 * @param type The type of the struct.
 * @param name Name of the list node inside the struct.
 */
#define LIST_CONTAINER_OF(ptr, type, name) \
    ((type *) ((char *) (ptr) - offsetof(type, name)))

/*
 * Iterate over all nodes in a list, excluding the head sentinel.
 *
 * @param pos  Loop cursor.
 * @param head Pointer to the list head sentinel.
 */
#define list_foreach(pos, head) \
    for (list_node_t *pos = (head)->next; pos != (head); pos = pos->next)

/*
 * Iterate over all nodes, safe against removal of the current node.
 *
 * @param pos  Loop cursor.
 * @param tmp  Temporary storage.
 * @param head Pointer to the list head sentinel.
 */
#define list_foreach_safe(pos, tmp, head)                   \
    for (list_node_t *pos = (head)->next, *tmp = pos->next; \
         pos != (head);                                     \
         pos = tmp, tmp = pos->next)

/*
 * @brief Initialize the sentinel head of a list.
 */
static inline void list_init(list_node_t *head)
{
    head->next = head;
    head->prev = head;
}

/*
 * @brief Insert a node between two nodes.
 *
 * @param node The node to insert.
 * @param prev The previous node.
 * @param next The next node.
 */
static inline void list_insert(list_node_t *node, list_node_t *prev, list_node_t *next)
{
    if (node == NULL || prev == NULL || next == NULL) {
        return;
    }

    /* Modify new node. */
    node->prev = prev;
    node->next = next;

    /* Modify previous node. */
    prev->next = node;
    next->prev = node;
}

/*
 * @brief Add a new node right before another.
 */
static inline void list_preappend(list_node_t *head, list_node_t *node)
{
    list_insert(node, head, head->next);
}

/*
 * @brief Add a new node right after another.
 */
static inline void list_append(list_node_t *head, list_node_t *node)
{
    list_insert(node, head->prev, head);
}

/*
 * @brief Remove a node inside a list.
 */
static inline void list_remove(list_node_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = NULL;
    node->next = NULL;
}

/*
 * @brief Check if a list is empty given its head.
 */
static inline int list_empty(const list_node_t *head)
{
    return head->next == head;
}

/*
 * @brief Return the first node, or head if empty.
 */
static inline list_node_t *list_first(const list_node_t *head)
{
    return head->next;
}

/*
 * @brief Return last node, or head if empty.
 */
static inline list_node_t *list_last(const list_node_t *head)
{
    return head->prev;
}

#endif /* KERNEL_API_LIST_H */
