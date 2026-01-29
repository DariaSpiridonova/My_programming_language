#include "tokens.h"
#include "root_functions.h"

/**
 * @brief
 * This function saves the tree to a file using prefix traversal 
 * @param tree a tree whose nodes are syntactic constructions of the language
 * @param name_of_file file for saving your tree
 */
Program_Errors SaveTreeToFile(program_tree *tree, const char *name_of_file)
{
    ASSERTS(tree);

    Program_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    FILE *file_to_read = fopen(name_of_file, "w");
    if (!OpenFileSuccess(file_to_read, name_of_file))
        return ERROR_DURING_OPENING_FILE;

    SaveTreeToFileRecursive(tree, file_to_read, tree->root);

    if (!CloseFileSuccess(file_to_read, name_of_file))
        return ERROR_DURING_CLOSING_FILE;

    return err;
}

void SaveTreeToFileRecursive(program_tree *tree, FILE *fp, node_t *node)
{
    if (node == NULL)
    {
        fprintf(fp, "nil ");
        return;
    }
    
    fprintf(fp, " ( ");

    ssize_t num_of_parameters = -1;
    ssize_t num_of_definitions = -1;
    if (node->type == FUNC_DEF_TYPE || node->type == FUNC_CALL_TYPE)
    {
        for (ssize_t i = 0; i < tree->functions_s.functions_size; i++)
        {
            if (!strcmp(tree->functions_s.functions[i].name, node->name))
            {
                num_of_parameters = tree->functions_s.functions[i].num_of_parameters;
                num_of_definitions = tree->functions_s.functions[i].num_of_definitions;
                break;
            }
        }
    }
    
    switch((int)node->type)
    {
        case (int)VAR_TYPE:
            fprintf(fp, " \"%d\" ", (int)node->type);
            fprintf(fp, " \"%s\" ", node->name);
            break;

        case (int)NUM_TYPE:
        case (int)PAR_TYPE:
        case (int)COND_TYPE:
        case (int)OP_TYPE:
        case (int)COMMA_TYPE:
        case (int)SEM_POINT_TYPE:
        case (int)COLON_TYPE:
        case (int)ASSIGN_TYPE:
        case (int)COMPARE_TYPE:
        case (int)SIGN_TYPE:
        case (int)COMM_TYPE:
            fprintf(fp, " \"%d\" ", (int)node->type);
            fprintf(fp, " \"%d\" ", node->number);
            break;

        case (int)FUNC_DEF_TYPE:
        case (int)FUNC_CALL_TYPE:
            fprintf(fp, " \"%d\" ", (int)node->type);
            fprintf(fp, " \"%s\" ", node->name);
            fprintf(fp, " \"%zd\" \"%zd\" ", num_of_parameters, num_of_definitions);
            break;

        default:
            printf("A non-existent type was found");
            printf(" \"%s\" ", node->name);
            break;
            
    }
    SaveTreeToFileRecursive(tree, fp, node->left);
    SaveTreeToFileRecursive(tree, fp, node->right);
    fprintf(fp, " ) ");
    return;
}

Program_Errors MakeTreeFromFile(program_tree *tree, const char *logfile_name, const char *name_of_file)
{
    Program_Errors err = NO_ERROR;
 
    char *expression = ReadExpressionFromFile(name_of_file, &err);
    printf("expression = %s\n", expression);
    if (err)
    {
        free(expression);
        return err;
    }
    
    SplitIntoParts(expression);
    char *position = expression;

    tree->num_of_el = 0;
    tree->variables_s.variables = (variable *)calloc((size_t)NUM_OF_VARIABLES, sizeof(variable));
    tree->variables_s.variables_size = 0;
    tree->variables_s.variables_capacity = NUM_OF_VARIABLES;
    tree->functions_s.functions = (function *)calloc((size_t)NUM_OF_FUNCTIONS, sizeof(function));
    tree->functions_s.functions_size = 0;
    tree->functions_s.functions_capacity = NUM_OF_FUNCTIONS;
    tree->file_name = logfile_name;
    tree->root = NULL;

    ReadNodeFromBuffer(tree, &position, &(tree->root), NULL);
    
    if (tree->root != NULL)
        tree->root->parent = NULL;

    free(expression);

    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    return err;
}

char *ReadNodeFromBuffer(program_tree *tree, char **position, node_t **node, node_t *parent)
{
    #ifdef ENABLE_THE_VERIFIER
    if ((err = CalculationTreeVerify(tree)))
    {
        return NULL;
    }
    #endif

    SkipSpaces(position);

    if (**position == '(')
    {
        (*position)++;
        SkipSpaces(position);

        if (**position == '"')
        {
            printf("Is PAR\n");
            (*position)++;
            NodeFromFileInit(tree, position, node, parent);
        }

        else return NULL;
    }

    else if (**position == 'n' && *(*position + 1) == 'i' && *(*position + 2) == 'l' && *(*position + 3) == ' ')
    {
        SkipSpaces(position);

        *position += 3;
        return *position;
    }

    *position = ReadNodeFromBuffer(tree, position, &((*node)->left), *node);
    *position = ReadNodeFromBuffer(tree, position, &((*node)->right), *node);

    SkipSpaces(position);
    if (**position == ')')
    {
        (*position)++;
        return *position;
    }

    #ifdef ENABLE_THE_VERIFIER
    if ((err = AkinatorVerify(tree)))
    {
        return *position;
    }
    #endif

    return *position;
}

