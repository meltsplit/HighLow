#ifndef __GAME_H__
#define __GAME_H__

#include <unistd.h>

typedef enum result {
    WIN = 0,
    LOSE,
    DRAW,
    NONE
} result_t;

typedef enum operator {
    PLUS = 0,
    MINUS,
    MULTIPLY,
    DIVIDE
} operator_t;

typedef struct __Calc
{
    int x;
    int y;
    char op;
} Calc;

typedef struct cards
{
    pid_t pid;
    int num_card[2];
    operator_t operator_card[1];
    
} cards_t;

typedef struct answer_result
{
    cards_t card; 
    int solution;
} answer_result_t;


typedef struct score
{
    int round;
    pid_t user1_pid;
    pid_t user2_pid;
    result_t user1_result[5];
    result_t user2_result[5];

} score_t;

#endif //!__GAME_H__
