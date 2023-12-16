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
int calculate(c_msg_answer_t answer);

void playGame();
// void settingScore();
void waitUser(score_t *score);
cards_t makeRandomCard(pid_t pid);
void sendRoundStart(score_t *score);
void waitAnswer(answer_result_t* answers);
pid_t judgeWinner(answer_result_t* answers);
void sendRoundEnd(score_t* score ,pid_t winner_pid, answer_result_t* answers);
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
        answer_result_t answers[2];
        pid_t winner_pid;

        sendRoundStart(&score);
        waitAnswer(answers); // User Answer 구조체 반환
        winner_pid = judgeWinner(answers); // 승리자 반환
        sendRoundEnd(&score, winner_pid, answers); // 
        // updateScore();
    }


}

 void waitUser(score_t *score)
{
    c_msg_login_t c_msg_login_user1;
    c_msg_login_t c_msg_login_user2;

    memset(&c_msg_login_user1, 0x00, sizeof(c_msg_login_t));
    memset(&c_msg_login_user2, 0x00, sizeof(c_msg_login_t));

    printf("\n\n유저를 기다리고 있습니다.\n");
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
    printf("\n[%d 라운드를 시작합니다] \n카드를 배부합니다.\n", score->round);
    s_msg_round_start_t s_msg_round_start;

    memset(&s_msg_round_start, 0x00, sizeof(s_msg_round_start_t));

    s_msg_round_start.mtype = SERVER_MSG_TYPE_ROUND_START;
    s_msg_round_start.round = score->round;
    
    s_msg_round_start.user1_card = makeRandomCard(score->user1_pid);
    s_msg_round_start.user2_card = makeRandomCard(score->user2_pid);

    msgsnd(msqid, &s_msg_round_start, SERVER_MSG_SIZE_ROUND_START, 0);
    msgsnd(msqid, &s_msg_round_start, SERVER_MSG_SIZE_ROUND_START, 0);
}

 void waitAnswer(answer_result_t* answers) //TODO2: 유저의 응답을 받아서 승리를 판별하는 로직
{
    c_msg_answer_t c_msg_answer_1;
    c_msg_answer_t c_msg_answer_2;

    memset(&c_msg_answer_1, 0x00, sizeof(c_msg_answer_t));
    memset(&c_msg_answer_2, 0x00, sizeof(c_msg_answer_t));

    printf("유저 답 입력중 \n");
    msgrcv(msqid, &c_msg_answer_1, CLIENT_MSG_SIZE_ANSWER, CLIENT_MSG_TYPE_ANSWER, 0);
    printf("한 유저가 답을 제출했습니다.\n");
    msgrcv(msqid, &c_msg_answer_2, CLIENT_MSG_SIZE_ANSWER, CLIENT_MSG_TYPE_ANSWER, 0);
    printf("두 유저가 모두 답을 제출했습니다.\n\n");
    answer_result_t user1_result;
    answer_result_t user2_result;

    memset(&user1_result, 0x00, sizeof(answer_result_t));
    memset(&user2_result, 0x00, sizeof(answer_result_t));

    user1_result.solution = calculate(c_msg_answer_1);
    user2_result.solution = calculate(c_msg_answer_2);
    
    user1_result.card.pid = c_msg_answer_1.pid;
    user2_result.card.pid = c_msg_answer_2.pid;

    for (int  i = 0; i < 2; i ++)
        user1_result.card.num_card[i] = c_msg_answer_1.num_card[i];

    for (int  i = 0; i < 2; i ++)
        user2_result.card.num_card[i] = c_msg_answer_2.num_card[i];

    for (int  i = 0; i < 1; i ++)
        user1_result.card.operator_card[i] = c_msg_answer_1.operator_card[i];
    for (int  i = 0; i < 1; i ++)
        user2_result.card.operator_card[i] = c_msg_answer_2.operator_card[i];

    answers[0] = user1_result; 
    answers[1] = user2_result;
    return;
}

pid_t judgeWinner(answer_result_t* answers) {

    printf("%d의 답 %d \n", answers[0].card.pid, answers[0].solution );
    printf("%d의 답 %d \n", answers[1].card.pid, answers[1].solution );

    if (answers[0].solution == answers[1].solution)
    { 
        
        return -1;
    }
    else if (answers[0].solution > answers[1].solution)
    {    
        
        return answers[0].card.pid; 
    }
    else 
    {
        
        return answers[1].card.pid;
    }
    
}

void sendRoundEnd(score_t* score ,pid_t winner_pid, answer_result_t* answers) {
    printf("%d라운드가 종료되었습니다.", score->round);
    s_msg_round_end_t s_msg_round_end;

    memset(&s_msg_round_end, 0x00, sizeof(s_msg_round_end_t));

    s_msg_round_end.mtype = SERVER_MSG_SIZE_ROUND_END;
    s_msg_round_end.round = score->round;
    score->round++;
    s_msg_round_end.next_round = score->round;
    

    s_msg_round_end.user1_answer_result = answers[0];
    s_msg_round_end.user2_answer_result = answers[1];
    
    if (winner_pid == -1) {
        printf("무승부 입니다.\n\n");
        s_msg_round_end.user1_result = DRAW;
        s_msg_round_end.user2_result = DRAW;
    } else if ( winner_pid == answers[0].card.pid) {
        printf("%d 가 승리하였습니다.\n\n", answers[0].card.pid);
        s_msg_round_end.user1_result = WIN;
        s_msg_round_end.user2_result = LOSE;
    } else {
        printf("%d 가 승리하였습니다.\n\n", answers[1].card.pid);
        s_msg_round_end.user1_result = LOSE;
        s_msg_round_end.user2_result = WIN;
    }

    msgsnd(msqid, &s_msg_round_end, SERVER_MSG_TYPE_ROUND_END, 0);
    msgsnd(msqid, &s_msg_round_end, SERVER_MSG_TYPE_ROUND_END, 0);
}

cards_t makeRandomCard(pid_t pid) {

    cards_t card;
    memset(&card, 0x00, sizeof(cards_t));

    card.pid = pid;    

    for (int i = 0; i < 2; i++) 
        card.num_card[i] = rand() % 10;
    

    for (int i = 0; i < 1; i++) 
        card.operator_card[i] = rand() % 4;

    return card;
}


void signalHandler(int signum)
{
    if (signum == SIGINT)
    {
        msgctl(msqid, IPC_RMID, NULL);
        exit(0);
    }
}

int calculate(c_msg_answer_t answer)
{
    switch (answer.operator_card[0])
    {
    case PLUS:
        return answer.num_card[0] + answer.num_card[1];
    case MINUS:
        return answer.num_card[0] - answer.num_card[1];
    case MULTIPLY:
        return answer.num_card[0] * answer.num_card[1];
    case DIVIDE:
        return (int)(answer.num_card[0] / answer.num_card[1]);
    default:
        return 0;
    }
}

