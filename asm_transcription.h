#ifndef ASM_TRANSCRIPTION
#define ASM_TRANSCRIPTION

#include "tokens.h"
#include "root_functions.h"

Program_Errors TranscriptionIntoAssembler(program_tree *tree, const char *asm_file_name);
void WriteToAsmFileRecursive(program_tree *tree, FILE *fp, node_t *node, ssize_t *mark, ssize_t *par_num, bool *inside_function_call, ssize_t *num_of_parameters, bool *inside_function_parameters, bool *inside_condititon_statement, ssize_t *cond_stat_mark, ssize_t *degree_of_nesting);
ssize_t FindFunctionIndex(program_tree *tree, const node_t *node);
ssize_t FindVariableIndex(program_tree *tree, const node_t *node);
void AssignValueToVar(program_tree *tree, FILE *fp, const node_t *node);
void PushVar(FILE *fp, ssize_t index);
void WriteMathAct(FILE *fp, node_t *node, ssize_t *mark);
void DoCommand(program_tree *tree, FILE *fp, node_t *node);

#endif