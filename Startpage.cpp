#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#define _CRT_SECURE_NO_WARNINGS
// 기본 정의 부분

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SUBMIT 4 //Select for Enter
//메뉴 함수 개발 부분
int keyControl();
void init();
void titleDraw();
int menuDraw();
void gotoxy(int, int);

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
	printf("테스트메뉴 4");
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
			if (y < 18) {
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

int main() {
	init();
	titleDraw();
	menuDraw();
	return 0;
}