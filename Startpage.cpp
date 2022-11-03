#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#define _CRT_SECURE_NO_WARNINGS
// �⺻ ���� �κ�

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SUBMIT 4 //Select for Enter
//�޴� �Լ� ���� �κ�
int keyControl();
void init();
void titleDraw();
int menuDraw();
void gotoxy(int, int);

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
	printf("		Test Mode Running                 Last Modified 221103\n");
	printf("		######################################################\n");
	printf("		NOT FOR PUBLIC RELEASE      DO NOT SUBMIT THIS VERSION\n");
	printf("		    Developed by Hyunjae Hwang, Frontend Department   \n");
	printf("		######################################################\n");
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

int menuDraw() {
	int x = 27;
	int y = 12;
	gotoxy(x - 2, y);
	printf("> �׽�Ʈ�޴� 1");
	gotoxy(x, y + 2);
	printf("�׽�Ʈ�޴� 2");
	gotoxy(x, y + 4);
	printf("�׽�Ʈ�޴� 3");
	gotoxy(x, y + 6);
	printf("�׽�Ʈ�޴� 4");
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