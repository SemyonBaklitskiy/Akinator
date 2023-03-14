#include <stdio.h>
#include "akinator_functions.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Wrong amount of arguments\n");
        return -1;
    }

#ifdef DUMP
    const char* filePath = argv[1];
    struct Node* tree = get_tree(filePath);

    if (tree == NULL) 
        return -1;

    dump(tree);
    return 0;
#else

    printf("MODES:\n");
    printf("1) GAME MODE\n");
    printf("2) DEFINITION MODE\n");
    printf("3) COMPARISON MODE\n");
    printf("Choose mode (1/2/3) or press other key to exit: ");
    int mode = 0;
    scanf("%d", &mode);
    clean_stdinput();

    if ((mode > 3) || (mode < 1)) {
        printf("Goodbye!\n");
        return 0;
    }

    const char* filePath = argv[1];
    struct Node* tree = get_tree(filePath);

    if (tree == NULL) 
        return -1;

    switch (mode) {
    case 1:
        printf("GAME MODE:\n");
        return start_game(tree, filePath);
        break;

    case 2: 
        printf("DEFINITION MODE:\n");
        return definition_mode(tree);
        break;

    case 3: 
        printf("COMPARISON MODE:\n");
        return comparison_mode(tree);
        break;

    default:
        return 0;
    } 
#endif
}