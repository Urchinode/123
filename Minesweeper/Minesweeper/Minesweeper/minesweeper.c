#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#pragma warning(disable:4996)

//초급 9x9 10 중급16x16 40 고급 16x30 99

void InputDifficulty(char* dc); //난이도를 입력받음
void SelectField(char* dc, char(*arr)[30]); //난이도에 맞는 필드 선택 
void DrawField(char(*arr)[30], int row, int col, int m); //필드의 지뢰 위치 선정 
int StartGame(char* dc, char(*arr)[30], char(*varr)[30], int len); //본격적으로 게임 시작(콘솔로 입력좌표를 받아 결과를 표시) 
void ViewField(char* dc, char(*varr)[30]); //가상 필드 출력
int UpdateField(char* dc, char(*arr)[30], char(*varr)[30], int x, int y); //가상 필드 갱신 
void Recursive(char(*arr)[30], char(*varr)[30], int x, int y, int row, int col); //좌표값 주변에 지뢰가 없으면 
int Compare(char* dc, char(*varr)[30]); //가상 필드에 밝혀지지 않은 좌표 개수가 필드의 지뢰 개수와 같은지 비교(승리/실패 여부)
int Console(char* dc, char(*varr)[30], int x, int y); //난이도에 맞게 입력이 적절한지 검사 
int main(void)
{
	char dc = 0; // 난이도 입력받는 변수 
	char f[16][30] = { 0 };
	char vf[16][30] = { 0 }; //가상 필드: 생성된 필드를 ?로 덮은 필드 -> 입력을 받으면 f배열로부터 값을 대입 
	// union으로 할수있는 방법 없을까? -> memset -> 당장 초기화하는건 가능한데 입력할때 문제가 생길거같다 
	int i = 0, j = 0, result;
	clock_t start, end;
	srand((unsigned)time(NULL));
	memset(vf, 63, sizeof(vf)); //가상필드 ?로 채우기 
	InputDifficulty(&dc);
	system("cls");
	SelectField(&dc, f);
	start = clock();
	result = StartGame(&dc, f, vf, sizeof(vf));
	end = clock();
	if (result == 1)printf("패배....\n");
	else printf("승리!!!\n");
	system("pause>nul");
	return 0;
}

void InputDifficulty(char* dc)
{
	printf("*******MINESWEEPER*******\n");
	while (!*dc) {
		printf("난이도를 설정해주세요 E:초급 I:중급 H:고급\n");
		*dc = getchar();
		rewind(stdin); //반복 입력시 공백처리 
		if (*dc == 'e' || *dc == 'E') *dc = 'E';
		else if (*dc == 'i' || *dc == 'I') *dc = 'I';
		else if (*dc == 'h' || *dc == 'H') *dc = 'H';
		else {
			printf("올바른 입력이 아닙니다. 다시 입력하세요\n"); // 올바른 입력을 받을때까지 반복 
			*dc = 0; //????
		}
	}
}

void SelectField(char* dc, char(*arr)[30])
{
	switch (*dc) //필드 선택 
	{
	case 'E':
		DrawField(arr, 9, 9, 10);
		break;
	case 'I':
		DrawField(arr, 16, 16, 40);
		break;
	case 'H':
		DrawField(arr, 16, 30, 99);
	}
}

void DrawField(char(*arr)[30], int row, int col, int m)
{
	int i = 0, j = 0;
	int r, c; //지뢰 위치 랜덤 
	while (m--) {
		r = rand() % row;
		c = rand() % col;
		if (arr[r][c] == '@') {
			m++; //이미 지뢰가 있으면 반복문 다시 실행 
			continue;
		}
		arr[r][c] = '@'; //지뢰 찍기 
		if (r >= 1 && c >= 1)
			if (arr[r - 1][c - 1] != '@') arr[r - 1][c - 1] += 1;
		if (r <= row - 2 && c <= col - 2)
			if (arr[r + 1][c + 1] != '@')arr[r + 1][c + 1] += 1;
		if (r >= 1 && c <= col - 2)
			if (arr[r - 1][c + 1] != '@')arr[r - 1][c + 1] += 1;
		if (r <= row - 2 && c >= 1)
			if (arr[r + 1][c - 1] != '@')arr[r + 1][c - 1] += 1;
		if (c >= 1)
			if (arr[r][c - 1] != '@')arr[r][c - 1] += 1;
		if (r >= 1)
			if (arr[r - 1][c] != '@')arr[r - 1][c] += 1;
		if (r <= row - 2)
			if (arr[r + 1][c] != '@')arr[r + 1][c] += 1;
		if (c <= col - 2)
			if (arr[r][c + 1] != '@')arr[r][c + 1] += 1; //지뢰 주변에 1더해주기
	}
}
int StartGame(char* dc, char(*arr)[30], char(*varr)[30], int len) {
	int x, y; //Console
	int result = 0; //승패 결과	
	printf("게임이 시작되었습니다. 지뢰를 터트리지 않고 모든 영역을 밝혀보세요.\n");
	printf("좌표를 입력하면서 영역을 밝히면 됩니다.\n");
	printf("x행 y열을 입력하고 싶으면 ""x y""형식으로 입력하세요.\n\n");
	do {
		ViewField(dc, varr);
		if (result == 1) return result; //지뢰 누르면 강제종료
		while (1) {	 //좌표 입력 
			printf("좌표를 입력하세요:");
			scanf("%d %d", &x, &y);
			if (Console(dc, varr, x, y) == 2) break; //필드 범위를 넘어서면 다시 입력 
		}
		system("cls");
		result = UpdateField(dc, arr, varr, x, y);
	} while (Compare(dc, varr));
	return 2; //승리
}

