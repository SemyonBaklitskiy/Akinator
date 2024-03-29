#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "akinator_functions.h"
#include "akinator_enums.h"
#include "stack_functions.h"

static const int capacityOfStack = 4;
static int search(const struct Node* node, const char* searchingData, struct stack* st, int* depth);
static int free_memory(struct stack* st, struct Node* tree, const int returnCode);

#define PRINT_ERROR(error) processor_of_errors(error, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define CHECK_NULLPTR(pointer, error, ...) if (pointer == NULL) { PRINT_ERROR(error); __VA_ARGS__; }

static void processor_of_errors(akinatorErrors error, const char* fileName, const char* functionName, const int line);
static struct Node* built_tree(FILE* stream);
static void free_tree(struct Node* node);

static char* get_str();
static struct Node* add_node();
static int add_info(struct Node* node);
static int play(struct Node* node);
static void print_tree(struct Node* node, FILE* stream);
static void output_in_definition_mode(const struct stack* st); 
static void output_in_comparison_mode(const struct stack* firstSt, const struct stack* secondSt);

void clean_stdinput() {
    int c = getchar();
    while ((c != EOF) && (c != '\n') && (c != '\0')) 
        c = getchar();
}

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
            printf("In file %s in function %s on line %d: NULL was given as an argument\n", fileName, functionName, line);
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

        default:
            return;
    }
}

static struct Node* built_tree(FILE* stream) {
    int c = getc(stream); 
    
