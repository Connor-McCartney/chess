#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "moves.h"
#include "pieces.h"

typedef struct TreeNode {
    move_t move;
    struct TreeNode *parent;
} treenode_t;


typedef struct TreeListNode {
    treenode_t *treenode;
    struct TreeListNode *next;
} treelistnode_t;


treelistnode_t *create_treelist_node(treenode_t *treenode){
    treelistnode_t *new_node = malloc(sizeof(treelistnode_t));
    new_node->treenode = treenode;
    new_node->next = NULL;
    return new_node;
}

treenode_t *treenode_create(move_t move, treenode_t *parent){
    treenode_t *new_node = malloc(sizeof(treenode_t));
    new_node->move = move;
    new_node->parent = parent;
    return new_node;
}

treenode_t *treenode_copy(treenode_t *original){
    assert(original!=NULL);
    treenode_t *copy = malloc(sizeof(treenode_t));
    copy->move = original->move;
    copy->parent = original->parent;
    return copy;
}

treelistnode_t* treelist_push_end(treelistnode_t *head, treenode_t *next) {
    treelistnode_t *new_node = create_treelist_node(next);
    treelistnode_t *current;

    if (head==NULL) {
        head = new_node;
    } else {
        for (current = head; current->next != NULL; current = current->next) {
            ;
        }
        current->next = new_node;
    }
    return head;
}

void treelist_free(treelistnode_t *head){
    treelistnode_t *next;
    for (treelistnode_t *current = head;  current != NULL;  current = next) {
        next = current->next;
        free(current->treenode);
        free(current);
    }
}

treelistnode_t *treelist_reverse(treelistnode_t *head) {
    treelistnode_t *prev = NULL;
    treelistnode_t *next;
    for (treelistnode_t *curr = head;  curr != NULL;  curr = next) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
    }
    return prev;
}


int treelist_length(treelistnode_t *head) {
    treelistnode_t *current;
    int length = 0;
    for (current = head; current != NULL; current = current->next) {
        length += 1;
    }
    return length;
}

treelistnode_t *walk_up_tree(treenode_t *node) {
    treelistnode_t *path = NULL;
    for (treenode_t* current = node; current != NULL; current = current->parent) {
        path = treelist_push_end(path, treenode_copy(current));
    }
    path = treelist_reverse(path);
    return path;
}

treelistnode_t *treelist_copy(treelistnode_t *head){
    treelistnode_t *ret = NULL; 
    for (treelistnode_t* current = head; current != NULL; current = current->next) {
        ret = treelist_push_end(ret, treenode_copy(current->treenode));
    }
    return ret;
}





move_t get_engine_move(square_t board[8][8]) {
    movelist_node_t *legal_moves = get_all_legal_moves(board); 
    int num_possible_moves = list_length(legal_moves);

    int r = rand() % num_possible_moves; 
    movelist_node_t *current = legal_moves;
    for (int t=0; t<r; t++) {
        current = current->next;
    }
    return current->move;
}
