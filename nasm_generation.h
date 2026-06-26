#ifndef BACKEND_H
#define BACKEND_H

#include "tokens.h"

Program_Errors GenerateNasmCode(program_tree *tree, SymbolTable *sym_table, const char *asm_file_name);
void GenerateNasmUserFunctions(program_tree *tree, SymbolTable *sym_table, FILE *asm_file);
void GenerateNasmCodeRecursive(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function);

void PrintGlobalVariables(SymbolTable *sym_table, FILE *asm_file);
void PushVariable(node_t *node, FILE *asm_file);
void GetVariableAddress(node_t *node, FILE *asm_file, const char *register_name);

void DefineFunction(SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function);
void ExecuteFunction(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function);
void PushParameters(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function);

void RunCommand(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function);
void SetExpressionResult(node_t *node, FILE *asm_file);
void SetCompareResult(FILE *asm_file, node_t *node);

void GenerateBranching(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function, size_t *branching_counter, size_t *chain_counter);
void GenerateBranchingIf(node_t *if_node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function, size_t *chain_counter, size_t branching_counter, bool branching);
void GenerateCircle(node_t *node, SymbolTable *sym_table, FILE *asm_file, sem_func_t *current_function, size_t *branching_counter, size_t *chain_counter);

#endif 