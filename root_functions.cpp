#include "tokens.h"
#include "root_functions.h"

void RunSemanticAnalysis(SymbolTable *sym_table, node_t *node)
{
    if (node == NULL)
        return;

    switch(node->type)
    {
        case FUNC_DEF_TYPE:
            sym_table->current_function = AddFunctionToSymbolTable(sym_table, node);
        
            sym_table->current_stack_pointer = -8; 

            // passing through the left subtree - registration of parameters
            AnalyzeParameters(sym_table, node->left);

            // passing through the right subtree — analyzing the function body
            RunSemanticAnalysis(sym_table, node->right);

            // moving out of the context of the function
            sym_table->current_function = NULL;
            return;

        case VAR_TYPE:
            AddVariableToSymbolTable(sym_table, node, false, false);
            return;

        default:
            break;
        
    }

    RunSemanticAnalysis(sym_table, node->left);
    RunSemanticAnalysis(sym_table, node->right); 

    return;
}

sem_func_t *AddFunctionToSymbolTable(SymbolTable *sym_table, node_t *node)
{
    for (size_t i = 0; i < sym_table->func_count; i++)
    {
        if (!strcmp(sym_table->functions[i].node->name, node->name))
        {
            if (++sym_table->functions[i].num_of_definitions > 1)
            {
                fprintf(stderr, "Redefinition of the %s function\n", node->name);
                exit(1);
            }
            return &sym_table->functions[i];
        }
    }

    if (sym_table->func_count >= sym_table->func_capacity) 
    {
        sym_table->func_capacity *= 2;
        sym_table->functions = (sem_func_t *)realloc(sym_table->functions, sym_table->func_capacity * sizeof(sem_func_t));
        if (sym_table->functions == NULL)
        {
            fprintf(stderr, "An error occured during memory allocation\n");
            exit(-1);
        }
    }

    sem_func_t *curr_func = &sym_table->functions[sym_table->func_count++];
    curr_func->node = node;
    curr_func->num_of_parameters = 0;
    curr_func->num_of_definitions = 1;
    curr_func->local_capacity = 16;
    curr_func->locals = (sem_var_t *)calloc(curr_func->local_capacity, sizeof(sem_var_t));

    curr_func->local_count = 0;
    if (curr_func->locals == NULL)
    {
        free(sym_table->functions);
        fprintf(stderr, "An error occured during memory allocation\n");
        exit(-2);
    }

    return curr_func;
}

void AnalyzeParameters(SymbolTable *sym_table, node_t *node)
{
    if (node == NULL)
        return;

    AnalyzeParameters(sym_table, node->left);
    AnalyzeParameters(sym_table, node->right);

    if (node->type == VAR_TYPE)
        AddVariableToSymbolTable(sym_table, node, true, true);

    return;
}

void AddVariableToSymbolTable(SymbolTable *sym_table, node_t *node, bool is_param, bool check_params)
{
    if (sym_table->current_function == NULL)
    {
        for (size_t i = 0; i < sym_table->globals_count; i++)
        {
            if (!strcmp(sym_table->globals[i].name, node->name))
                return;
        }
        
        sym_table->globals[sym_table->globals_count++].name = node->name;
        return;
    }

    sem_func_t *current_func = sym_table->current_function;

    for (size_t i = 0; i < current_func->local_count; i++)
    {
        if (!strcmp(current_func->locals[i].name, node->name))
        {
            if (check_params)
            {
                fprintf(stderr, "redefinition of parameter '%s' inside definition of function '%s')", node->name, sym_table->current_function->node->name);
                exit(2);
            }
            node->stack_offset = current_func->locals[i].stack_offset;
            return;
        }
    }
    if (CheckVariablesCapacity(current_func->local_count, current_func->local_capacity, current_func->locals) == NULL ||
        CheckVariablesCapacity(sym_table->globals_count, sym_table->globals_capacity,   sym_table->globals)   == NULL)
    {
        fprintf(stderr, "memory allocate error");
        exit(2);
    }

    sym_table->current_stack_pointer -= 8;

    sem_var_t *curr_var = &current_func->locals[current_func->local_count++];

    curr_var->name = node->name; // !!!!!! NO MEMORY ALLOCATE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    curr_var->stack_offset = sym_table->current_stack_pointer;
    node->stack_offset = sym_table->current_stack_pointer;

    curr_var->is_parameter = is_param ? true : false;

    return;
}

