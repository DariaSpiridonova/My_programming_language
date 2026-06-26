#include "tokens.h"
#include "root_functions.h"
#include "nasm_generation.h"

int main()
{
    program_tree program_tree_ = {0};

    Program_Errors err = NO_ERROR;

    if ((err = MakeTreeFromProgram(&program_tree_, "logfile_for_tree.htm", "program.txt")))
    {
        PrintError(err);
        return 1;
    }

    CALCULATION_TREE_DUMP(&program_tree_);

    if ((err = SaveTreeToFile(&program_tree_, "file_program.txt")))
    {
        PrintError(err);
        return 2;
    }
    
    program_tree program_tree_1 = {0};
    
    if ((err = MakeTreeFromFile(&program_tree_1, "logfile_for_tree_from_second_file.htm", "file_program.txt")))
    {
        PrintError(err);
        return 3;
    }

    SymbolTable sym_table = {0};
    if ((err = SymbolTableInit(&sym_table)))
    {
        PrintError(err);
        return 4;
    }
    
    RunSemanticAnalysis(&sym_table, program_tree_.root);
    PrintSymbolTable(&sym_table);
    
    if ((err = GenerateNasmCode(&program_tree_, &sym_table, "program_nasm.asm")))
    {
        PrintError(err);
        return 4;
    }

    SymbolTableDestroy(&sym_table);

    CALCULATION_TREE_DUMP(&program_tree_1);

    if ((err = CalculationTreeDestroy(&program_tree_)))
    {
        PrintError(err);
        return 5;
    }

    if ((err = CalculationTreeDestroy(&program_tree_1)))
    {
        PrintError(err);
        return 6;
    }

    system("nasm -f elf64 stdlib.asm -o stdlib.o");
    system("nasm -f elf64 math_lib.asm -o math_lib.o");
    system("ar rcs libmynasm.a stdlib.o math_lib.o");
    system("nasm -f elf64 program_nasm.asm -o program_nasm.o");
    system("g++ -no-pie program_nasm.o -L. -lmynasm -o program_nasm");
    system("./program_nasm");
    
    return 0;
}