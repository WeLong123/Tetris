#pragma once
#include<easyx.h>
#include<iostream>
#include<math.h>
#include<time.h>
#include"common.hpp"
#define WINDOWS_WIDE 878
#define WINDOWS_HIGH 1072
#define IMAGE_COUNT 78
#define IMAGE_FPS 30
#define BLOCK_WIDE 36
#define BLOCK_HIGH 36
#define GAME_X 262
#define GAME_Y 133
#define GAME_WIDE 360
#define GAME_HIGH 720
#define SUB_GAME_X 690
#define SUB_GAME_Y 150
#define SUB_GAME_WIDE 144
#define SUB_GAME_HIGH 144
#define ELIMINATE_UIX 7
#define ELIMINATE_UIY 55
#define ELIMINATE_WIDE 360
#define ELIMINATE_HIGH 36
#define ELIMINATE_COUNT 12
#define SCORE_X 666
#define SCORE_Y 736
#define HIGH_SCORE_X 666
#define HIGH_SCORE_Y 826
#define SPEED_X 95
#define SPEED_Y 730
#define LINE_X 155
#define LINE_Y 830
#define END_HIGHSCORE_X 480
#define END_HIGHSCORE_Y 280
#define END_SCORE_X 480
#define END_SCORE_Y 370
#define OVER_X 0
#define OVER_Y 0
#define OVER_WIDE 360
#define OVER_HIGH 119
#define BOOM_X 90
#define BOOM_Y 590
#define BOOM_WIDE 36
#define BOOM_HIGH 36
#define SHADOW_COUNT 7
//绘制图像
constexpr static int Matrix_Size_ = 4;
//绘制主界面
void drawMainImage(ExMessage& msg, int& flag);
//加载所需图片
void loadSerImage();
//绘制主要逻辑网格
void drawPicture(const int(&MatrixBlock)[Matrix_Size_][Matrix_Size_], const Static_block(&static_block)[20][10], int x, int y, int type, int rorate);
//绘制下一方块图片
void drawSubPicture(const int(&MatrixBlock)[4][4], int Type);
//绘制消除特效
void drawEliminateUI(int y);
//绘制文本
void drawTest(int& score, int& speed, int& highScore, int& line);
//绘制开始界面
void drawStartUI();
//绘制结束界面
void drawEndUI(int score, int scoreHigh);
//绘制游戏失败后的结束图片
void drawOver();
//绘制炸弹状态
void drawBoom(int flag);
//绘制影子
void drawShadow(const int(&MatrixBlock)[Matrix_Size_][Matrix_Size_],int draw_x,int draw_y,int type,int rorate);