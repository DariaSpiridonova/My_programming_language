#ifndef TOKENS
#define TOKENS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>

#define BOLD "\x1b[1m"
#define UNDERLINE "\x1b[4m"
#define RED_COLOR_BEGIN "\033[31m"
#define RED_COLOR_END   "\033[0m\n"
#define BLUE_BOLD       "\e[1;34m"
#define RESET           "\x1b[0m"

#define ASSERTS(tree)\
        assert(tree != NULL);\
        assert(tree->num_of_el >= 0);\
        assert(tree->root != NULL);\
        assert(tree->root->parent == NULL);

#define CALCULATION_TREE_DUMP(tree)\
        CalculationTreeDump(tree,__FILE__, __LINE__)

const ssize_t SIZE_OF_NAME = 200;
const ssize_t NUM_OF_VARIABLES = 10;
const ssize_t NUM_OF_FUNCTIONS = 10;
const ssize_t NUM_OF_TYPES = 14;
const ssize_t NUM_OF_SIGNES = 10;
const ssize_t NUM_OF_OPERATIONS = 6;
const ssize_t NUM_OF_SYMBOLS = 7;
const ssize_t NUM_OF_TOKENS = 30;
const ssize_t LEN_OF_SIGNES_ARRAY = 8;
const ssize_t LEN_OF_KEY_WORDS_ARRAY = 7;
const ssize_t LEN_OF_FUNCS_COMMANDS = 3;
const ssize_t NUM_OF_PARS = 5;
const ssize_t NUM_OF_CONDS = 5;
const ssize_t NUM_OF_IO = 4;
const ssize_t NUM_OF_DELIMITERS = 4;

const char link_to_graphviz_file[] = "../Graphviz/program_tree_";

const char * const type_buffer[NUM_OF_TYPES] = {"VAR_TYPE", "NUM_TYPE", "PAR_TYPE", "COND_TYPE", "OP_TYPE", "FUNC_DEF_TYPE", "FUNC_CALL_TYPE", "COMMA_TYPE", "SEM_POINT_TYPE", "COLON_TYPE", "ASSIGN_TYPE", "COMPARE_TYPE", "SIGN_TYPE", "COMM_TYPE"};
enum type_t 
{
    VAR_TYPE,
    NUM_TYPE,
    PAR_TYPE,
    COND_TYPE,
    OP_TYPE,
    FUNC_DEF_TYPE,
    FUNC_CALL_TYPE,
    COMMA_TYPE,
    SEM_POINT_TYPE,
    COLON_TYPE,
    ASSIGN_TYPE,
    COMPARE_TYPE,
    SIGN_TYPE,
    COMM_TYPE
};

typedef struct 
{
    const char *signes;
    type_t type;
} types;

const types array_of_signes[LEN_OF_SIGNES_ARRAY] = 
{
    {"(){}" , PAR_TYPE},
    {"+-*/^", SIGN_TYPE},
    {";", SEM_POINT_TYPE},
    {":", COLON_TYPE},
    {",", COMMA_TYPE},
    {"<>!", COMPARE_TYPE},
    {"=", ASSIGN_TYPE},
    {"0123456789", NUM_TYPE},
};

const types array_of_key_words[LEN_OF_KEY_WORDS_ARRAY] = 
{
    {"if" , COND_TYPE},
    {"elif" , COND_TYPE},
    {"else" , COND_TYPE},
    {"while", COND_TYPE},
    {"getnum", COMM_TYPE},
    {"print", COMM_TYPE},
    {"stop", COMM_TYPE},
};