sem_var_t *CheckVariablesCapacity(size_t var_count, size_t var_capacity, sem_var_t *var_buffer)
{
    if (var_count >= var_capacity)
    {
        var_capacity *= 2;
        var_buffer = (sem_var_t *)realloc(var_buffer, var_capacity * sizeof(sem_var_t));
        if (var_buffer == NULL)
        {
            fprintf(stderr, "memory allocate error");
            exit(3);
        }
    }

    return var_buffer;
}

Program_Errors SymbolTableInit(SymbolTable *sym_table)
{
    assert(sym_table != NULL);
    
    if (sym_table == NULL) 
        return ERROR_DURING_MEMORY_ALLOCATION;

    sym_table->func_capacity = 16;
    sym_table->functions     = (sem_func_t *)calloc(sym_table->func_capacity, sizeof(sem_func_t));
    sym_table->func_count    = 0;
    
    sym_table->globals_capacity = 32;
    sym_table->globals          = (sem_var_t *)calloc(sym_table->globals_capacity, sizeof(sem_var_t));
    sym_table->globals_count    = 0;

    if (sym_table->functions == NULL || sym_table->globals == NULL) 
        return ERROR_DURING_MEMORY_ALLOCATION;
    
    sym_table->current_function      = NULL;
    sym_table->current_stack_pointer = -8;

    return NO_ERROR;
}

void SymbolTableDestroy(SymbolTable *sym_table)
{
    assert(sym_table != NULL);

    for (size_t i = 0; i < sym_table->func_count; i++)
        free(sym_table->functions[i].locals);

    free(sym_table->functions);

    free(sym_table->globals);

    return;
}

Program_Errors CalculationTreeVerify(program_tree *tree)
{
    if (tree == NULL) return NULL_POINTER_ON_TREE;
    else if (tree->num_of_el < 0) return NEGATIVE_NUM_OF_ELEMENTS;
    else if (tree->root == NULL) return NULL_POINTER_ON_ROOT; 
    else if (tree->root->parent != NULL) return ROOT_HAVE_PARENT;  
    
    node_t  *parent = NULL;
    node_t  *son = NULL;

    if (!CheckSonsAndParents(tree->root, &son, &parent))
    {
        printf("son %p has a parent %p, but not %p", son, son->parent, parent);
        return SON_HAS_WRONG_PARENT;
    } 

    return NO_ERROR;
}

bool CheckSonsAndParents(node_t  *node, node_t  **son, node_t  **parent)
{
    bool flag = true;
    CheckSonsAndParentsRecursive(node, &flag, son, parent);

    return flag;
}

void CheckSonsAndParentsRecursive(node_t  *node, bool *flag, node_t  **son, node_t  **parent)
{
    if (node == NULL)
        return;

    CheckSonsAndParentsRecursive(node->left, flag, son, parent);

    if (node->left != NULL) 
    {
        if (node->left->parent != node) 
        {
            *flag = false;
            *son = node->left->parent;
            *parent = node;
        }
    }
    if (node->right != NULL) 
    {
        if (node->right->parent != node) 
        {
            *flag = false;
            *son = node->right->parent;
            *parent = node;
        }
    }

    CheckSonsAndParentsRecursive(node->right, flag, son, parent);
    
    return;
}

Program_Errors  CalculationTreeDestroy(program_tree  *tree)
{
    ASSERTS(tree);

    Program_Errors  err = NO_ERROR;
    err = CalculationTreeDestroyRecursive(tree, &(tree->root));

    return err;
}

Program_Errors  CalculationTreeDestroyRecursive(program_tree  *tree, node_t  **node)
{
    assert(tree != NULL);

    Program_Errors  err = NO_ERROR;

    #ifdef ENABLE_THE_VERIFIER
    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }
    #endif

    if (*node == NULL)
    {
        return NO_ERROR;
    }
    CalculationTreeDestroyRecursive(tree, &((*node)->left));
    CalculationTreeDestroyRecursive(tree, &((*node)->right));

    if ((*node)->type == NUM_TYPE || (*node)->type == VAR_TYPE || (*node)->type == FUNC_CALL_TYPE || (*node)->type == FUNC_DEF_TYPE)
        free((*node)->name);
    free(*node);
    tree->num_of_el--;
    *node = NULL;

    return err;
}

