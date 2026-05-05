#define _CRT_SECURE_NO_WARNINGS
#include "tetris.h"
static IMAGE img[IMAGE_COUNT];
static IMAGE img_Eli[ELIMINATE_COUNT];
static IMAGE img_tool1;
static IMAGE img_tool2;
static IMAGE img_shadow[SHADOW_COUNT];
void drawMainImage(ExMessage& msg, int& flag)
{
	//建立CPU时钟频率
	LARGE_INTEGER fre;
	//存储开始和时间结束的时间戳（以CPU tick数为单位）
	LARGE_INTEGER start_time, end_time;
	//捕获计算机高性能计时器频率
	QueryPerformanceFrequency(&fre);
	//捕获当前时刻高性能计时器的值
	QueryPerformanceCounter(&start_time);
	//计算每一帧应该占用的时间长度（单位：秒）
	double continue_time = 1.0 / IMAGE_FPS;
	//用来记录距离上一次绘制过去的时间（滴答数->时间）
	double elapsed_time = 0.0;
	for (int i = 0; i < IMAGE_COUNT;) {
		//捕获鼠标信息
		if (peekmessage(&msg, EX_MOUSE)) {
			if (msg.message == WM_LBUTTONDOWN) {
				flag++;
				if (flag == 1)break;
			}
		}
		//按下计时器
		QueryPerformanceCounter(&end_time);
		//获取间隔滴答数
		double delte_time = (double)(end_time.QuadPart - start_time.QuadPart) / fre.QuadPart;
		//更新起始时间
		start_time = end_time;
		//将以上的计算时间累加
		elapsed_time += delte_time;
		//如果累计时间大于我们想要的帧率，那就赶紧插入下一张图片
		if (elapsed_time >= continue_time) {
			i++;
			if (i % 3)cleardevice();
			putimage(0, 0, &img[i]);
			//此时减去帧率，就能获得余下的累加时间（累加时间置零）
			elapsed_time -= continue_time;
			Sleep(0);
		}
		else {
			Sleep(0);
		}

	}
}
//加载图片
void loadSerImage()
{
	for (int i = 1; i <= IMAGE_COUNT; i++) {
		wchar_t filename[50];
		swprintf(filename, sizeof(filename) / sizeof(wchar_t), L"./res/welcome/_00%d_图层-%d.png", 93 - i, i);
		loadimage(&img[i - 1], filename);
	}
	for (int i = 0; i < ELIMINATE_COUNT; i++) {
		wchar_t filename[50];
		swprintf(filename, sizeof(filename) / sizeof(wchar_t), L"./res/tx/%d.png", i);
		loadimage(&img_Eli[i], filename);
	}
	loadimage(&img_tool1, L"./res/tool1.png");
	loadimage(&img_tool2, L"./res/tool2.png");
	for (int i = 0; i < SHADOW_COUNT; i++) {
		wchar_t name[40];
		swprintf(name, sizeof(name) / sizeof(wchar_t), L"./res/s%d.png", i + 1);
		loadimage(&img_shadow[i], name);
	}
}
//绘制下一格方块
void drawSubPicture(const int(&MatrixBlock)[4][4], int Type)
{
	IMAGE img;
	loadimage(&img, L"./res/tiles.png");
	for (int i = 0; i < Matrix_Size_; i++) {
		for (int j = 0; j < Matrix_Size_; j++) {
			if (MatrixBlock[i][j]) {
				int dstX = SUB_GAME_X + j * BLOCK_WIDE;
				int dstY = SUB_GAME_Y + i * BLOCK_HIGH;
				int srcX = Type * BLOCK_WIDE;
				int srcY = 0;
				putimage(dstX, dstY, BLOCK_WIDE, BLOCK_WIDE, &img, srcX, srcY);
			}
		}
	}
}
//绘制消除特效
void drawEliminateUI(int y)
{
	LARGE_INTEGER current_time, frequency, last_time;
	QueryPerformanceCounter(&last_time);
	QueryPerformanceFrequency(&frequency);
	double Frame_rate_ = 1.0 / (IMAGE_FPS);
	double elapsed_time = 0.0;
	for (int i = 0; i < ELIMINATE_COUNT; ) {
		QueryPerformanceCounter(&current_time);
		elapsed_time += (double)(current_time.QuadPart - last_time.QuadPart) / frequency.QuadPart;
		last_time = current_time;
		if (elapsed_time >= Frame_rate_) {
			i++;
			int dstX = GAME_X;
			int dstY = GAME_Y + y * BLOCK_HIGH;
			int srcX = ELIMINATE_UIX;
			int srcY = ELIMINATE_UIY;
			putimage(dstX, dstY, GAME_WIDE, BLOCK_HIGH, &img_Eli[i], srcX, srcY);
			elapsed_time -= Frame_rate_;
			Sleep(0);
		}
		else {
			Sleep(0);
		}

	}

}
//绘制所需文本
void drawTest(int& score, int& speed, int& highScore, int& line)
{
	wchar_t str[20];
	settextstyle(56, 0, L"微软雅黑");
	swprintf(str, sizeof(str) / sizeof(wchar_t), L"%d", score);
	settextcolor(WHITE);
	setbkmode(TRANSPARENT);
	int dstX = SCORE_X;
	int dstY = SCORE_Y;
	outtextxy(dstX, dstY, str);

	swprintf(str, sizeof(str) / sizeof(wchar_t), L"%d", highScore);
	dstX = HIGH_SCORE_X;
	dstY = HIGH_SCORE_Y;
	outtextxy(dstX, dstY, str);

	if (speed == 1)swprintf(str, sizeof(str) / sizeof(wchar_t), L"slow");
	else if (speed == 2)swprintf(str, sizeof(str) / sizeof(wchar_t), L"middle");
	else swprintf(str, sizeof(str) / sizeof(wchar_t), L"fast");
	dstX = SPEED_X;
	dstY = SPEED_Y;
	outtextxy(dstX, dstY, str);

	swprintf(str, sizeof(str) / sizeof(wchar_t), L"%d", line);
	dstX = LINE_X;
	dstY = LINE_Y;
	outtextxy(dstX, dstY, str);

}

