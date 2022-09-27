#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

using namespace std;
#define NUMBER_OF_PHILOSOPHERS 11 //直接開11個當作上限

//哲學家相關
void *philosopher1(void *);
void *philosopher2(void *);
void *philosopher3(void *);
void waitting(int );//等待
void think(int );//思考
void pickUp1(int , int );
void pickUp2(int , int );
void pickUp3(int , int );
void eat(int ) ;//吃飯
void putDown(int , int );//放下筷子

//輸出相關
string outputFormat(int , string );
void print(string );

//全域變數設定
pthread_mutex_t chopsticks[NUMBER_OF_PHILOSOPHERS];
pthread_t philosophers[NUMBER_OF_PHILOSOPHERS];
pthread_attr_t attributes[NUMBER_OF_PHILOSOPHERS];
int phi_num, phi_mode;
const int EAT_NUM = 10;//最後改10

int main() {
	srand(0);
	cin >> phi_mode >> phi_num;
	//輸入範圍判讀
	if(phi_mode > 3 || phi_mode < 0)return 0;
	if(phi_num > 11 || phi_num < 3)return 0;
	
	for (int i = 0; i < phi_num; ++i) {
		pthread_mutex_init(&chopsticks[i], NULL);
	}
	for (int i = 0; i < phi_num; ++i) {
		pthread_attr_init(&attributes[i]);
	}
	int *id=(int *)malloc(sizeof(int)*phi_num);
	if(phi_mode == 1){
		for (int i = 0; i < phi_num; ++i) {
			id[i]=i;
			pthread_create(&philosophers[i], &attributes[i], philosopher1, (void *)(&id[i]));
		}
	}else if(phi_mode == 2){
		for (int i = 0; i < phi_num; ++i) {
			id[i]=i;
			pthread_create(&philosophers[i], &attributes[i], philosopher2, (void *)(&id[i]));
		}
	}else if(phi_mode == 3){
		for (int i = 0; i < phi_num; ++i) {
			id[i]=i;
			pthread_create(&philosophers[i], &attributes[i], philosopher3, (void *)(&id[i]));
		}
	}
	else{return 0;}
	for (int i = 0; i < phi_num; ++i) {
		pthread_join(philosophers[i], NULL);
	}
	return 0;
}

void *philosopher1(void *philosopherNumber) {
	int phi_id=*(int *)philosopherNumber;
	int eat_num = 0;
	waitting(phi_id);
	while (eat_num < EAT_NUM) {
		pickUp1(phi_id, phi_num);
		eat(phi_id);
		putDown(phi_id, phi_num);
		think(phi_id);
		++eat_num;
	}
	 pthread_exit(NULL);
}

void *philosopher2(void *philosopherNumber) {
	int phi_id=*(int *)philosopherNumber;
	int eat_num = 0;
	waitting(phi_id);
	while (eat_num < EAT_NUM) {
		pickUp2(phi_id, phi_num);
		eat(phi_id);
		putDown(phi_id, phi_num);
		think(phi_id);
		++eat_num;
	}
	 pthread_exit(NULL);
}

void *philosopher3(void *philosopherNumber) {
	int phi_id=*(int *)philosopherNumber;
	int eat_num = 0;
	waitting(phi_id);
	while (eat_num < EAT_NUM) {
		pickUp3(phi_id, phi_num);
		eat(phi_id);
		putDown(phi_id, phi_num);
		think(phi_id);
		++eat_num;
	}
	pthread_exit(NULL);
}

void waitting(int philosopherNumber) {
	sleep(rand()%5+1);//隨機等待1~5秒
}

void think(int philosopherNumber) {
	
	print(outputFormat(philosopherNumber, "thinking"));
	sleep(5+rand()%5);//思考5~10秒
}

void pickUp1(int philosopherNumber, int phi_num) {
	int right = (philosopherNumber + 1) % phi_num;
	int left = (philosopherNumber);
	pthread_mutex_lock(&chopsticks[right]);
	print(outputFormat(philosopherNumber, "picked up right chopstick"));
	pthread_mutex_lock(&chopsticks[left]);
	print(outputFormat(philosopherNumber, "picked up left chopstick"));
}

void pickUp2(int philosopherNumber, int phi_num) {
	int right = (philosopherNumber + 1) % phi_num;
	int left = (philosopherNumber + phi_num) % phi_num;
	cout << "pickUp2 ";
	if ((philosopherNumber+1) % 2) {
		cout << 1 << endl;
		pthread_mutex_lock(&chopsticks[left]);
		print(outputFormat(philosopherNumber, "picked up left chopstick"));
		pthread_mutex_lock(&chopsticks[right]);
		print(outputFormat(philosopherNumber, "picked up right chopstick"));
	}
	else {	
		cout << 2 << endl;
		pthread_mutex_lock(&chopsticks[right]);
		print(outputFormat(philosopherNumber, "picked up right chopstick"));
		pthread_mutex_lock(&chopsticks[left]);
		print(outputFormat(philosopherNumber, "picked up left chopstick"));
	}
}

void pickUp3(int philosopherNumber, int phi_num) {
	int right = (philosopherNumber + 1) % phi_num;
	int left = (philosopherNumber + phi_num) % phi_num;
	
	while(1) {
		pthread_mutex_lock(&chopsticks[right]);
		if(pthread_mutex_trylock(&chopsticks[left])) {
			pthread_mutex_unlock(&chopsticks[right]);
			continue;
		}
		break;
	}
	
	print(outputFormat(philosopherNumber, "picked up right chopstick"));
	print(outputFormat(philosopherNumber, "picked up left chopstick"));
	
}

void eat(int philosopherNumber) {
	print(outputFormat(philosopherNumber, "eating"));
	sleep(rand()%5+1);//隨機吃飯1~5秒
}

void putDown(int philosopherNumber, int phi_num) {
	print(outputFormat(philosopherNumber, "putting right chopstick"));
	pthread_mutex_unlock(&chopsticks[(philosopherNumber + 1) % phi_num]);
	print(outputFormat(philosopherNumber, "putting left chopstick"));
	pthread_mutex_unlock(&chopsticks[(philosopherNumber + phi_num) % phi_num]);
}

string outputFormat(int philosopherNumber, string content) {
	char tmp[10];
	time_t t = time(0);
	strftime( tmp, sizeof(tmp), "%X",localtime(&t));
	return string(tmp) + "-Phi " + to_string(philosopherNumber+1) + "-" + content;
}

void print(string content) {
	static pthread_mutex_t print_lock;

	pthread_mutex_lock(&print_lock);
	cout << content << endl;
	pthread_mutex_unlock(&print_lock);
}
