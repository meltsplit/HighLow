#ifndef __CLIENTMSG_H__
#define __CLIENTMSG_H__

#include "game.h"
#include <unistd.h>

#define CLIENT_MSG_TYPE_LOGIN 1
#define CLIENT_MSG_TYPE_ANSWER 2

#define CLIENT_MSG_SIZE_LOGIN (sizeof(c_msg_login_t) - sizeof(long))
#define CLIENT_MSG_SIZE_ANSWER (sizeof(c_msg_answer_t) - sizeof(long))

typedef struct c_msg_login
{
    long mtype;
    pid_t pid;
} c_msg_login_t;

typedef struct c_msg_answer
{
    long mtype;
    pid_t pid;
    int num_card[2];
    operator_t operator_card[1];
} c_msg_answer_t;

#endif //!__SERVERMSG_H__
