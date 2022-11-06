#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#include <WinInet.h>
#include <time.h>
#include <ctime>
//#include "parson.h"
#pragma comment(lib, "WinInet.lib")

// 기본 정의 부분

#define BUFF_SIZE 1024

//인터넷 연결 확인 관련
DWORD dwFlag;
TCHAR szName[256];

//메뉴 함수 개발 부분
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SUBMIT 4 //Select for Enter

time_t timer;
struct tm* t;

int cal_idx;
char* tmp_dt;

struct Event {
	char* eventname;
	int year;
	int month;
	int day;
	char* first_grade;
	char* second_grade;
	char* third_grade;
};
struct Event evt[500];

char buff[BUFF_SIZE];
FILE* confp;
FILE* calfp;
bool internet_status = false;


int keyControl();
void init();
void titleDraw();
int menuDraw();
void gotoxy(int, int);
void console();
void remove_scrollbar();
void add_scrollbar();
bool InetStatus();
void Request_Calendar();
void Parse_Calendar();
void chkcal();
void timechk();
int File_Line_CHK();

void remove_scrollbar()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	COORD new_size =
	{
		info.srWindow.Right - info.srWindow.Left + 1,
		info.srWindow.Bottom - info.srWindow.Top + 1
	};
	SetConsoleScreenBufferSize(handle, new_size);
}

void add_scrollbar()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	COORD new_size =
	{
		info.srWindow.Right + info.srWindow.Left + 1024,
		info.srWindow.Bottom + info.srWindow.Top + 1024
	};
	SetConsoleScreenBufferSize(handle, new_size);
}

int keyControl() {
	int key_value = _getch();
	if (key_value == 0xE0) {
		//확장키일 경우 키 밸류를 하나 더 입력받는다.
		key_value = _getch();
		if (key_value == 72) {
			return UP;
		}//UP

		if (key_value == 80) {
			return DOWN;
		}//DOWN

		if (key_value == 75) {
			return LEFT;
		}//LEFT

		if (key_value == 77) {
			return RIGHT;
		}//RIGHT
	}
	if (key_value == 32) return SUBMIT;
}

void init() {
	system("mode con cols=107 lines=35");
	system("title 학교 알리미 알파테스트");
	//콘솔창 크기 및 정의 함수
}

void titleDraw() {
	printf("\n\n\n\n");
	printf("		########  #######  ####### #######  #			#  #    # ####### ###### \n");
	printf("		#         #        #     # #     #  #			#  ##   # #       #    # \n");
	printf("		########  #        #     # #     #  #			#  # #  # ####### #    # \n");
	printf("		       #  #        #     # #     #  #			#  #  # # #       #    # \n");
	printf("		########  #######  ####### #######  #######		#  #   ## #       ###### \n");
	//Initial Bootup 메뉴 출력 함수, 추후 변경 가능
}

void gotoxy(int x, int y)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(consoleHandle, pos);
	//커서 위치 이동 함수
}

