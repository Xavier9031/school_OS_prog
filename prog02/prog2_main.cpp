#include <iostream> //io
#include <time.h>  //計算時間
#include <pthread.h>//thread
#include <string> //字串處理
#include <vector> //主要儲存容器
#include <cmath> //字串轉數字用
#include <algorithm>//反轉vector用

//開檔及取tid用
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include "sys/syscall.h"

using namespace std;

#define gettid() syscall(SYS_gettid)

unsigned long long int str_to_int(string _in){         

	unsigned long long temp = 0;

	unsigned long long length = _in.length();

	for(int i = length; i > 0; i--){

		temp += ((unsigned long long)(_in[i-1])-48)*pow(10,length-i);

	}

	return temp; 

}
struct Token{
	string row;
	string proporty;
};

typedef struct my_data {
	vector <Token> expr;
	unsigned long long int result;
	long double used_time;
	int tid;
} my_data;

bool t_isOperator(Token c)
{	
	if(c.proporty ==  "SUBTRACT_TOKEN" || c.proporty ==  "ADD_TOKEN" || c.proporty ==  "MUTIPLY_TOKEN" || c.proporty ==  "DEVIDE_TOKEN")return true;
	else return false;
}
bool t_isOperand(Token c)
{
    if(c.proporty ==  "NUMBER_TOKEN")return true;
	else return false;
}
 
int t_getPriority(Token C)
{
    if (C.proporty == "SUBTRACT_TOKEN" || C.proporty == "ADD_TOKEN")
		return 1;
    else if (C.proporty == "MUTIPLY_TOKEN" || C.proporty == "DEVIDE_TOKEN")
        return 2;
        
    return 0;
}
void print_expr(vector <Token> expr){
	for(int i = 0 ; i < int(expr.size()); ++i){
		cout << expr[i].row << ' ';
	}
	cout << "= ";
}

vector <Token> token_infixToPostfix(vector <Token> infix);
long double token_calculate_Postfix(vector <Token> post_exp);
class Reader{//讀取，轉為TOKEN 輸出為 vector <string>
	protected:
		string data; //原始vector 
		vector <Token> token;//token
		Token temp_token;
		Token END;
		int currPos;//保留位 
		int decPos;//探查位 
		string temp_str;
		int dataLength;//資料長度
		int line;
		
	public:
		Reader(string str){
			dataLength = str.size();
			currPos = 0;
			decPos = 0; 
			data = str;
			line = 1;
		}
		friend class Paser;
		void make_token(){
			while(currPos <  dataLength&&decPos <  dataLength){
				//單字元 
				if(int(data[currPos]) == 10){currPos++;}
				else if(data[currPos] == '('){temp_token.row = "(";temp_token.proporty = "LEFTPAREN_TOKEN";token.push_back(temp_token);currPos++;}
				else if(data[currPos] == ')'){temp_token.row = ")";temp_token.proporty = "RIGHTPAREN_TOKEN";token.push_back(temp_token);currPos++;}
				//運算子
				else if(data[currPos]=='+'){temp_token.row = "+";temp_token.proporty = "ADD_TOKEN";token.push_back(temp_token);currPos++;} 
				else if(data[currPos]=='-'){temp_token.row = "-";temp_token.proporty = "SUBTRACT_TOKEN";token.push_back(temp_token);currPos++;}
				else if(data[currPos]=='*'){temp_token.row = "*";temp_token.proporty = "MUTIPLY_TOKEN";token.push_back(temp_token);currPos++;}
				else if(data[currPos]=='/'){temp_token.row = "/";temp_token.proporty = "DEVIDE_TOKEN";token.push_back(temp_token);currPos++;}
				//數字
				else if(data[currPos]=='0'||
						data[currPos]=='1'||
						data[currPos]=='2'||
						data[currPos]=='3'||
						data[currPos]=='4'||
						data[currPos]=='5'||
						data[currPos]=='6'||
						data[currPos]=='7'||
						data[currPos]=='8'||
						data[currPos]=='9'){
							decPos = currPos;
							temp_str.push_back(data[decPos++]);
							while(data[decPos]=='0'||
								  data[decPos]=='1'||
								  data[decPos]=='2'||
								  data[decPos]=='3'||
								  data[decPos]=='4'||
								  data[decPos]=='5'||
								  data[decPos]=='6'||
								  data[decPos]=='7'||
								  data[decPos]=='8'||
								  data[decPos]=='9'){
								  	temp_str.push_back(data[decPos++]);
								  }
							temp_token.row = temp_str;
							temp_token.proporty = "NUMBER_TOKEN";
							token.push_back(temp_token);
							temp_str = "";
							currPos = decPos ;
				}
				else{
					Error();
				}	 
			}
		}
		void Error(){
			temp_token.row = "SYNTAX ERROR";
			temp_token.proporty = "SYNTAX_ERROR_TOKEN";
			token.push_back(temp_token);
			cout << "SYNTAX ERROR, Line:" << line << " : "<<data[currPos] <<endl ;
			cout << "currPos : " << currPos <<endl;
			exit(0);
		}
		
