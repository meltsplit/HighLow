#ifndef __GAME_H__
#define __GAME_H__

#include <unistd.h>

typedef enum result {
    WIN = 0,
    LOSE,
    DRAW
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

    int num_card[4];
    operator_t operator_card[3];
    
} cards_t;

typedef struct answer
{
    pid_t pid;
    int round;
    char answer[8]; 
} asnwer_t;

typedef struct answer_result
{
    cards_t card; 
    char answer[8];
    int solution;
} answer_result_t;

typedef struct round_end
{
    int round;
    answer_result_t your_result;
    answer_result_t component_result;
    result_t round_result;
    int next_round;
} round_end_t;

typedef struct game_over
{
    result_t game_result;

} game_over_t;

typedef struct score
{
    int round;
    pid_t user1_pid;
    pid_t user2_pid;
    result_t user1_result[5];
    result_t user2_result[5];

} score_t;

#endif //!__GAME_H__