//UI绘制
void drawPicture(const int(&MatrixBlock)[Matrix_Size_][Matrix_Size_], const Static_block(&static_block)[20][10], int x, int y, int type, int rorate)
{
	IMAGE img, gmi;
	loadimage(&img, L"./res/bg3.png");
	putimage(0, 0, &img);
	loadimage(&gmi, L"./res/tiles.png");
	for (int i = 20 - 1; i >= 0; i--) {
		for (int j = 0; j < 10; j++) {
			if (static_block[i][j].state) {
				int dstX = GAME_X + j * BLOCK_WIDE;
				int dstY = GAME_Y + i * BLOCK_HIGH;
				int srcX = (static_block[i][j].color - 1) * BLOCK_WIDE;
				int srcY = 0;
				putimage(dstX, dstY, BLOCK_WIDE, BLOCK_HIGH, &gmi, srcX, srcY);
			}
		}
	}
	for (int i = 0; i < Matrix_Size_; i++) {
		for (int j = 0; j < Matrix_Size_; j++) {
			if (MatrixBlock[i][j]) {
				int logic_x = j + x;
				int logic_y = i + y;
				if (logic_y >= 0) {
					int dstX = GAME_X + logic_x * BLOCK_WIDE;
					int dstY = GAME_Y + logic_y * BLOCK_HIGH;
					int srcX = type * BLOCK_WIDE;
					int srcY = 0;
					putimage(dstX, dstY, BLOCK_WIDE, BLOCK_HIGH, &gmi, srcX, srcY);
				}
			}
		}
	}
}
//绘制起始UI
void drawStartUI() {
	IMAGE img;
	loadimage(&img, L"./res/bg6.png");
	putimage(0, 0, &img);
	return;
}
//绘制结束UI
void drawEndUI(int score, int highScore)
{
	IMAGE img;
	loadimage(&img, L"./res/bg7.png");
	putimage(0, 0, &img);
	wchar_t str[20];
	settextstyle(56, 0, L"微软雅黑");
	swprintf(str, sizeof(str) / sizeof(wchar_t), L"%d", score);
	settextcolor(WHITE);
	setbkmode(TRANSPARENT);
	int dstX = END_SCORE_X;
	int dstY = END_SCORE_Y;
	outtextxy(dstX, dstY, str);

	swprintf(str, sizeof(str) / sizeof(wchar_t), L"%d", highScore);
	dstX = END_HIGHSCORE_X;
	dstY = END_HIGHSCORE_Y;
	outtextxy(dstX, dstY, str);
	return;
}
//绘制游戏结束
void drawOver()
{
	IMAGE img;
	loadimage(&img, L"./res/over.png");
	int dstX = GAME_X;
	int dstY = GAME_Y + BLOCK_HIGH * 9;
	int srcX = OVER_X;
	int srcY = OVER_Y;
	putimage(dstX, dstY, OVER_WIDE, OVER_HIGH, &img, srcX, srcY);
}
//绘制炸弹
void drawBoom(int flag)
{
	if (flag) {
		putimage(BOOM_X, BOOM_Y, &img_tool2);
	}
	else {
		putimage(BOOM_X, BOOM_Y, &img_tool1);
	}
}

void drawShadow(const int(&MatrixBlock)[Matrix_Size_][Matrix_Size_],int draw_x,int draw_y, int type, int rorate)
{
	for (int i = 0; i < Matrix_Size_; i++) {
		for (int j = 0; j < Matrix_Size_; j++) {
			if (MatrixBlock[i][j]) {
				int logic_x = j + draw_x;
				int logic_y = i + draw_y;
				int dstX = GAME_X+logic_x * BLOCK_WIDE;
				int dstY = GAME_Y+logic_y * BLOCK_HIGH;
				putimage(dstX, dstY, &img_shadow[type]);
			}
		}
	}
}