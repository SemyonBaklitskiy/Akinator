#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "akinator_functions.h"
#include "akinator_enums.h"
#include "stack_functions.h"

#define PRINT_ERROR(error) processor_of_errors(error, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define CHECK_NULLPTR(pointer, error, ...) if (pointer == NULL) { PRINT_ERROR(error); __VA_ARGS__; }

static const int capacityOfStacks = 4;

static void processor_of_errors(akinatorErrors error, const char* fileName, const char* functionName, const int line);
static size_t size_of_word(FILE* stream);
static char* get_word(FILE* stream);
static struct Node* built_tree(FILE* stream);
static void clean_stdinput();
static struct Node* add_node();
static int add_info(struct Node* node);
static int play(struct Node* node);
static void print_tree(struct Node* node, FILE* stream);
static void free_tree(struct Node* node);
static void remove_question_mark(char* str);
static void output_in_definition_mode(const struct stack* st, const struct stack* sideSt);
static int search(const struct Node* node, const char* searchingData, struct stack* st, struct stack* sideSt);

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

struct Node* get_tree(const char* filePath) {
    CHECK_NULLPTR(filePath, NULLPTR_GIVEN, return NULL);

    FILE* stream = fopen(filePath, "r");
    CHECK_NULLPTR(stream, FILE_DONT_EXIST, return NULL);

    struct Node* tree = built_tree(stream);

    fclose(stream);
    return tree;
}

int definition_mode(struct Node* tree, const char* searchingData) {
    CHECK_NULLPTR(tree, NULLPTR_GIVEN, return -1);
    CHECK_NULLPTR(searchingData, NULLPTR_GIVEN, return -1);

    struct stack* st = (struct stack*)calloc(1, sizeof(stack));
    struct stack* sideSt = (struct stack*)calloc(1, sizeof(stack)); //mb rename
    CHECK_NULLPTR(st, NULL_RETURNED, return -1);
    CHECK_NULLPTR(sideSt, NULL_RETURNED, return -1);

    stack_ctor(st, capacityOfStacks);
    stack_ctor(sideSt, capacityOfStacks);

    int result = search(tree, searchingData, st, sideSt);

    switch (result) {
        case FOUND: 
        {
            output_in_definition_mode(st, sideSt);

            stack_destor(st);
            stack_destor(sideSt);

            free(st);
            free(sideSt);
            free_tree(tree);
        
            return 0;
            break;
        }    

        case NOT_FOUND:
            printf("No result found\n");

            stack_destor(st);
            stack_destor(sideSt);

            free(st);
            free(sideSt);
            free_tree(tree);

            return 0;
            break;

        default:  
            printf("Some error happened\n");

            stack_destor(st);
            stack_destor(st);

            free(st);
            free(sideSt);
            free_tree(tree);

            return -1;
            break;
    }
}

static int play(struct Node* node) {
    printf("%s (Y/N): ", node->data);
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

static size_t size_of_word(FILE* stream) {
    size_t size = 0;
    int symbol = getc(stream);

    while ((symbol != ')') && (symbol != '(') && (symbol != EOF)) {
        ++size;
        symbol = getc(stream);
    }

    fseek(stream, -size - 1, SEEK_CUR);
    return size;
}

static char* get_word(FILE* stream) {
    size_t size = size_of_word(stream); 
    char* word = (char*)calloc(size + 1, sizeof(char));

    CHECK_NULLPTR(word, NULL_RETURNED, return NULL);

    int symbol = getc(stream);
    int wordIndex = 0;

    while ((symbol != ')') && (symbol != '(') && (symbol != EOF)) {
        word[wordIndex] = symbol;
        ++wordIndex;

        symbol = getc(stream);
    }

    word[wordIndex] = '\0';

    ungetc(symbol, stream);
    return word;
}

static struct Node* built_tree(FILE* stream) {
    int c = getc(stream); 
    
    if (c == '(') {
        struct Node* node = (struct Node*)calloc(1, sizeof(Node));

        CHECK_NULLPTR(node, NULL_RETURNED, PRINT_ERROR(BUILT_TREE_ERROR), assert(0));

        char* data = get_word(stream);
        CHECK_NULLPTR(data, BUILT_TREE_ERROR, assert(0));

        if (data[0] == '\0') {
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
    size_t size = 0;
    int realSize = getline(&data, &size, stdin);

    if (realSize == -1) {
        PRINT_ERROR(GETLINE_ERROR);
        return NULL;
    }

    data[realSize - 1] = '\0';

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

static int search(const struct Node* node, const char* searchingData, struct stack* st, struct stack* sideSt) {
    int result = NOT_FOUND;
    char* tmp = NULL;
    push(st, node->data);

    if (strcmp(node->data, searchingData) == 0) 
        return FOUND;
    
    if ((node->left == NULL) && (node->right == NULL)) {
        stack_pop(st, &tmp);
        stack_pop(sideSt, &tmp);
        return NOT_FOUND;
    }

    if (node->left != NULL) {
        stack_push(sideSt, (char*)"a");
        result = search(node->left, searchingData, st, sideSt);
    }

    if (result == FOUND)
        return result;

    if (node->right != NULL) {
        stack_push(sideSt, (char*)"not a");
        result = search(node->right, searchingData, st, sideSt);
    }

    if (result == NOT_FOUND) {
        stack_pop(st, &tmp);  //think about optimisation
        stack_pop(sideSt, &tmp);
    }

    return result;
}

static void remove_question_mark(char* str) {
    if (strchr(str, '?') != NULL) { 
        *strchr(str, '?') = '\0';

    } else if (strchr(str, '!') != NULL) {
        *strchr(str, '!') = '\0';

    } else if (strchr(str, '.') != NULL) {
        *strchr(str, '.') = '\0';

    } else if (strchr(str, ',') != NULL) {
        *strchr(str, ',') = '\0';

    } else {
        return;
    }
}

static void output_in_definition_mode(const struct stack* st, const struct stack* sideSt) {
    remove_question_mark(st->buffer[st->size - 1]);
    printf("%s:\n", st->buffer[st->size - 1]);

    for (unsigned int stackIndex = 0; stackIndex < sideSt->size; ++stackIndex) {
        remove_question_mark(st->buffer[stackIndex]);
        remove_question_mark(sideSt->buffer[stackIndex]);
        printf("%s %s ", sideSt->buffer[stackIndex], st->buffer[stackIndex]);
    }

    printf("\n");
}