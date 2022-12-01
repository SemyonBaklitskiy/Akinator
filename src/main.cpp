#include "functions.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Wrong amount of arguments\n");
        return -1;
    }

    const char* filePath = argv[1];
    struct Node* tree = get_tree(filePath);

    if (tree == NULL) 
        return -1;

    return start_game(tree, filePath);
}