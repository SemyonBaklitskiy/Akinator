#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "akinator_functions.h"
#include "akinator_enums.h"

#ifdef DEFINITION
#include "stack_functions.h"
static const int capacityOfStack = 4;
#endif

#define PRINT_ERROR(error) processor_of_errors(error, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define CHECK_NULLPTR(pointer, error, ...) if (pointer == NULL) { PRINT_ERROR(error); __VA_ARGS__; }

static void processor_of_errors(akinatorErrors error, const char* fileName, const char* functionName, const int line);
static struct Node* built_tree(FILE* stream);
static void free_tree(struct Node* node);

#ifdef GAME
static void clean_stdinput();
static struct Node* add_node();
static int add_info(struct Node* node);
static int play(struct Node* node);
static void print_tree(struct Node* node, FILE* stream);

#elif defined DEFINITION
static int free_memory(struct stack* st, struct Node* tree, const int returnCode);
static void output_in_definition_mode(const struct stack* st);
static int search(const struct Node* node, const char* searchingData, struct stack* st, int* depth);
#endif

struct Node* get_tree(const char* filePath) {
    CHECK_NULLPTR(filePath, NULLPTR_GIVEN, return NULL);

    FILE* stream = fopen(filePath, "r");
    CHECK_NULLPTR(stream, FILE_DONT_EXIST, return NULL);

    struct Node* tree = built_tree(stream);

    fclose(stream);
    return tree;
}

static void processor_of_errors(akinatorErrors error, const char* fileName, const char* functionName, const int line) { 
    switch (error) {
        case NULLPTR_GIVEN:
            printf("In file %s in function %s on line %d: NULL was given as argument\n", fileName, functionName, line);
            break;

        case FILE_DONT_EXIST:
            printf("In file %s in function %s on line %d: can`t open file or file don`t exist\n", fileName, functionName, line);
            break;

        case BUILT_TREE_ERROR:
            printf("In file %s in function %s on line %d: can`t built tree some error was happened assert called\n", fileName, functionName, line);
            break;

        case NULL_RETURNED:
            printf("In file %s in function %s on line %d: calloc returned NULL can`t give memory\n", fileName, functionName, line);
            break;

        case GETLINE_ERROR:
            printf("In file %s in function %s on line %d: input error (getline returned -1)\n", fileName, functionName, line);
            break;

        default:
            return;
    }
}

static struct Node* built_tree(FILE* stream) {
    int c = getc(stream); 
    
    if (c == '(') {
        struct Node* node = (struct Node*)calloc(1, sizeof(Node));

        CHECK_NULLPTR(node, NULL_RETURNED, PRINT_ERROR(BUILT_TREE_ERROR), assert(0));

        char* data = NULL;
        fscanf(stream, "%m[^()]s", &data);

        if (data == NULL) {
            free(data);
            free(node);
            node = NULL;

        } else {
            node->data = data;
            node->left = NULL;
            node->right = NULL;
        }

        c = getc(stream);

        if (c == ')') {
            return node;

        } else {
            ungetc(c, stream);
            node->left = built_tree(stream);
            node->right = built_tree(stream);
        }

        return node;

    } else if (c == ')') {
        return built_tree(stream); 

    } else {
        PRINT_ERROR(BUILT_TREE_ERROR);
        assert(0);
    }
}

static void free_tree(struct Node* node) {
    if (node == NULL) 
        return;

    if (node->data != NULL) 
        free(node->data);
    
    free_tree(node->left);
    free_tree(node->right);

    free(node);
    return;
}

#ifdef GAME

int start_game(struct Node* tree, const char* filePath) {
    CHECK_NULLPTR(tree, NULLPTR_GIVEN, return -1);
    CHECK_NULLPTR(filePath, NULLPTR_GIVEN, return -1);

    int result = play(tree);

    switch (result) {
        case CORRECT_ANSWER:
            printf("I knew it)))\n");
            free_tree(tree);
            return 0;
            break;

        case ADD_DATA:
        {
            FILE* stream = fopen(filePath, "w");
            CHECK_NULLPTR(stream, FILE_DONT_EXIST, return -1);

            print_tree(tree, stream);
            printf("New data added\n");

            fclose(stream);
            free_tree(tree);
            return 0;
            break;
        }

        case DONT_ADD_DATA:
            free_tree(tree);
            return 0;
            break;

        default:
            free_tree(tree);
            printf("Some error happened\n");
            return -1;
    }
}

