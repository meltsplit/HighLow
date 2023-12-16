#include "game.h"
#include "client_msg.h"
#include "server_msg.h"

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

void sendLogin();
void playGame();
void waitRoundStart();
void sendAnswer();
void waitRoundEnd();
void waitGameOver();

char convert(operator_t op);
operator_t convertToOperator(char op);

key_t g_mykey = 0;
int g_msqid = 0;

int main(int argc, char const *argv[])
{
    g_mykey = ftok("HIGHLOW", 1);
    g_msqid = msgget(g_mykey, IPC_CREAT);

    sendLogin();
    playGame();
    return 0;
}

void playGame()
{

    while (1)
    {
        waitRoundStart();
        sendAnswer();
        waitRoundEnd();
    }
    return;
}

void sendLogin()
{
    c_msg_login_t c_msg_login;
    memset(&c_msg_login, 0x00, sizeof(c_msg_login_t));
    c_msg_login.mtype = CLIENT_MSG_TYPE_LOGIN;
    c_msg_login.pid = getpid();
    msgsnd(g_msqid, &c_msg_login, CLIENT_MSG_SIZE_LOGIN, 0);
}

void waitRoundStart()
{
    s_msg_round_start_t s_msg_round_start;
    memset(&s_msg_round_start, 0x00, sizeof(s_msg_round_start_t));
    msgrcv(g_msqid, &s_msg_round_start, SERVER_MSG_SIZE_ROUND_START, SERVER_MSG_TYPE_ROUND_START, 0);

    int round = s_msg_round_start.round;
    cards_t card = s_msg_round_start.user1_card.pid == getpid() ? s_msg_round_start.user1_card : s_msg_round_start.user2_card;

    printf("\n\n[ROUND %d]\n게임을 시작합니다.\n", round);
    printf("CARD PID: %d\n", card.pid);

    for (int i = 0; i < 2; i++)
        printf("CARD %d번째 숫자 카드: %d\n", i+1, card.num_card[i]);

    for (int i = 0; i < 1; i++)
        printf("CARD %d번째 연산자 카드: %c\n", i+1, convert(card.operator_card[i]));
}

void sendAnswer()
{ // TODO1: 유저 값선택 로직 구현
    int x;
    int y;
    char op;
    scanf("%d %c %d", &x, &op, &y);

    c_msg_answer_t c_msg_answer;
    memset(&c_msg_answer, 0x00, sizeof(c_msg_answer_t));
    c_msg_answer.mtype = CLIENT_MSG_TYPE_ANSWER;
    c_msg_answer.pid = getpid();

    c_msg_answer.num_card[0] = x;
    c_msg_answer.num_card[1] = y;
    c_msg_answer.operator_card[0] = convertToOperator(op); 
    printf("%d, %d , %c 입력했습니다.", c_msg_answer.num_card[0], c_msg_answer.num_card[1], c_msg_answer.operator_card[0]);
    msgsnd(g_msqid, &c_msg_answer, CLIENT_MSG_SIZE_ANSWER, 0);
}

void waitRoundEnd()
{
    return;
}

void waitGameOver()
{
    return;
}

char convert(operator_t op)
{
    switch (op)
    {
    case PLUS:
        return '+';
    case MINUS:
        return '-';
    case MULTIPLY:
        return '*';
    case DIVIDE:
        return '/';
    default:
        printf("에러 발생");
        return '\0';
    }
}

operator_t convertToOperator(char op)
{
    switch (op)
    {
    case '+':
        printf("asdfasdf");
        return PLUS;
    case '-':
        return MINUS;
    case '*':
        return MULTIPLY;
    case '/':
        return DIVIDE;
    default:
        printf("WTFFF");
        return '\0';
    }
}



// while (1)
// {
//     printf("<<< ");
//     scanf("%d %c %d", &x, &op, &y);
//     fflush(stdout);
//     fflush(stdin);

//     memset(&msgCalc, 0x00, sizeof(MsgCalc));

//     msgCalc.mtype = MSG_TYPE_CALC;
//     msgCalc.calc.x = x;
//     msgCalc.calc.y = y;
//     msgCalc.calc.op = op;

//     msgsnd(msqid, &msgCalc, MSG_SIZE_CALC, 0);
//     memset(&msgRslt, 0x00, sizeof(MsgRslt));
//     msgrcv(msqid, &msgRslt, MSG_SIZE_RSLT, MSG_TYPE_RSLT, 0);
//     printf(">>> %d\n", msgRslt.rslt);
// }