#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#include <WinInet.h>
#include <time.h>
#include <ctime>
//#include <iostream>
//#include "parson.h"
#pragma comment(lib, "WinInet.lib")
#pragma warning(disable:0546)

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
#define ESCAPE 5 //ESC
//using namespace std;

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
	if (key_value == 27) return ESCAPE;
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
		system("curl -G \"https://open.neis.go.kr/hub/SchoolSchedule?ATPT_OFCDC_SC_CODE=B10&SD_SCHUL_CODE=7010059\" -d \"KEY=c4d7533bcef9417097be47a4f311dd68&Type=xml&pIndex=1&pSize=300\" -o ./calendar.xml");
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

void Parse_Calendar() {
	add_scrollbar();
	printf("\nInternet File Parser ver 1.0\n");
	printf("Powered by Python\n");
	system(".\\CalendarParser\\PythonApplication1.exe");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//Green Text
	printf("외부 정보 파싱 성공\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
	
	/*
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
	
	for (int i = 0; i < cal_idx; i++) {
		printf("%d %d %d %s\n", evt[i].year, evt[i].month, evt[i].day, evt[i].eventname);
	}
	fclose(calfp); // 파일 닫기
	*/
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
	int cal_move_mode = 1;//기본적으로 정순 이동 1, 역순 이동 0
	int move_finalized_end = 0;//기본적으로 OFF, 끝 날짜에 도달시 ON
	int move_finalized_first = 0;//기본적으로 OFF, 시작 날짜에 도달시 ON
	system("cls");
	add_scrollbar();
	printf("LOADING CONSOLE LOG\n");
	Sleep(200);
	printf("CALENDAR PROGRAM INITIAL STATUS : ONLINE\n");
	Sleep(200);
	printf("START LOADING FIRST DATE\n");
	Sleep(200);
	calfp = fopen("./calendar_files/first.txt", "r");
	char d_first[9];
	fgets(d_first, 1024, calfp);
	fclose(calfp);
	printf("SUCCESS LOADING FIRST DATE\n");
	Sleep(200);
	printf("START LOADING LAST DATE\n");
	Sleep(200);
	calfp = fopen("./calendar_files/last.txt", "r");
	char d_last[9];
	fgets(d_last, 1024, calfp);
	fclose(calfp);
	printf("SUCCESS LOADING LAST DATE\n");
	Sleep(200);
	printf("INPUT FIRST DATE : %s, LAST DATE : %s", d_first, d_last);
	Sleep(200);
	timechk();
	int c_year = t->tm_year + 1900;
	int c_mon = t->tm_mon + 1;
	int c_day = t->tm_mday;
	printf("DATETIME FUNCTION ACTIVE\nPROCEED TO MAIN PROCESS\n");
	Sleep(800);
calendarsetup:
	char buffer[1000] = { 0, };
	char filename_extension[] = ".txt";
	char filename[50] = { 0, };
	char finalfilename[50] = { 0, };
	sprintf(filename, "%d%02d%02d", c_year, c_mon, c_day);
	if (strcmp(filename, d_last) == 0 && cal_move_mode == 1) {
		move_finalized_end = 1;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("현재 계신 날짜가 마지막 일정입니다.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		goto ForceKeyInput;
	}
	else if (strcmp(filename, d_first) == 0 && cal_move_mode == 0) {
		move_finalized_first = 1;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("현재 계신 날짜가 첫번째 일정입니다.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		goto ForceKeyInput;
	}
	move_finalized_first = 0;
	move_finalized_end = 0;
	strcat(filename, filename_extension);
	strcat(finalfilename, "./calendar_files/");
	strcat(finalfilename, filename);
	printf("%s", filename);
	system("cls");
	calfp = fopen(finalfilename, "r");
	if (calfp == NULL)
	{
		if (cal_move_mode == 1) {
			c_day++;
			if (c_day > 31) {
				c_day = 1;
				c_mon++;
			}
			if (c_mon > 12) {
				c_mon = 1;
				c_year++;
			}
			goto calendarsetup;
		}
		if (cal_move_mode == 0) {
			c_day--;
			if (c_day < 1) {
				c_day = 31;
				c_mon--;
			}
			if (c_mon < 1) {
				c_mon = 12;
				c_year--;
			}
			goto calendarsetup;
		}

	}
	printf("\n\n\n          가장 가까운 행사부터 보여줍니다. \n          이전 행사나 다음 행사를 확인하시려면 화살표 키로 조정하십시오.\n          ESC로 화면을 나갑니다.\n\n");
	

	fread(buffer, 1, 1000, calfp); //전체 읽기
	printf("%s", buffer);

	fclose(calfp); //파일 포인터 닫기
ForceKeyInput:
	while (1) {
		int key_input = keyControl();
		if (key_input == ESCAPE) return;
		else if (key_input == LEFT && move_finalized_first == 0) {
			cal_move_mode = 0;
			c_day--;
			if (c_day < 1) {
				c_day = 31;
				c_mon--;
			}
			if (c_mon < 1) {
				c_mon = 12;
				c_year--;
			}
			break;
		}
		else if (key_input == RIGHT && move_finalized_end == 0) {
			cal_move_mode = 1;
			c_day++;
			if (c_day > 31) {
				c_day = 1;
				c_mon++;
			}
			if (c_mon > 12) {
				c_mon = 1;
				c_year++;
			}
			break;
		}
		else if (key_input == RIGHT && move_finalized_end == 1)
		{
			cal_move_mode = 0;
		}
		else if (key_input == LEFT && move_finalized_end == 1)
		{
			move_finalized_end = 0;
			move_finalized_first = 0;
			cal_move_mode = 0;
			c_day--;
			if (c_day < 1) {
				c_day = 31;
				c_mon--;
			}
			if (c_mon < 1) {
				c_mon = 12;
				c_year--;
			}
			break;
		}
		else if (key_input == LEFT && move_finalized_first == 1)
		{
			cal_move_mode = 1;
		}
		else if (key_input == RIGHT && move_finalized_first == 1)
		{
			move_finalized_first = 0;
			move_finalized_end = 0;
			cal_move_mode = 1;
			c_day++;
			if (c_day > 31) {
				c_day = 1;
				c_mon++;
			}
			if (c_mon > 12) {
				c_mon = 1;
				c_year++;
			}
			break;
		}
	}
	goto calendarsetup;
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