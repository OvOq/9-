#define _CRT_SECURE_NO_WARNINGS    
#include<windows.h> 
#include<math.h>
#include<cstring>
#include<cstdio>
#include<cstdlib> 
#include<iostream>
#include<string> 
#include<time.h>
#define can -2
#define cannot -3
using namespace std;

#define PI 3.1415926
#define SX 8
#define SY 16
#define DX PI / SX
#define DY PI * 2 / SY
#define X(a, b) (cx + v[a][b].x * r), (cy + v[a][b].y * r)
typedef struct { double x, y; } Vec;

int blackx; // 黑棋落子坐标x 
int blacky; // 黑棋落子坐标y 
int opp; // dfs对方棋子
int curplayer;
int Alive_judgemnet;
int map[9][9]; //0黑 1白
int steps[9][9]; // 记录dfs足迹
int badplace[9][9]; // 对于黑棋方来说的坏子位置,1-在这里必死，0-安全
int dis[9][9];

void InitGame(); // 游戏初始化
void Beginner();
void Calc(double ,double, double, Vec*);
void NewGame(); // 新游戏
void PrintfMap(); // 游戏地图
void Putchess(); // 落子函数
void WeWin();
void dfs(int, int, int);
int KO(int  ,int ,int);
int Suicide(int, int ,int);
int JudgeTime();
int GetBlack(int player, int x, int y); 

int main() {
	srand((unsigned)time(NULL));
	InitGame();
	return 0;
}
void Calc(double i, double j, double rot, Vec* v) {
	double x = sin(i) * cos(j), y = sin(i) * sin(j), z = cos(i),
		s = sin(rot), c = cos(rot), c1 = 1 - c, u = 1 / sqrt(3), u2 = u * u;
	v->x = x * (c + u2 * c1) + y * (u2 * c1 - u * s) + z * (u2 * c1 + u * s);
	v->y = x * (u2 * c1 + u * s) + y * (c + u2 * c1) + z * (u2 * c1 - u * s);
}
void Beginner() {
	HWND hwnd = GetConsoleWindow(); HDC hdc1 = GetDC(hwnd);
	double rot = 0;
	double t; // 运行时间
	clock_t start, end;
	start = clock();
	while (true) {
		end = clock();
		RECT rect; GetClientRect(hwnd, &rect); int w = rect.right, h = rect.bottom, cx = w / 2, cy = h / 2, r = h * 0.375;
		HDC hdc2 = CreateCompatibleDC(hdc1); HBITMAP bmp = CreateCompatibleBitmap(hdc1, w, h); SelectObject(hdc2, bmp);
		SelectObject(hdc2, GetStockObject(WHITE_PEN));
		Vec v[SX + 1][SY + 1];
		for (int i = 0; i <= SX; ++i) for (int j = 0; j <= SY; ++j) Calc(i * DX, j * DY, rot, &v[i][j]);
		for (int i = 0; i < SX; ++i) for (int j = 0; j < SY; ++j) {
			MoveToEx(hdc2, X(i, j), NULL); LineTo(hdc2, X(i + 1, j));
			MoveToEx(hdc2, X(i, j), NULL); LineTo(hdc2, X(i, j + 1));
		}
		BitBlt(hdc1, 0, 0, w, h, hdc2, 0, 0, SRCCOPY); DeleteObject(bmp); DeleteDC(hdc2);
		rot += 0.01; Sleep(5);
		t = (double)(end - start) / CLOCKS_PER_SEC;
		if (t >= 4.2) break;
	}
}
void InitGame() {
	system("title     Simple 9-way Go");
	int beginner;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	cout << "welcome to Simple 9-way Go" << endl;
	system("pause");
	reinput:
	system("cls");
	cout << "Now please enter the number to choose next step:" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 0xf);
	cout << "1 - Start the game directly" << endl;
	cout << "2 - Game operation instructions" << endl;
	cout << "3 - Stop and exit" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	cout << "your choice:";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 0xf);
	cin >> beginner;
	Beginner();

	switch (beginner) // (Ｔ▽Ｔ)
	{
		case 3: exit(0); // 退出
		case 1: NewGame(); break; // 直接开始
		case 2: { // 操作说明，开始游戏
			system("cls");
			cout << "1.This is a simple 9-way go, whose initialization settings means the computer will always be the first(black)" << endl;
			cout << "2.and other rules are the same" << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << "3.The player determines the position of the drop by entering coordinates, for example, You can type 4, 3 to indicate \n that you want to place the pieces in 4 rows and 3 columns. as the picture shows." << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 0xf);
			memset(map, -1, sizeof(map));
			map[3][2] = 1;
			PrintfMap();
			cout << "white choose: 4,3" << endl;
			cout << endl;
			cout << "4.the numbers of rows and columns are all from 0 to 8" << endl;
			cout << "5.the Input method should be set to Englih input method mode" << endl;
			cout << endl;
			cout << "Now please press any key to begin the game" << endl;
			system("pause");
			NewGame();
			break;
		}
		default: { // 输入错误
			system("cls");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
			cout << "Input error, silly goose" << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xf);
			system("pause");
			goto reinput;
			break;
		}
	}
}
void PrintfMap() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j] == 0) { cout << "○"; continue; }
			if (map[i][j] == 1) { cout << "●"; continue; }
			else {
				if (i == 0 && j == 8) { cout << "┐"; continue; }
				if (i == 0 && j == 0) { cout << "┌ "; continue; }
				if (i == 8 && j == 8) { cout << "┘"; continue; }
				if (i == 8 && j == 0) { cout << "└ "; continue; }
				if (i == 0 && j < 8) { cout << "┬ "; continue; }
				if (i == 8 && j < 8) { cout << "┴ "; continue; }
				if (i < 8 && j == 8) { cout << "┤"; continue; }
				if (i < 8 && j == 0) { cout << "├ "; continue; }
				cout << "┼ ";
			}
		}
		cout << endl;
	}
	cout << endl;
}