void console() {
	system("cls");
	add_scrollbar();
	while (1)
	{
		system("cls");
		char message[1024];
		gotoxy(0, 0);
		printf("CONSOLE MODE |");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//Green Text
		printf(" ONLINE");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		printf("                      INPUT exit TO ESCAPE\n");
		printf(" > ");
		scanf("%s", message);
		if (strcmp(message,"exit")==0)
		{
			gotoxy(0, 0);
			printf("CONSOLE MODE |");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
			printf(" OFFLINE\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
			system("PAUSE");
			break;
		}
		confp = _popen(message, "r");
		if (NULL == confp) {
			perror("Console Connection Failed");
		}
		while (fgets(buff, BUFF_SIZE, confp))
		{
			printf("%s", buff);
		}
		_pclose(confp);
		printf("\n");
		system("PAUSE");
	}
	remove_scrollbar();
	return;
}

int menuDraw() {
	int x = 27;
	int y = 12;
	gotoxy(x - 2, y);
	printf("> 테스트메뉴 1");
	gotoxy(x, y + 2);
	printf("테스트메뉴 2");
	gotoxy(x, y + 4);
	printf("학사 일정 확인");
	gotoxy(x, y + 6);
	printf("CONSOLE");
	gotoxy(x, y + 8);
	printf("인터넷 연결 재 확인");
	while (1) {
		int n = keyControl();
		switch (n) {
		case UP: {
			if (y > 12)
			{
				gotoxy(x - 2, y);
				printf(" ");
				gotoxy(x - 2, y -= 2);
				printf(">");
			}
			break;
		}

		case DOWN: {
			if (y < 20) {
				gotoxy(x - 2, y);
				printf(" ");
				gotoxy(x - 2, y += 2);
				printf(">");
			}
			break;
		}

		case SUBMIT: {
			return y - 12;
		}
		}
	}
}

bool InetStatus() {
	system("cls");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	printf("Universal Internet Connection Verification System\n");
	if (internet_status == TRUE)
	{
		printf("이미 인터넷에 연결되어 있습니다. 다시 검사하시겠습니까?\n");
	}
	if (internet_status == FALSE)
	{
		printf("현재 인터넷 연결 상태를 검사합니다.\n");
	}
	printf("\n");
	int YN_return_value;
	int x = 3;
	int y = 3;
	gotoxy(x - 2, y); //1
	printf("> 인터넷 연결 검사");
	gotoxy(x + 20, y); //13
	printf("건너뛰기");
	while (1) {
		int n = keyControl();
		switch (n) {
		case RIGHT: {
			if (x < 23) {
				gotoxy(x - 2, y);
				printf(" ");
				x += 20;
				gotoxy(x - 2, y);
				printf(">");
			}
			break;
		}
		case LEFT: {
			if (x > 3) {
				gotoxy(x - 2, y);
				printf(" ");
				x -= 20;
				gotoxy(x - 2, y);
				printf(">");
			}
			break;
		}

		case SUBMIT: {
			YN_return_value = x - 2;
			goto cp1;
		}
		}
	}
cp1:
	if (YN_return_value == 1) {
		if (::InternetGetConnectedStateEx(&dwFlag, szName, 256, 0)) {
			gotoxy(0, 4);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//Green Text
			printf(" ONLINE(연결상태양호)\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
			return true;
		}
		else {
			gotoxy(0, 4);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
			printf(" OFFLINE(연결상태불량)\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
			return false;
		}
	}
	if (YN_return_value == 21) {
		gotoxy(0, 4);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("\n오프라인 상태로 프로그램을 구동합니다.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		return false;
	}
}

void Request_Calendar() {
	if (internet_status == TRUE)
	{
		system("curl -G \"https://open.neis.go.kr/hub/SchoolSchedule?ATPT_OFCDC_SC_CODE=B10&SD_SCHUL_CODE=7010059\" -d \"KEY=c4d7533bcef9417097be47a4f311dd68&Type=xml&pIndex=1&pSize=300\" -o calendar.xml");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//Green Text
		printf("인터넷에서 정보를 가져오기를 성공하였습니다.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		system("PAUSE");
		Parse_Calendar();
		return;
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("인터넷이 연결되지 않았으므로 정보 가져오기를 실패하였습니다.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		system("PAUSE");
	}
	return;
}

int File_Line_CHK() {
	int count_line = 0;
	const char* f_name = "calendar.txt";
	char tmp;
	calfp = fopen(f_name, "r");
	while (fscanf(calfp, "%c", &tmp) != EOF) {
		if (tmp == '\n') {
			count_line++;
		}
	}
	fseek(calfp, 0L, SEEK_SET);
	return count_line;
}

void Parse_Calendar() {
	add_scrollbar();
	printf("\nInternet File Parser ver 1.0\n");
	printf("Powered by Python\n");
	system("PythonApplication1.exe");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//Green Text
	printf("외부 정보 파싱 성공\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
	
	
	cal_idx = File_Line_CHK();
	calfp = fopen("calendar.txt", "r");
	if (calfp == NULL) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("ERROR : FAILED TO READ FILE. MIGHT BE INTERNET PROBLEM. CHECK AGAIN\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		system("PAUSE");
		return;
	}
	fclose(calfp);
	FILE* ddd;
	ddd = fopen("calendar.txt", "rt");
	int i = 0;
	while (fscanf(ddd, "%s %s %s %s %s",
		tmp_dt, evt[i].eventname, evt[i].first_grade, evt[i].second_grade, evt[i].third_grade) != EOF)
	{
		printf("success");
		int date = atoi(tmp_dt);
		int year = date / 10000;
		int month = (date / 100) % 100;
		int day = date % 100;
		evt[i].year = year;
		evt[i].month = month;
		evt[i].day = day;

	}
	fclose(ddd);
		
		
		
	/*
	char buffer[1001], * token;

	int i = 0;
	int idx = 0;
	while (!feof(calfp)) {
		printf("진입");
		i = 0;//i초기화
		fgets(buffer, 1001, calfp);
		token = strtok(buffer, " "); // 
		while (token != NULL) {
			printf("진입2");
			if (i == 0) {
				int date = atoi(token);
				int year = date / 10000;
				int month = (date / 100) % 100;
				int day = date % 100;
				
			}
			else if (i == 1) {
				strcpy(evt[idx].eventname, token);
			}
			else if (i == 2) {
				bool temp_b_grade;
				if (strcmp(token, "Y") == 0) temp_b_grade = true;
				else temp_b_grade = false;
				evt[idx].first_grade = temp_b_grade;
			}
			else if (i == 3)
			{
				bool temp_b_grade;
				if (strcmp(token, "Y") == 0) temp_b_grade = true;
				else temp_b_grade = false;
				evt[idx].second_grade = temp_b_grade;
			}
			else if (i == 4)
			{
				bool temp_b_grade;
				if (strcmp(token, "Y") == 0) temp_b_grade = true;
				else temp_b_grade = false;
				evt[idx].third_grade = temp_b_grade;
			}
			i++;
			token = strtok(NULL, " ");
		}
		idx++;
	}
	/*
	int i = 0;
	int idx = 0;
	while (!feof(calfp)) {
		i = 0;//i초기화
		fgets(buffer, 1000, calfp);
		token = strtok(buffer, " "); // 
		while (token != NULL) {
			if (i == 0) {
				
			}
			if (i == 1) {
				
			}
			if (i == 2) {
				
			}
			if (i == 3)
			{
				
			}
			if (i == 4) {
				
			}
			i++;
			token = strtok(NULL, " ");
		}
		idx++;
	}
	*/
	for (int i = 0; i < cal_idx; i++) {
		printf("%d %d %d %s\n", evt[i].year, evt[i].month, evt[i].day, evt[i].eventname);
	}
	fclose(calfp); // 파일 닫기
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//Green Text
	printf("파싱 후 저장 성공\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
	system("PAUSE");
	remove_scrollbar();
	return;
}

void timechk() {
	timer = time(NULL); // 1970년 1월 1일 0시 0분 0초부터 시작하여 현재까지의 초
	t = localtime(&timer); // 포맷팅을 위해 구조체에 넣기
	//year 사용시 +1900 필요
	//month 사용시 +1 필요
	return;
}

void chkcal() {
	system("cls");

	timechk();
	int c_year = t->tm_year - 100;
	int c_mon = t->tm_mon + 1;
	int c_day = t->tm_mday;
	int target_idx=0;

	for (int i = 0; i < cal_idx; i++)
	{
		if (evt[i].year == c_year) {
			if (evt[i].month == c_mon)
			{
				if (evt[i].day >= c_day) {
					target_idx = i;
					break;
				}
			}
			if (evt[i].month > c_mon)
			{
				target_idx = i;
				break;
			}
		}
	}
	printf("tidx:%d", target_idx);
	system("PAUSE");
	while (1)
	{
//		system("cls");
		printf("\n\n\n          가장 가까운 행사부터 보여줍니다. \n          이전 행사나 다음 행사를 확인하시려면 화살표 키로 조정하십시오.\n\n\n");

		printf("          현재 날짜는 : %d년 %d월 %d일 입니다.\n\n", t->tm_year - 100, t->tm_mon + 1, t->tm_mday);
		printf("진행 날짜 : %d년 %d월 %d일\n행사 명 : %s\n행사 대상 - 1학년 : %s\n        2학년 : %s\n        3학년 : %s\n행사 %d개중 %d번째 열람중\n\n\n", evt[target_idx].year, evt[target_idx].month, evt[target_idx].day, evt[target_idx].eventname, evt[target_idx].first_grade, evt[target_idx].second_grade, evt[target_idx].third_grade, cal_idx, target_idx);
//		int key_get = keyControl();
//		if(key_get==LEFT
		system("PAUSE");
	}
	system("PAUSE");
}

int main() {
	init();
	titleDraw();
	internet_status = InetStatus();
	Request_Calendar();
	while (1)
	{
		system("cls");
		titleDraw();
		int menuReturn = menuDraw();
		if (menuReturn == 4) chkcal();
		if (menuReturn == 6) console();
		if (menuReturn == 8) internet_status = InetStatus();
	}
	return 0;
}