void ViewField(char* dc, char(*varr)[30]) { //가상필드 출력 
	int row = 0, col = 0;
	int i, j;
	switch (*dc) //난이도에 맞는 행/열 결정 
	{
	case 'E':
		row = 9; col = 9;
		break;
	case 'I':
		row = 16; col = 16;
		break;
	case 'H':
		row = 16; col = 30;
		break;
	}
	printf("  ");
	for (i = 0; i < col; i++) printf(" %2d", i);
	puts("");
	for (i = 0; i < row; i++)
	{
		printf("%2d", i);
		for (j = 0; j < col; j++) {
			if (varr[i][j] == '?') printf("  %c", varr[i][j]);
			else if (varr[i][j] == '@') printf("  %c", varr[i][j]);
			else printf("  %d", varr[i][j]);
		}
		printf("\n");
	}
}

int UpdateField(char* dc, char(*arr)[30], char(*varr)[30], int x, int y) { //입력 좌표에 지뢰 있으면 1을 반환해서 강제종료 
	int row = 0, col = 0, i, j;
	switch (*dc)
	{
	case 'E':
		row = 9; col = 9;
		break;
	case 'I':
		row = 16; col = 16;
		break;
	case 'H':
		row = 16; col = 30;
		break;
	}
	if (arr[x][y] == '@') {
		for (i = 0; i < row; i++) {
			for (j = 0; j < col; j++) {
				if (arr[i][j] == '@') varr[i][j] = arr[i][j];
			}
		}
		return 1;
	}
	else Recursive(arr, varr, x, y, row, col);
	return 2;
}
void Recursive(char(*arr)[30], char(*varr)[30], int x, int y, int row, int col) {
	if (varr[x][y] != '?'); // 이미 밝혀진 영역이면 재귀처리 면제
	else if (varr[x][y] == '?' && arr[x][y] != '\0') varr[x][y] = arr[x][y]; //좌표값이 0이 아니면 좌표값만 대입
	else if (varr[x][y] == '?' && arr[x][y] == '\0') { //좌표값이 0일때(주변에 지뢰가 없으면) 주변 좌표들을 재귀처리(주변 좌표도 대입)
		varr[x][y] = arr[x][y];
		if (x >= 1 && y >= 1) {
			Recursive(arr, varr, x - 1, y - 1, row, col);
		}
		if (x >= 1) {
			Recursive(arr, varr, x - 1, y, row, col);
		}
		if (x >= 1 && y <= col - 2) {
			Recursive(arr, varr, x - 1, y + 1, row, col);
		}
		if (y >= 1) {
			Recursive(arr, varr, x, y - 1, row, col);
		}
		if (y <= col - 2) {
			Recursive(arr, varr, x, y + 1, row, col);
		}
		if (x <= row - 2 && y >= 1) {
			Recursive(arr, varr, x + 1, y - 1, row, col);
		}
		if (x <= row - 2) {
			Recursive(arr, varr, x + 1, y, row, col);
		}
		if (x <= row - 2 && y <= col - 2) {
			Recursive(arr, varr, x + 1, y + 1, row, col);
		}
	}
}
int Compare(char* dc, char(*varr)[30]) {
	int m = 0, count = 0, row = 0, col = 0, result = 0;
	int i, j;
	switch (*dc)
	{
	case 'E':
		m = 10; row = 9; col = 9;
		break;
	case 'I':
		m = 40; row = 16; col = 16;
		break;
	case 'H':
		m = 99; row = 16; col = 30;
		break;
	}
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if (varr[i][j] == '?') count++;
		}
	}
	if (count == m) {
		result = 0;
		for (i = 0; i < col; i++) printf(" %2d", i);
		puts("");
		for (i = 0; i < row; i++)
		{
			printf("  ");
			printf("%2d", i);
			for (j = 0; j < col; j++) {
				if (varr[i][j] == '?') printf("  %c", varr[i][j]);
				else printf("  %d", varr[i][j]);
			}
			printf("\n");
		}
	}
	else result = 1;
	return result;
}
int Console(char* dc, char(*varr)[30], int x, int y) {
	int row = 0, col = 0;
	switch (*dc) //난이도에 맞는 행/열 결정 
	{
	case 'E':
		row = 9; col = 9;
		break;
	case 'I':
		row = 16; col = 16;
		break;
	case 'H':
		row = 16; col = 30;
		break;
	}
	if (!(x >= 0 && x < row && y >= 0 && y < col)) {
		printf("입력 범위가 맞지 않습니다. 다시 입력해주세요.\n\n");
		rewind(stdin);
		return 1;
	}
	else if (varr[x][y] != '?') {
		printf("\n이미 밝혀진 영역입니다. 다른 좌표로 입력하세요.\n\n");
		rewind(stdin);
		return 1;
	}
	else return 2;
}