const char * const array_of_funcs_commands[LEN_OF_FUNCS_COMMANDS] = {"\0", "func_def", "func_call"};
const char * const pars_buffer[NUM_OF_PARS] = {"\0", "(", ")", "{", "}"};
const char * const signes_buffer[NUM_OF_SIGNES] = {"\0", "+", "-", "*", "/", "^"};
const char * const signes_asm_buffer[NUM_OF_SIGNES] = {"\0", "add", "sub", "imul", "div", "DEG"};
const char * const delimiters[NUM_OF_DELIMITERS] = {"\0", ";", ",", ":"};
const char * const compare_symbols[NUM_OF_SYMBOLS] = {"\0", "==", "!=", ">", "<", ">=", "<="};
const char * const compare_asm_symbols[NUM_OF_SYMBOLS] = {"\0", "je", "jne", "jg", "jl", "jge", "jle"};
const char * const assign_buffer[NUM_OF_SYMBOLS] = {"\0", "="};
const char * const operations_buffer[NUM_OF_OPERATIONS] = {"\0", "log", "sin", "cos", "tg", "ctg"}; // OP_TYPE
const char * const symbols_in_words[NUM_OF_SYMBOLS] = {"\0", "==", "&jt;", "&lt;", "&ge;", "&le;"};
const char * const io_buffer[NUM_OF_IO] = {"\0", "getnum", "print", "stop"};
const char * const conditions_buffer[NUM_OF_CONDS] = {"\0", "if", "elif", "else", "while"};

struct buffer_info
{
    const char * const *buffer;
    const ssize_t buffer_len;
};

const struct buffer_info array_of_structures_info[] = 
{
    {NULL, 0},                     
    {NULL, 0},                     
    {pars_buffer, NUM_OF_PARS},    
    {conditions_buffer, NUM_OF_CONDS}, 
    {operations_buffer, NUM_OF_OPERATIONS},
    {array_of_funcs_commands, LEN_OF_FUNCS_COMMANDS}, 
    {array_of_funcs_commands, LEN_OF_FUNCS_COMMANDS}, 
    {delimiters, NUM_OF_DELIMITERS},  
    {delimiters, NUM_OF_DELIMITERS}, 
    {delimiters, NUM_OF_DELIMITERS}, 
    {assign_buffer, 1},
    {compare_symbols, NUM_OF_SYMBOLS}, 
    {signes_buffer, LEN_OF_SIGNES_ARRAY - 1},
    {io_buffer, NUM_OF_IO}    
};

enum functionss_t 
{
    NO_FUNC,
    FUNC_DEF,
    FUNC_CALL
};

enum pars_t 
{
    NO_PAR,
    ROUND_PAR_OPEN,
    ROUND_PAR_CLOSE,
    FIGURE_PAR_OPEN,
    FIGURE_PAR_CLOSE
};

enum delimiters_t
{
    NO_DELIMIT,
    SEM_POINT,
    COMMA
};

enum input_output_stop_t 
{
    NO_COM,
    GETNUM,
    PRINT,
    STOP
};

enum condition_t
{
    NO_COND,
    IF,
    ELIF,
    ELSE,
    WHILE
};

enum sign_t 
{
    NO_SIGN,
    ADD,
    SUB,
    MUL,
    DIV,
    DEG,
    SQRT
};

enum operation_t 
{
    NO_OP,
    LOG,
    SIN,
    COS,
    TG,
    CTG
};

enum symbol_t 
{
    NO_SYM,
    JE,
    JNE,
    JA,
    JB,
    JAE,
    JBE
};

typedef struct
{
    type_t type;
    char *name;
    int number;
} token_t;

typedef struct 
{
    token_t *tokens_buffer;
    ssize_t tokens_size;
    ssize_t tokens_capacity;
} tokens_t;

typedef struct 
{
    char *name;          
    int stack_offset;    
    bool is_parameter;   
} sem_var_t;

typedef struct node_s 
{
    type_t type;
    char *name;
    int number;            
    int stack_offset;   
    struct node_s *left;
    struct node_s *right;
    struct node_s *parent;
} node_t;

typedef struct 
{
    node_t *node;                
    size_t num_of_parameters;    
    size_t num_of_definitions;   
    
    sem_var_t *locals;
    size_t local_count;
    size_t local_capacity;
} sem_func_t;


typedef struct 
{
    sem_func_t *functions;
    size_t func_count;
    size_t func_capacity;

    sem_var_t *globals;
    size_t globals_count;
    size_t globals_capacity;

    sem_func_t *current_function; 
    int current_stack_pointer;    
} SymbolTable;

typedef struct
{
    ssize_t num_of_el;
    node_t *root;
    const char *file_name;
} program_tree;

