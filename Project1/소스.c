#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define Map_X_MAX 80
#define Map_Y_MAX 40
#define Max_value Map_X_MAX * Map_Y_MAX
#define MAXBULLET 10

int b_index = 0; //캐릭터 총알 인덱스
int i_index = 0; //적 수 인덱스
int eb_index = 0; //적 총알 인덱스
int r_index = 0; //랭킹 인덱스
int gameStatus = 0; // 게임 상태(게임씬) 변수
int score = 0; // 점수

//맵 출력을 위한 데이터
char mapData[Max_value];

//캐릭터 위치, 크기를 설정하는 구조체
typedef struct Character {
    int x, y, size_x, size_y;
    char sprite[6];
}plane;

plane myPlane;
//탄막 정보 구조체
typedef struct bullet {
    int x, y;
    bool Active;
}Bullet;

Bullet Bullet_info[MAXBULLET];
Bullet Enemy_Bullet[MAXBULLET*10000];

typedef struct enemy {
    int x, y;
    bool Active;
    Bullet Enemy_Bullet;
}Enemy;

Enemy Enemy_info[11];

typedef struct rank_info {
    char* name;
    int score;
    struct rank_info* next;
}RNKINFO;

typedef struct list {
    struct rank_info* cur;
    struct rank_info* head;
    struct rank_info* tail;
}LINKEDLIST;

void SetConsole();
void textcolor();
void gotoxy();
void FillMap();
void EditMap();
void DrawSprite();
void controlcharacter();
void gameover();
void gamestart();
void gameclear();
void gamereset();
void Bullet_Shoot();
void Enemy_Create();
void Enemy_Move();
void Enemy_Attack();
void ShowOption();
void rank_input();
void printScore(LINKEDLIST* lnk);
void insertData(LINKEDLIST* lnk, char* name, int score);
void deleteLastNode(LINKEDLIST* lnk);
void RankingShow(void);


int main(void)
{
    srand(time(NULL));
    SetConsole();
    myPlane.x = Map_X_MAX / 2 + 1;
    myPlane.y = Map_Y_MAX / 3 * 2 + 1;
    myPlane.size_x = 3;
    myPlane.size_y = 2;
    strncpy(myPlane.sprite, "_^_^^^", sizeof(myPlane.sprite));
    int count = 0;

    while (1) {
        while (gameStatus == 0) {
            gamestart();
            printf("%s", mapData);
        }
        while (gameStatus == 1) {
            textcolor(7);
            FillMap(mapData, ' ', Max_value);
            controlcharacter(&myPlane);
            DrawSprite(myPlane.x, myPlane.y, myPlane.size_x, myPlane.size_y, myPlane.sprite);
            Bullet_Shoot(&Bullet_info, &myPlane, &Enemy_info);
            Enemy_Create(&Enemy_info, count);
            Enemy_Move(&Enemy_info);
            Enemy_Attack(&Enemy_info, &Enemy_Bullet, &myPlane);
            ShowOption();
            printf("%s", mapData);
            Sleep(50);
            count++;
            score++;
        }
        if (gameStatus == 2) {
            FillMap(mapData, ' ', Max_value);
            printf("%s", mapData);
            rank_input();
            while (gameStatus == 2) {
                gameover();
                printf("%s", mapData);
                count = 0;
            }
        }
        while (gameStatus == 3) {
            gameclear();
            printf("%s", mapData);
            exit(1);
        }
        while (gameStatus == 4) {
            textcolor(6);
            RankingShow();
            exit(1);
        }
    }
    return 0;
}

