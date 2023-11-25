#include "game.h"
#include "server_msg.h"
#include "client_msg.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h> // time()함수 포함 라이브러리


typedef enum server_wait {
    SERVER_WAIT_USER = 0,
    SERVER_WAIT_ANSWER
} server_wait_t;

void signalHandler(int signum);
int calculate(Calc calc);

void playGame();
// void settingScore();
cards_t makeRandomCard(pid_t pid);
void sendRoundStart(score_t *score);
void waitUser(score_t *score);
void waitAnswer();
// void sendRoundEnd();
// void updateScore();
// void sendGameOver();

key_t mykey = 0;
int msqid = 0;

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    mykey = ftok("HIGHLOW", 1);              // 키 생성
    msqid = msgget(mykey, IPC_CREAT | 0600); // 메시지 큐 생성
    signal(SIGINT, signalHandler);

    playGame();

    return 0;
}

void playGame() {

    score_t score;
    memset(&score, 0x00, sizeof(score_t));

    waitUser(&score); // uid 반환
    score.round = 1;
    printf("첫 번째 유저 %d\n", score.user1_pid);
    printf("두 번째 유저 %d\n", score.user2_pid);
    printf("게임을 시작합니다.\n\n\n");
    

    while (score.round <= 5)  {
        sendRoundStart(&score);
        waitAnswer(); // User Answer 구조체 반환
        // judgeWinner(); // 승리자 반환
        // sendRoundEnd(); // 
        // updateScore();
    }
}

 void waitUser(score_t *score)
{
    c_msg_login_t c_msg_login_user1;
    c_msg_login_t c_msg_login_user2;

    memset(&c_msg_login_user1, 0x00, sizeof(c_msg_login_t));
    memset(&c_msg_login_user2, 0x00, sizeof(c_msg_login_t));

    puts("\n\n유저를 기다리고 있습니다.\n");
    msgrcv(msqid, &c_msg_login_user1, CLIENT_MSG_SIZE_LOGIN, CLIENT_MSG_TYPE_LOGIN, 0);
    score->user1_pid = c_msg_login_user1.pid;
    printf("첫 번째 유저가 접속했습니다. %d\n", score->user1_pid);
    printf("다른 유저를 기다리고 있습니다.\n");

    msgrcv(msqid, &c_msg_login_user2, CLIENT_MSG_SIZE_LOGIN, CLIENT_MSG_TYPE_LOGIN, 0);
    score->user2_pid = c_msg_login_user2.pid;
    printf("두 번째 유저가 접속했습니다. %d\n", score->user2_pid);

    return;
}


void sendRoundStart(score_t *score) {
    s_msg_round_start_t s_msg_round_start;

    memset(&s_msg_round_start, 0x00, sizeof(s_msg_round_start_t));

    s_msg_round_start.mtype = SERVER_MSG_TYPE_ROUND_START;
    s_msg_round_start.round = score->round;
    
    s_msg_round_start.user1_card = makeRandomCard(score->user1_pid);
    s_msg_round_start.user2_card = makeRandomCard(score->user2_pid);

    msgsnd(msqid, &s_msg_round_start, SERVER_MSG_SIZE_ROUND_START, 0);
    msgsnd(msqid, &s_msg_round_start, SERVER_MSG_SIZE_ROUND_START, 0);
}

cards_t makeRandomCard(pid_t pid) {

    cards_t card;
    memset(&card, 0x00, sizeof(cards_t));

    card.pid = pid;    

    for (int i = 0; i < 4; i++) 
        card.num_card[i] = rand() % 10;
    

     for (int i = 0; i < 3; i++) 
        card.operator_card[i] = rand() % 4;

    return card;
} 

 void waitAnswer() //TODO2: 유저의 응답을 받아서 승리를 판별하는 로직
{

    c_msg_answer_t c_msg_answer_1;
    c_msg_answer_t c_msg_answer_2;

    memset(&c_msg_answer_1, 0x00, sizeof(c_msg_answer_t));
    memset(&c_msg_answer_2, 0x00, sizeof(c_msg_answer_t));

    puts("유저 답 입력중 \n");
    msgrcv(msqid, &c_msg_answer_1, CLIENT_MSG_SIZE_ANSWER, CLIENT_MSG_TYPE_ANSWER, 0);
    puts("한 유저가 답을 제출했습니다..\n");
    msgrcv(msqid, &c_msg_answer_2, CLIENT_MSG_SIZE_ANSWER, CLIENT_MSG_TYPE_ANSWER, 0);
    puts("두 유저가 모두 답을 제출했습니다..\n");
    

    return;
}


// void calculateListner()
// {
//     MsgCalc msgCalc;
//     MsgRslt msgRslt;

//     while (1)
//     {
//         puts("calculateSituation Wait ...");

//         memset(&msgCalc, 0x00, sizeof(MsgCalc));

//         msgrcv(msqid, &msgCalc, MSG_SIZE_CALC, MSG_TYPE_CALC, 0);

//         printf("Receive: %d %c %d\n", msgCalc.calc.x, msgCalc.calc.op, msgCalc.calc.y);
//         memset(&msgRslt, 0x00, sizeof(MsgRslt));
//         msgRslt.mtype = MSG_TYPE_RSLT;
//         msgRslt.rslt = calculate(msgCalc.calc);
//         msgsnd(msqid, &msgRslt, MSG_SIZE_RSLT, 0);
//         printf("Send: %d\n", msgRslt.rslt);
//         fflush(stdout);
//     }
//     return ;
// }






void signalHandler(int signum)
{
    if (signum == SIGINT)
    {
        //msgctl(msqid, IPC_RMID, NULL);
        exit(0);
    }
}

int calculate(Calc calc)
{
    switch (calc.op)
    {
    case '+':
        return calc.x + calc.y;
    case '-':
        return calc.x - calc.y;
    case '*':
        return calc.x * calc.y;
    case '/':
        return (int)(calc.x / calc.y);
    default:
        return 0;
    }
}