Program_Errors NodeFromFileInit(program_tree *tree, char **position, node_t **node, node_t *parent)
{
    Program_Errors err = NO_ERROR;

    printf("inside\n");

    *node = (node_t *)calloc(1, sizeof(node_t));

    if (*node == NULL)
    {
        printf("Error during memory allocation\n");
    }

    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->parent = parent;

    int type_num = atoi(*position);
    (*node)->type = (type_t)type_num;
    printf("type_num = <%d>\n", type_num);
    
    *position = strchr(*position, '\0') + 1;
    *position = strchr(*position, '"') + 1;
    printf("*position = <%s>\n", *position);
    
    if (type_num == (int)VAR_TYPE  || type_num == (int)NUM_TYPE || type_num == (int)FUNC_DEF_TYPE || type_num == (int)FUNC_CALL_TYPE)
    {
        (*node)->name = strdup(*position);
        if (type_num == (int)NUM_TYPE)
            (*node)->number = atoi(*position);
        printf("(*node)->name = <%s>\n", *position);
        *position = strchr(*position, '\0') + 1;
    }

    else 
    {
        (*node)->name = (char *)array_of_structures_info[type_num].buffer[atoi(*position)];
        (*node)->number = atoi(*position);
        *position = strchr(*position, '\0') + 1;
    }

    ssize_t num_of_parameters = -1;
    ssize_t num_of_defs = -1;
    if (type_num == (int)FUNC_DEF_TYPE || type_num == (int)FUNC_CALL_TYPE)
    {
        *position = strchr(*position, '"') + 1;
        printf("*position = <%s>\n", *position);

        num_of_parameters = (ssize_t)atoi(*position);
        
        *position = strchr(*position, '\0') + 1;
        *position = strchr(*position, '"') + 1;

        num_of_defs = (ssize_t)atoi(*position);

        *position = strchr(*position, '\0') + 1;
    }

    switch(type_num)
    {
        case (int)VAR_TYPE:
            NewNodeVarInitByPos(tree, node, &err);
            break;

        case (int)FUNC_DEF_TYPE:
        case (int)FUNC_CALL_TYPE:
            NewNodeFuncInitByPos(tree, node, num_of_parameters, num_of_defs, type_num, &err);
            break;

        default:
            break;
    }

    if ((err = CalculationTreeVerify(tree)))
        return err;

    return err;
}

node_t *NewNodeFuncInitByPos(program_tree *tree, node_t **node, ssize_t num_of_parameters, ssize_t num_of_defs, int type_num, Program_Errors *err)
{
    bool is_func = false;
    for (ssize_t i = 0; i < tree->functions_s.functions_size; i++)
    {
        if (!strcmp(tree->functions_s.functions[i].name, (*node)->name))
        {
            free((*node)->name);
            (*node)->name = tree->functions_s.functions[i].name;
            is_func = true;
            printf("func = %s\n\n\n", tree->functions_s.functions[i].name);
            break;
        }
    }

    if (!is_func)
    {
        tree->functions_s.functions[tree->functions_s.functions_size] = {num_of_parameters, strdup((*node)->name), false, num_of_defs};
        free((*node)->name);
        (*node)->name = tree->functions_s.functions[tree->functions_s.functions_size].name;
        printf("func = %s\n\n\n", tree->functions_s.functions[tree->functions_s.functions_size].name);
        tree->functions_s.functions_size++;
    }

    (type_num == (int)FUNC_DEF_TYPE) ? (*node)->type = FUNC_DEF_TYPE : (*node)->type = FUNC_CALL_TYPE;

    if (tree->functions_s.functions_size >= tree->functions_s.functions_capacity - 1)
    {
        tree->functions_s.functions_capacity *= 2;
        tree->functions_s.functions = (function *)realloc(tree->functions_s.functions, (size_t)tree->functions_s.functions_capacity*sizeof(function));

        if (tree->functions_s.functions == NULL)
        {
            printf("ERROR_DURING_MEMORY_ALLOCATION in NewNodeVarInit\n");
            *err = ERROR_DURING_MEMORY_ALLOCATION;
        }
    }

    return *node;
}

node_t *NewNodeVarInitByPos(program_tree *tree, node_t **node, Program_Errors *err)
{
    bool is_var = false;
    for (ssize_t i = 0; i < tree->variables_s.variables_size; i++)
    {
        if (!strcmp(tree->variables_s.variables[i].name, (*node)->name))
        {
            free((*node)->name);
            (*node)->name = tree->variables_s.variables[i].name;
            is_var = true;
            printf("var = %s\n\n\n", tree->variables_s.variables[i].name);
            break;
        }
    }

    if (!is_var)
    {
        tree->variables_s.variables[tree->variables_s.variables_size] = {NAN, strdup((*node)->name), false};
        free((*node)->name);
        (*node)->name = tree->variables_s.variables[tree->variables_s.variables_size].name;
        printf("var = %s\n\n\n", tree->variables_s.variables[tree->variables_s.variables_size].name);
        tree->variables_s.variables_size++;
    }

    (*node)->type = VAR_TYPE;

    if (tree->variables_s.variables_size >= tree->variables_s.variables_capacity - 1)
    {
        tree->variables_s.variables_capacity *= 2;
        tree->variables_s.variables = (variable *)realloc(tree->variables_s.variables, (size_t)tree->variables_s.variables_capacity*sizeof(variable));

        if (tree->variables_s.variables == NULL)
        {
            printf("ERROR_DURING_MEMORY_ALLOCATION in NewNodeVarInit\n");
            *err = ERROR_DURING_MEMORY_ALLOCATION;
        }
    }

    return *node;
}

void SplitIntoParts(char *tree_buffer)
{
    assert(tree_buffer != NULL);
    
    char *ptr_to_quotation_mark = tree_buffer;
    for (size_t i = 0; (ptr_to_quotation_mark = strchr(ptr_to_quotation_mark, '"')) != NULL; i++)
    {
        if (i % 2) *ptr_to_quotation_mark = '\0';
        ptr_to_quotation_mark++;
    }
}