		void print_token(){
			for(int i = 0 ;i < int(token.size());i++){
				cout << token[i].row << " : " << token[i].proporty << endl;
			}
		}
		vector <Token> get_token(){return token;}
		int get_line(){return line;}
};


vector <Token> token_infixToPostfix(vector <Token> infix){
	
	vector <Token> opr;
	vector <Token> output;
    
	for (int i = 0; i < infix. size(); i++) {
		if (t_isOperand(infix[i])){
			output.push_back(infix[i]);
		}
		else if (infix[i].proporty == "LEFTPAREN_TOKEN"){
			opr.push_back(infix[i]);	
		}   
		else if (infix[i].proporty == "RIGHTPAREN_TOKEN") {
		    while (opr.back().proporty != "LEFTPAREN_TOKEN") {
		        output.push_back(opr.back());
		        opr.pop_back();
		    }
		    opr.pop_back();
		}
		// Operator found
		else{
			if (opr.size() > 0 && t_isOperator(opr.back())){
				while (opr.size() > 0 && t_getPriority(infix[i]) < t_getPriority(opr.back())){
			    		output.push_back(opr.back());
					opr.pop_back();	
		        	}  
		        }
		        opr.push_back(infix[i]);
		}
	}
    
	while(!opr.empty()){
		output.push_back(opr.back());
		opr.pop_back();        
	}
	
	return output;
}



vector <Token> token_infixToPrefix(vector <Token> infix){
	vector <Token> output;
	vector <Token> opr;
	reverse(infix.begin(), infix.end());
	for (int i = 0; i < infix. size(); i++) {
		if (t_isOperand(infix[i])){
			output.push_back(infix[i]);
		}
		else if (infix[i].proporty == "RIGHTPAREN_TOKEN"){
			opr.push_back(infix[i]);
		}   
		else if (infix[i].proporty == "LEFTPAREN_TOKEN") {
		    while (opr.back().proporty != "RIGHTPAREN_TOKEN") {
		        output.push_back(opr.back());
		        opr.pop_back();
		    }
		    opr.pop_back();
		}
		else{
			if (opr.size() > 0 && t_isOperator(opr.back())){
				while (opr.size() > 0 && t_getPriority(infix[i]) < t_getPriority(opr.back())){
			    		output.push_back(opr.back());
							opr.pop_back();
		    }  
			}
		        opr.push_back(infix[i]);
		        
		}
	}
    
      	while(!opr.empty()){
        	output.push_back(opr.back());
        	opr.pop_back();
    	}
	reverse(output.begin(), output.end());
	return output;
}

long double token_calculate_Postfix(vector <Token> post_exp){
    stack <long double> stack;
    int len = post_exp.size();
    for (int i = 0; i < len; i++){
        if ( t_isOperand(post_exp[i])){
            stack.push(str_to_int(post_exp[i].row));
        }
        else{
            long double a = stack.top();
            stack.pop();
            long double b = stack.top();
            stack.pop();
            switch (post_exp[i].row[0]){
                case '+': // addition
                          stack.push(b + a);
                          break;
                case '-': // subtraction
                          stack.push(b - a);
                          break;
                case '*': // multiplication
                          stack.push(b * a);
                          break;
                case '/': // division
                          stack.push(b / a);
                          break;
            }
        }
    }
    return stack.top();
}

