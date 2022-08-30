#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <conio.h>
#include "NPC1.h"
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
/*size of gameboard*/
#define GBOARD_WIDTH 50//커질수록 어려워짐=    =3 =3 
#define GBOARD_HEIGHT 36///커질수록 쉬워짐
/*starting point of gameboard*/
#define GBOARD_ORIGIN_X 4
#define GBOARD_ORIGIN_Y 2
#define UP 72   //위 방향키
#define DOWN 80   //아래 방향키
#define spaceKey 32
#define EnterKey 13//시작 엔터키
typedef struct PC
{
	int curPosX;//커서 x좌표
	int curPosY;//커서 y좌표
	int speed;
}PC;
typedef struct NPC
{
	int npc_id;//npc의 배열 번호
	//자동차:1, 기차:2, 비행기:3, 자전거:4, 산소:5
	int count;//더 맞춰야하는 횟수(1씩 줄다가 0이되면 NPC삭제)
	int curPosX;
	int curPosY;
	int speed;//npc의 속도
	int exist;
	int random; //11.26 박지윤(매연 랜덤생성위해서)
	int downcnt; //11.26 박지윤(바닥으로 내려갈때마다 횟수세기)
}NPC;
typedef struct Shot
{
	int exist;   //존재하면 1, 존재안하면 0
	int curPosX;
	int curPosY;
} Shot;
typedef struct Life {
	int curPosX;
	int curPosY;
}Life;
typedef struct Gas {
	int exist; //존재하면 1
	int curPosX;
	int curPosY;
}Gas;
//함수 원형
void setcolor(unsigned short text, unsigned short back);
void RemoveCursor(void);
void SetCurrentCursorPos(int x, int y);
COORD GetCurrentCursorPos(void);
void DrawGameBoard();
void ShowNpc(char blockInfo[6][20], NPC* npc);
void DeleteNpc(char blockInfo[6][20], NPC* npc);
void ShowLife(char blockInfo[1][2], Life* life);//김아인 수정
void ShowShot(char blockInfo[1][1], Shot* shot);//김아인
void ShowGas(char blockInfo[2][2], Gas* gas);  //11.25 수정 박지윤
void ShowPc(PC* pc);//김아인
int DetectGroundCollision(int PosX, int PosY, char npcModel[6][20], NPC* npc);
void NpcDown(NPC* npc); // 11.26수정 김원희
int IsGameOver();
void DeleteLife(char blockInfo[1][2], Life* life);
void DeleteShot(char blockInfo[1][1], Shot* shot);
void creationShot(Shot* shot, PC* pc);
void ShotLeft(Shot* shot);
void DeleteGas(char blockInfo[1][3], Gas* gas);//11.25수정 박지윤
void GasRight(Gas* gas); //11.25수정 박지윤
int DetectShotCollision(int posX, int posY, char blockModel[1][1]);
int DetectGasCollision(int posX, int posY, char blockModel[1][3], Gas* gas);//11.27수정 박지윤
int DetectCollisionPc(int posX, int posY);
void DeletePc(PC* pc);
int PcDown(PC* pc);
int PcUp(PC* pc);
void ProcessKeyInput(PC* pc, Shot* shot); //김아인
void initPc(PC* pc); //김아인
void InitNpc(NPC* npc); // 11.20수정 김원희//11.26수정 김혜율 //11.27수정 박지윤
void InitItem(NPC* item); // 11.26  김원희//11.26수정 김혜율
void initShot(Shot* shot, PC* pc);
void initLife(Life* life);
void initGas(Gas* gas); //  11.27수정 박지윤
void SpeedFunc(PC* pc, NPC* npc);
void AddShotToBoard(Shot* shot);
void DeleteShotToBoard(Shot* shot);
void AddPCToBoard();//11.20 김아인
void DeletePCToBoard();//11.20 김아인
void AddNpcToBoard(NPC* npc); // 11.22 김원희 //11.26수정 김혜율
void DeleteNpcToBoard(NPC* npc); // 11.22 김원희 //11.26수정 김혜율
void DeleteGasToBoard(Gas* gas); //  11.25수정 박지윤
void AddGasToBoard(Gas* gas);//  11.27수정 박지윤
void GameBoardInfoUpdate();//11.22 김혜율
void printInformation();//11.22 김혜율
int DetectNpc_ShotCollision(NPC* npc);//11.23 김아인// 11.26수정 김원희
int DetectNpcCollision(int PosX, int PosY, char npcModel[6][20], NPC* npc);//11.23 김아인
void creationGas(Gas* gas, NPC* npc); //11.27 박지윤
void startScreen();//11.28 김혜율
void stageScreen();//11.28 김혜율
void blackScreen();//11.28 김혜율
void gameClearScreen();//11.28 김혜율
void gameOverScreen();//11.28 김혜율
void initializeStage(NPC* npc, NPC* item);//11.28 김혜율

