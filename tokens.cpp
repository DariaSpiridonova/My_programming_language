#include "tokens.h"
#include "root_functions.h"

static bool LookForBufferType(tokens_t *tokens, type_t type);

Program_Errors MakeTreeFromProgram(program_tree *tree, const char *logfile_name, const char *name_of_file_with_expression)
{
    Program_Errors err = NO_ERROR;
    
    char *expression = ReadExpressionFromFile(name_of_file_with_expression, &err);
    char *expression_beginning = expression;

    printf("expression = %s\n", expression);

    tokens_t tokens = {0};

    if ((err = MakeTokensBuffer(&tokens, &expression)))
    {
        return err;
    }

    printf("tokens_size = %zd\n", tokens.tokens_size);

    for (ssize_t i = 0; i < tokens.tokens_size; i++)
    {
        printf("%zd: name = %s\n", i, tokens.tokens_buffer[i].name);
        printf("     type = %s\n", type_buffer[(int)tokens.tokens_buffer[i].type]);
    }
    
    free(expression_beginning);
    
    if (err)
    {
        return err;
    }
    
    tree->num_of_el = 0;
    tree->variables_s.variables = (variable *)calloc((size_t)NUM_OF_VARIABLES, sizeof(variable));
    tree->variables_s.variables_size = 0;
    tree->variables_s.variables_capacity = NUM_OF_VARIABLES;
    tree->functions_s.functions = (function *)calloc((size_t)NUM_OF_FUNCTIONS, sizeof(function));
    tree->functions_s.functions_size = 0;
    tree->functions_s.functions_capacity = NUM_OF_FUNCTIONS;
    tree->file_name = logfile_name;
    
    tree->root = BuildingATree(tree, tokens, &err);
    
    if (tree->root != NULL)
        tree->root->parent = NULL;

    DestroyTokensBuffer(&tokens);

    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    return err;
}

Program_Errors MakeTokensBuffer(tokens_t *tokens, char **expression)
{
    Program_Errors err = NO_ERROR;
    
    assert(expression != NULL);

    tokens->tokens_buffer = (token_t *)calloc(NUM_OF_TOKENS, sizeof(token_t));
    if (tokens->tokens_buffer == NULL)
    {
        printf("ERROR_DURING_MEMORY_ALLOCATION during creating a tokens_buffer");
        err = ERROR_DURING_MEMORY_ALLOCATION;
    }

    tokens->tokens_capacity = NUM_OF_TOKENS;
    tokens->tokens_size = 0;

    while (**expression != '\0')
    {
        NeccesaryExpansion(tokens);
        bool is_correct_symbol = false;
        bool cont = false;
        const char *ch = 0;

        for (ssize_t i = 0; i < LEN_OF_SIGNES_ARRAY; i++)
        {
            if ((ch = IsChrInside(expression, array_of_signes[i].signes)))
            {
                if (array_of_signes[i].type == COMPARE_TYPE)
                    MakeCompOrAssignToken(tokens, expression, false);
                else if (array_of_signes[i].type == ASSIGN_TYPE)
                    MakeCompOrAssignToken(tokens, expression, true);
                else if (array_of_signes[i].type == NUM_TYPE || (**expression == '-' && tokens->tokens_size - 1 >= 0 && tokens->tokens_buffer[tokens->tokens_size - 1].type != VAR_TYPE && tokens->tokens_buffer[tokens->tokens_size - 1].type != NUM_TYPE))
                    MakeNumToken(tokens, expression);
                else
                {
                    printf("%c\n", **expression);
                    MakeToken(tokens, expression, array_of_signes[i].type, 1);
                }
                is_correct_symbol = true;
                SkipSpaces(expression);
                cont = true;
                break;
            }
        }

        if (cont) continue;

        for (ssize_t i = 0; i < LEN_OF_KEY_WORDS_ARRAY; i++)
        {
            if (!strncmp(*expression, array_of_key_words[i].signes, strlen(array_of_key_words[i].signes)))
            {
                is_correct_symbol = true;
                printf("Word : %s\n", array_of_key_words[i].signes);
                MakeToken(tokens, expression, array_of_key_words[i].type, strlen(array_of_key_words[i].signes));
                continue;
            }
        }

        for (ssize_t i = 1; i < LEN_OF_FUNCS_COMMANDS; i++)
        {
            if (!strncmp(*expression, array_of_funcs_commands[i], strlen(array_of_funcs_commands[i])))
            {
                is_correct_symbol = true;
                MakeFunctionToken(tokens, expression, array_of_funcs_commands[i]);
                continue;
            }
        }

        for (ssize_t i = 1; i < NUM_OF_OPERATIONS; i++)
        {
            if (!strncmp(*expression, operations_buffer[i], strlen(operations_buffer[i])))
            {
                is_correct_symbol = true;
                MakeToken(tokens, expression, OP_TYPE, strlen(operations_buffer[i]));
                continue;
            }
        }

        if (isalnum(**expression))
        {
            is_correct_symbol = true;
            MakeVarToken(tokens, expression);
            SkipSpaces(expression);

            continue;
        }

        if (!is_correct_symbol)
        {
            printf(RED_COLOR_BEGIN "The symbol \"%c\" in the file program.txt is incorrect\n", **expression);
            printf("Please, fix the error in the program\n" RED_COLOR_END);
            exit(1);
        }
    }

    return err;
}

