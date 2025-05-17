#include "linked_list.h"
#include <stddef.h>

void append_move(node_t head, move_t move) {
    node_t current = head;
    while (current.next != NULL) {
        current = *current.next;
    }
    node_t next = {move, NULL};
    current.next = &next;
}
