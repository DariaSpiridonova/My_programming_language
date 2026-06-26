#include "nasm_generation.h"
#include "root_functions.h"

Program_Errors GenerateNasmCode(program_tree *tree, SymbolTable *sym_table, const char *asm_file_name)
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

    
    fprintf(asm_file, "extern pow       \n");
    fprintf(asm_file, "extern sys_table \n");
    fprintf(asm_file, "extern sys_print \n");
    fprintf(asm_file, "extern sys_getnum\n");
    fprintf(asm_file, "extern sys_stop  \n\n");

    fprintf(asm_file, "global main\n\n");
    
    // data segment
    fprintf(asm_file, "section .data\n");
    PrintGlobalVariables(sym_table, asm_file);
    fprintf(asm_file, "\n");

    // code segment
    fprintf(asm_file, "section .text\n\n");
    GenerateNasmUserFunctions(tree, sym_table, asm_file);

    // the entry point to the main program
    fprintf(asm_file, "main:\n");

    fprintf(asm_file, "    push rbp\n");
    fprintf(asm_file, "    mov rbp, rsp\n\n");
    GenerateNasmCodeRecursive(tree->root, sym_table, asm_file, NULL);
    fprintf(asm_file, "    mov rsp, rbp\n");
    fprintf(asm_file, "    pop rbp\n");

    // automatic program termination
    fprintf(asm_file, "    call sys_stop  \n\n");
    fprintf(asm_file, "section .note.GNU-stack noalloc noexec nowrite progbits\n");

    if (!CloseFileSuccess(asm_file, asm_file_name))
        return ERROR_DURING_CLOSING_FILE;

    return err;
}

void GenerateNasmUserFunctions(program_tree *tree, SymbolTable *sym_table, FILE *asm_file)
{
    assert(tree != NULL);
    assert(sym_table != NULL);
    assert(asm_file != NULL);

    sem_func_t *current_function = NULL;
    for (size_t i = 0; i < sym_table->func_count; i++)
    {
        current_function = &sym_table->functions[i];
        DefineFunction(sym_table, asm_file, current_function);
    }

    fprintf(asm_file, "\n\n");
    return;
}

void GenerateNasmCodeRecursive(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function)
{
    static size_t chain_counter = 0;  
    static size_t branching_counter = 0;

    if (node == NULL)
        return;

    switch(node->type)
    {
        case FUNC_CALL_TYPE:
            for (size_t i = 0; i < sym_table->func_count; i++)
            {
                if (!strcmp(sym_table->functions[i].node->name, node->name))
                {
                    ExecuteFunction(node, sym_table, asm_file, &sym_table->functions[i]);
                    return;
                }
            }
            perror("Function was not find in Symbol table\n");
            exit(5);

        case SIGN_TYPE:
            if (node->number == DEG)
            {
                GenerateNasmCodeRecursive(node->left, sym_table, asm_file, current_function);
                fprintf(asm_file, "    pop r8             \n");
                fprintf(asm_file, "    cvtsi2sd xmm0, r8  \n");
                fprintf(asm_file, "    sqrtsd xmm0, xmm0  \n");
                fprintf(asm_file, "    cvttsd2si r8, xmm0 \n");
                fprintf(asm_file, "    push r8            \n");
                return;
            }
            break;

        case COMM_TYPE:
            RunCommand(node, sym_table, asm_file, current_function);
            return;

        case ASSIGN_TYPE:
            GetVariableAddress(node->left, asm_file, "rdi");
            fprintf(asm_file, "    push rdi\n");
            GenerateNasmCodeRecursive(node->right, sym_table, asm_file, current_function);
            fprintf(asm_file, "    pop  rax    \n");
            fprintf(asm_file, "    pop  rdi    \n");
            fprintf(asm_file, "    mov [rdi], rax\n");
            return;
        
        case COLON_TYPE:
            GenerateBranching(node, sym_table, asm_file, current_function, &branching_counter, &chain_counter);
            return;
           
        case COND_TYPE:
            if (node->number == IF)
            {
                GenerateBranchingIf(node, sym_table, asm_file, current_function, &chain_counter, 0, false);
            }
            else if (node->number == WHILE)
            {
                GenerateCircle(node, sym_table, asm_file, current_function, &branching_counter, &chain_counter);
            }
            else    
            {
                perror("Uncorrect branching\n");
                exit(4);
            }

        case FUNC_DEF_TYPE:
            return;
            
        default:
            break;
    }

    GenerateNasmCodeRecursive(node->left, sym_table, asm_file, current_function);
    GenerateNasmCodeRecursive(node->right, sym_table, asm_file, current_function);

    switch(node->type)
    {
        case VAR_TYPE:
            PushVariable(node, asm_file);
            break;

        case NUM_TYPE:
            fprintf(asm_file, "    push %d\n", node->number);
            break;

        case OP_TYPE:
            fprintf(asm_file, "    call sys_%s\n", operations_buffer[node->number]);
            break;

        case COMPARE_TYPE:
            SetCompareResult(asm_file, node);
            break;

        case SIGN_TYPE:
            SetExpressionResult(node, asm_file);
            break;

        case SEM_POINT_TYPE:
            fprintf(asm_file, "; ------------------------------------------------------\n");
            break;

        case COMMA_TYPE:
            perror("Unexpected comma\n");
            exit(4);
            
        default:
            break;
    }

    return;
}