void DestroyTokensBuffer(tokens_t *tokens)
{
    for (ssize_t i = 0; i < tokens->tokens_size; i++)
    {
        free(tokens->tokens_buffer[i].name);
    }

    free(tokens->tokens_buffer);
    tokens->tokens_size = 0;
    tokens->tokens_capacity = 0;

    tokens->tokens_buffer = NULL;
}

void NeccesaryExpansion(tokens_t *tokens)
{
    if (tokens->tokens_size >= tokens->tokens_capacity)
    {
        tokens->tokens_capacity *= 2;
        tokens->tokens_buffer = (token_t *)realloc(tokens->tokens_buffer, (size_t)tokens->tokens_capacity * sizeof(token_t));
    }
}

const char *IsChrInside(char **expression, const char *string)
{
    return strchr(string, **expression);
}

void MakeFunctionToken(tokens_t *tokens, char **expression, const char *func_act)
{
    (*expression) += strlen(func_act);
    SkipSpaces(expression);

    if (isalnum(**expression))
    {
        MakeFuncToken(tokens, expression, !strcmp(func_act, "func_def") ? 0 : 1);
        SkipSpaces(expression);
    }

    NeccesaryExpansion(tokens);
}

void MakeFuncToken(tokens_t *tokens, char **expression, const bool is_func_call)
{
    char *func = *expression;
    size_t len = 0;
    while (isalnum(**expression))
    {
        (*expression)++;
        len++;
    }

    tokens->tokens_buffer[tokens->tokens_size].type = (is_func_call ? FUNC_CALL_TYPE : FUNC_DEF_TYPE);
    tokens->tokens_buffer[tokens->tokens_size].number = is_func_call + 1;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(func, len);

    tokens->tokens_size++;
}

void MakeCompOrAssignToken(tokens_t *tokens, char **expression, bool is_equal)
{
    size_t n = 1;
    if (*(*expression+1) == '=')
    {
        n++;
    }
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, n);
    
    if (!is_equal || (is_equal && n > 1)) 
    {
        tokens->tokens_buffer[tokens->tokens_size].type = COMPARE_TYPE;
        if (!LookForBufferType(tokens, COMPARE_TYPE))
        {
            printf(RED_COLOR_BEGIN "The element <%s> was not found in any of the arrays" RED_COLOR_END, tokens->tokens_buffer[tokens->tokens_size].name);
            exit(-2);
        }
    }
    else
    {
        tokens->tokens_buffer[tokens->tokens_size].type = ASSIGN_TYPE;
        tokens->tokens_buffer[tokens->tokens_size].number = 1;
    }
    
    tokens->tokens_size++;
    (*expression) += n;

    NeccesaryExpansion(tokens);
}