enum Program_Errors
{
    NO_ERROR,
    NULL_POINTER_ON_TREE,
    TREE_IS_EMPTY,
    ELEMENT_NOT_FOUND,
    ERROR_DURING_THE_CREATION_OF_THE_TREE,
    NEGATIVE_NUM_OF_ELEMENTS,
    NULL_POINTER_ON_ROOT,
    ROOT_HAVE_PARENT,
    SON_HAS_WRONG_PARENT,
    ERROR_DURING_OPENING_FILE,
    ERROR_DURING_CLOSING_FILE,
    ERROR_DURING_MEMORY_ALLOCATION,
    ERROR_DURING_READ_FILE,
    INCORRECT_DATA_IN_FILE,
    COLORS_ARE_NOT_SUPPORTED,
    NON_EXISTENT_VALUE,
    DIVISION_BY_ZERO_IS_UNACCEPTABLE,
    ABSENCE_SEMICOLON,
    ABSENCE_PAR_OPEN,
    ABSENCE_PAR_CLOSE,
    EMPTY_FUNC_ARG,
    ABSENCE_ASSIGN,
    ABSENCE_PARAMS,
    ABSENCE_RIGHT_PARAM,
    FUNCTION_REDEFINING
};

// ******************* PROGRAM FILE READING FUNCTIONS *******************

char *ReadExpressionFromFile(const char *name_of_file, Program_Errors *err);
size_t return_num_of_bytes_in_file(int fd1);

// ******************* TOKEN FUNCTIONS *******************

Program_Errors MakeTokensBuffer(tokens_t *tokens, char **expression);
void DestroyTokensBuffer(tokens_t *tokens);
void NeccesaryExpansion(tokens_t *tokens);
const char *IsChrInside(char **expression, const char *string);
void MakeToken(tokens_t *tokens, char **expression, type_t type, size_t len_of_token_name);
void MakeFunctionToken(tokens_t *tokens, char **expression, const char *func_act);
void MakeFuncToken(tokens_t *tokens, char **expression, const bool is_func_def);
void MakeCompOrAssignToken(tokens_t *tokens, char **expression, bool is_equal);
void MakeNumToken(tokens_t *tokens, char **expression);
void MakeVarToken(tokens_t *tokens, char **expression);

// ******************* TREE BUILDING FUNCTIONS *******************

Program_Errors MakeTreeFromProgram(program_tree *tree, const char *logfile_name, const char *name_of_file_with_tokens);
node_t *BuildingATree(program_tree *tree, tokens_t tokens, Program_Errors *err);
node_t *GetG(program_tree *tree, tokens_t tokens, Program_Errors *err);

Program_Errors MakeTreeFromFile(program_tree *tree, const char *logfile_name, const char *name_of_file);
void SplitIntoParts(char *tree_buffer);
char *ReadNodeFromBuffer(program_tree *tree, char **position, node_t **node, node_t *parent);
Program_Errors NodeFromFileInit(program_tree *tree, char **position, node_t **node, node_t *parent);
node_t *InitNewNode(program_tree *tree, token_t token, node_t *node_left, node_t *node_right, Program_Errors *err);

// ******************* TREE SAVING FUNCTIONS *******************

Program_Errors SaveTreeToFile(program_tree *tree, const char *name_of_file);
void SaveTreeToFileRecursive(program_tree *tree, FILE *fp, node_t *node);

// ******************* SEMANTIC ANALYSIS FUNCTIONS *******************

void RunSemanticAnalysis(SymbolTable *sym_table, node_t *node);
Program_Errors SymbolTableInit(SymbolTable *sym_table);
void SymbolTableDestroy(SymbolTable *sym_table);
void AddVariableToSymbolTable(SymbolTable *sym_table, node_t *node, bool is_param, bool check_params);
sem_var_t *CheckVariablesCapacity(size_t var_count, size_t var_capacity, sem_var_t *var_buffer);
void AnalyzeParameters(SymbolTable *sym_table, node_t *node);
sem_func_t *AddFunctionToSymbolTable(SymbolTable *sym_table, node_t *node);

#endif