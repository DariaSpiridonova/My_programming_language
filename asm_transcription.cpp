#include "asm_transcription.h"

Program_Errors TranscriptionIntoAssembler(program_tree *tree, const char *asm_file_name)
{
    ASSERTS(tree);

    Program_Errors err = NO_ERROR;
    if ((err = CalculationTreeVerify(tree)))
    {
        return err;
    }

    FILE *asm_file = fopen(asm_file_name, "w");
    if (!OpenFileSuccess(asm_file, asm_file_name))
        return ERROR_DURING_OPENING_FILE;

    ssize_t mark = 0;
    ssize_t cond_stat_mark = 0;

    ssize_t par_num = 0;
    bool inside_function_call = false;
    bool inside_function_parameters = false;
    bool inside_condititon_statement = false;
    ssize_t num_of_parameters = 0;
    ssize_t degree_of_nesting = 0;

    WriteToAsmFileRecursive(tree, asm_file, tree->root, &mark, &par_num, &inside_function_call, &num_of_parameters, &inside_function_parameters, &inside_condititon_statement, &cond_stat_mark, &degree_of_nesting);
    fprintf(asm_file, "HLT\n");

    if (!CloseFileSuccess(asm_file, asm_file_name))
        return ERROR_DURING_CLOSING_FILE;

    return err;
}

void WriteToAsmFileRecursive(program_tree *tree, FILE *fp, node_t *node, ssize_t *mark, ssize_t *par_num, bool *inside_function_call, ssize_t *num_of_parameters, bool *inside_function_parameters, bool *inside_condititon_statement, ssize_t *cond_stat_mark, ssize_t *degree_of_nesting)
{
    if (node == NULL)
    return;

    ssize_t mark_cycle = 0;
    ssize_t mark_end_cycle = 0;
    ssize_t mark_begin_cycle = 0;
    
    if (node->type == COND_TYPE)
    {
        mark_cycle = (*mark)++;
        if (node->number != (int)ELSE) 
            fprintf(fp, ":%zd ", mark_cycle);
        fprintf(fp, "; entering the \"%s\" \n", node->name);
    }
    
    ssize_t limit_index_for_func = -1;
    
    ssize_t index_ = -1;
    if (node->type == FUNC_DEF_TYPE || node->type == FUNC_CALL_TYPE) 
    {
        index_ = FindFunctionIndex(tree, node);
    }
    
    if (node->type == FUNC_DEF_TYPE)
    {
        limit_index_for_func = (*mark)++;
        *inside_function_parameters = true;
        fprintf(fp, "JMP :%zd  \n\n", limit_index_for_func);
        fprintf(fp, ":%zd    ; definition of the \"%s\" function \n", index_ + 200, tree->functions_s.functions[index_].name);
    }

    else if (node->type == FUNC_CALL_TYPE)
    {
        *inside_function_call = true;
        *num_of_parameters = tree->functions_s.functions[index_].num_of_parameters;
    }

    if (node->type == (int)COLON_TYPE && !(node->parent != NULL && node->parent->type == COLON_TYPE))
    {
        fprintf(fp, "; $$$$$$$$$$$$$$$$$$$$$$$$$\n\n");
        *inside_condititon_statement = true;
        if (*degree_of_nesting == 0)
            *cond_stat_mark = (*mark)++;
        (*degree_of_nesting)++;
        (*mark)++;
    }

    WriteToAsmFileRecursive(tree, fp, node->left , mark, par_num, inside_function_call, num_of_parameters, inside_function_parameters, inside_condititon_statement, cond_stat_mark, degree_of_nesting);
    
    if (node->type == COND_TYPE && node->number != (int)ELSE)
    {
        fprintf(fp, "PUSH 0 \n");
        mark_begin_cycle = (*mark)++;
        fprintf(fp, "JNE :%zd  \n\n", mark_begin_cycle);
        fprintf(fp, "POPR   BP\n");
        fprintf(fp, "POPR   BP\n");
        mark_end_cycle = (*mark)++;
        fprintf(fp, "JMP :%zd  \n\n", mark_end_cycle);
        fprintf(fp, ":%zd  \n\n", mark_begin_cycle);
    }

    if (limit_index_for_func != -1) 
        *inside_function_parameters = false;
    
    WriteToAsmFileRecursive(tree, fp, node->right, mark, par_num, inside_function_call, num_of_parameters, inside_function_parameters, inside_condititon_statement, cond_stat_mark, degree_of_nesting);
    
    ssize_t index = -1;
    switch((int)node->type)
    {
        case (int)VAR_TYPE: 
        if (!(node->parent->type == ASSIGN_TYPE && node->parent->left == node) && !(node->parent->type == COMM_TYPE && node->parent->number == (int)GETNUM))
        {
            index = FindVariableIndex(tree, node);
            if (*inside_function_parameters)
            {
                AssignValueToVar(tree, fp, node);
            }
            else if (!*inside_function_call || (*inside_function_call && (*par_num < *num_of_parameters)))
            {
                PushVar(fp, index);
                if (*inside_function_call) 
                    (*par_num)++;
            }
        }
            break;

        case (int)NUM_TYPE: 
            fprintf(fp, "PUSH %d\n", (int)node->number);
            break;

        case (int)COND_TYPE:
            if (node->number == (int)WHILE)
            {
                fprintf(fp, "JMP :%zd\n", mark_cycle);
                fprintf(fp, ":%zd\n", mark_end_cycle);
            }
            else 
            {
                if (*inside_condititon_statement && node->number != (int)ELSE)
                    fprintf(fp, "JMP :%zd ; cond_stat_mark from %s\n", *cond_stat_mark + *degree_of_nesting - 1, node->name);
                
                if (node->number != (int)ELSE)
                    fprintf(fp, ":%zd\n", mark_end_cycle);
            } 
            break;

        case (int)FUNC_DEF_TYPE: 
            fprintf(fp, "RET  ; end of the definition of the \"%s\" function\n\n", tree->functions_s.functions[index_].name);
            fprintf(fp, ":%zd \n\n", limit_index_for_func);
            break;

        case (int)FUNC_CALL_TYPE: 
            fprintf(fp, "CALL :%zd    ; calling the \"%s\" function \n", index_ + 200, tree->functions_s.functions[index_].name);
            *inside_function_call = false;
            *num_of_parameters = 0;
            *par_num = 0;
            break;

        case COMMA_TYPE:
            break;

        case (int)SEM_POINT_TYPE: 
            fprintf(fp, "; ****************************************\n");
            break;

        case (int)COLON_TYPE:
            if (!(node->parent != NULL && node->parent->type == COLON_TYPE))
            {
                fprintf(fp, ":%zd ; exiting an if-else statement\n", *cond_stat_mark + *degree_of_nesting - 1);
                if (*degree_of_nesting == 1) 
                {
                    *inside_condititon_statement = false;
                    (*degree_of_nesting)--;
                }
                else if (*degree_of_nesting <= 0)
                {
                    printf(RED_COLOR_BEGIN "Error of nesting if-else statement\n" RED_COLOR_END);
                    exit(5);
                }
                else
                    (*degree_of_nesting)--;
            }
            break;

        case (int)ASSIGN_TYPE:
            AssignValueToVar(tree, fp, node->left);
            break;

        case (int)COMPARE_TYPE:
            WriteMathAct(fp, node, mark);
            break;

        case (int)SIGN_TYPE:
            fprintf(fp, "%s\n", signes_asm_buffer[node->number]);
            break;

        case (int)COMM_TYPE:
            DoCommand(tree, fp, node);
            break;

        default:
            printf("A non-existent type was found: ");
            printf(" \"%s\" ", node->name);
            break;
    }
}