//콘솔 창의 크기와 제목을 지정하는 함수
void SetConsole() {
    system("title Flight Game"); // 콘솔창 이름 설정
    system("mode con:cols=80 lines=40"); // 콘솔창 크기설정
    //커서 안보이게 설정
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

//화면을 초기화하는 함수(잔상 지우기용)
void FillMap(char str[], char str_s, int max_value) {
    for (int i = 0; i < max_value; i++) {
        str[i] = str_s;
    }
}

//맵 데이터를 수정하는 함수
void EditMap(int x, int y, char str) {
    if (x > 0 && y > 0 && x - 1 < Map_X_MAX - 1 && y - 1 < Map_Y_MAX - 1)
        mapData[(y - 1) * Map_X_MAX + x - 1] = str;
}
//텍스트를 이미지화 하여 출력하는 함수
void DrawSprite(int x, int y, int size_x, int size_y, char spr[]) {
    for (int i = 0; i < size_y; i++) {
        for (int j = 0; j < size_x; j++) {
            EditMap(x + j, y + i, spr[i * size_x + j]);
        }
    }
}

//캐릭터 조종 함수
void controlcharacter(plane* pPlane) {
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        if (pPlane->y > 1)
            pPlane->y--;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        if (pPlane->y < Map_Y_MAX - 5)
            pPlane->y++;
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if (pPlane->x > 1)
            pPlane->x -= 2;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (pPlane->x < Map_X_MAX - 3)
            pPlane->x += 2;
    }
}
//게임시작메뉴
void gamestart() {
    FillMap(mapData, ' ', Max_value);
    textcolor(11);
    DrawSprite(25, 4, 30, 7, "■■■■■■■■■■■■■■■■■□□□□■■■□□□□■■■■□■■■■■□■■■■■■■■□□□■■■□■□□□■■■■□■■■■■□■■□□■■■■□■■■■■■□□■□■■■■■■■■■■■■■■■■■");
    //점수
    DrawSprite(36, 16, 11, 1, "1. 게임시작");
    DrawSprite(36, 21, 7, 1, "2. 랭킹");
    DrawSprite(36, 26, 7, 1, "3. 종료");
    if (getch() == '1') {
        gameStatus = 1;
    }
    else if (getch() == '2') {
        gameStatus = 4;
    }
    else if (getch() == '3') {
        textcolor(11);
        exit(1);
    }
}
//게임오버 화면
void gameover() { 
    FillMap(mapData, ' ', Max_value);
    textcolor(12);
    DrawSprite(35, 5, 12, 1, "GAME OVER !!");
    //점수
    DrawSprite(36, 16, 9, 1, "1. 재도전");
    DrawSprite(36, 21, 7, 1, "2. 랭킹");
    DrawSprite(36, 26, 7, 1, "3. 종료");
    if (getch()=='1') {
        gamereset(&Enemy_Bullet, &Enemy_info, &Bullet_info, &myPlane);
        gameStatus = 1;
    }
    else if (getch() == '2') {
        gameStatus = 4;
    }
    else if (getch() == '3') {
        textcolor(11);
        exit(1);
    }
}

