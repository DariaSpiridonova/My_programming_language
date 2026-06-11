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

    if ((err = CalculationTreeVerify(tree)))
        return err;

    return err;
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