void NewGame() {
	int nowplayer = 0; // 当前需要走棋的玩家，0-黑棋，1-白棋
	int whitex; // 白棋落子坐标x
	int whitey; // 白棋落子坐标y
	memset(map, -1, sizeof(map));
	curplayer = 0;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xf);

	while (TRUE) {
		system("cls");
		switch (nowplayer)
		{
			case 0: { // 黑棋
				curplayer = nowplayer;
				cout << "Now it’s Black’s turn to play chess." << endl;
				PrintfMap();
				Putchess();
				map[blackx][blacky] = 0;
				Sleep(600);
				system("cls");
				cout << "Now it’s Black’s turn to play chess." << endl;
				PrintfMap();
				printf("balck chose: %d,%d", blackx, blacky);
				if (JudgeTime() == 1) {	WeWin();}
				system("pause");
				break;
				}
			case 1: { // 白棋
				choose2:
				cout << "Now it’s White’s turn to play chess." << endl;
				PrintfMap();
				cout << "white choose: ";
				scanf("%d,%d", &whitex, &whitey);
				system("cls");
				if (map[whitex][whitey] == cannot) map[whitex][whitey] = -1;
				if (whitex > 8 || whitex < 0 || whitey > 8 || whitey < 0 || map[whitex][whitey] != -1) {
					printf("map:%d\n", map[8][6]);
					system("pause");
					goto choose2;
				}
				cout << "Now it’s White’s turn to play chess." << endl;
				map[whitex][whitey] = 1;
				PrintfMap();
				printf("white chose: %d,%d", whitex, whitey);
				if (JudgeTime() == 1) { WeWin();}
				break;
			}
			default: { 
				break;
			}
		}
		nowplayer = (nowplayer + 1) % 2;
		//system("pause");
	}
	system("cls");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xf);
	cout << "out of game" << endl;
	system("pause");
}
void Putchess() { // 落子函数，因为棋盘较小，所以遍历每一个没有棋子的点，对该点进行α-β搜索
	int x; // 当前横坐标
	int y; // 当前纵坐标
	int leftplace = 0; // 剩余可走的位置数
	memset(badplace, 0, sizeof(badplace));

	choose:
	x = rand() % 9;
	y = rand() % 9;
	if (map[x][y] != -1) goto choose;
	map[x][y] = 1;
	if (KO(x, y, 0)) {
		blackx = x; blacky = y;
		map[x][y] = -1;
		return;
	}
	if (Suicide(x, y, 0)) {
		badplace[x][y] = 1;
		map[x][y] = -1;
		goto choose;
	}
	if (map[x][y] != -1 || (badplace[x][y] == 1 && map[x][y] == -1)) goto choose;
}
int Suicide(int x, int y, int a) {
	memset(steps, 0, sizeof(steps));
	Alive_judgemnet = 0;
	dfs(x, y, a);

	return Alive_judgemnet == 1 ? 0 : 1;
}
int KO(int x, int y, int player) {
	int count = 0;//周围棋子死亡个数 
	int turns[4][2] = { -1, 0, 0, 1, 1, 0, 0, -1 };
	// oringinal: 
	opp = curplayer == 1 ? 0 : 1;
	//opp = 1;
	for (int i = 0; i < 4; i++) {
		Alive_judgemnet = 0;
		if (map[x + turns[i][0]][y + turns[i][1]] == opp) {
			dfs(x + turns[i][0], y + turns[i][1], opp);// dfs有改变map值的bug！~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~done
			if (!Alive_judgemnet) count++;
		}
	}
	return count != 0 ? 1 : 0;
}
void dfs(int x, int y, int a) {
	steps[x][y] = cannot;
	if (x < 0 || x > 8 || y < 0 || y > 8) {
		return;//撞墙
	}
	if (map[x][y] == a) { //周围棋子是己方,分别对己方进行深搜 
		if (steps[x - 1][y] == cannot) dfs(x - 1, y, a);
		if (steps[x][y - 1] == cannot) dfs(x, y - 1, a);
		if (steps[x + 1][y] == cannot) dfs(x + 1, y, a);
		if (steps[x][y + 1] == cannot) dfs(x, y + 1, a);
		return;
	}
	if (map[x][y] == opp) {
		return;
	}
	if (map[x][y] == -1) {
		Alive_judgemnet = 1;
		return;
	}
}
int GetBlack(int player, int x, int y) {//输赢端
	int op = player == 0 ? 1 : 0;
	if (x < 0 || x>8 || y < 0 || y>8 || dis[x][y] == 1 || map[x][y] == op || map[x][y] == -1) return -1;// 没全围上
	if (map[x][y] == player) return 1;
	dis[x][y] = 1;//标记走过
	GetBlack(player, x + 1, y);
	GetBlack(player, x - 1, y);
	GetBlack(player, x, y + 1);
	GetBlack(player, x, y - 1);
	dis[x][y] = 0;//撤销
}
int JudgeTime() {//所剩空格，都被包围了
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j] == -1) {//空格
				if (GetBlack(0, i, j) == -1 || GetBlack(1, i, j) == -1) { return -1; }//不是终局
			}
		}
	}
	return 1;
}
void WeWin() {
	int white_num = 0, black_num = 0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {//黑白棋所存的活子
			if (map[i][j] == 0) { white_num++; continue; }
			if (map[i][j] == 1) { black_num++; continue; }
			else {//以及包围的空格
				if (GetBlack(0, i, j) == 1) { white_num++; continue; }
				if (GetBlack(1, i, j) == 1) { black_num++; continue; }
			}
		}
	}
	if (black_num > white_num) { cout << "Aha! You Are a Losser!!!" << endl; }
	if (black_num < white_num) { cout << "Oh! You Win!!!" << endl; }
}
