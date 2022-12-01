#include <stdio.h>

struct Node {
    char* data;
    struct Node* left;
    struct Node* right;
};

struct Node* get_tree(const char* filePath);
int start_game(struct Node* tree, const char* filePath);