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
		system("curl -G \"https://open.neis.go.kr/hub/SchoolSchedule?ATPT_OFCDC_SC_CODE=B10&SD_SCHUL_CODE=7010059\" -d \"KEY=c4d7533bcef9417097be47a4f311dd68&Type=xml&pIndex=1&pSize=300\" -o calendar.xml");
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
	printf("�ܺ� ���� �Ľ� ����\n");
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
	*/
	for (int i = 0; i < cal_idx; i++) {
		printf("%d %d %d %s\n", evt[i].year, evt[i].month, evt[i].day, evt[i].eventname);
	}
	fclose(calfp); // ���� �ݱ�
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
		printf("\n\n\n          ���� ����� ������ �����ݴϴ�. \n          ���� ��糪 ���� ��縦 Ȯ���Ͻ÷��� ȭ��ǥ Ű�� �����Ͻʽÿ�.\n\n\n");

		printf("          ���� ��¥�� : %d�� %d�� %d�� �Դϴ�.\n\n", t->tm_year - 100, t->tm_mon + 1, t->tm_mday);
		printf("���� ��¥ : %d�� %d�� %d��\n��� �� : %s\n��� ��� - 1�г� : %s\n        2�г� : %s\n        3�г� : %s\n��� %d���� %d��° ������\n\n\n", evt[target_idx].year, evt[target_idx].month, evt[target_idx].day, evt[target_idx].eventname, evt[target_idx].first_grade, evt[target_idx].second_grade, evt[target_idx].third_grade, cal_idx, target_idx);
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