    if (c == '(') {
        struct Node* node = (struct Node*)calloc(1, sizeof(Node));

        CHECK_NULLPTR(node, NULL_RETURNED, PRINT_ERROR(BUILT_TREE_ERROR), exit(BUILT_TREE_ERROR));

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
        exit(BUILT_TREE_ERROR);
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
    node = NULL;
    return;
}

int start_game(struct Node* tree, const char* filePath) {
    CHECK_NULLPTR(tree, NULLPTR_GIVEN, return -1);
    CHECK_NULLPTR(filePath, NULLPTR_GIVEN, free_tree(tree); return -1);

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

        case WRONG_COMMAND:
            free_tree(tree);
            printf("Wrong command, game finished!\n");
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

    } else if ((answer != 'y') && (answer != 'Y') && (answer != 'n') && (answer != 'N')) {
        return WRONG_COMMAND;

    } else {
        return -1;
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

int definition_mode(struct Node* tree) {
    CHECK_NULLPTR(tree, NULLPTR_GIVEN, return -1);

    printf("Please enter object to definite or press ENTER to quit: ");
    char* searchingData = get_str();
    CHECK_NULLPTR(searchingData, NOERRORS, free_tree(tree); return 0);

    struct stack* st = (struct stack*)calloc(1, sizeof(stack));
    CHECK_NULLPTR(st, NULL_RETURNED, return -1);

    stack_ctor(st, capacityOfStack);

    int depth = 0;
    int result = search(tree, searchingData, st, &depth);
    clean_stdinput();

    switch (result) {
        case true: 
        {
            output_in_definition_mode(st);
        
            free(searchingData);
            return free_memory(st, tree, 0);
            break;
        }    

        case false:
        {
            printf("No result found\n");

            free(searchingData);
            return free_memory(st, tree, 0);
            break;
        }

        default:  
        {
            printf("Some error happened\n");

            free(searchingData);
            return free_memory(st, tree, -1);
            break;
        }
    }
}

static void output_in_definition_mode(const struct stack* st) {
    printf("%s:\n", st->buffer[st -> size - 1]);

    for (unsigned int stackIndex = 0; stackIndex + 1 < st -> size - 1; stackIndex += 2) 
        printf("%s %s ", st->buffer[stackIndex], st->buffer[stackIndex + 1]);

    printf("\n");
}

int comparison_mode(struct Node* tree) {
    CHECK_NULLPTR(tree, NULLPTR_GIVEN, return -1);

    printf("Please enter first object to compare or press ENTER to quit: ");
    char* firstObject = get_str();
    CHECK_NULLPTR(firstObject, NOERRORS, free_tree(tree); return 0);
    clean_stdinput();
    printf("Please enter second object to compare or press ENTER to quit: ");
    char* secondObject = get_str();
    CHECK_NULLPTR(secondObject, NOERRORS, free(firstObject); free_tree(tree); return 0);

    struct stack* firstSt = (struct stack*)calloc(1, sizeof(stack));
    CHECK_NULLPTR(firstSt, NULL_RETURNED, return -1);

    struct stack* secondSt = (struct stack*)calloc(1, sizeof(stack));
    CHECK_NULLPTR(secondSt, NULL_RETURNED, return -1);

    stack_ctor(firstSt, capacityOfStack);
    stack_ctor(secondSt, capacityOfStack);

    int depth = 0;
    int firstResult = search(tree, firstObject, firstSt, &depth);

    depth = 0;
    int secondResult = search(tree, secondObject, secondSt, &depth);

    if (firstResult && secondResult) {
        output_in_comparison_mode(firstSt, secondSt);

        free_memory(firstSt, tree, 0);
        tree = NULL;
        free(secondObject);
        free(firstObject);
        return free_memory(secondSt, tree, 0);

    } else if (!(firstResult || secondResult)) {
        printf("No result found for both objects: %s %s\n", firstObject, secondObject);

        free_memory(secondSt, tree, 0);
        tree = NULL;
        free(secondObject);
        free(firstObject);
        return free_memory(firstSt, tree, 0);

    } else if (!firstResult) {
        printf("No result found for first object: %s\n", firstObject);

        free_memory(secondSt, tree, 0);
        tree = NULL;
        free(secondObject);
        free(firstObject);
        return free_memory(firstSt, tree, 0);

    } else {
        printf("No result found for second object: %s\n", secondObject);

        free_memory(secondSt, tree, 0);
        tree = NULL;
        free(secondObject);
        free(firstObject);
        return free_memory(firstSt, tree, 0);
    }

    printf ("Some error happened\n");

    free_memory(firstSt, tree, -1);
    tree = NULL;
    free(secondObject);
    free(firstObject);
    return free_memory(secondSt, tree, -1);
}

static void output_in_comparison_mode(const struct stack* firstSt, const struct stack* secondSt) {
    printf("%s and %s:\n", firstSt->buffer[firstSt->size - 1], secondSt->buffer[secondSt->size - 1]);
    printf("Common: ");

    unsigned int stIndex = 0;
    for (stIndex = 0; (stIndex < firstSt->size) && (stIndex < secondSt ->size) && (strcmp(firstSt->buffer[stIndex], secondSt->buffer[stIndex]) == 0); ++stIndex) 
        printf("%s ", firstSt->buffer[stIndex]);
    printf("\n");
     
    printf("But %s: ", firstSt->buffer[firstSt->size - 1]);
    for (unsigned int firstStIndex = stIndex; firstStIndex < firstSt->size - 1; ++firstStIndex) 
        printf("%s ", firstSt->buffer[firstStIndex]);
    printf("\n");

    printf("But %s: ", secondSt->buffer[secondSt->size - 1]);
    for (unsigned int secondStIndex = stIndex; secondStIndex < secondSt->size - 1; ++secondStIndex) 
        printf("%s ", secondSt->buffer[secondStIndex]);
    printf("\n");
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

static int free_memory(struct stack* st, struct Node* tree, const int returnCode) {
    stack_destor(st);
    free(st);
    free_tree(tree);

    return returnCode;
}

static char* get_str() {
    char* str = NULL;
    scanf("%m[^\n]s", &str);

    if (str == NULL)
        return NULL;

    return str;
}

#ifdef DUMP
static const char dumpFilePath[] = "tree.gv";
static const char nullName[] = "NULL";

static void make_labels(const struct Node* node, FILE* stream, int* counter);
static void make_edges(const struct Node* node, FILE* stream, bool* secondCall, int* counter);

void dump(struct Node* tree) {
    CHECK_NULLPTR(tree, NULLPTR_GIVEN, return);
    
    FILE* stream = fopen(dumpFilePath, "w");
    CHECK_NULLPTR(stream, FILE_DONT_EXIST, return);

    fprintf(stream, "digraph Tree {\n");
    int counter = 0;
    make_labels(tree, stream, &counter);
    counter = 0;
    bool secondCall = false;
    make_edges(tree, stream, &secondCall, &counter);
    fprintf(stream, "}");

    fclose(stream);
    free_tree(tree);
}

static void make_labels(const struct Node* node, FILE* stream, int* counter) {
    if (node == NULL) {
        return;
    }

    fprintf(stream, "\"%p\" [label = %s];\n", node, node->data);

    if (node->left == NULL) 
        fprintf(stream, "\"%s%d\" [label = \"Win\"];\n", nullName, ++(*counter));

    if (node->right == NULL)
        fprintf(stream, "\"%s%d\" [label = \"No data\"];\n", nullName, ++(*counter));

    make_labels(node->left, stream, counter);
    make_labels(node->right, stream, counter);
}

static void make_edges(const struct Node* node, FILE* stream, bool* secondCall, int* counter) {
    if (node == NULL) {
        fprintf(stream, "\"%s%d\";\n", nullName, ++(*counter));
        return;
    }

    fprintf(stream, "\"%p\" -> ", node);

    if (*secondCall)
        return;

    make_edges(node->left, stream, secondCall, counter);

    *secondCall = true;
    make_edges(node, stream, secondCall, counter);
    *secondCall = false;

    make_edges(node->right, stream, secondCall, counter);
}
#endif