#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#include <WinInet.h>
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

//캘린더 파싱용 구조체 선언
struct Event {
	char* eventname;
	int year;
	int month;
	int day;
	bool first_grade;
	bool second_grade;
	bool third_grade;
};
struct Event evt[373];

char buff[BUFF_SIZE];
FILE* fp;

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
//void Parse_Calendar();

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
	printf("		Test Mode Running                 Last Modified 221103\n");
	printf("		######################################################\n");
	printf("		NOT FOR PUBLIC RELEASE      DO NOT SUBMIT THIS VERSION\n");
	printf("		    Developed by Hyunjae Hwang, Frontend Department   \n");
	printf("		######################################################\n");
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
		fp = _popen(message, "r");
		if (NULL == fp) {
			perror("Console Connection Failed");
		}
		while (fgets(buff, BUFF_SIZE, fp))
		{
			printf("%s", buff);
		}
		_pclose(fp);
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
	printf("테스트메뉴 3");
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
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("인터넷이 연결되지 않았으므로 정보 가져오기를 실패하였습니다.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		system("PAUSE");
	}
}

/*
void Parse_Calendar() {
	JSON_Value* rootValue;
	JSON_Object* rootObject;
	
	rootValue = json_parse_file("calendar.json");
	rootObject = json_value_get_object(rootValue);

}
*/
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
		if (menuReturn == 6) console();
		if (menuReturn == 8) internet_status = InetStatus();
	}
	return 0;
}