ssize_t FindFunctionIndex(program_tree *tree, const node_t *node)
{
    ssize_t index = -1;
    for (ssize_t i = 0; i < tree->functions_s.functions_size; i++)
    {
        if (!strcmp(node->name, tree->functions_s.functions[i].name))
        {
            index = i;
            if (tree->functions_s.functions[i].num_of_definitions == 0)
            {
                printf("%sThe function %s%s%s was not declared in your code%s", RED_COLOR_BEGIN, BOLD, tree->functions_s.functions[i].name, BOLD, RED_COLOR_END);
            }
            break;
        }
    }

    if (index == -1)
        printf("%sThe function %s%s%s %swas not found%s%s", RED_COLOR_BEGIN, BOLD, node->name, BOLD, UNDERLINE, UNDERLINE, RED_COLOR_END);        

    return index;
}

ssize_t FindVariableIndex(program_tree *tree, const node_t *node)
{
    ssize_t index = -1;
    for (ssize_t i = 0; i < tree->variables_s.variables_size; i++)
    {
        if (!strcmp(node->name, tree->variables_s.variables[i].name))
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        printf("%sThe variable %s%s%s %swas not found%s%s", RED_COLOR_BEGIN, BOLD, node->name, BOLD, UNDERLINE, UNDERLINE, RED_COLOR_END);
    }

    return index;
}

void AssignValueToVar(program_tree *tree, FILE *fp, const node_t *node)
{
    ssize_t index = -1;

    index = FindVariableIndex(tree, node);
    fprintf(fp, "PUSH %zd \n", index);
    fprintf(fp, "POPR DI  \n");
    fprintf(fp, "POPM [DI]\n");
}

void PushVar(FILE *fp, ssize_t index)
{
    fprintf(fp, "PUSH %zd  \n", index);
    fprintf(fp, "POPR SI   \n");
    fprintf(fp, "PUSHM [SI]\n");
}

void WriteMathAct(FILE *fp, node_t *node, ssize_t *mark)
{
    fprintf(fp, "%s :%zd\n", compare_asm_symbols[node->number], (*mark)++);
    fprintf(fp, "POPR   BP\n");
    fprintf(fp, "POPR   BP\n");
    fprintf(fp, "PUSH 0\n");
    fprintf(fp, "JMP :%zd\n", *mark);
    fprintf(fp, ":%zd\n", *mark - 1);
    fprintf(fp, "PUSH 1\n");
    fprintf(fp, ":%zd\n", (*mark)++);
}

void DoCommand(program_tree *tree, FILE *fp, node_t *node)
{
    ssize_t index = -1;
    if      (node->number == (int)GETNUM)
    {
        fprintf(fp, "IN\n");
        index = FindVariableIndex(tree, node->right);
        fprintf(fp, "PUSH %zd \n", index);
        fprintf(fp, "POPR DI  \n");
        fprintf(fp, "POPM [DI]\n");
    }
    else if (node->number == (int)PRINT)
    {
        fprintf(fp, "OUT\n");
    }
}