void CalculationTreeDump(program_tree  *tree, const char *file, int line)
{
    ASSERTS(tree);

    ssize_t rank = 0;
    DumpToConsole(tree, file, line, &rank);

    time_t rawtime;      
    struct tm *timeinfo; 
    char buffer[80];

    // Get the current calendar time
    time(&rawtime);

    // Convert the calendar time to local time
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", timeinfo);

    // Print the formatted time string
    struct timespec tstart={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    char gvfile_name[SIZE_OF_NAME] = {0};
    sprintf(gvfile_name, "%s%s%s%ld.gv", link_to_graphviz_file, tree->file_name, buffer, tstart.tv_nsec);

    DumpToLogfile(tree, tree->file_name, gvfile_name, &rank);

    char texfile_name[SIZE_OF_NAME] = {0};
    sprintf(texfile_name, "%s.tex", tree->file_name);

    //DumpToTexFile(tree, texfile_name);

    CreateGraph(tree, gvfile_name);
}

void DumpToConsole(const program_tree  *tree, const char *file, int line, ssize_t *rank)
{
    printf("ListDump called from %s :%d\n", file, line);
    Dump(stdout, tree, rank);
}

void Dump(FILE *fp, const program_tree  *tree, ssize_t *rank)
{
    fprintf(fp, "TREE[%p]\n", tree);
    fprintf(fp, "  |  \n");
    fprintf(fp, "ROOT[%p]\n", tree->root);
    fprintf(fp, "    Tree content:\n");

    ssize_t cur_rank = 0;
    ShowTree(fp, tree->root, rank, &cur_rank);

    fprintf(fp, "    rank = %zd\n", *rank);
    fprintf(fp, "\n}");
    fprintf(fp, "\n");
}

void DumpToLogfile(const program_tree  *tree, const char *logfile_name, const char *gvfile_name, ssize_t *rank)
{
    FILE *fp = fopen(logfile_name, "a");

    if (!OpenFileSuccess(fp, logfile_name))
    {
        return;
    }

    fprintf(fp, "<pre>\n");
    Dump(fp, tree, rank);

    fprintf(fp, "<img src=\"%*s.png\" alt=\"Graphviz image\" width=\"1000\">", (int)strlen(gvfile_name) - 3, gvfile_name);

    if (!CloseFileSuccess(fp, logfile_name))
    {
        return;
    }
}

void CreateGraph(const program_tree  *tree, const char *gvfile_name)
{
    FILE *fp = fopen(gvfile_name, "w");

    if (!OpenFileSuccess(fp, gvfile_name))
    {
        return;
    }

    fprintf(fp, "digraph CalculationTree_game {\n");
    fprintf(fp, "   rankdir=TB;\n");
    fprintf(fp, "   node[shape=\"record\", style=\"filled\", fillcolor=\"Pink\", fontsize=20, pin = true];\n");
    fprintf(fp, "   rank = same;\n");
    fprintf(fp, "   nodesep = 0.5;\n");
    fprintf(fp, "   edge[arrowsize = 0.5];\n");
    fprintf(fp, "   bgcolor=\"LightBlue\";\n");

    PrintEdges(fp, tree->root);
    LinkEdges(fp, tree->root);

    fprintf(fp, "\n}");
    if (!CloseFileSuccess(fp, gvfile_name))
    {
        return;
    }

    char command[SIZE_OF_NAME * 2] = {0};
    sprintf(command, "dot %s -Tpng -o %*s.png", gvfile_name, (int)strlen(gvfile_name) - 3, gvfile_name);
    
    int error = system(command);
    if (error)
    {
        printf("%d", error);
    }
}

void PrintEdges(FILE *fp, node_t  *node)
{
    if (node == NULL)
        return;

    PrintEdges(fp, node->left);

    const char *color = ChooseColor(node);

    if (node->type == COMPARE_TYPE)
    {
        for (ssize_t i = 0; i < NUM_OF_SYMBOLS; i++)
        {
            if (!strcmp(compare_symbols[i], node->name))
            {
                fprintf(fp, "\"node_%p\" [fillcolor = \"%s\", label = \"{<parent> %p | <type> %s | <data> %s | {<left_operand> %p |<right_operand> %p}}\"];\n", node, color, node->parent, type_buffer[(int)node->type], symbols_in_words[i], node->left, node->right);
                break;
            }
        }
    }

    else
        fprintf(fp, "\"node_%p\" [fillcolor = \"%s\", label = \"{<parent> %p | <type> %s | <data> %s | {<left_operand> %p |<right_operand> %p}}\"];\n", node, color, node->parent, type_buffer[(int)node->type], node->name, node->left, node->right);

    PrintEdges(fp, node->right);

    return;
}

const char *ChooseColor(node_t  *node)
{
    switch (node->type)
    {
        case FUNC_DEF_TYPE:
            return "purple";
        
        case FUNC_CALL_TYPE:
            return "mediumpurple";

        case COND_TYPE:
            return "hotpink";

        case COMM_TYPE:
            return "deeppink";

        case VAR_TYPE:
            return "violet";

        case NUM_TYPE:
            return "darkred";

        default:
            return "Pink";
    }
}

void LinkEdges(FILE *fp, node_t  *node)
{
    if (node == NULL)
        return;

    LinkEdges(fp, node->left);
    
    if (node->left != NULL)
        fprintf(fp, "\"node_%p\":left_operand -> \"node_%p\";\n", node, node->left);

    if (node->right != NULL)
        fprintf(fp, "\"node_%p\":right_operand -> \"node_%p\";\n", node, node->right);
    
    LinkEdges(fp, node->right);

    return;
}

void ShowTree(FILE *fp, node_t  *node, ssize_t *rank, ssize_t *cur_rank)
{
    if (node == NULL)
        return;
    fprintf(fp, "(");
    (*cur_rank)++;
    ShowTree(fp, node->left, rank, cur_rank);
    if (*cur_rank > *rank) *rank = *cur_rank;

    fprintf(fp, "\n        node.parent = %p\n        node.left = %p\n        node.right = %p\n        NODE.type_t = %s\n        ", node->parent, node->left, node->right, type_buffer[(int)node->type]);
    fprintf(fp, "node.value = %s\n\n", node->name);
    
    ShowTree(fp, node->right, rank, cur_rank);
    fprintf(fp, ")");
    (*cur_rank)--;
    return;
}

void SkipSpaces(char **expression)
{
    while (isspace(**expression)) 
        (*expression)++;
}

bool OpenFileSuccess(FILE *fp, const char * file_name)
{
    if (fp == NULL)
    {
        printf("An error occurred when opening the file %s", file_name);
        return false;
    }

    return true;
}

bool CloseFileSuccess(FILE *fp, const char * file_name)
{
    if (fclose(fp))
    {
        printf("An error occurred when closing the file %s", file_name);
        return false;
    }
    
    return true;
}

bool PrintError(Program_Errors  err)
{
    switch (err)
    {
        case NO_ERROR:
            return true;

        case NULL_POINTER_ON_TREE:
            printf("The pointer on the TREE is empty\n");
            return true;

        case TREE_IS_EMPTY:
            printf("The TREE is empty\n");
            return true;

        case ELEMENT_NOT_FOUND:
            printf("The item was not found in the tree\n");
            return true;

        case ERROR_DURING_THE_CREATION_OF_THE_TREE:
            printf("An error occurred when was the tree created\n");
            return true;

        case NEGATIVE_NUM_OF_ELEMENTS:
            printf("The number of elements takes a negative value\n");
            return true;

        case NULL_POINTER_ON_ROOT:
            printf("The pointer on the ROOT is empty\n");
            return true;

        case ROOT_HAVE_PARENT:
            printf("The ROOT have a parent\n");
            return true;

        case SON_HAS_WRONG_PARENT:
            printf("A son has a wrong parent\n");
            return true;

        case ERROR_DURING_OPENING_FILE:
            printf("An error occurred while opening the file\n");
            return true;

        case ERROR_DURING_CLOSING_FILE:
            printf("An error occurred while closing the file\n");
            return true;

        case ERROR_DURING_MEMORY_ALLOCATION:
            printf("Couldn't allocate dynamic memory\n");
            return true;

        case ERROR_DURING_READ_FILE:
            printf("The number of successfully read characters from the file is less than its content\n");
            return true;

        case INCORRECT_DATA_IN_FILE:
            printf("There are incorrect data in the file with tree nodes\n");
            return true;

        case COLORS_ARE_NOT_SUPPORTED:
            printf("Your terminal does not support color\n");
            return true;

        case NON_EXISTENT_VALUE:
            printf("Non existent value found in the file\n");
            return true;

        default:
            return false;
    }

    return false;
}

