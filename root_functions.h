#ifndef ROOT_FUNCTIONS
#define ROOT_FUNCTIONS

#include "tokens.h"

Program_Errors CalculationTreeVerify(program_tree *tree);
bool CheckSonsAndParents(node_t *node, node_t **son, node_t **parent);
void CheckSonsAndParentsRecursive(node_t *node, bool *flag, node_t **son, node_t **parent);

Program_Errors CalculationTreeDestroy(program_tree *tree);
Program_Errors CalculationTreeDestroyRecursive(program_tree *tree, node_t **node);

void CalculationTreeDump(program_tree *tree, const char *file, int line);
void DumpToConsole(const program_tree *tree, const char *file, int line, ssize_t *rank);
void DumpToLogfile(const program_tree *tree, const char *logfile_name, const char *gvfile_name, ssize_t *rank);
void Dump(FILE *fp, const program_tree *tree, ssize_t *rank);
void ShowTree(FILE *fp, node_t *node, ssize_t *rank, ssize_t *cur_rank);
void PrintEdges(FILE *fp, node_t *node);
const char *ChooseColor(node_t  *node);
void LinkEdges(FILE *fp, node_t *node);
void CreateGraph(const program_tree *tree, const char *gvfile_name);

void SkipSpaces(char **expression);

bool OpenFileSuccess(FILE *fp, const char * file_name);
bool CloseFileSuccess(FILE *fp, const char * file_name);

bool PrintError(Program_Errors err);

#endif