#include <stdio.h>
#include "akinator_functions.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Wrong amount of arguments\n");
        return -1;
    }

    const char* filePath = argv[1];
    struct Node* tree = get_tree(filePath);

    if (tree == NULL) 
        return -1;

#ifdef GAME

    printf("GAME MODE:\n");
    return start_game(tree, filePath);

#elif defined DEFINITION

    const char* searchingData = argv[2];
    printf("DEFINITION MODE:\n");
    return definition_mode(tree, searchingData);

#else 

    const char* firstObject = argv[2];
    const char* secondObject = argv[3];
    printf("COMPARISON MODE:\n");
    return comparison_mode(tree, firstObject, secondObject);
    
#endif

}