static int play(struct Node* node) {
    printf("%s? (Y/N): ", node->data);
    char answer = 0;
    scanf("%c", &answer);
    clean_stdinput();

    if (((answer == 'y') || (answer == 'Y')) && (node->left != NULL)) {
        return play(node->left);

    } else if (((answer == 'y') || (answer == 'Y')) && (node->left == NULL)) {
        return CORRECT_ANSWER;

    } else if (((answer == 'n') || (answer == 'N')) && (node->right != NULL)) {
        return play(node->right);

    } else if (((answer == 'n') || (answer == 'N')) && (node->right == NULL)) {
        return add_info(node);

    } else {
        return -1;
    }
}

static void clean_stdinput() {
    int c = getchar();
    while (c != EOF && c != '\n' && c != '\0') {
        c = getchar();
    }
}

static struct Node* add_node() {
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));
    CHECK_NULLPTR(node, NULL_RETURNED, return NULL);

    char* data = NULL;
    printf("Add data here: ");
    scanf("%m[^\n]s", &data);
    clean_stdinput();

    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

static int add_info(struct Node* node) {
    printf("Would you like to add new data there? (Y/N): ");
    char answer = 0;
    scanf("%c", &answer);
    clean_stdinput();

    if ((answer == 'y') || (answer == 'Y')) {
        node->right = add_node();

        if (node->right == NULL)
            return -1;

        return ADD_DATA;

    } else if ((answer == 'n') || (answer == 'N')) {
        return DONT_ADD_DATA;
    }

    return -1;
}

static void print_tree(struct Node* node, FILE* stream) {
    if (node == NULL) {
        fprintf(stream, "()");
        return;
    }
    
    fprintf(stream, "(%s", node->data);
 
    print_tree(node->left, stream);
    print_tree(node->right, stream);
    
    fprintf(stream, ")");
    return;
}

#elif defined DEFINITION

int definition_mode(struct Node* tree, const char* searchingData) {
    CHECK_NULLPTR(tree, NULLPTR_GIVEN, return -1);
    CHECK_NULLPTR(searchingData, NULLPTR_GIVEN, return -1);

    struct stack* st = (struct stack*)calloc(1, sizeof(stack));
    CHECK_NULLPTR(st, NULL_RETURNED, return -1);

    stack_ctor(st, capacityOfStack);

    int depth = 0;
    int result = search(tree, searchingData, st, &depth);

    switch (result) {
        case true: 
        {
            output_in_definition_mode(st);
        
            return free_memory(st, tree, 0);
            break;
        }    

        case false:
            printf("No result found\n");

            return free_memory(st, tree, 0);
            break;

        default:  
            printf("Some error happened\n");

            return free_memory(st, tree, -1);
            break;
    }
}

static int search(const struct Node* node, const char* searchingData, struct stack* st, int* depth) { 
    ++(*depth);
    bool found = false;
    char* tmp = NULL;

    if (strcmp(node->data, searchingData) == 0) {
        push(st, node->data);
        --(*depth);
        return true;
    }
    

    if (node->left != NULL) {
        push(st, (char*)"a");
        push(st, node->data);
        found = search(node->left, searchingData, st, depth);
    }

    if (found) {
        --(*depth);
        return found;
    }

    if (node->right != NULL) {
        push(st, (char*)"not a");
        push(st, node->data);
        found = search(node->right, searchingData, st, depth);
    }

    if ((node->left == NULL) && (node->right == NULL)) {
        pop(st, &tmp);
        pop(st, &tmp);
        --(*depth);
        return false;
    }

    if ((!found) && (*depth != 1)) {
        pop(st, &tmp); 
        pop(st, &tmp);
    }

    --(*depth);
    return found;
}

static void output_in_definition_mode(const struct stack* st) {
    printf("%s:\n", st->buffer[st -> size - 1]);

    for (unsigned int stackIndex = 0; stackIndex + 1 < st -> size - 1; stackIndex += 2) 
        printf("%s %s ", st->buffer[stackIndex], st->buffer[stackIndex + 1]);

    printf("\n");
}

static int free_memory(struct stack* st, struct Node* tree, const int returnCode) {
    stack_destor(st);
    free(st);
    free_tree(tree);

    return returnCode;
}

#endif