void GenerateCircle(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function, size_t *branching_counter, size_t *chain_counter)
{
    size_t branch_number_counter = (*branching_counter)++;

    // calculation and verification of the cycle condition
    fprintf(asm_file, ".branch_success_%zu:\n", branch_number_counter);
    GenerateBranchingIf(node->left, sym_table, asm_file, current_function, chain_counter, branch_number_counter, true);
}

void GenerateBranching(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function, size_t *branching_counter, size_t *chain_counter)
{
    size_t branch_number_counter = (*branching_counter)++;

    if (node->left != NULL) 
    {
        if (node->left->type == COLON_TYPE) 
        {
            GenerateNasmCodeRecursive(node->left, sym_table, asm_file, current_function);
        }
        else if (node->left->type == COND_TYPE) 
        {
            if (node->left->number == IF)
            {
                fprintf(asm_file, "\n    ; ---- IF ----\n");
                GenerateBranchingIf(node->left, sym_table, asm_file, current_function, chain_counter, branch_number_counter, true);
            }
            else
            {
                perror("Uncorrect branching\n");
                exit(2);
            }
        }
    }
    
    if (node->right != NULL && node->right->type == COND_TYPE) 
    {
        node_t *right_node = node->right;

        if (right_node->number == ELIF)
        {
            fprintf(asm_file, "\n    ; ---- ELIF ----\n");
            GenerateBranchingIf(right_node, sym_table, asm_file, current_function, chain_counter, branch_number_counter, true);
        }

        else if (right_node->number == ELSE)
        {
            fprintf(asm_file, "\n    ; ---- ELSE ----\n");
            GenerateNasmCodeRecursive(right_node->right, sym_table, asm_file, current_function);
            
            // end of the chain
            fprintf(asm_file, ".branch_success_%zu:\n", branch_number_counter);
        }
        else
        {
            perror("Uncorrect branching\n");
            exit(2);
        }
    }
    else
    {
        perror("Uncorrect branching\n");
        exit(3);
    }
}

void GenerateBranchingIf(node_t *if_node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function, size_t *chain_counter, size_t branching_counter, bool branching)
{
    size_t fail_counter = (*chain_counter)++;

    // calculate condition
    GenerateNasmCodeRecursive(if_node->left, sym_table, asm_file, current_function);
    
    // get the comparison result to check for true/false compliance
    fprintf(asm_file, "    pop rax    \n");
    fprintf(asm_file, "    cmp rax, 0 \n");
    
    // if false
    fprintf(asm_file, "    je .branch_fail_%zu\n", fail_counter);

    // condition body
    GenerateNasmCodeRecursive(if_node->right, sym_table, asm_file, current_function);
    
    // if true, go to the end of the branch
    if (branching)
        fprintf(asm_file, "    jmp .branch_success_%zu\n", branching_counter);
    
    fprintf(asm_file, ".branch_fail_%zu:\n", fail_counter);
}

void DefineFunction(SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function)
{
    fprintf(asm_file, "%s:\n", current_function->node->name);
    
    // prologue
    fprintf(asm_file, "    push rbp\n");
    fprintf(asm_file, "    mov rbp, rsp\n");
    
    // pure locals count
    size_t pure_locals = current_function->local_count - current_function->num_of_parameters;
    
    // stack alignment
    size_t stack_space = ((pure_locals * 8 + 15) / 16) * 16;
    if (stack_space > 0) 
        fprintf(asm_file, "    sub rsp, %zu\n", stack_space);

    // recursive traversal of the function body 
    GenerateNasmCodeRecursive(current_function->node->right, sym_table, asm_file, current_function);

    // epilogue
    fprintf(asm_file, "    mov rsp, rbp\n");
    fprintf(asm_file, "    pop rbp\n");
    fprintf(asm_file, "    ret\n\n");
}

