#ifndef __SERVERMSG_H__
#define __SERVERMSG_H__

#include "game.h"
#include <unistd.h>

#define SERVER_MSG_TYPE_ROUND_START 11
#define SERVER_MSG_TYPE_ROUND_END 12
#define SERVER_MSG_TYPE_GAME_OVER 13

#define SERVER_MSG_SIZE_ROUND_START (sizeof(s_msg_round_start_t) - sizeof(long))
#define SERVER_MSG_SIZE_ROUND_END (sizeof(s_msg_round_end_t) - sizeof(long))
#define SERVER_MSG_SIZE_GAME_OVER (sizeof(s_msg_game_over_t) - sizeof(long))

typedef struct s_msg_round_start
{
    long mtype;
    int round;
    cards_t user1_card;
    cards_t user2_card;
} s_msg_round_start_t;

typedef struct s_msg_round_end
{
    long mtype;
    int round;
    answer_result_t user1_answer_result;
    answer_result_t user2_answer_result;
    result_t user1_result;
    result_t user2_result;
    int next_round;       
} s_msg_round_end_t;

typedef struct s_msg_game_over
{
    long mtype;
    score_t score;
} s_msg_game_over_t;

#endif //!__SERVERMSG_H__
