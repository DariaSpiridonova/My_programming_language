#include "tokens.h"

static node_t *GetE(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetT(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetP(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetN(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetF(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetV(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetOp(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetCond(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetCondContent(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num, token_t condition);
static node_t *GetCondBody(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num, node_t *val1, token_t condition);
static node_t *GetFuncDef(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *PerfomFunc(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetCommand(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetA(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetD(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);
static node_t *GetDeg(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num);

node_t *GetG(program_tree *tree, tokens_t tokens, Program_Errors *err)
{
    ssize_t token_num = 0;
    node_t *val = GetOp(tree, tokens, err, &token_num);
    if (tokens.tokens_buffer[token_num].type != SEM_POINT_TYPE)
    {
        printf("ABSENCE_SEMICOLON_0\n");
        *err = ABSENCE_SEMICOLON;
        //exit(__LINE__);
    }
    printf("G\n");

    return val;
}

static node_t *GetOp(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = NULL;
    node_t *val2 = NULL;
    token_t sem;

    if (tokens.tokens_buffer[*token_num].type == COND_TYPE)
    {
        if (tokens.tokens_buffer[*token_num].number == (int)IF)
            val = GetCond(tree, tokens, err, token_num);
        else
        {
            printf(RED_COLOR_BEGIN "You can`t start if-else statement with the word \"%s\". You should use" BOLD "if.\n" RESET RED_COLOR_END, tokens.tokens_buffer[*token_num].name);
            exit(4);
        }
    }

    else if (tokens.tokens_buffer[*token_num].type == FUNC_DEF_TYPE)
    {
        val = GetFuncDef(tree, tokens, err, token_num);
    }

    else if (tokens.tokens_buffer[*token_num].type == FUNC_CALL_TYPE)
    {
        val = PerfomFunc(tree, tokens, err, token_num);
    }

    else if (tokens.tokens_buffer[*token_num].type == COMM_TYPE)
    {
        val = GetCommand(tree, tokens, err, token_num);
    }

    else 
    {
        val = GetA(tree, tokens, err, token_num);
    }

    while (tokens.tokens_buffer[*token_num].type == SEM_POINT_TYPE)
    {
        if (*token_num >= tokens.tokens_size - 1 || (*token_num < tokens.tokens_size - 1 && tokens.tokens_buffer[*token_num + 1].type == PAR_TYPE))
        {
            return val;
        }

        sem = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        if (tokens.tokens_buffer[*token_num].type == COND_TYPE)
        {
            val2 = GetCond(tree, tokens, err, token_num);
        }

        else if (tokens.tokens_buffer[*token_num].type == FUNC_DEF_TYPE)
        {
            val2 = GetFuncDef(tree, tokens, err, token_num);
        }

        else if (tokens.tokens_buffer[*token_num].type == FUNC_CALL_TYPE)
        {
            val2 = PerfomFunc(tree, tokens, err, token_num);
        }

        else if (tokens.tokens_buffer[*token_num].type == COMM_TYPE)
        {
            val2 = GetCommand(tree, tokens, err, token_num);
        }

        else 
        {
            val2 = GetA(tree, tokens, err, token_num);
        }

        val = InitNewNode(tree, sem, val, val2, err);
    }

    return val;
}

static node_t *PerfomFunc(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *var = NULL;
    node_t *var1 = NULL;
    token_t comma;

    token_t func = tokens.tokens_buffer[*token_num];
    (*token_num)++;
    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == ROUND_PAR_OPEN)
    {
        (*token_num)++;
        if (tokens.tokens_buffer[*token_num].type != PAR_TYPE)
        {
            var = GetV(tree, tokens, err, token_num);
            while (tokens.tokens_buffer[*token_num].type == COMMA_TYPE)
            {
                comma = tokens.tokens_buffer[*token_num];
                (*token_num)++;
                var1 = GetV(tree, tokens, err, token_num);
                var = InitNewNode(tree, comma, var, var1, err);
            }
        }
        if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == ROUND_PAR_CLOSE)
        {
            (*token_num)++;
            var = InitNewNode(tree, func, var, NULL, err);
        }
        else 
        {
            printf("ABSENCE_PAR_CLOSE\n");
            *err = ABSENCE_PAR_CLOSE;
        }
    }
    else 
    {
        printf("ABSENCE_PAR_OPEN\n");
        *err = ABSENCE_PAR_OPEN;
    }

    return var;
}

static node_t *GetFuncDef(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *param = NULL;
    node_t *param1 = NULL;
    token_t comma;

    token_t func = tokens.tokens_buffer[*token_num];
    (*token_num)++;
    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == ROUND_PAR_OPEN)
    {
        (*token_num)++;
        if (tokens.tokens_buffer[*token_num].type != PAR_TYPE)
        {
            param = GetV(tree, tokens, err, token_num);
            while (tokens.tokens_buffer[*token_num].type == COMMA_TYPE)
            {
                comma = tokens.tokens_buffer[*token_num];
                (*token_num)++;
                param1 = GetV(tree, tokens, err, token_num);
                param = InitNewNode(tree, comma, param, param1, err);
            }
        }
        if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == ROUND_PAR_CLOSE)
        {
            (*token_num)++;
            if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == FIGURE_PAR_OPEN)
            {
                (*token_num)++;
                param1 = GetOp(tree, tokens, err, token_num);

                if (tokens.tokens_buffer[*token_num].type != SEM_POINT_TYPE)
                {
                    printf("ABSENCE_SEMICOLON\n");
                    *err = ABSENCE_SEMICOLON;
                }
                else 
                {
                    (*token_num)++;
                }

                if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == FIGURE_PAR_CLOSE)
                {
                    (*token_num)++;
                    param = InitNewNode(tree, func, param, param1, err);
                }
                else
                {
                    printf("ABSENCE_PAR_CLOSE 94\n");
                    *err = ABSENCE_PAR_CLOSE;
                }
            }
            else 
            {
                printf("ABSENCE_PAR_OPEN\n");
                *err = ABSENCE_PAR_OPEN;
            }
        }
        else 
        {
            printf("ABSENCE_PAR_CLOSE\n");
            *err = ABSENCE_PAR_CLOSE;
        }
    }
    else 
    {
        printf("ABSENCE_PAR_OPEN\n");
        *err = ABSENCE_PAR_OPEN;
    }

    return param;
}

static node_t *GetCommand(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = NULL;

    token_t command = tokens.tokens_buffer[*token_num];
    (*token_num)++;

    switch (command.number)
    {
        case PRINT:
            val = GetD(tree, tokens, err, token_num);
            break;

        case GETNUM:
            val = GetV(tree, tokens, err, token_num);
            break;

        case STOP:
            break;

        default:
            perror("incorrect command\n");
            exit(-1);
    }

    val = InitNewNode(tree, command, NULL, val, err);

    return val;
}

static node_t *GetCond(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val1 = NULL;
    node_t *val = NULL;

    token_t condition = tokens.tokens_buffer[*token_num];
    (*token_num)++;
    val = GetCondContent(tree, tokens, err, token_num, condition);

    token_t colon;
    ssize_t num_of_else = 0;

    while (tokens.tokens_buffer[*token_num].type == COLON_TYPE)
    {
        colon = tokens.tokens_buffer[*token_num];
        (*token_num)++;

        if (tokens.tokens_buffer[*token_num].type != COND_TYPE)
        {
            printf("After \":\" in if-else statement you can`t use \"%s\". You should use" BOLD "elif" RESET "or" BOLD "else\n" RESET, tokens.tokens_buffer[*token_num].name);
            exit(-5);
        }

        if (tokens.tokens_buffer[*token_num].number == (int)IF)
        {
            printf(RED_COLOR_BEGIN "After \":\" you should use \"elif\" or \"else\"\n" RED_COLOR_END);
            exit(-4);
        }

        if (tokens.tokens_buffer[*token_num].number == (int)ELSE && num_of_else == 1)
        {
            printf("After \":\" in if-else statement you can use \"else\" not more than once\n");
            exit(-5);
        }

        condition = tokens.tokens_buffer[*token_num];
        (*token_num)++;

        if (condition.number == (int)ELIF)
        {
            val1 = GetCondContent(tree, tokens, err, token_num, condition);
        }

        else if (condition.number == (int)ELSE)
        {
            num_of_else++;
            val1 = GetCondBody(tree, tokens, err, token_num, NULL, condition);
        }
        val = InitNewNode(tree, colon, val, val1, err);
    }
    
    return val;
}

static node_t *GetCondContent(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num, token_t condition)
{
    node_t *val = NULL;
    node_t *val1 = NULL;

    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == (int)ROUND_PAR_OPEN)
    {
        (*token_num)++;
        val1 = GetD(tree, tokens, err, token_num);
        if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == (int)ROUND_PAR_CLOSE)
        {
            (*token_num)++;
            val = GetCondBody(tree, tokens, err, token_num, val1, condition);
        }
        else 
        {
            printf("ABSENCE_PAR_CLOSE 106\n");
            *err = ABSENCE_PAR_CLOSE;
        }
    }
    else 
    {
        printf("ABSENCE_PAR_OPEN\n");
        *err = ABSENCE_PAR_OPEN;
    }

    return val;
}

static node_t *GetCondBody(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num, node_t *val1, token_t condition)
{
    node_t *val  = NULL;
    node_t *val2 = NULL;

    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == (int)FIGURE_PAR_OPEN)
    {
        (*token_num)++;
        val2 = GetOp(tree, tokens, err, token_num);

        if (tokens.tokens_buffer[*token_num].type != SEM_POINT_TYPE)
        {
            printf("ABSENCE_SEMICOLON\n");
            *err = ABSENCE_SEMICOLON;
        }
        else 
        {
            (*token_num)++;
        }

        if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == (int)FIGURE_PAR_CLOSE)
        {
            (*token_num)++;
            val = InitNewNode(tree, condition, val1, val2, err);
        }
        else
        {
            printf("ABSENCE_PAR_CLOSE 94\n");
            *err = ABSENCE_PAR_CLOSE;
        }
    }
    else 
    {
        printf("ABSENCE_PAR_OPEN\n");
        *err = ABSENCE_PAR_OPEN;
    }

    return val;
}

static node_t *GetA(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    printf("GetA\n\n\n");

    node_t *val2 = NULL;
    node_t *val = NULL;

    val = GetV(tree, tokens, err, token_num);
    if (tokens.tokens_buffer[*token_num].type == ASSIGN_TYPE)
    {
        token_t assign = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        val2 = GetD(tree, tokens, err, token_num);
        val = InitNewNode(tree, assign, val, val2, err);
    }

    else 
    {
        printf("ABSENCE_ASSIGN\n");
        *err = ABSENCE_ASSIGN;
    }

    return val;
}

static node_t *GetD(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t * val2 = NULL;
    node_t * val = NULL;

    val = GetE(tree, tokens, err, token_num);
    if (tokens.tokens_buffer[*token_num].type == COMPARE_TYPE)
    {
        token_t symbol = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        val2 = GetE(tree, tokens, err, token_num);
        val = InitNewNode(tree, symbol, val, val2, err);
    }

    return val;
}

static node_t *GetE(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = GetT(tree, tokens, err, token_num);
    while (tokens.tokens_buffer[*token_num].type == SIGN_TYPE && (tokens.tokens_buffer[*token_num].number == (int)ADD || tokens.tokens_buffer[*token_num].number == (int)SUB))
    {
        token_t sign = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        node_t *val2 = GetT(tree, tokens, err, token_num);
        val = InitNewNode(tree, sign, val, val2, err);
    }

    return val;
}

static node_t *GetT(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = GetDeg(tree, tokens, err, token_num);
    while (tokens.tokens_buffer[*token_num].type == SIGN_TYPE && (tokens.tokens_buffer[*token_num].number == (int)MUL || tokens.tokens_buffer[*token_num].number == (int)DIV))
    {
        token_t sign = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        node_t *val2 = GetDeg(tree, tokens, err, token_num);
        val = InitNewNode(tree, sign, val, val2, err);
    }

    return val;
}

static node_t *GetDeg(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = GetP(tree, tokens, err, token_num);
    while (tokens.tokens_buffer[*token_num].type == SIGN_TYPE && tokens.tokens_buffer[*token_num].number == (int)DEG)
    {
        token_t sign = tokens.tokens_buffer[*token_num];
        (*token_num)++;
        node_t *val2 = GetP(tree, tokens, err, token_num);
        val = InitNewNode(tree, sign, val, val2, err);
    }

    return val;
}

static node_t *GetP(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == (int)ROUND_PAR_OPEN)
    {
        (*token_num)++;
        node_t *val = GetE(tree, tokens, err, token_num);
        if (tokens.tokens_buffer[*token_num].number != ROUND_PAR_CLOSE) 
        {
            printf("ABSENCE_PAR_CLOSE\n");
            *err = ABSENCE_PAR_CLOSE;
        }
        else 
            (*token_num)++;

        return val;
    }

    if (tokens.tokens_buffer[*token_num].type == NUM_TYPE)
    { 
        return GetN(tree, tokens, err, token_num);
    }

    else if (tokens.tokens_buffer[*token_num].type == OP_TYPE)
    {
        return GetF(tree, tokens, err, token_num);
    }

    else 
        return GetV(tree, tokens, err, token_num);
}

static node_t *GetN(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    token_t val = tokens.tokens_buffer[*token_num];
    (*token_num)++;

    return InitNewNode(tree, val, NULL, NULL, err);
}

static node_t *GetF(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    node_t *val = NULL;
    token_t func = tokens.tokens_buffer[*token_num];
    (*token_num)++;

    if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == ROUND_PAR_OPEN)
    {
        (*token_num)++;
        val = GetE(tree, tokens, err, token_num);
        if (tokens.tokens_buffer[*token_num].type == PAR_TYPE && tokens.tokens_buffer[*token_num].number == ROUND_PAR_CLOSE)
        {
            (*token_num)++;
        }
        else 
        {
            printf("ABSENCE_PAR_CLOSE\n");
            *err = ABSENCE_PAR_CLOSE;
        }
    }
    else 
    {
        printf("ABSENCE_PAR_OPEN\n");
        *err = ABSENCE_PAR_OPEN;
    }

    if (val != NULL)
        return InitNewNode(tree, func, NULL, val, err);

    else    
    {
        printf("The %s function has an empty argument.\n", tokens.tokens_buffer[*token_num].name);
        *err = EMPTY_FUNC_ARG;
    }

    return val;
}

static node_t *GetV(program_tree *tree, tokens_t tokens, Program_Errors *err, ssize_t *token_num)
{
    token_t var = tokens.tokens_buffer[*token_num];
    (*token_num)++;

    return InitNewNode(tree, var, NULL, NULL, err);
}

node_t *InitNewNode(program_tree *tree, token_t token, node_t *node_left, node_t *node_right, Program_Errors *err)
{
    node_t *node = (node_t *)calloc(1, sizeof(node_t));

    if (node == NULL)
    {
        printf("ERROR_DURING_MEMORY_ALLOCATION in InitNewNode\n");
        *err = ERROR_DURING_MEMORY_ALLOCATION;
        return NULL;
    }

    tree->num_of_el++;

    node->left = node_left;
    node->right = node_right;
    node->number = token.number;
    node->type = token.type;
    if (node->type != NUM_TYPE && node->type != VAR_TYPE && node->type != FUNC_DEF_TYPE && node->type != FUNC_CALL_TYPE)
        node->name = (char *)array_of_structures_info[(int)node->type].buffer[node->number];
    else    
        node->name = strdup(token.name);

    if (node_left != NULL)
        node->left->parent = node;

    if (node_right != NULL)
        node->right->parent = node;

    return node;
}