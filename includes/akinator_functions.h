struct Node {
    char* data;
    struct Node* left;
    struct Node* right;
};

void clean_stdinput();
struct Node* get_tree(const char* filePath);
int start_game(struct Node* tree, const char* filePath);
int definition_mode(struct Node* tree);
int comparison_mode(struct Node* tree);
void dump(struct Node* tree);