//전역변수
int  score;
int npc_PosX, npc_PosY;//임시 전역변수
int gameBoardInfo[GBOARD_HEIGHT + 1][GBOARD_WIDTH + 2];
int shotMax = 5000;
int life_count = 5;//생명의 수 5개로 시작
int npcMax = 10;
int gasMax = 10;
int stage;
int lastNpcPosX;//11.26 김혜율
int gauge = 0; //11.27 박지윤
int shot_speed;//11.27 김아인
int speed_cnt;   //11.27김아인
int gameover;//11.28 김혜율
NPC npc[10];   //배열 인덱스바꿀때 npcMax변수도 바꾸기
Life life[5];
PC pc;
Shot shot[5000];   //배열인덱스바꿀때 shotMax변수도 바꾸기
Gas gas[10]; //11.24 박지윤
NPC item[10]; // 11.26 김원희
main(void)
{
	PlaySound(TEXT("gamestart.wav"), NULL, SND_FILENAME | SND_ASYNC);

	RemoveCursor();
	srand(time(NULL));
	system("mode con: cols=120 lines=40");//콘솔창 크기 정하기
	int i = 0;
	int cnt;
	speed_cnt = 0;   //11.27김아인
	score = 0;
	int speed = 50;
	stage = 1;
	gameover = 0;

	startScreen();
	PlaySound(NULL, 0, 0);
	blackScreen();
	PlaySound(TEXT("stageclear.wav"), NULL, SND_FILENAME | SND_ASYNC);
	stageScreen();
	blackScreen();
	PlaySound(TEXT("gamesound.wav"), NULL, SND_FILENAME | SND_ASYNC);
	//게임판 그리기
	DrawGameBoard();
	shot_speed = 2; //11.27김아인
	initPc(&pc);//pc 초기 설정
	initShot(shot, &pc);
	initLife(life);
	initGas(gas);

	//1스테이지 목표점수 1000
	cnt = 0;
	while (1)
	{
		if (IsGameOver())
		{
			if (gameover == 0)
			{
				blackScreen();
				gameOverScreen();
				PlaySound(TEXT("dead.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			gameover = 1;
			break;
		}

		if (score >= 1000)
		{
			PlaySound(TEXT("stageclear.wav"), NULL, SND_FILENAME | SND_ASYNC);
			stage = 2;
			blackScreen();
			break;
		}

		if (cnt % 100 == 0)
		{
			InitNpc(npc);
		}
		if (cnt % 550 == 0)
		{
			InitItem(item);
		}
		if (cnt % 20 == 0)
		{
			GasRight(gas);
		}
		if (cnt % 30 == 0)
		{
			NpcDown(npc);
			NpcDown(item);
		}
		if (cnt % shot_speed == 0)
		{
			if (shot_speed == 5)   //11.27김아인 수정
			{
				speed_cnt += 5;
				if (speed_cnt >= 300)   //3초뒤에
					shot_speed = 2;   //speed다시 2로 초기화
			}
			ShotLeft(shot);
		}

		if (cnt % 1 == 0)
		{
			ProcessKeyInput(&pc, shot);
		}

		if (gauge == 3 && life_count < 5) {//11.28 김혜율 수정
			life_count++;
			ShowLife(lifeModel, &life[5 - life_count]);//뒤에서부터 채워지게
			gauge = 0;
		}

		printInformation();
		Sleep(10);
		cnt++;
	}
	//2스테이지 목표점수 5000
	cnt = 0;
	if (IsGameOver() == 0)
	{
		stageScreen(); blackScreen(); DrawGameBoard(); initializeStage(npc, item);
		PlaySound(TEXT("gamesound.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	while (1)
	{
		if (IsGameOver())
		{
			if (gameover == 0)
			{
				blackScreen();
				gameOverScreen();
				PlaySound(TEXT("dead.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			gameover = 1;
			break;
		}

		if (score >= 5000)
		{
			PlaySound(TEXT("stageclear.wav"), NULL, SND_FILENAME | SND_ASYNC);
			stage = 3;
			blackScreen();
			break;
		}

		if (cnt % 150 == 0)
		{
			InitNpc(npc);
		}
		if (cnt % 550 == 0)
		{
			InitItem(item);
		}
		if (cnt % 20 == 0)
		{
			GasRight(gas);

		}
		if (cnt % 30 == 0)
		{
			NpcDown(npc);
			NpcDown(item);
		}
		if (cnt % shot_speed == 0)
		{
			if (shot_speed == 5)   //11.27김아인 수정
			{
				speed_cnt += 5;
				if (speed_cnt >= 300)   //3초뒤에
					shot_speed = 2;   //speed다시 2로 초기화
			}
			ShotLeft(shot);
		}

		if (cnt % 1 == 0)
		{
			ProcessKeyInput(&pc, shot);

		}
		if (gauge == 3 && life_count < 5) {//11.28 김혜율 수정
			life_count++;
			ShowLife(lifeModel, &life[5 - life_count]);//뒤에서부터 채워지게
			gauge = 0;
		}
		printInformation();
		Sleep(10);
		cnt++;
	}
	//3스테이지 목표점수 11000
	cnt = 0;
	if (IsGameOver() == 0)
	{
		stageScreen(); blackScreen(); DrawGameBoard(); initializeStage(npc, item);
		PlaySound(TEXT("gamesound.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	while (1)
	{
		if (IsGameOver())
		{
			if (gameover == 0)
			{
				blackScreen();
				PlaySound(TEXT("dead.wav"), NULL, SND_FILENAME | SND_ASYNC);
				gameOverScreen();
			}
			gameover = 1;
			break;
		}

		if (score >= 11000)
		{
			PlaySound(TEXT("stageclear.wav"), NULL, SND_FILENAME | SND_ASYNC);
			stage = 4;
			blackScreen();
			break;
		}

		if (cnt % 200 == 0)
		{
			InitNpc(npc);
		}
		if (cnt % 550 == 0)
		{
			InitItem(item);
		}
		if (cnt % 20 == 0)
		{
			GasRight(gas);

		}
		if (cnt % 30 == 0)
		{
			NpcDown(npc);
			NpcDown(item);
		}
		if (cnt % shot_speed == 0)
		{
			if (shot_speed == 5)   //11.27김아인 수정
			{
				speed_cnt += 5;
				if (speed_cnt >= 300)   //3초뒤에
					shot_speed = 2;   //speed다시 2로 초기화
			}
			ShotLeft(shot);
		}

		if (cnt % 1 == 0)
		{
			ProcessKeyInput(&pc, shot);
		}
		if (gauge == 3 && life_count < 5) {//11.28 김혜율 수정
			life_count++;
			ShowLife(lifeModel, &life[5 - life_count]);//뒤에서부터 채워지게
			gauge = 0;
		}
		printInformation();
		Sleep(10);
		cnt++;
	}
	//4스테이지 목표 20000
	cnt = 0;
	if (IsGameOver() == 0)
	{
		stageScreen(); blackScreen(); DrawGameBoard(); initializeStage(npc, item);
		PlaySound(TEXT("gamesound.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	while (1)
	{
		if (IsGameOver())
		{
			if (gameover == 0)
			{
				blackScreen();
				gameOverScreen();
				PlaySound(TEXT("dead.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			gameover = 1;
			break;
		}

		if (score >= 20000)
		{
			blackScreen();
			break;
		}

		if (cnt % 200 == 0)
		{
			InitNpc(npc);
		}
		if (cnt % 550 == 0)
		{
			InitItem(item);
		}
		if (cnt % 20 == 0)
		{
			GasRight(gas);

		}
		if (cnt % 20 == 0)
		{
			NpcDown(npc);
			NpcDown(item);
		}
		if (cnt % shot_speed == 0)
		{
			if (shot_speed == 5)   //11.27김아인 수정
			{
				speed_cnt += 5;
				if (speed_cnt >= 300)   //3초뒤에
					shot_speed = 2;   //speed다시 2로 초기화
			}
			ShotLeft(shot);
		}

		if (cnt % 1 == 0)
		{
			ProcessKeyInput(&pc, shot);

		}
		if (gauge == 3 && life_count < 5) {//11.28 김혜율 수정
			life_count++;
			ShowLife(lifeModel, &life[5 - life_count]);//뒤에서부터 채워지게
			gauge = 0;
		}
		printInformation();
		Sleep(10);
		cnt++;
	}
	if (score >= 20000)
	{
		while (1)
		{
			PlaySound(TEXT("stageclear.wav"), NULL, SND_FILENAME | SND_ASYNC);
			gameClearScreen();
		}
	}
	getchar();
}
void setcolor(unsigned short text, unsigned short back)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | (back << 4));
}
void SetCurrentCursorPos(int x, int y)
{
	COORD position = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}
COORD GetCurrentCursorPos(void)
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}
void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}
void GameBoardInfoUpdate()
{
	int x, y;
	for (y = 0; y < GBOARD_HEIGHT; y++)
	{
		gameBoardInfo[y][0] = 1;//왼쪽 선
		gameBoardInfo[y][GBOARD_WIDTH - 3] = 5;//사다리 충돌 못하게
	}
	for (x = 0; x < GBOARD_WIDTH + 2; x++)
	{
		gameBoardInfo[0][x] = 1;//하늘 라인
		gameBoardInfo[GBOARD_HEIGHT - 2][x] = 1;//초록색 땅 라인
	}
}
void printInformation()
{
	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 3);
	printf("STAGE:%d", stage);
	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 5);
	printf("SCORE:%d", score);
	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 7);
	printf("SPROUT:%d", life_count);
	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 9);
	printf("ITEM:%d", gauge);
}
void DrawGameBoard()
{
	int x, y;
	int curPosX, curPosY;
	int i;
	int initpos = 16;
	printInformation();

	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 12);
	printf("[MOVE KEY]");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 14);
	printf("UP:↑");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 16);
	printf("DOWN:↓");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 19);
	printf("[SHOOT]");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 2) * 2, 21);
	printf("SPACE BAR");

	for (y = 0; y <= GBOARD_HEIGHT; y++)//왼쪽 라인
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
		{
			printf("└");
		}
		else if (y == 0)
		{
			printf("┌");
		}
		else
		{
			printf("│");
		}
	}
	for (y = 1; y < GBOARD_HEIGHT; y++)//사다리
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH - 2) * 2, GBOARD_ORIGIN_Y + y);
		setcolor(6, 0);//갈색
		printf("├─");
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH - 1) * 2, GBOARD_ORIGIN_Y + y);
		printf("──");
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH) * 2, GBOARD_ORIGIN_Y + y);
		printf("┨");
		setcolor(7, 0);//원래 색
	}
	for (y = 0; y <= GBOARD_HEIGHT; y++)//오론쪽 라인
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
		{
			printf("┘");
		}
		else if (y == 0)
		{
			printf("┐");
		}
		else
		{
			printf("│");
		}
	}
	for (x = 1; x < GBOARD_WIDTH + 1; x++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT - 1);
		setcolor(2, 2);//진한 초록색
		printf("■");
		setcolor(7, 0);//원래색
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
		printf("─");//아래 라인
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y);
		setcolor(7, 0);//원래 색
		printf("─");//위 라인
	}
	curPosX = GBOARD_ORIGIN_X + GBOARD_WIDTH;
	curPosY = 0;
	SetCurrentCursorPos(curPosX, curPosY);
	for (y = 0; y < GBOARD_HEIGHT; y++)
	{
		for (x = 0; x < GBOARD_WIDTH + 2; x++)
		{
			gameBoardInfo[y][x] = 0;//초기화
		}
	}
	for (y = 0; y < GBOARD_HEIGHT; y++)
	{
		gameBoardInfo[y][0] = 1;//왼쪽 선
		gameBoardInfo[y][GBOARD_WIDTH - 3] = 5;//사다리 충돌 못하게
	}
	for (x = 0; x < GBOARD_WIDTH + 2; x++)
	{
		gameBoardInfo[0][x] = 1;//하늘 라인
		gameBoardInfo[GBOARD_HEIGHT - 2][x] = 1;//초록색 땅 라인
	}
	initpos = 16;
	for (i = 0; i < 5; i++)//새싹 보이게
	{
		life[i].curPosX = initpos + 18 * i;
		life[i].curPosY = GBOARD_HEIGHT;
		ShowLife(lifeModel, &life[i]);
	}
	for (i = 0; i < 5 - life_count; i++)
	{
		DeleteLife(lifeModel, &life[i]);
	}
}
int IsGameOver()
{
	if (life_count == 0)
	{
		return 1;
	}
	return 0;
}
void ShowNpc(char blockInfo[6][20], NPC* npc)
{
	int y, x;

	if (npc->count == 3)//3번 더 맞춰야 하면 노란색
	{
		setcolor(6, 0);
	}
	else if (npc->count == 2)//2번 더 맞춰야 하면 파란색
	{
		setcolor(11, 0);
	}
	if (npc->npc_id == 4)
	{
		setcolor(12, 0);
	}
	else if (npc->npc_id == 6)
	{
		setcolor(10, 0);
	}

	for (y = 0; y < 6; y++)
	{
		for (x = 0; x < 20; x++)
		{
			//SetCurrentCursorPos(curPosX + x * 2, curPosY + y);
			SetCurrentCursorPos(npc->curPosX + x, npc->curPosY + y);
			//특수기호 때문에 칸을 반으로 쪼갬
			if (blockInfo[y][x] == 2) { printf("●"); }
			else if (blockInfo[y][x] == 3) { printf("＿"); }
			else if (blockInfo[y][x] == 4) { printf("￣"); }
			else if (blockInfo[y][x] == 5) { printf("/"); }
			else if (blockInfo[y][x] == 6) { printf("|"); }
			else if (blockInfo[y][x] == 7) { printf("＼"); }
			else if (blockInfo[y][x] == 8) { printf("┌"); }
			else if (blockInfo[y][x] == 9) { printf("┐"); }
			else if (blockInfo[y][x] == 10) { printf("└"); }
			else if (blockInfo[y][x] == 11) { printf("┘"); }
			else if (blockInfo[y][x] == 12 && npc->count == 1) { printf("│"); }
			else if (blockInfo[y][x] == 12 && npc->count == 2) { printf("││"); }
			else if (blockInfo[y][x] == 13) { printf("─"); }
			else if (blockInfo[y][x] == 14) { printf("┴"); }
			else if (blockInfo[y][x] == 15) { printf("┯"); }
			else if (blockInfo[y][x] == 16) { printf("▣"); }
			else if (blockInfo[y][x] == 22) { printf("O "); }
			else if (blockInfo[y][x] == 24) { printf("2 "); }
			else if (blockInfo[y][x] == 25) { printf("★"); }
		}
	}
	setcolor(7, 0);
	SetCurrentCursorPos(npc->curPosX, npc->curPosY);
}
void DeleteNpc(char blockInfo[6][20], NPC* npc)
{
	int y, x;
	for (y = 0; y < 6; y++)
	{
		for (x = 0; x < 20; x++)
		{
			SetCurrentCursorPos(npc->curPosX + x, npc->curPosY + y);

			if (blockInfo[y][x] != 0)
				printf("  ");
		}
	}
	SetCurrentCursorPos(npc->curPosX, npc->curPosY);
}
void ShowLife(char blockInfo[1][2], Life* life)
{
	int y, x;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 2; x++)
		{
			SetCurrentCursorPos(life->curPosX + x * 2, life->curPosY + y);
			if (blockInfo[y][x] == 20)
			{
				setcolor(2, 0); printf("♣"); setcolor(7, 0);
			}
		}
	}
	SetCurrentCursorPos(life->curPosX, life->curPosY);
}
void DeleteLife(char blockInfo[1][2], Life* life)
{
	int y, x;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 2; x++)
		{
			SetCurrentCursorPos(life->curPosX + x * 2, life->curPosY + y);
			if (blockInfo[y][x] == 20)
			{
				printf("  ");
			}
		}
	}
	SetCurrentCursorPos(life->curPosX, life->curPosY);
}
void DeleteShotToBoard(Shot* shot)
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 1; x++)
		{
			/*Convert X,Y coordinate to array index*/
			arrCurX = (shot->curPosX - GBOARD_ORIGIN_X) / 2;
			arrCurY = shot->curPosY - GBOARD_ORIGIN_Y;

			if (shotModel[y][x] == 21)
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;   //delete:0으로 갱신
			}
		}
	}
}
void AddShotToBoard(Shot* shot)
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 1; x++)
		{
			/*Convert X,Y coordinate to array index*/
			arrCurX = (shot->curPosX - GBOARD_ORIGIN_X) / 2;
			arrCurY = shot->curPosY - GBOARD_ORIGIN_Y;

			if (shotModel[y][x] == 21)//2를 21로 바꿈!!!!!!
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 3;   //add: 3으로 갱신
			}
		}
	}
}
void DeleteGasToBoard(Gas* gas)
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 3; x++)
		{
			/*Convert X,Y coordinate to array index*/
			arrCurX = (gas->curPosX - GBOARD_ORIGIN_X) / 2;
			arrCurY = gas->curPosY - GBOARD_ORIGIN_Y;

			if (gasModel[y][x] != 0)
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;   //delete:0으로 갱신
			}
		}
	}
	GameBoardInfoUpdate();//게임보드의 테두리 다시 1로 채워주기
}
void AddGasToBoard(Gas* gas)
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 3; x++)
		{
			/*Convert X,Y coordinate to array index*/
			arrCurX = (gas->curPosX - GBOARD_ORIGIN_X) / 2;
			arrCurY = gas->curPosY - GBOARD_ORIGIN_Y;

			if (gasModel[y][x] != 0)
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 4;   //add: 4로 갱신
			}
		}
	}
	GameBoardInfoUpdate();//게임보드의 테두리 다시 1로 채워주기
}
void ShowShot(char blockInfo[1][1], Shot* shot)
{
	int y, x;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 1; x++)
		{
			SetCurrentCursorPos(shot->curPosX + x * 2, shot->curPosY + y);
			if (blockInfo[y][x] == 21)//2를 21로 바꿈!!!!!!
			{
				setcolor(14, 0); printf("■"); setcolor(7, 0);
			}
		}
	}
	SetCurrentCursorPos(shot->curPosX, shot->curPosY);

}
void DeleteShot(char blockInfo[1][1], Shot* shot)
{
	int y, x;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 1; x++)
		{
			SetCurrentCursorPos(shot->curPosX + x * 2, shot->curPosY + y);
			if (blockInfo[y][x] == 21)
			{
				printf("  ");
			}
		}
	}
	SetCurrentCursorPos(shot->curPosX, shot->curPosY);
}
void ShotLeft(Shot* shot) {
	for (int i = 0; i < shotMax; i++)
	{
		if (shot[i].exist == 1) {
			if (DetectShotCollision(shot[i].curPosX - 2, shot[i].curPosY, shotModel) == 2)
			{
				DetectNpc_ShotCollision(npc);
				DetectNpc_ShotCollision(item);
				DeleteShot(shotModel, &shot[i]);
				DeleteShotToBoard(&shot[i]);
				shot[i].exist = 0;
				continue;
			}
			else if (DetectShotCollision(shot[i].curPosX - 2, shot[i].curPosY, shotModel) == 0) {
				DeleteShot(shotModel, &shot[i]);
				DeleteShotToBoard(&shot[i]);
				shot[i].exist = 0;   //충돌할 경우 다시 0으로 초기화
				continue;// 충돌o=>0반환=>!0=참이면 ==>충돌하면 아랫줄 실행x
			}
			DeleteShot(shotModel, &shot[i]);
			DeleteShotToBoard(&shot[i]);
			shot[i].curPosX -= 2;
			SetCurrentCursorPos(shot[i].curPosX, shot[i].curPosY);
			AddShotToBoard(&shot[i]);
			ShowShot(shotModel, &shot[i]);

		}
	}
}
void ShowGas(char blockInfo[1][3], Gas* gas)
{
	int y, x;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 3; x++)
		{
			SetCurrentCursorPos(gas->curPosX + x * 2, gas->curPosY + y);
			setcolor(12, 0);
			if (blockInfo[y][x] == 22) { printf("O"); }
			else if (blockInfo[y][x] == 23) { printf("C"); }
			else if (blockInfo[y][x] == 24) { printf("2"); }
			setcolor(7, 0);
		}
	}
	SetCurrentCursorPos(gas->curPosX, gas->curPosY);
}
void DeleteGas(char blockInfo[1][3], Gas* gas)
{
	int y, x;
	for (y = 0; y < 1; y++)
	{
		for (x = 0; x < 3; x++)
		{
			SetCurrentCursorPos(gas->curPosX + x * 2, gas->curPosY + y);
			if (blockInfo[y][x] != 0) { printf("  "); }
		}
	}
	SetCurrentCursorPos(gas->curPosX, gas->curPosY);
}
void GasRight(Gas* gas) {
	for (int i = 0; i < gasMax; i++) {
		if (gas[i].exist == 1) {
			if (!DetectGasCollision(gas[i].curPosX + 2, gas[i].curPosY, gasModel, &gas[i])) {
				DeleteGas(gasModel, &gas[i]);
				DeleteGasToBoard(&gas[i]);
				gas[i].exist = 0;
				continue;// 
			}
			DeleteGas(gasModel, &gas[i]);
			DeleteGasToBoard(&gas[i]);
			gas[i].curPosX += 2;
			SetCurrentCursorPos(gas[i].curPosX, gas[i].curPosY);
			AddGasToBoard(&gas[i]);
			ShowGas(gasModel, &gas[i]);
		}
	}
}
int DetectShotCollision(int posX, int posY, char blockModel[1][1]) {//새로추가한 함수 , 콘솔창에 닿으면 사라져야함
	int x, y;
	int arrX = (posX - GBOARD_ORIGIN_X) / 2;   //평행이동!!!!!
	int arrY = posY - GBOARD_ORIGIN_Y;
	for (y = 0; y < 1; y++) {
		for (x = 0; x < 1; x++) {
			if (blockModel[y][x] == 21 && gameBoardInfo[arrY + y][arrX + x] == 2)//npc충돌 //2를 21로 바꿈!!!!!!
			{
				return 2;
			}
			if (blockModel[y][x] == 21 && gameBoardInfo[arrY + y][arrX + x] == 1)//2를 21로 바꿈!!!!!!
				return 0;   //벽 충돌
		}
	}

	return 1;   //충돌 x 
}
int DetectGasCollision(int posX, int posY, char blockModel[1][3], Gas* gas) {//새로추가한 함수 , 콘솔창에 닿으면 사라져야함
	int x, y;
	int arrX = (posX - GBOARD_ORIGIN_X) / 2;   //평행이동!!!!!
	int arrY = posY - GBOARD_ORIGIN_Y;

	for (y = 0; y < 1; y++) {
		for (x = 0; x < 3; x++) {
			if (blockModel[y][x] != 0 && gameBoardInfo[arrY + y][arrX + x] == 5)
			{
				return 0;   //사다리와 충돌 발생
			}
			else if (blockModel[y][x] != 0 && gameBoardInfo[arrY + y][arrX + x] == 1)
			{
				shot_speed = 5;   //11.27 김아인 수정
				speed_cnt = 0;
				return 0;   //피씨와 충돌 발생
			}
		}
	}
	return 1;   //충돌 x 
}
int DetectCollisionPc(int posX, int posY)//충돌검사 함수
{
	int x, y;
	int arrX = (posX - GBOARD_ORIGIN_X) / 2;
	int arrY = posY - GBOARD_ORIGIN_Y;

	for (y = 0; y < 3; y++)
	{
		for (x = 0; x < 2; x++)
		{
			if (pcModel[y][x] != 0 && gameBoardInfo[arrY + y][arrX + x] == 1)
				return 0;
		}
	}
	return 1;
}
void DeletePCToBoard()
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 3; y++)
	{
		for (x = 0; x < 2; x++)
		{
			/*Convert X,Y coordinate to array index*/
			arrCurX = (pc.curPosX - GBOARD_ORIGIN_X) / 2;
			arrCurY = pc.curPosY - GBOARD_ORIGIN_Y;

			if (pcModel[y][x] != 0)
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;   //delete: pc 0로 갱신
			}
		}
	}
	GameBoardInfoUpdate();//게임보드의 테두리 다시 1로 채워주기
}
void AddPCToBoard()
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 3; y++)
	{
		for (x = 0; x < 2; x++)
		{
			/*Convert X,Y coordinate to array index*/
			arrCurX = (pc.curPosX - GBOARD_ORIGIN_X) / 2;
			arrCurY = pc.curPosY - GBOARD_ORIGIN_Y;

			if (pcModel[y][x] != 0)
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 1;   //add: pc는 1로 갱신
			}
		}
	}
}
void AddNpcToBoard(NPC* npc) {
	int x, y, arrCurX, arrCurY;

	if (npc->npc_id == 1) {//자동차
		for (y = 0; y < 4; y++) {
			for (x = 0; x < 5; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 2;
			}
		}
	}
	else if (npc->npc_id == 2) {//기차
		for (y = 0; y < 4; y++) {
			for (x = 0; x < 9; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 2;
			}
		}
	}
	else if (npc->npc_id == 3) {//비행기
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 10; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 2;
			}
		}
	}
	else if (npc->npc_id == 5) {//자전거
		for (y = 0; y < 4; y++) {
			for (x = 0; x < 3; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 2;
			}
		}
	}
	else if (npc->npc_id == 6) {//산소
		for (y = 0; y < 1; y++) {
			for (x = 0; x < 2; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 2;
			}
		}
	}
	else if (npc->npc_id == 4) {//방해아이템
		for (y = 0; y < 2; y++) {
			for (x = 0; x < 2; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 2;
			}
		}
	}
}
void DeleteNpcToBoard(NPC* npc) {
	int x, y, arrCurX, arrCurY;

	if (npc->npc_id == 1) {//자동차
		for (y = 0; y < 4; y++) {
			for (x = 0; x < 5; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;
			}
		}
	}
	else if (npc->npc_id == 2) {//기차
		for (y = 0; y < 4; y++) {
			for (x = 0; x < 9; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;
			}
		}
	}
	else if (npc->npc_id == 3) {//비행기
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 10; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;
			}
		}
	}
	else if (npc->npc_id == 5) {//자전거
		for (y = 0; y < 5; y++) {
			for (x = 0; x < 3; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;
			}
		}
	}
	else if (npc->npc_id == 6) {//산소
		for (y = 0; y < 1; y++) {
			for (x = 0; x < 2; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;
			}
		}
	}
	else if (npc->npc_id == 4) {//방해아이템
		for (y = 0; y < 2; y++) {
			for (x = 0; x < 2; x++) {
				arrCurX = (npc->curPosX - GBOARD_ORIGIN_X) / 2;
				arrCurY = npc->curPosY - GBOARD_ORIGIN_Y;

				gameBoardInfo[arrCurY + y][arrCurX + x] = 0;
			}
		}
	}
	GameBoardInfoUpdate();//게임보드의 테두리 다시 1로 채워주기
}
void ShowPc(PC* pc)
{
	int y, x;
	for (y = 0; y < 3; y++)
	{
		for (x = 0; x < 2; x++)
		{
			SetCurrentCursorPos(pc->curPosX + x * 2, pc->curPosY + y);
			if (pcModel[y][x] == 2) { printf("●"); }
			else if (pcModel[y][x] == 17) { setcolor(8, 0); printf("ㄱ"); setcolor(7, 0); }
			else if (pcModel[y][x] == 18) { printf("┤"); }
			else if (pcModel[y][x] == 19) { printf("∧"); }
		}
	}
	SetCurrentCursorPos(pc->curPosX, pc->curPosY);
}
void DeletePc(PC* pc)
{
	int y, x;

	for (y = 0; y < 3; y++)
	{
		for (x = 0; x < 2; x++)
		{
			SetCurrentCursorPos(pc->curPosX + x * 2, pc->curPosY + y);

			if (pcModel[y][x] != 0) { printf("  "); }
		}
	}
	SetCurrentCursorPos(pc->curPosX, pc->curPosY);
}
int PcDown(PC* pc)//아래로 한칸이동
{
	DeletePCToBoard();
	if (!DetectCollisionPc(pc->curPosX, pc->curPosY + 1))
	{
		return 0;
	}
	DeletePc(pc);
	pc->curPosY += 1;
	SetCurrentCursorPos(pc->curPosX, pc->curPosY);
	AddPCToBoard();
	ShowPc(pc);

	return 1;
}
int PcUp(PC* pc)
{
	DeletePCToBoard();
	if (!DetectCollisionPc(pc->curPosX, pc->curPosY - 1))
	{
		return 0;
	}
	DeletePc(pc);
	pc->curPosY -= 1;
	SetCurrentCursorPos(pc->curPosX, pc->curPosY);
	AddPCToBoard();
	ShowPc(pc);

	return 1;
}
void ProcessKeyInput(PC* pc, Shot* shot)
{
	int i, key;
	for (i = 0; i < 20; i++)
	{
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case UP:
				PcUp(pc);
				break;
			case DOWN:
				PcDown(pc);
				break;
			case spaceKey:
				creationShot(shot, pc);
				break;
			}
		}
	}
}
void creationShot(Shot* shot, PC* pc)   //총알 시작위치생성
{
	int i;
	for (i = 0; i < shotMax; i++)
	{
		if (shot[i].exist == 0)   //존재하지 않는다면 해당배열에 생성
			break;
	}
	shot[i].curPosX = pc->curPosX - 2;
	shot[i].curPosY = pc->curPosY + 1;
	shot[i].exist = 1;
	AddShotToBoard(&shot[i]);
	ShowShot(shotModel, &shot[i]);
}
void creationGas(Gas* gas, NPC* npc)   //가스 시작위치생성
{
	int i;
	for (i = 0; i < gasMax; i++)
	{
		if (gas[i].exist == 0)
			break;
	}
	gas[i].curPosX = npc->curPosX + 20; //이때 npc는 비행기일때만 생성
	gas[i].curPosY = npc->curPosY + 3;
	gas[i].exist = 1;
	AddGasToBoard(&gas[i]);
	ShowGas(gasModel, &gas[i]);
}
void initLife(Life* life) {
	int i;
	int initpos = 16;
	for (i = 0; i < 5; i++)
	{
		life[i].curPosX = initpos + 18 * i;
		life[i].curPosY = GBOARD_HEIGHT;
		ShowLife(lifeModel, &life[i]);
	}
}
void initShot(Shot* shot, PC* pc)
{
	int i;
	for (i = 0; i < shotMax; i++)
	{
		shot[i].curPosX = 0;
		shot[i].curPosY = 0;
		shot[i].exist = 0;
	}
}
void initializeStage(NPC* npc, NPC* item)
{
	int i;
	initPc(&pc);//pc 초기 설정
	initShot(shot, &pc);
	initGas(gas);
	for (i = 0; i < npcMax; i++)
	{
		npc[i].curPosX = 0;
		npc[i].curPosY = 0;
		npc[i].exist = 0;
		npc[i].random = 0;
		npc[i].downcnt = 0;
		npc[i].npc_id = 0;
		npc[i].count = 0;
		npc[i].speed = 0;
	}
	for (i = 0; i < 10; i++)
	{
		item[i].curPosX = 0;
		item[i].curPosY = 0;
		item[i].exist = 0;
		item[i].random = 0;
		item[i].downcnt = 0;
		item[i].npc_id = 0;
		item[i].count = 0;
		item[i].speed = 0;
	}
}
void initPc(PC* pc) {
	pc->curPosX = GBOARD_ORIGIN_X + (GBOARD_WIDTH - 4) * 2;//임시 좌표값들 다 지워야함
	pc->curPosY = 10;//임시 좌표값들 다 지워야함
	pc->speed = 10;
	ShowPc(pc);
}
void InitNpc(NPC* npc) {
	int i;
	int X;
	for (i = 0; i < npcMax; i++)
	{
		if (npc[i].exist == 0)
			break;
	}
	npc[i].exist = 1;
	if (stage == 1)
	{
		while (1)
		{
			npc[i].npc_id = rand() % 4 + 1;//자동차, 방해
			if (npc[i].npc_id != 2 && npc[i].npc_id != 3)
			{
				break;
			}
		}
	}
	else if (stage == 2)
	{
		while (1)
		{
			npc[i].npc_id = rand() % 4 + 1;//자동차, 기차, 방해
			if (npc[i].npc_id != 3)
			{
				break;
			}
		}
	}
	else if (stage == 3 || stage == 4)
	{
		npc[i].npc_id = rand() % 4 + 1;//자동차, 기차, 비행기, 방해
	}
	if (npc[i].npc_id <= 3)
		npc[i].count = npc[i].npc_id;
	if (npc[i].npc_id == 4)
	{
		npc[i].count = 1;//방해아이템 카운트 초기화 //11.28 김혜율
	}
	npc[i].curPosX = GBOARD_ORIGIN_X + (rand() % 50 + 2);
	while (1)
	{
		if (npc[i].curPosX - lastNpcPosX >= 0)
		{
			X = npc[i].curPosX - lastNpcPosX;
		}
		else
		{
			X = lastNpcPosX - npc[i].curPosX;
		}

		if (X > 23)
		{
			lastNpcPosX = npc[i].curPosX;
			break;
		}
		else
		{
			npc[i].curPosX = GBOARD_ORIGIN_X + (rand() % 50 + 2);
		}
	}
	npc[i].curPosY = GBOARD_ORIGIN_Y + 1;
	npc[i].random = rand() % 6;
	npc[i].downcnt = 0;
	AddNpcToBoard(&npc[i]);
	ShowNpc(npcModel[npc[i].npc_id], &npc[i]);
}
void initGas(Gas* gas) {
	int i;
	for (i = 0; i < gasMax; i++)
	{
		gas[i].curPosX = 0;
		gas[i].curPosY = 0;
		gas[i].exist = 0;
	}
}
void InitItem(NPC* item) {
	int i;
	int X;
	for (i = 0; i < 10; i++) {
		if (item[i].exist == 0)
			break;
	}
	item[i].exist = 1;
	if (stage == 1) {
		return;
	}
	if (stage == 2) {
		item[i].npc_id = 5;
	}
	if (stage == 3 || stage == 4) {
		item[i].npc_id = rand() % 2 + 5;
	}

	item[i].curPosX = GBOARD_ORIGIN_X + (rand() % 50 + 2);
	while (1)
	{
		if (item[i].curPosX - lastNpcPosX >= 0)
		{
			X = item[i].curPosX - lastNpcPosX;
		}
		else
		{
			X = lastNpcPosX - item[i].curPosX;
		}

		if (X > 23)
		{
			lastNpcPosX = item[i].curPosX;
			break;
		}
		else
		{
			item[i].curPosX = GBOARD_ORIGIN_X + (rand() % 50 + 2);
		}
	}
	item[i].curPosY = GBOARD_ORIGIN_Y + 2;
	AddNpcToBoard(&item[i]);
	ShowNpc(npcModel[item[i].npc_id], &item[i]);
}
void SpeedFunc(PC* pc, NPC* npc) {
	pc->speed = 10;
	npc->speed = 100;
}
int DetectGroundCollision(int PosX, int PosY, char npcModel[6][20], NPC* npc)
{
	int x, y;
	int arrX = (PosX - GBOARD_ORIGIN_X) / 2;
	int arrY = PosY - GBOARD_ORIGIN_Y;
	for (y = 0; y < 6; y++)
	{
		for (x = 0; x < 20; x++)
		{
			if (npcModel[y][x] != 0 && gameBoardInfo[arrY + y][arrX + x] == 1)
			{
				return 0; // 바닥과 충돌하면 0 반환
			}
		}
	}
	return 1;
}
int DetectNpcCollision(int PosX, int PosY, char npcModel[6][20], NPC* npc)
{
	int x, y;
	int arrX = (PosX - GBOARD_ORIGIN_X) / 2;
	int arrY = PosY - GBOARD_ORIGIN_Y;
	for (y = 0; y < 6; y++)
	{
		for (x = 0; x < 20; x++)
		{
			if (npcModel[y][x] != 0 && gameBoardInfo[arrY + y][arrX + x] == 3)//바꿔야함
			{
				return 0; // 총알과 충돌하면 0 반환
			}
		}
	}
	return 1;
}
void NpcDown(NPC* npc) {
	for (int i = 0; i < npcMax; i++) {
		if (npc[i].exist == 1) {
			if (!DetectGroundCollision(npc[i].curPosX, npc[i].curPosY + 1, npcModel[npc[i].npc_id], &npc[i]))
			{ // 충돌 했을때
				if (npc[i].npc_id < 4) { // item일때는 실행 안됨
					life_count--;
					DeleteLife(lifeModel, &life[4 - life_count]);
				}
				DeleteNpc(npcModel[npc[i].npc_id], &npc[i]);
				DeleteNpcToBoard(&npc[i]);
				npc[i].exist = 0;
				continue;
			}
			DeleteNpc(npcModel[npc[i].npc_id], &npc[i]);
			DeleteNpcToBoard(&npc[i]);
			npc[i].curPosY++;
			npc[i].downcnt++;
			SetCurrentCursorPos(npc[i].curPosX, npc[i].curPosY);
			AddNpcToBoard(&npc[i]);
			ShowNpc(npcModel[npc[i].npc_id], &npc[i]);
			if ((npc[i].random == npc[i].downcnt) && npc[i].npc_id == 3) {
				creationGas(gas, &npc[i]);
			}
		}
	}
}
int DetectNpc_ShotCollision(NPC* npc) {
	int i, j;
	//npc와 충돌검사
	for (i = 0; i < npcMax; i++)
	{
		if (npc[i].exist == 1) {

			if (!DetectNpcCollision(npc[i].curPosX + 2, npc[i].curPosY, npcModel[npc[i].npc_id], &npc[i]))
			{
				npc[i].count--;
				if (npc[i].count == 0) {
					DeleteNpc(npcModel[npc[i].npc_id], &npc[i]);
					DeleteNpcToBoard(&npc[i]);
					npc[i].exist = 0;
					if (npc[i].npc_id == 1) {//자동차이면
						score += 200;
					}
					else if (npc[i].npc_id == 2) {//기차이면
						score += 300;
					}
					else if (npc[i].npc_id == 3) {//비행기이면
						score += 500;
					}
					else if (npc[i].npc_id == 4) {//방해 npc//11.28 김혜율
						life_count--;
						DeleteLife(lifeModel, &life[4 - life_count]);
					}
				}
			}
		}
	}
	//item과 충돌검사
	for (i = 0; i < 10; i++)
	{
		if (item[i].exist == 1) {

			if (!DetectNpcCollision(item[i].curPosX + 2, item[i].curPosY, npcModel[item[i].npc_id], &item[i]))
			{
				DeleteNpc(npcModel[item[i].npc_id], &item[i]);
				DeleteNpcToBoard(&item[i]);
				item[i].exist = 0;

				if (item[i].npc_id == 5) { //자전거 먹을 경우 게이지 증가//11.28 박지윤
					if (gauge < 3)
					{
						gauge++;
					}
				}

				if (item[i].npc_id == 6)   //산소 먹을 경우 매연 다 없애기
				{
					for (j = 0; j < gasMax; j++)
					{
						if (gas[j].exist == 1)
						{
							DeleteGas(gasModel, &gas[j]);
							DeleteGasToBoard(&gas[j]);
							gas[j].exist = 0;
						}
					}
				}
			}
		}
	}
}
void blackScreen()
{
	int i, j;
	system("color 0");//콘솔창 배경 색 검은색으로 바꾸기
	setcolor(7, 0);
	SetCurrentCursorPos(0, 0);
	for (i = 0; i < 120; i++)
	{
		for (j = 0; j < 40; j++)
		{
			SetCurrentCursorPos(i, j);
			printf(" ");
		}
	}
}
void startScreen()
{
	system("color 1f");//콘솔창 배경 색 정하기
	int i = 30;
	int j = 14;
	int boxcolor = 6;
	int background = 1;
	int key = 0;
	//S
	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("        ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	//H

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■      ■ \n");
	i += 12;

	//O

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	//O

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	//T

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("    ■     \n");
	SetCurrentCursorPos(i, j + 2);
	printf("    ■     \n");
	SetCurrentCursorPos(i, j + 3);
	printf("    ■     \n");
	SetCurrentCursorPos(i, j + 4);
	printf("    ■     \n");
	i += 12;

	//다음줄
	boxcolor = 10;

	i = 24;
	j = 20;
	//P

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■         \n");
	i += 12;

	//L

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	//A

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("    ■     \n");
	SetCurrentCursorPos(i, j + 1);
	printf("  ■  ■   \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■      ■ \n");
	i += 12;

	//N

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■■    ■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■  ■  ■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■    ■■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■      ■ \n");
	i += 12;

	//E

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	//T
	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("    ■     \n");
	SetCurrentCursorPos(i, j + 2);
	printf("    ■     \n");
	SetCurrentCursorPos(i, j + 3);
	printf("    ■     \n");
	SetCurrentCursorPos(i, j + 4);
	printf("    ■     \n");

	//별 출력
	boxcolor = 14;
	for (i = 0; i < 6; i++)
	{
		SetCurrentCursorPos(i * 20 + 10, 2);
		setcolor(boxcolor, background);
		printf("★");
	}
	for (i = 0; i < 5; i++)
	{
		SetCurrentCursorPos(i * 20 + 20, 6);
		setcolor(boxcolor, background);
		printf("★");
	}

	//땅 출력
	boxcolor = 10;
	for (i = 0; i < 60; i++)
	{
		for (j = 35; j < 40; j++)
		{
			SetCurrentCursorPos(i * 2, j);
			setcolor(boxcolor, 10);
			printf("▒");
		}

	}
	for (i = 0; i < 5; i++)
	{
		SetCurrentCursorPos(i * 20 + 20, 34);
		setcolor(boxcolor, background);
		printf("♣");
	}

	for (i = 0;; i++)
	{
		if (i % 2 == 0)
		{
			SetCurrentCursorPos(49, 30);
			setcolor(14, background);
			printf("★ PRESS THE ENTER KEY ★");
			Sleep(500);
		}
		else
		{
			SetCurrentCursorPos(49, 30);
			setcolor(background, background);
			printf("★ PRESS THE ENTER KEY ★");
			Sleep(500);
		}
		if (_kbhit() != 0)
		{
			key = _getch();
		}
		if (key == EnterKey)
		{
			break;
		}
	}
}
void stageScreen()
{
	system("color 1f");//콘솔창 배경 색 정하기
	int i;
	int j;
	int k;
	int boxcolor = 6;
	int background = 1;

	//별 출력
	boxcolor = 14;
	for (i = 0; i < 6; i++)
	{
		SetCurrentCursorPos(i * 20 + 10, 2);
		setcolor(boxcolor, background);
		printf("★");
	}
	for (i = 0; i < 5; i++)
	{
		SetCurrentCursorPos(i * 20 + 20, 6);
		setcolor(boxcolor, background);
		printf("★");
	}

	//땅 출력
	boxcolor = 10;
	for (i = 0; i < 60; i++)
	{
		for (j = 35; j < 40; j++)
		{
			SetCurrentCursorPos(i * 2, j);
			setcolor(boxcolor, 10);
			printf("▒");
		}
	}
	for (i = 0; i < 5; i++)
	{
		SetCurrentCursorPos(i * 20 + 20, 34);
		setcolor(boxcolor, background);
		printf("♣");
	}

	for (k = 0; k < 6; k++)
	{
		if (k % 2 == 0)
		{
			i = 28;
			j = 18;
			//S
			boxcolor = 6;
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 1);
			printf("■         \n");
			SetCurrentCursorPos(i, j + 2);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 3);
			printf("        ■ \n");
			SetCurrentCursorPos(i, j + 4);
			printf("■■■■■ \n");
			i += 12;

			//T
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 1);
			printf("    ■     \n");
			SetCurrentCursorPos(i, j + 2);
			printf("    ■     \n");
			SetCurrentCursorPos(i, j + 3);
			printf("    ■     \n");
			SetCurrentCursorPos(i, j + 4);
			printf("    ■     \n");
			i += 12;

			//A
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("    ■     \n");
			SetCurrentCursorPos(i, j + 1);
			printf("  ■  ■   \n");
			SetCurrentCursorPos(i, j + 2);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 3);
			printf("■      ■ \n");
			SetCurrentCursorPos(i, j + 4);
			printf("■      ■ \n");
			i += 12;

			//G
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 1);
			printf("■         \n");
			SetCurrentCursorPos(i, j + 2);
			printf("■    ■■ \n");
			SetCurrentCursorPos(i, j + 3);
			printf("■      ■ \n");
			SetCurrentCursorPos(i, j + 4);
			printf("■■■■■ \n");
			i += 12;

			//E
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 1);
			printf("■         \n");
			SetCurrentCursorPos(i, j + 2);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 3);
			printf("■         \n");
			SetCurrentCursorPos(i, j + 4);
			printf("■■■■■ \n");
			i += 12;
			boxcolor = 10;

			if (stage == 1)
			{
				setcolor(boxcolor, background);
				SetCurrentCursorPos(i, j);
				printf("  ■■     \n");
				SetCurrentCursorPos(i, j + 1);
				printf("    ■     \n");
				SetCurrentCursorPos(i, j + 2);
				printf("    ■     \n");
				SetCurrentCursorPos(i, j + 3);
				printf("    ■     \n");
				SetCurrentCursorPos(i, j + 4);
				printf("  ■■■   \n");
			}
			else if (stage == 2)
			{
				setcolor(boxcolor, background);
				SetCurrentCursorPos(i, j);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 1);
				printf("      ■   \n");
				SetCurrentCursorPos(i, j + 2);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 3);
				printf("  ■       \n");
				SetCurrentCursorPos(i, j + 4);
				printf("  ■■■   \n");
			}
			else if (stage == 3)
			{
				setcolor(boxcolor, background);
				SetCurrentCursorPos(i, j);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 1);
				printf("      ■   \n");
				SetCurrentCursorPos(i, j + 2);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 3);
				printf("      ■   \n");
				SetCurrentCursorPos(i, j + 4);
				printf("  ■■■   \n");
			}
			else if (stage == 4)
			{
				setcolor(boxcolor, background);
				SetCurrentCursorPos(i, j);
				printf("  ■  ■   \n");
				SetCurrentCursorPos(i, j + 1);
				printf("  ■  ■   \n");
				SetCurrentCursorPos(i, j + 2);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 3);
				printf("      ■   \n");
				SetCurrentCursorPos(i, j + 4);
				printf("      ■   \n");
			}
		}
		else
		{
			i = 28;
			j = 18;
			boxcolor = 1;
			//S
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 1);
			printf("■         \n");
			SetCurrentCursorPos(i, j + 2);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 3);
			printf("        ■ \n");
			SetCurrentCursorPos(i, j + 4);
			printf("■■■■■ \n");
			i += 12;

			//T
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 1);
			printf("    ■     \n");
			SetCurrentCursorPos(i, j + 2);
			printf("    ■     \n");
			SetCurrentCursorPos(i, j + 3);
			printf("    ■     \n");
			SetCurrentCursorPos(i, j + 4);
			printf("    ■     \n");
			i += 12;

			//A
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("    ■     \n");
			SetCurrentCursorPos(i, j + 1);
			printf("  ■  ■   \n");
			SetCurrentCursorPos(i, j + 2);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 3);
			printf("■      ■ \n");
			SetCurrentCursorPos(i, j + 4);
			printf("■      ■ \n");
			i += 12;

			//O
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 1);
			printf("■         \n");
			SetCurrentCursorPos(i, j + 2);
			printf("■    ■■ \n");
			SetCurrentCursorPos(i, j + 3);
			printf("■      ■ \n");
			SetCurrentCursorPos(i, j + 4);
			printf("■■■■■ \n");
			i += 12;

			//E
			setcolor(boxcolor, background);
			SetCurrentCursorPos(i, j);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 1);
			printf("■         \n");
			SetCurrentCursorPos(i, j + 2);
			printf("■■■■■ \n");
			SetCurrentCursorPos(i, j + 3);
			printf("■         \n");
			SetCurrentCursorPos(i, j + 4);
			printf("■■■■■ \n");
			i += 12;

			if (stage == 1)
			{
				setcolor(boxcolor, background);
				SetCurrentCursorPos(i, j);
				printf("  ■■     \n");
				SetCurrentCursorPos(i, j + 1);
				printf("    ■     \n");
				SetCurrentCursorPos(i, j + 2);
				printf("    ■     \n");
				SetCurrentCursorPos(i, j + 3);
				printf("    ■     \n");
				SetCurrentCursorPos(i, j + 4);
				printf("  ■■■   \n");
			}
			else if (stage == 2)
			{
				setcolor(boxcolor, background);
				SetCurrentCursorPos(i, j);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 1);
				printf("      ■   \n");
				SetCurrentCursorPos(i, j + 2);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 3);
				printf("  ■       \n");
				SetCurrentCursorPos(i, j + 4);
				printf("  ■■■   \n");
			}
			else if (stage == 3)
			{
				setcolor(boxcolor, background);
				SetCurrentCursorPos(i, j);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 1);
				printf("      ■   \n");
				SetCurrentCursorPos(i, j + 2);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 3);
				printf("      ■   \n");
				SetCurrentCursorPos(i, j + 4);
				printf("  ■■■   \n");
			}
			else if (stage == 4)
			{
				setcolor(boxcolor, background);
				SetCurrentCursorPos(i, j);
				printf("  ■  ■   \n");
				SetCurrentCursorPos(i, j + 1);
				printf("  ■  ■   \n");
				SetCurrentCursorPos(i, j + 2);
				printf("  ■■■   \n");
				SetCurrentCursorPos(i, j + 3);
				printf("      ■   \n");
				SetCurrentCursorPos(i, j + 4);
				printf("      ■   \n");
			}
		}
		Sleep(500);
	}
}
void gameClearScreen()
{
	//Game Clear
	//You Save The Earth!!
	//Made By 김혜율, 김아인, 김원희, 박지윤
	system("color 1f");//콘솔창 배경 색 정하기
	int i;
	int j;
	int k;
	int boxcolor = 10;
	int background = 1;

	//별 출력
	boxcolor = 14;
	for (i = 0; i < 6; i++)
	{
		SetCurrentCursorPos(i * 20 + 10, 2);
		setcolor(boxcolor, background);
		printf("★");
	}
	for (i = 0; i < 5; i++)
	{
		SetCurrentCursorPos(i * 20 + 20, 6);
		setcolor(boxcolor, background);
		printf("★");
	}

	//땅 출력
	boxcolor = 10;
	for (i = 0; i < 60; i++)
	{
		for (j = 35; j < 40; j++)
		{
			SetCurrentCursorPos(i * 2, j);
			setcolor(boxcolor, 10);
			printf("▒");
		}
	}
	for (i = 0; i < 5; i++)
	{
		SetCurrentCursorPos(i * 20 + 20, 34);
		setcolor(boxcolor, background);
		printf("♣");
	}

	SetCurrentCursorPos(47, 24);
	setcolor(14, background);
	printf("★ YOU SAVE THE EARTH ★");
	SetCurrentCursorPos(38, 27);
	printf("MADE BY 김혜율 & 김아인 & 김원희 & 박지윤");

	for (k = 0;; k++)
	{
		if (k % 2 == 0)
		{
			boxcolor = 10;
		}
		else
		{
			boxcolor = 1;
		}
		i = 4;
		j = 14;
		//G
		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("■■■■■ \n");
		SetCurrentCursorPos(i, j + 1);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■    ■■ \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■      ■ \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■■■■■ \n");
		i += 12;

		//A

		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("    ■     \n");
		SetCurrentCursorPos(i, j + 1);
		printf("  ■  ■   \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■■■■■ \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■      ■ \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■      ■ \n");
		i += 12;

		//O

		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("■      ■ \n");
		SetCurrentCursorPos(i, j + 1);
		printf("■■  ■■ \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■  ■  ■ \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■      ■ \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■      ■ \n");
		i += 12;

		//E

		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("■■■■■ \n");
		SetCurrentCursorPos(i, j + 1);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■■■■■ \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■■■■■ \n");
		i += 12;

		i += 6;

		//C
		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("■■■■■ \n");
		SetCurrentCursorPos(i, j + 1);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■■■■■ \n");
		i += 12;

		//L

		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 1);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■■■■■ \n");
		i += 12;

		//E

		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("■■■■■ \n");
		SetCurrentCursorPos(i, j + 1);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■■■■■ \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■         \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■■■■■ \n");
		i += 12;

		//A

		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("    ■     \n");
		SetCurrentCursorPos(i, j + 1);
		printf("  ■  ■   \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■■■■■ \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■      ■ \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■      ■ \n");
		i += 12;

		//R

		setcolor(boxcolor, background);
		SetCurrentCursorPos(i, j);
		printf("■■■■   \n");
		SetCurrentCursorPos(i, j + 1);
		printf("■      ■ \n");
		SetCurrentCursorPos(i, j + 2);
		printf("■■■■   \n");
		SetCurrentCursorPos(i, j + 3);
		printf("■      ■ \n");
		SetCurrentCursorPos(i, j + 4);
		printf("■      ■ \n");
		Sleep(800);
	}
}
void gameOverScreen()
{
	int i = 36;
	int j = 14;
	int boxcolor = 8;
	int background = 0;

	//G
	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■    ■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	//A

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("    ■     \n");
	SetCurrentCursorPos(i, j + 1);
	printf("  ■  ■   \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■      ■ \n");
	i += 12;

	//O

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■■  ■■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■  ■  ■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■      ■ \n");
	i += 12;

	//E

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	i = 36;
	j = 20;

	//O

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	//V

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("  ■  ■   \n");
	SetCurrentCursorPos(i, j + 4);
	printf("    ■     \n");
	i += 12;

	//E

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■■ \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■         \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■■■■■ \n");
	i += 12;

	//R

	setcolor(boxcolor, background);
	SetCurrentCursorPos(i, j);
	printf("■■■■   \n");
	SetCurrentCursorPos(i, j + 1);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 2);
	printf("■■■■   \n");
	SetCurrentCursorPos(i, j + 3);
	printf("■      ■ \n");
	SetCurrentCursorPos(i, j + 4);
	printf("■      ■ \n");

}