void gameclear() {
    FillMap(mapData, ' ', Max_value);
    textcolor(6);
    DrawSprite(24, 10, 34, 19, "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■□□□□□□□□□□□□□■■■■□■■□□□□□□□■■□■■■■□■■□□□□□□□■■□■■■■■□□□□□□□□□□□■■■■■■■■□□□□□□□■■■■■■■■■■■□□□□□■■■■■■■■■■■■■□□□■■■■■■■■■■■■■■■□■■■■■■■■■■■■■■■■□■■■■■■■■■■■■■■■■□■■■■■■■■■■■■■■■■□■■■■■■■■■■■■■■□□□□□■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
    DrawSprite(36, 35, 9, 1, "VICTORY!!");
}

void Bullet_Shoot(Bullet* pBullet, plane* pPlane, Bullet* Enemy) {

    //총알 이동
    for (int i = 0; i < MAXBULLET; i++) {
        if (pBullet[i].Active) {
            pBullet[i].y--;
            mapData[pBullet[i].y * Map_X_MAX + pBullet[i].x] = '*';

            if (pBullet[i].y < 1) {
                pBullet[i].Active = false;
            }
            
            //총알 적 충돌
            for (int j = 0; j < 10; j++) {
                if (Enemy[j].Active && Enemy[j].x == pBullet[i].x && Enemy[j].y == pBullet[i].y) {
                    Enemy[j].Active = false; //적 상태 비활성화
                    pBullet[i].Active = false; //총알 상태 비활성화
                    score += 1000;
                }
            }
            
        }
    }
    int a = pPlane->x;
    int b = pPlane->y;

    //총알 발사
    if (GetAsyncKeyState(VK_SPACE)) {
        if (b_index >= 9) b_index = 0;
        pBullet[b_index].x = a;
        pBullet[b_index].y = b;
        pBullet[b_index].Active = true;
        b_index++;
    }

    if (GetAsyncKeyState(VK_CONTROL)) {
        for (int i = 0; i < 10; i++) {
            pBullet[b_index].Active = FALSE;
        }
        b_index = 0;
    }
}

void Enemy_Create(Enemy* Enemy, int count) {
    int a = rand() % 50 + 10;
    int b = rand() % 7 + 5;
    if(count%200==0){
        Enemy[i_index].x = a;
        Enemy[i_index].y = b;
        Enemy[i_index].Active = TRUE;
        i_index++;
        if (Enemy[10].Active == TRUE) {
            return gameStatus = 3;
        }
    }
    for(int i=0; i<11; i++){
        if (Enemy[i].Active)
               mapData[Enemy[i].y * Map_X_MAX + Enemy[i].x - 1] = '@';
    }
}

void Enemy_Move(Enemy* Enemy) {

    for (int i = 0; i < 11; i++) {
        if (Enemy[i].Active == TRUE) {
            if (Enemy[i].x < 2)Enemy[i].x++;
            else if (Enemy[i].x > 78)Enemy[i].x--;
            else if (Enemy[i].y < 2)Enemy[i].y++;
            else if (Enemy[i].y > 17)Enemy[i].y--;
            else {
                if (rand() % 4 == 0) {
                    Enemy[i].x++;
                }
                else if (rand() % 4 == 1) {
                    Enemy[i].x--;
                }
                else if (rand() % 4 == 2) {
                    Enemy[i].y++;
                }
                else if (rand() % 4 == 3) {
                    Enemy[i].y--;
                }
            }
        }
    }
}

void Enemy_Attack(Enemy* enemy, Bullet* Enemy_Bullet, plane* Plane) {

    //적 총알 이동
    for (int i = 0; i < MAXBULLET*100; i++) {
         if (Enemy_Bullet[i].Active) {
               Enemy_Bullet[i].y++;
               mapData[Enemy_Bullet[i].y * Map_X_MAX + Enemy_Bullet[i].x - 1] = '!';

               if (Enemy_Bullet[i].y >= 35) {
                   Enemy_Bullet[i].Active = false;
               }

               //적 총알과 캐릭터의 충돌 확인
               if (Plane->x == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) {
                   gameStatus = 2;
               }
               if (Plane->x + 1 == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) {
                   gameStatus = 2;
               }
               if (Plane->x + 2 == Enemy_Bullet[i].x && Plane->y == Enemy_Bullet[i].y) {
                  gameStatus = 2;
               }
         }
    }

    if (rand() % 10 != 0)return; //발사될 확률
    if (eb_index == MAXBULLET*100 - 1)eb_index = 0; //총알 초기화
    //적 총알 발사
    for (int i = 0; i < MAXBULLET; i++) {
        if (enemy[i].Active) {
            Enemy_Bullet[eb_index].x = enemy[i].x;
            Enemy_Bullet[eb_index].y = enemy[i].y + 1;
            Enemy_Bullet[eb_index].Active = true;
            eb_index++;
        }
    }
}

void gamereset(Bullet* Enemy_Bullet, Enemy* Enemy, Bullet* pBullet, plane* pPlane) {
    i_index = 0;
    b_index = 0;
    eb_index = 0;
    score = 0;
    pPlane->x = Map_X_MAX / 2 + 1;
    pPlane->y = Map_Y_MAX / 3 * 2 + 1;
    for (int i = 0; i < 10; i++) {
        Enemy[i].Active = FALSE;
        pBullet[i].Active = FALSE;
        for (int j = 0; j < 1000; j++) {
            Enemy_Bullet[j * 10 + i].Active = FALSE;
        }
    }
}


//게임 UI
void ShowOption() {
    int score_num = score;
    DrawSprite(1, 39, 7, 1, "총알 : ");
    for (int i = 0; i <= 10; i++) {
        if (b_index - 1 >= i)
            EditMap(10 + i, 39, 'X');
        else 
            EditMap(10 + i, 39, 'O');
    }

    DrawSprite(60, 37, 18, 1, "조작 : ↑,↓,←,→");
    DrawSprite(60, 38, 12, 1, "공격 : Space");
    DrawSprite(60, 39, 13, 1, "재장전 : Ctrl");
    DrawSprite(30, 1, 8, 1, "SCORE : ");
    for (int j = 0; j <= 10; j++) {
        char str = score_num % 10 + 48; //일의자리 숫자부터 아스키코드값으로 가져옴
        EditMap(48 - j, 1, str);
        score_num = score_num / 10;
    }
}
//랭크작성
void rank_input(void) {
    char player_name[50];

    printf("게임오버! 이름을 입력해주세요(10자이내): ");
    scanf("%s", player_name);

    // 메모장 열기
    FILE* file = fopen("Rank_List.txt", "a");
    if (file == NULL) {
        printf("Error opening file!");
        return;
    }

    // 입력받은 이름과 게임 점수 입력하기
    fprintf(file, "%s\t", player_name);
    fprintf(file, "%d\n", score);

    fclose(file);
}
//랭크 보여주는 장면(콘솔)
void RankingShow(void) {
    system("mode con: cols=50 lines=30");
    system("title 랭킹 확인");

    FILE* fp = fopen("Rank_List.txt", "rt");
    if (fp == NULL) {
        MessageBox(NULL, TEXT("The file does not exist."), NULL, NULL);
        return;
    }

    LINKEDLIST* lnk = (LINKEDLIST*)malloc(sizeof(LINKEDLIST));
    lnk->cur = NULL;
    lnk->head = NULL;
    lnk->tail = NULL;

    char tempName[50];
    int tempScore;
    while (fscanf(fp, "%s %d", tempName, &tempScore) == 2) {
        insertData(lnk, tempName, tempScore);
    }

    fclose(fp);

    printScore(lnk);

    while (lnk->head != NULL) {
        deleteLastNode(lnk);
    }

    Sleep(1000);

    return;
}
//콘솔에 내림차순으로 print하기
void printScore(LINKEDLIST* lnk) {
    int cnt = 0;
    RNKINFO* cur;
    cur = lnk->head;

    while (cur != NULL) {
        cnt++;
        cur = cur->next;
    }

    RNKINFO** ptr = (RNKINFO**)malloc(sizeof(RNKINFO*) * cnt);
    int i, j;
    RNKINFO* temp;

    for (i = 0, cur = lnk->head; i < cnt; i++) {
        ptr[i] = cur;
        if (cur == NULL)
            break;
        cur = cur->next;
    }

    for (i = 0; i < cnt - 1; i++) {
        for (j = i + 1; j < cnt; j++) {
            if (ptr[i]->score <= ptr[j]->score) {
                temp = ptr[i];
                ptr[i] = ptr[j];
                ptr[j] = temp;
            }
        }
    }

    for (i = 0; i < cnt; i++) {
        printf("%d. Name: %s \t\t Score: %d\n", i + 1, ptr[i]->name, ptr[i]->score);
    }

    free(ptr);
}
//메모장 데이터를 노드에 입력하기
void insertData(LINKEDLIST* lnk, char* name, int score) {
    RNKINFO* newRNK = (RNKINFO*)malloc(sizeof(RNKINFO));
    newRNK->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(newRNK->name, name);
    newRNK->score = score;
    newRNK->next = NULL;

    if (lnk->head == NULL && lnk->tail == NULL) {
        lnk->head = lnk->tail = newRNK;
    }
    else {
        lnk->tail->next = newRNK;
        lnk->tail = newRNK;
    }
    return;
}

// 마지막 노드 지우기
void deleteLastNode(LINKEDLIST* lnk) {
    if (lnk->head == NULL) {
        return;  // No nodes to delete
    }
    else if (lnk->head == lnk->tail) {
        // Only one node in the list
        free(lnk->head->name);
        free(lnk->head);
        lnk->head = lnk->tail = NULL;
    }
    else {
        RNKINFO* cur = lnk->head;
        while (cur->next != lnk->tail) {
            cur = cur->next;
        }
        cur->next = NULL;
        free(lnk->tail->name);
        free(lnk->tail);
        lnk->tail = cur;
    }
    return;
}
