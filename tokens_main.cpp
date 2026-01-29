#include "tokens.h"
#include "root_functions.h"
#include "asm_transcription.h"

int main()
{
    program_tree program_tree_ = {0};

    Program_Errors err = NO_ERROR;

    if ((err = MakeTreeFromProgram(&program_tree_, "logfile_for_tree.htm", "program.txt")))
    {
        PrintError(err);
        return 3;
    }

    CALCULATION_TREE_DUMP(&program_tree_);

    if ((err = SaveTreeToFile(&program_tree_, "file_program.txt")))
    {
        PrintError(err);
        return 4;
    }
    
    program_tree program_tree_1 = {0};
    
    if ((err = MakeTreeFromFile(&program_tree_1, "logfile_for_tree_from_second_file.htm", "file_program.txt")))
    {
        PrintError(err);
        return 4;
    }
    
    CALCULATION_TREE_DUMP(&program_tree_1);

    if ((err = TranscriptionIntoAssembler(&program_tree_1, "program_tree.asm")))
    {
        PrintError(err);
        return 5;
    }

    CALCULATION_TREE_DUMP(&program_tree_1);
    
    if ((err = CalculationTreeDestroy(&program_tree_)))
    {
        PrintError(err);
        return 8;
    }

    if ((err = CalculationTreeDestroy(&program_tree_1)))
    {
        PrintError(err);
        return 9;
    }

    system("cd ../Processor/Assembler/ && ./assembler.exe");
    system("cd ../Processor/SPU/ && ./processor.exe");
    
    return 0;
}