#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "moves.h"
#include "pieces.h"



#include <stdio.h>



typedef struct TreeNode {
    move_t move;
    struct TreeNode *parent;
} treenode_t;


typedef struct TreeListNode {
    treenode_t *treenode;
    struct TreeListNode *next;
} treelistnode_t;


treelistnode_t *treelist_node_create(treenode_t *treenode){
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
    treelistnode_t *new_node = treelist_node_create(next);
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


move_t get_engine_move(square_t original_board[8][8]) {
    square_t board_copy[8][8];
    movelist_node_t *legal_moves;


    treelistnode_t *current_tree; 
    treelistnode_t *tree = NULL; 
    movelist_node_t *current_move; 
    treelistnode_t *to_push; 
    move_t move;

    // create root branch
    legal_moves = get_all_legal_moves(original_board); 
    for (current_move = legal_moves;  current_move != NULL;  current_move = current_move->next) {
        move = current_move->move;
        treenode_t *treenode = treenode_create(move, NULL);
        tree = treelist_push_end(tree, treenode_copy(treenode));
    }

    // create rest of tree
    int max_depth = 2*1; // keep even
    for (int d=1; d<max_depth+1; d++) {
        turn *= -1;
        to_push = NULL;
        for (current_tree = tree;  current_tree != NULL;  current_tree = current_tree->next) {
            treelistnode_t *path = walk_up_tree(current_tree->treenode);
            int length = treelist_length(path);


            if (length == d) {
                // play the moves
                copy_board(board_copy, original_board);
                for (treelistnode_t *current_tree_move = path;  current_tree_move != NULL;  current_tree_move = current_tree_move->next) {
                    move = current_tree_move->treenode->move;
                    move_piece(board_copy, move);
                }

                // add legal moves to the tree (tmp to_push to avoid infinite loop)
                legal_moves = get_all_legal_moves(board_copy);
                for (current_move = legal_moves;  current_move != NULL;  current_move = current_move->next) {
                    treenode_t *treenode = treenode_create(current_move->move, current_tree->treenode);
                    to_push = treelist_push_end(to_push, treenode_copy(treenode));
                }
            }
        }
        // add legal moves to the tree (tmp to_push to avoid infinite loop)
        for (current_tree = to_push; current_tree != NULL; current_tree = current_tree->next) {
            tree = treelist_push_end(tree, treenode_copy(current_tree->treenode));
        }
        treelist_free(to_push);
    }


    // now choose the best move in the tree
    int best_depth = 0;
    int best_score = 0;
    int score;
    move_t best_move;
    for (current_tree = tree; current_tree != NULL; current_tree = current_tree->next) {
        treelistnode_t* path = walk_up_tree(current_tree->treenode);  // maybe no copy needed

        // play the moves
        copy_board(board_copy, original_board);
        for (treelistnode_t *current_tree_move = path;  current_tree_move != NULL;  current_tree_move = current_tree_move->next) {
            move = current_tree_move->treenode->move; // random segfaults?!
            move_piece(board_copy, move);
        }

       score = 0;
        for (int x=0; x<8; x++) {
            for (int y=0; y<8; y++) {
                switch (board_copy[x][y].piece.piece) {
                    case PAWN:
                        score += board_copy[x][y].piece.colour * 1;
                        break;
                    case ROOK:
                        score += board_copy[x][y].piece.colour * 5;
                        break;
                    case KNIGHT:
                        score += board_copy[x][y].piece.colour * 3;
                        break;
                    case BISHOP:
                        score += board_copy[x][y].piece.colour * 3;
                        break;
                    case QUEEN:
                        score += board_copy[x][y].piece.colour * 9;
                        break;
                }
            }
        }
        score *= turn;

        int depth = treelist_length(path);
        if (depth > best_depth) {
            best_score = score; // prioritise depth more than x
        }
        if (depth >= best_depth) {
            if (score > best_score) {
                best_move = path->treenode->move;
                best_score = score;
            }
            best_depth = depth;
        }
        treelist_free(path);
    }

    return best_move;
}
