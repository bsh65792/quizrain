#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <pthread.h>
using namespace std;
#include "quiz.h"
#include "quizmanager.h"
#include "map.h"
#include "playerManager.h"

#ifndef LINUX_KBHIT_H_
#define LINUX_KBHIT_H_
#include <stdio.h>
#include <termios.h>
//#include <unistd.h>

void cleanup(void *arg)
{
    struct termios oldt, newt;     
    tcgetattr( STDIN_FILENO, &oldt ); 
    newt = oldt;
    newt.c_lflag |= ( ICANON | ECHO ); 
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );            
}


int linux_kbhit(void)
{
    struct termios oldt, newt; 
    int ch;
    tcgetattr( STDIN_FILENO, &oldt ); 
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO ); 
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );    
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}
#endif



void getAnswer(Map* map)
{
    pthread_cleanup_push(cleanup, 0);
    string res;
    char c;    

    while(c != '\n'){        
        c = linux_kbhit();           
        if(c == '\n') break;
        res += (char)c;
        map->showAnswer(res);
    }    
    
    map->strAnswer = res;     
    pthread_cleanup_pop(0);
}

thread thread_answer;

int main(void)
{
    string name;
	// 퀴즈관리를 목적으로 설계된 컨트롤 클래스의 객체생성
	QuizManager q_manager;
    time_t new_time, old_time;

	// 퀴즈 등록
	vector<string> wordVector;
    ifstream fin("quizlist.txt");
    if(!fin){
        cout << "quizlist.txt 파일을 열수 없습니다" << endl;
        return 0;
    }
    q_manager.fileRead(fin);
    fin.close();
   
    ////게임 시작////////
    PlayerManager p_manager;
    Map map;
    

    while(true){
        system("clear");
        string ans;
        int choice;
        
        cout << "게임" << endl;
        cout << "1. 게임 시작" << endl;
        cout << "2. 랭크 확인" << endl;                
        cout << "3. 종료" << endl;

        cin >> choice;
        cin.ignore();
        switch(choice){
            case 1:{
                q_manager.randomQuiz(); // 퀴즈 셔플

                cout << "이름을 입력해주세요 :";
                getline(cin, name);
                q_manager.setTotalSocre();
                p_manager.addPlayer(make_shared<Player>(name, 0));

                for (int i = 0; i < q_manager.getNumQuiz(); i++){                    
                    system("clear");
                    map.resetForWriteAnswer();                    
                    map.showFrame('*');
                    map.showName(name);
                    map.showQuiz(q_manager.getQuiz(i));
                    map.showLevel(q_manager.getQuizLevel(i));
                    p_manager.setScoreByName(name, q_manager.getTotalScore());
                    map.showScore(q_manager.getTotalScore());
                    map.showRain(q_manager.sendGetAnswers(i), 1000000);                                        
                    thread_answer = thread(getAnswer, &map);                    
                    string quizans = map.waitAnswer();                            
                    pthread_cancel(thread_answer.native_handle());                    
                    thread_answer.join();
                                        

                    if (q_manager.callCheckAnswer(i,quizans) == true){
                        p_manager.setScoreByName(name, q_manager.getTotalScore());
                        map.showScore(q_manager.getTotalScore());
                        map.removeRain();
                        map.removeAnswer();
                        map.showAnswerResult("정답입니다.\n");                 
                        sleep(2);                  
                        continue;
                    }
                    else{
                        p_manager.setScoreByName(name, q_manager.getTotalScore());
                        map.showScore(q_manager.getTotalScore());
                        map.removeAnswer();
                        map.showAnswerResult("오답입니다.\n");
                        map.removeRain();
                        sleep(2);                        
                        break;
                    }
                    
                     
                }
                
                map.removeRain();
                map.showResultFrame();
                map.showResultInfo(p_manager);
                getchar();     
                         
                break;
            }        
            case 2:
                map.showResultFrame();
                map.showResultInfo(p_manager);
                getchar();
                break;    
            
                
            case 3:
                return 0;
                
            default :
                cout << "잘못 입력하셨습니다 다시 입력해주세요" << endl;
                sleep(3);
                break;
        }
        
    }
	return 0;
}