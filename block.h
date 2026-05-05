#pragma once
#include<easyx.h>
#include<iostream>
#include<math.h>
#include<string>
#include"common.hpp"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") 
#define TETRIS_WIDE 10
#define TETRIS_HIGH 20
constexpr static int Block_Type = 7;//一共有七种类型的方块
constexpr static int Block_State = 4;//每种方块分别有四种形态
constexpr static int Matrix_Size = 4;//用一个4*4的矩阵模拟
static bool isDownPress = false;
static bool isAccelerate = false;
static  double Normal_speed = 0.5;
static constexpr double Fast_speed = 0.05;
//方块的矩阵数据
namespace BlockData {
	const int Block_Shape[Block_Type][Block_State][Matrix_Size][Matrix_Size] = {
		//I-Block
		{{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
		 {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
		 {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
		 {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}},
		 //O-Block
		{{{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
		 {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
		 {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
		 {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}}},
		 //T-Block
		{{{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}},
		 {{0,0,0,0},{0,1,0,0},{0,1,1,0},{0,1,0,0}},
		 {{0,0,0,0},{0,0,0,0},{1,1,1,0},{0,1,0,0}},
		 {{0,0,0,0},{0,1,0,0},{1,1,0,0},{0,1,0,0}}},
		 //Z-Block
		{{{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
		 {{0,0,0,0},{0,0,1,0},{0,1,1,0},{0,1,0,0}},
		 {{0,0,0,0},{0,0,0,0},{1,1,0,0},{0,1,1,0}},
		 {{0,0,0,0},{0,1,0,0},{1,1,0,0},{1,0,0,0}}},
		 //N-Block
		{{{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
		 {{0,0,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0}},
		 {{0,0,0,0},{0,0,0,0},{0,1,1,0},{1,1,0,0}},
		 {{0,0,0,0},{1,0,0,0},{1,1,0,0},{0,1,0,0}}},
		 //J-Block
		 {{{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
		  {{0,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,0,0}},
		  {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}},
		  {{0,0,0,0},{0,1,0,0},{0,1,0,0},{0,1,1,0}}},
		  //L-Block
		 {{{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
		  {{0,0,0,0},{0,0,1,0},{0,0,1,0},{0,1,1,0}},
		  {{0,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,0,0}},
		  {{0,0,0,0},{1,1,0,0},{1,0,0,0},{1,0,0,0}}},
	};
	const int(&inquireMatrix(int Type, int State))[Matrix_Size][Matrix_Size];
	const int Block_High[Block_Type][Block_State] = {
		//I-block
		{1,3,2,3},
		//O-block
		{3,3,3,3},
		//T-block
		{2,3,3,3},
		//Z-block
		{2,3,3,3},
		//N-block
		{2,3,3,3},
		//J-block
		{2,3,2,3},
		//L-block
		{2,3,2,3}
	};
	int inquireMatrixHigh(int Type, int State);
};

class Block {
protected:
	int x, y, type, rorate_index, p_x, p_y, p_type;
	int draw_y;

protected:
	//碰撞墙体检测
	bool checkCollisionWall();
	bool checkCollisionWall(int rorate);
	bool checkCollisionWall(int p_x, int p_y);
	//碰撞静止方块检测
	bool checkCollisionBlock();
	bool checkCollisionBlock(int rorate);
	bool checkCollisionBlock(int p_x, int p_y);
	//获取矩阵引用
	const int(&getBlockShape(int Type, int State)const)[Matrix_Size][Matrix_Size];
	//消除函数
	void Eliminate(void(*drawUI)(int y), int& score, int& speed, int& line);
	void Eliminate(void(*drawUI)(int y), int& line);
	//判输
	bool JudgeFull();
	//计时器
	bool timeTick(LARGE_INTEGER& last_time, double& accumulate_time);
	//获取当前列的最低行
	int getBottom(const int(&MatrixBlock)[Matrix_Size][Matrix_Size], int j);
public:
	Block();
	virtual ~Block() {}
	//初始化方块，用于生成下一个方块
	void initBlock();
	//交互下降模块
	void interactionDown(ExMessage& msg, LARGE_INTEGER& main_time, int& flag);
	//交互左右位移模块
	void interactionLeftRight(ExMessage& msg);
	//旋转交互模块
	void interactionUp(ExMessage& msg);
	//初始化游戏
	void creatGame();
	//工具交互
	void interactionTool(ExMessage& msg, LARGE_INTEGER& tool_time, double& accumulate_time, int& state_flag, int& boom_flag, void(*drawUI)(int y), int& line);
	//Space键
	void interactionSpace(ExMessage& msg,int&flag);
	//消除函数接口
	void HandyEliminate(void(*drawUI)(int y), int& score, int& speed, int& line);
	//get函数
	int getX();
	int getY();
	int getType();
	int getRorate_index();
	int getp_type();
	int getline(); 
	int getDrawX();
	int getDrawY();
public:
	static Static_block static_block[TETRIS_HIGH][TETRIS_WIDE];
};