void MakeToken(tokens_t *tokens, char **expression, type_t type, size_t len_of_token_name)
{
    tokens->tokens_buffer[tokens->tokens_size].type = type;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(*expression, len_of_token_name);
    if (!LookForBufferType(tokens, type))
    {
        printf(RED_COLOR_BEGIN "The element <%s> was not found in any of the arrays!" RED_COLOR_END, tokens->tokens_buffer[tokens->tokens_size].name);
        exit(-1);
    }

    *expression += len_of_token_name;

    tokens->tokens_size++;
    SkipSpaces(expression);
    NeccesaryExpansion(tokens);
}

static bool LookForBufferType(tokens_t *tokens, type_t type)
{
    for (int i = 1; i < array_of_structures_info[(int)type].buffer_len; i++)
    {
        if (!strcmp(tokens->tokens_buffer[tokens->tokens_size].name, array_of_structures_info[(int)type].buffer[i]))
        {
            tokens->tokens_buffer[tokens->tokens_size].number = i;
            return true;
        }
    }  

    return false;
}

void MakeNumToken(tokens_t *tokens, char **expression)
{
    tokens->tokens_buffer[tokens->tokens_size].type = NUM_TYPE;
    bool is_minus = false;
    if ((**expression) == '-')
    {
        is_minus = true;
        (*expression)++;
        SkipSpaces(expression);
    }

    if (!IsChrInside(expression, "0123456789"))
    {
        printf(RED_COLOR_BEGIN "The unary minus sign can only be used before a number.\n" RED_COLOR_END);
        exit(-3);
    }
    
    char *value = (*expression)++;
    size_t n = 1;
    ssize_t num_of_dots = 0;
    while (IsChrInside(expression, "0123456789.")) 
    {
        if (**expression == '.')
            num_of_dots++;
        (*expression)++;
        n++;
    }

    if (num_of_dots > 1)
    {
        printf(RED_COLOR_BEGIN "A number cannot contain more than one dot inside it.\n" RED_COLOR_END);
        exit(2);
    }

    tokens->tokens_buffer[tokens->tokens_size].name = strndup(value, n);
    tokens->tokens_buffer[tokens->tokens_size].number = is_minus ? (0 - atoi(value)) : atoi(value);

    tokens->tokens_size++;
}

void MakeVarToken(tokens_t *tokens, char **expression)
{
    char *var = *expression;
    size_t len = 0;
    while (isalnum(**expression))
    {
        (*expression)++;
        len++;
    }

    tokens->tokens_buffer[tokens->tokens_size].type = VAR_TYPE;
    tokens->tokens_buffer[tokens->tokens_size].name = strndup(var, len);

    tokens->tokens_size++;
}

char *ReadExpressionFromFile(const char *name_of_file, Program_Errors *err)
{
    FILE *file_to_read = fopen(name_of_file, "r");
    if (!OpenFileSuccess(file_to_read, name_of_file))
    {
        *err = ERROR_DURING_OPENING_FILE;
        return NULL;
    }

    fseek(file_to_read, 0, SEEK_SET);
    size_t num_of_bytes_in_file = return_num_of_bytes_in_file(fileno(file_to_read));

    char *expression = (char *)calloc(num_of_bytes_in_file + 2, sizeof(char));
    if (expression == NULL)
    {
        *err = ERROR_DURING_MEMORY_ALLOCATION;
        return NULL;
    }

    size_t num_success_read_symbols = fread(expression, sizeof(char), num_of_bytes_in_file, file_to_read);
    if (num_success_read_symbols < num_of_bytes_in_file)
    {
        free(expression);
        *err = ERROR_DURING_READ_FILE;
        return NULL;
    }

    expression[num_success_read_symbols] = '\0';
    
    if (!CloseFileSuccess(file_to_read, name_of_file))
    {
        free(expression);
        *err = ERROR_DURING_CLOSING_FILE;
        return NULL;
    }

    return expression;
}

size_t return_num_of_bytes_in_file(int fd1)
{
    struct stat st1 = {};
    fstat(fd1, &st1);

    return (size_t)st1.st_size;
}

node_t *BuildingATree(program_tree *tree, tokens_t tokens, Program_Errors *err)
{
    return GetG(tree, tokens, err);
}