void ExecuteFunction(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function)
{
    assert(node != NULL);
    assert(current_function != NULL);

    PushParameters(node->left, sym_table, asm_file, current_function);
    fprintf(asm_file, "    call %s\n", node->name);

    if (current_function != NULL && current_function->num_of_parameters > 0) 
    {
        size_t bytes_to_clear = current_function->num_of_parameters * 8;
        fprintf(asm_file, "    add rsp, %zu       ; clear the stack of parameters after returning\n", bytes_to_clear);
    }
}

void PushParameters(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function)
{
    if (node == NULL) return;

    if (node->type == COMMA_TYPE) 
    {
        GenerateNasmCodeRecursive(node->right, sym_table, asm_file, current_function);
        PushParameters(node->left, sym_table, asm_file, current_function);
    }
    
    else 
    {
        GenerateNasmCodeRecursive(node, sym_table, asm_file, current_function);
    }

    return;
}

void PushVariable(node_t *node, FILE *asm_file)
{
    if (node->stack_offset != 0) 
    {
        if (node->stack_offset > 0) 
            fprintf(asm_file, "    push qword [rbp + %d] ; function argument %s\n", node->stack_offset, node->name);
        else 
            fprintf(asm_file, "    push qword [rbp %d] ; local variable %s\n", node->stack_offset, node->name);
    } 
    
    else 
    {
        fprintf(asm_file, "    push qword [%s] ; global variable\n", node->name);
    }
}

void GetVariableAddress(node_t *node, FILE *asm_file, const char *register_name)
{
    if (node->stack_offset != 0) 
    {
        if (node->stack_offset > 0) 
            fprintf(asm_file, "    lea %s, [rbp + %d] ; function argument %s\n", register_name, node->stack_offset, node->name);
        else 
            fprintf(asm_file, "    lea %s, [rbp  %d] ; local variable %s\n", register_name, node->stack_offset, node->name);
    } 
    
    else 
    {
        fprintf(asm_file, "    lea %s, [%s] ; global variable\n", register_name, node->name);
    }
}

void RunCommand(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function)
{
    assert(node != NULL);
    assert(asm_file != NULL);

    switch (node->number)
    {
        case GETNUM:
            GetVariableAddress(node->right, asm_file, "rdi");
            fprintf(asm_file, "    call sys_getnum\n");
            break;

        case PRINT:
            GenerateNasmCodeRecursive(node->right, sym_table, asm_file, current_function);
            fprintf(asm_file, "    pop rdi\n");
            fprintf(asm_file, "    call sys_print\n");
            break;

        case STOP:
            fprintf(asm_file, "    call sys_stop\n");
            break;

        default:
            perror("Unknown command\n");
            exit(-1);
    }
}

void SetCompareResult(FILE *asm_file, node_t *node)
{
    fprintf(asm_file, "    pop r8       \n");
    fprintf(asm_file, "    pop r9       \n");
    fprintf(asm_file, "    cmp r9, r8   \n");
    fprintf(asm_file, "    set%s al     \n", compare_asm_symbols[node->number] + 1);
    fprintf(asm_file, "    movzx rax, al\n");
    fprintf(asm_file, "    push rax     \n");
}

void SetExpressionResult(node_t *node, FILE *asm_file)
{
    if (node->number == DEG)
    {
        perror("Skipping the sqrt operation earlier\n");
        exit(1);
    }

    fprintf(asm_file, "    pop r8       \n");
    fprintf(asm_file, "    pop rax       \n");
    if (node->number != DIV)
    {
        fprintf(asm_file, "    %s rax, r8   \n", node->number == ADD ? "add" : (node->number == SUB ? "sub" : "imul"));
    }
    else
    {
        fprintf(asm_file, "    cqo          \n");
        fprintf(asm_file, "    idiv r8      \n");
    }
    fprintf(asm_file, "    push rax      \n");
}

void PrintGlobalVariables(SymbolTable *sym_table, FILE *asm_file)
{
    assert(sym_table != NULL && asm_file != NULL);

    for (size_t i = 0; i < sym_table->globals_count; i++) 
        fprintf(asm_file, "    %s dq 0\n", sym_table->globals[i].name);

    return;
}