#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#define Map_X_MAX 60
#define Map_Y_MAX 40
#define Max_value Map_X_MAX * Map_Y_MAX

char mapData[Map_X_MAX * Map_Y_MAX];

//콘솔 창의 크기와 제목을 지정하는 함수
void SetConsole() {
	system("title Flight Game"); // 콘솔창 이름 설정
	system("mode con:cols=60 lines=40"); // 콘솔창 크기설정

	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = 0;
	ConsoleCursor.dwSize = 1;
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(consoleHandle, &ConsoleCursor);
}

void textcolor(int color_number) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_number);
}

//커서를 이동하는 함수
void gotoxy(int x, int y) {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(consoleHandle, Cur);
}

//비행기 캐릭터를 그리는 함수
typedef struct Character {
	int x = Map_X_MAX / 2 + 1, y = Map_Y_MAX / 3 * 2 + 1;
	int size_x = 3, size_y = 2;
	char sprite[6] = " ▲ ▲ ▲";
}plane;

void controlcharacter() {
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		if (y > 0)
			y--;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		if (y < Map_Y_MAX - 1)
			y++;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		if (x > 0)
			x -= 2;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		if (x < Map_X_MAX - 2)
			x += 2;
	}

}

void FillMap(char str[], char str_s, int max_value) {
	for (int i = 0; i < max_value; i++) {
		str[i] = str_s;
	}
}

void EditMap(int x, int y, char str) {
	if (x > 0 && y > 0 && x - 1 < Map_X_MAX - 1 && y - 1 < Map_Y_MAX - 1)
		str = mapData[(y - 1) * Map_X_MAX + x - 1];
}

void DrawSprite(int x, int y, int size_x, int size_y, char spr[]) {
	for (int i = 0; i < size_y; i++) {
		for (int j = 0; j < size_x; j++) {
			EditMap(x + j, y + i, spr[i * size_x + j]);
		}
	}
}

int main(void)
{
	SetConsole();
	
	while (1) {
		Drawplane();

		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			if (y > 0)
				y--;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			if (y < Map_Y_MAX - 1)
				y++;
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			if (x > 0)
				x -= 2;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			if (x < Map_X_MAX - 2)
				x += 2;
		}
		Sleep(50);
	}
	return 0;
}