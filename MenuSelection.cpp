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

// �⺻ ���� �κ�

#define BUFF_SIZE 1024

//���ͳ� ���� Ȯ�� ����
DWORD dwFlag;
TCHAR szName[256];

//�޴� �Լ� ���� �κ�
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
		//Ȯ��Ű�� ��� Ű ����� �ϳ� �� �Է¹޴´�.
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
	system("title �б� �˸��� �����׽�Ʈ");
	//�ܼ�â ũ�� �� ���� �Լ�
}

void titleDraw() {
	printf("\n\n\n\n");
	printf("		########  #######  ####### #######  #			#  #    # ####### ###### \n");
	printf("		#         #        #     # #     #  #			#  ##   # #       #    # \n");
	printf("		########  #        #     # #     #  #			#  # #  # ####### #    # \n");
	printf("		       #  #        #     # #     #  #			#  #  # # #       #    # \n");
	printf("		########  #######  ####### #######  #######		#  #   ## #       ###### \n");
	//Initial Bootup �޴� ��� �Լ�, ���� ���� ����
}

void gotoxy(int x, int y)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(consoleHandle, pos);
	//Ŀ�� ��ġ �̵� �Լ�
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
	printf("> �׽�Ʈ�޴� 1");
	gotoxy(x, y + 2);
	printf("�׽�Ʈ�޴� 2");
	gotoxy(x, y + 4);
	printf("�л� ���� Ȯ��");
	gotoxy(x, y + 6);
	printf("CONSOLE");
	gotoxy(x, y + 8);
	printf("���ͳ� ���� �� Ȯ��");
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
		printf("�̹� ���ͳݿ� ����Ǿ� �ֽ��ϴ�. �ٽ� �˻��Ͻðڽ��ϱ�?\n");
	}
	if (internet_status == FALSE)
	{
		printf("���� ���ͳ� ���� ���¸� �˻��մϴ�.\n");
	}
	printf("\n");
	int YN_return_value;
	int x = 3;
	int y = 3;
	gotoxy(x - 2, y); //1
	printf("> ���ͳ� ���� �˻�");
	gotoxy(x + 20, y); //13
	printf("�ǳʶٱ�");
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
			printf(" ONLINE(������¾�ȣ)\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
			return true;
		}
		else {
			gotoxy(0, 4);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
			printf(" OFFLINE(������ºҷ�)\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
			return false;
		}
	}
	if (YN_return_value == 21) {
		gotoxy(0, 4);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("\n�������� ���·� ���α׷��� �����մϴ�.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		return false;
	}
}

void Request_Calendar() {
	if (internet_status == TRUE)
	{
		system("curl -G \"https://open.neis.go.kr/hub/SchoolSchedule?ATPT_OFCDC_SC_CODE=B10&SD_SCHUL_CODE=7010059\" -d \"KEY=c4d7533bcef9417097be47a4f311dd68&Type=xml&pIndex=1&pSize=300\" -o ./calendar.xml");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//Green Text
		printf("���ͳݿ��� ������ �������⸦ �����Ͽ����ϴ�.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		system("PAUSE");
		Parse_Calendar();
		return;
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("���ͳ��� ������� �ʾ����Ƿ� ���� �������⸦ �����Ͽ����ϴ�.\n");
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
	printf("�ܺ� ���� �Ľ� ����\n");
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
		printf("����");
		i = 0;//i�ʱ�ȭ
		fgets(buffer, 1001, calfp);
		token = strtok(buffer, " "); // 
		while (token != NULL) {
			printf("����2");
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
		i = 0;//i�ʱ�ȭ
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
	fclose(calfp); // ���� �ݱ�
	*/
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//Green Text
	printf("�Ľ� �� ���� ����\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
	system("PAUSE");
	remove_scrollbar();
	return;
}

void timechk() {
	timer = time(NULL); // 1970�� 1�� 1�� 0�� 0�� 0�ʺ��� �����Ͽ� ��������� ��
	t = localtime(&timer); // �������� ���� ����ü�� �ֱ�
	//year ���� +1900 �ʿ�
	//month ���� +1 �ʿ�
	return;
}

void chkcal() {
	int cal_move_mode = 1;//�⺻������ ���� �̵� 1, ���� �̵� 0
	int move_finalized_end = 0;//�⺻������ OFF, �� ��¥�� ���޽� ON
	int move_finalized_first = 0;//�⺻������ OFF, ���� ��¥�� ���޽� ON
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
		printf("���� ��� ��¥�� ������ �����Դϴ�.\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);//WHITE TEXT
		goto ForceKeyInput;
	}
	else if (strcmp(filename, d_first) == 0 && cal_move_mode == 0) {
		move_finalized_first = 1;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//RED Text
		printf("���� ��� ��¥�� ù��° �����Դϴ�.\n");
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
	printf("\n\n\n          ���� ����� ������ �����ݴϴ�. \n          ���� ��糪 ���� ��縦 Ȯ���Ͻ÷��� ȭ��ǥ Ű�� �����Ͻʽÿ�.\n          ESC�� ȭ���� �����ϴ�.\n\n");
	

	fread(buffer, 1, 1000, calfp); //��ü �б�
	printf("%s", buffer);

	fclose(calfp); //���� ������ �ݱ�
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