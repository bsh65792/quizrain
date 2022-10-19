#include "shortanswer.h"

ShortAnswer::ShortAnswer(string question, int level, int score, string answer) : Quiz(question, level, score){
    this->answer = answer;
}

string ShortAnswer::choiceAnswer(string ans){ return ans; }

string ShortAnswer::getAnswer(){ return answer ;}

void ShortAnswer::showAnswer(){
    cout << "정답은 " << getAnswer() << " 입니다." << endl;
}