long double token_evaluatePrefix(vector <Token> exprsn)
{
    stack<long double> Stack;
 
    for (int j = exprsn.size() - 1; j >= 0; j--) {
       
        if (t_isOperand(exprsn[j])){
            Stack.push(str_to_int(exprsn[j].row));
        }
        else {
            long double o1 = Stack.top();
            Stack.pop();
            long double o2 = Stack.top();
            Stack.pop();
            switch (exprsn[j].row[0]) {
            case '+':
                Stack.push(o1 + o2);
                break;
            case '-':
                Stack.push(o1 - o2);
                break;
            case '*':
                Stack.push(o1 * o2);
                break;
            case '/':
                Stack.push(o1 / o2);
                break;
            }
        }
    }
    return Stack.top();
}

bool error_dect(vector <Token> expression){
        bool result = false;
        vector <Token> output;
        vector <Token> opr;
        vector <int> st3;
        bool isTrue = false;
        Token temp;
        bool flag;
        for (int i = 0; i < expression.size(); i++) {
            temp = expression[i];
            if (i == expression.size() && st3.size() > 0){
        	return true;
        }
            if (t_isOperand(temp)) {
                output.push_back(temp);
                if(!isTrue) {
                    isTrue = true;
                }
                else {
                    return true;
                }
            } 
            else if (t_isOperator(temp)) {
                opr.push_back(temp);
                isTrue = false;
            }else if (temp.proporty == "LEFTPAREN_TOKEN"){
			opr.push_back(temp);
			st3.push_back(1);	
	    }else if (temp.proporty == "RIGHTPAREN_TOKEN") {
		    if(st3.size() == 0){return true;}
		    st3.pop_back();
	    }else{
	    }
            
        }
        if(st3.size() > 0){return true;}
        return result;
}


void *f_pos(void *arg) {
	clock_t start = clock();
	my_data *data=(my_data *)arg; 
	data->tid = gettid();
	
	//error detect
	if(error_dect(data->expr)){
		cout << "[Child 2] ERROR INPUT!" << endl;
		data->used_time = difftime(clock() , start);
		pthread_exit(NULL);
	}
	
	data->expr = token_infixToPostfix(data->expr);
	data->result = roundl(token_calculate_Postfix(data->expr));
	
	cout << "[Child 2] ";
	print_expr(data->expr);
	cout << data->result << endl;
	
	
	data->used_time = difftime(clock() , start);
	pthread_exit(NULL);
}

void *f_pre(void *arg) {
	clock_t start = clock();
	
	my_data *data=(my_data *)arg; 
	data->tid = gettid();
	//error detect
	if(error_dect(data->expr)){
		cout << "[Child 1] ERROR INPUT!" << endl;
		data->used_time = difftime(clock() , start);
		pthread_exit(NULL);
	}
	data->expr = token_infixToPrefix(data->expr);
	data->result = roundl(token_evaluatePrefix(data->expr));
	
	
	cout << "[Child 1] ";
	print_expr(data->expr);
	cout << data->result << endl;
	
	
	data->used_time = difftime(clock() , start);
	pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
	clock_t start;
	start = clock();
	
	string str ;
	ifstream inFile;
	inFile.open(argv[1]);
	if(getline(inFile,str)){}
	else{
		cout << "Fail to Open File!" << endl;
		return 0;
	};
	cout << "The infix input: " << str << endl;
	
	
	Reader a(str);
	a.make_token();
	vector <Token> str_token = a.get_token();
	pthread_t t_pos,t_pre;
	my_data d_pos, d_pre;
	d_pos.expr = str_token;
	d_pre.expr = str_token;
	vector<Token>().swap(str_token);
	pthread_create(&t_pre, NULL, f_pre, (void*) &d_pre);
	pthread_create(&t_pos, NULL, f_pos, (void*) &d_pos);
	pthread_join(t_pre, NULL);
	pthread_join(t_pos, NULL);
	
		
	cout << "[Child 1 tid=" << d_pre.tid << "] " << d_pre.used_time << "ms" << endl;
	cout << "[Child 2 tid=" << d_pos.tid << "] " << d_pos.used_time << "ms" << endl;
	cout << "[Main thread] " << difftime(clock() , start) << "ms" << endl;
	return 0;
}


