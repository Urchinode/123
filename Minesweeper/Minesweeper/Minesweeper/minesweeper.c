#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#pragma warning(disable:4996)

//�ʱ� 9x9 10 �߱�16x16 40 ��� 16x30 99

void InputDifficulty(char* dc); //���̵��� �Է¹���
void SelectField(char* dc, char(*arr)[30]); //���̵��� �´� �ʵ� ���� 
void DrawField(char(*arr)[30], int row, int col, int m); //�ʵ��� ���� ��ġ ���� 
int StartGame(char* dc, char(*arr)[30], char(*varr)[30], int len); //���������� ���� ����(�ַܼ� �Է���ǥ�� �޾� ����� ǥ��) 
void ViewField(char* dc, char(*varr)[30]); //���� �ʵ� ���
int UpdateField(char* dc, char(*arr)[30], char(*varr)[30], int x, int y); //���� �ʵ� ���� 
void Recursive(char(*arr)[30], char(*varr)[30], int x, int y, int row, int col); //��ǥ�� �ֺ��� ���ڰ� ������ 
int Compare(char* dc, char(*varr)[30]); //���� �ʵ忡 �������� ���� ��ǥ ������ �ʵ��� ���� ������ ������ ��(�¸�/���� ����)
int Console(char* dc, char(*varr)[30], int x, int y); //���̵��� �°� �Է��� �������� �˻� 
int main(void)
{
	char dc = 0; // ���̵� �Է¹޴� ���� 
	char f[16][30] = { 0 };
	char vf[16][30] = { 0 }; //���� �ʵ�: ������ �ʵ带 ?�� ���� �ʵ� -> �Է��� ������ f�迭�κ��� ���� ���� 
	// union���� �Ҽ��ִ� ��� ������? -> memset -> ���� �ʱ�ȭ�ϴ°� �����ѵ� �Է��Ҷ� ������ ����Ű��� 
	int i = 0, j = 0, result;
	clock_t start, end;
	srand((unsigned)time(NULL));
	memset(vf, 63, sizeof(vf)); //�����ʵ� ?�� ä��� 
	InputDifficulty(&dc);
	system("cls");
	SelectField(&dc, f);
	start = clock();
	result = StartGame(&dc, f, vf, sizeof(vf));
	end = clock();
	if (result == 1)printf("�й�....\n");
	else printf("�¸�!!!\n");
	system("pause>nul");
	return 0;
}

void InputDifficulty(char* dc)
{
	printf("*******MINESWEEPER*******\n");
	while (!*dc) {
		printf("���̵��� �������ּ��� E:�ʱ� I:�߱� H:���\n");
		*dc = getchar();
		rewind(stdin); //�ݺ� �Է½� ����ó�� 
		if (*dc == 'e' || *dc == 'E') *dc = 'E';
		else if (*dc == 'i' || *dc == 'I') *dc = 'I';
		else if (*dc == 'h' || *dc == 'H') *dc = 'H';
		else {
			printf("�ùٸ� �Է��� �ƴմϴ�. �ٽ� �Է��ϼ���\n"); // �ùٸ� �Է��� ���������� �ݺ� 
			*dc = 0; //????
		}
	}
}

void SelectField(char* dc, char(*arr)[30])
{
	switch (*dc) //�ʵ� ���� 
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
	int r, c; //���� ��ġ ���� 
	while (m--) {
		r = rand() % row;
		c = rand() % col;
		if (arr[r][c] == '@') {
			m++; //�̹� ���ڰ� ������ �ݺ��� �ٽ� ���� 
			continue;
		}
		arr[r][c] = '@'; //���� ��� 
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
			if (arr[r][c + 1] != '@')arr[r][c + 1] += 1; //���� �ֺ��� 1�����ֱ�
	}
}
int StartGame(char* dc, char(*arr)[30], char(*varr)[30], int len) {
	int x, y; //Console
	int result = 0; //���� ���	
	printf("������ ���۵Ǿ����ϴ�. ���ڸ� ��Ʈ���� �ʰ� ��� ������ ����������.\n");
	printf("��ǥ�� �Է��ϸ鼭 ������ ������ �˴ϴ�.\n");
	printf("x�� y���� �Է��ϰ� ������ ""x y""�������� �Է��ϼ���.\n\n");
	do {
		ViewField(dc, varr);
		if (result == 1) return result; //���� ������ ��������
		while (1) {	 //��ǥ �Է� 
			printf("��ǥ�� �Է��ϼ���:");
			scanf("%d %d", &x, &y);
			if (Console(dc, varr, x, y) == 2) break; //�ʵ� ������ �Ѿ�� �ٽ� �Է� 
		}
		system("cls");
		result = UpdateField(dc, arr, varr, x, y);
	} while (Compare(dc, varr));
	return 2; //�¸�
}

void ViewField(char* dc, char(*varr)[30]) { //�����ʵ� ��� 
	int row = 0, col = 0;
	int i, j;
	switch (*dc) //���̵��� �´� ��/�� ���� 
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

int UpdateField(char* dc, char(*arr)[30], char(*varr)[30], int x, int y) { //�Է� ��ǥ�� ���� ������ 1�� ��ȯ�ؼ� �������� 
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
	if (varr[x][y] != '?'); // �̹� ������ �����̸� ���ó�� ����
	else if (varr[x][y] == '?' && arr[x][y] != '\0') varr[x][y] = arr[x][y]; //��ǥ���� 0�� �ƴϸ� ��ǥ���� ����
	else if (varr[x][y] == '?' && arr[x][y] == '\0') { //��ǥ���� 0�϶�(�ֺ��� ���ڰ� ������) �ֺ� ��ǥ���� ���ó��(�ֺ� ��ǥ�� ����)
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
	switch (*dc) //���̵��� �´� ��/�� ���� 
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
		printf("�Է� ������ ���� �ʽ��ϴ�. �ٽ� �Է����ּ���.\n\n");
		rewind(stdin);
		return 1;
	}
	else if (varr[x][y] != '?') {
		printf("\n�̹� ������ �����Դϴ�. �ٸ� ��ǥ�� �Է��ϼ���.\n\n");
		rewind(stdin);
		return 1;
	}
	else return 2;
}



