#pragma once
#define _CRT_SECURE_NO_WARNINGS

#ifndef _MAIN_H_
#define _MAIN_H_

#include<stdio.h>
#include<stdlib.h>

struct stu {
	char name[15];
	int grade;
	int cls;
	int id;
	//추후 특수 분류 추가 바람
};

struct document {
	char name[100];
	int grade; //특정 학년 대상, 0일경우 전체 대상
	int cls; //특정 반 대상, 0일 경우 전체 대상
	int id; //특정 학생 대상, 0일경우 전체 대상
	int date; //기한 설정, YYMMDD 형식으로 입력 받음.
	char link[500]; //e알리미로 바로가기 링크
	int cansubmit;
};

void printinfo(struct document doc[1001], int bin) {
	printf("\n\t이름: \t\t%s\n", doc[bin].name);
	if (doc[bin].grade != 0) printf("\t대상 학년: \t%d\n", doc[bin].grade);
	if (doc[bin].cls != 0) printf("\t대상 반: \t%d\n", doc[bin].cls);
	if (doc[bin].date != 0) printf("\t기한: \t\t%d\n", doc[bin].date);
	printf("\t신청 가능 여부: %s", doc[bin].cansubmit ? "가능" : "불가능");
}

int getstuinfo(struct stu student[101]) {
	char tmp;
	int i, count_line = 0;

	FILE* fp = fopen("students.txt", "r");
	if (fp == NULL)
	{
		perror("Error");
		exit(1);
	}

	while (fscanf(fp, "%c", &tmp) != EOF)  if (tmp == '\n') count_line++;
	fseek(fp, 0L, SEEK_SET);
	//printf("%d\n",count_line);

	for (i = 0; i < count_line; i++)   fscanf(fp, "%s %d %d %d", &student[i].name, &student[i].grade, &student[i].cls, &student[i].id);
	fclose(fp);

	return count_line;
}

int getdocinfo(struct document doc[1001]) {
	char tmp;
	int i, count_line = 0;

	FILE* fp = fopen("documents.txt", "r");
	if (fp == NULL)
	{
		perror("Error");
		exit(1);
	}

	while (fscanf(fp, "%c", &tmp) != EOF)  if (tmp == '\n') count_line++;
	fseek(fp, 0L, SEEK_SET);
	//printf("%d\n",count_line);

	for (i = 0; i < count_line; i++)   fscanf(fp, "%s %d %d %d %d %s %d", &doc[i].name, &doc[i].grade, &doc[i].cls, &doc[i].id, &doc[i].date, &doc[i].link, &doc[i].cansubmit);
	fclose(fp);

	return count_line;
}

void submit_page(struct document doc[1000], int line)
{
	//printf("\n\n\n");
	int submit[1000], issubmitted[1000] = { 0, };
	int i, j = 0, bin = -1;

	FILE* fp = fopen("submitlist.txt", "r+");
	if (fp == NULL) //init
	{
		perror("Error");
		exit(1);
	}
	fscanf(fp, "%d", &bin);
	if (bin == -1)
	{
		//printf("파일 생성중...\n");
		for (i = 0; i < line; i++)
		{
			if (doc[i].cansubmit == 1)  fprintf(fp, "%d %d\n", i, 0);
		}
	}
	fclose(fp);
	fp = fopen("submitlist.txt", "r+");
	for (i = 0; i < line; i++)
	{
		if (doc[i].cansubmit == 1)
		{
			submit[j] = i;
			fscanf(fp, "%d %d", &bin, &issubmitted[j]);
			j++;
		}
	}
	fclose(fp);
	fp = fopen("submitlist.txt", "w+");
	printf("현재 신청 목록:\n");
	for (i = 0; i < j; i++)    printf("%d. %s \t\t\t 신청여부: %d\n", i + 1, doc[submit[i]].name, issubmitted[i]);
	printf("다음으로 수행할 동작을 번호로 입력하세요(1 : 신청, 2 : 신청취소, 3 : 상세정보 확인) ");
	scanf("%d", &bin);
	switch (bin)
	{
	case 1:
		printf("신청할 항목의 번호를 입력하세요 ");
		scanf("%d", &bin);
		bin = submit[bin - 1];
		issubmitted[bin] = 1;
		for (i = 0; i < line; i++)
		{
			if (doc[i].cansubmit == 1)  fprintf(fp, "%d %d\n", i, issubmitted[i]);
		}
		break;
	case 2:
		printf("신청취소할 항목의 번호를 입력하세요 ");
		scanf("%d", &bin);
		bin = submit[bin - 1];
		issubmitted[bin] = 0;
		for (i = 0; i < line; i++)
		{
			if (doc[i].cansubmit == 1)  fprintf(fp, "%d %d\n", i, issubmitted[i]);
		}
		break;
	case 3:
		printf("상세정보를 표시할 항목의 번호를 입력하세요 ");
		scanf("%d", &bin);
		bin = submit[bin - 1];
		printinfo(doc, bin);
		break;
	default:
		printf("1부터 3까지의 수를 입력하세요\n");
	}

	fclose(fp);
}

#endif

/*
int main()
{

	//텍스트 파일 내 저장 순서
	//예)홍길동 1 14 1


	//저장된 학생 정보 불러오기
	int line_stu, line_doc;
	struct stu student[101];
	struct document doc[1000];
	line_stu = getstuinfo(student);
	line_doc = getdocinfo(doc);

	//for(i=0;i<line_stu;i++) printf("%s %d %d %d\n", student[i].name,student[i].grade,student[i].cls,student[i].id);
	//for(i=0;i<line_doc;i++) printf("%s %d %d %d %d %s %d\n", doc[i].name, doc[i].grade, doc[i].cls, doc[i].id, doc[i].date, doc[i].link, doc[i].cansubmit);

	submit_page(doc, line_doc);
	return 0;
}
*/