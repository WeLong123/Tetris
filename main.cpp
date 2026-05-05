#define _CRT_SECURE_NO_WARNINGS
#include"tetris.h"
#include"block.h"
#include"common.hpp"
#include <time.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") 
#define BUTTON_X 265
#define BUTTON_Y 400
#define BUTTON_WIDE 360
#define BUTTON_HIGH 110
#define BUTTON_X1 316
#define BUTTON_Y1 612
#define BUTTON_WIDE1 264
#define BUTTON_HIGH1 71
#define BUTTON_X2 316
#define BUTTON_Y2 700
#define BUTTON_WIDE2 264
#define BUTTON_HIGH2 71
bool JudgeTerrior(ExMessage& msg);
bool JudgeTerrior1(ExMessage& msg);
bool JudgeTerrior2(ExMessage& msg);
void initGame();
void store_score(int score);
int getHighScore();
void PlayMainMusic(const std::wstring& musicName);
int score = 0;//分数
int scoreHigh = 0;//最高分数
int speed = 1;//速度
int line__ = 0;//行数
int flag = -1;//操作标志 0为空 1 2 3对应相应操作
int state_flag = 1;//游戏标志 0为空 1为方块落地判定 2为游戏结束
int boom_flag = 0;//炸弹标志 0为没准备好 1为已经准备好
double tool_accumulate_time = 0.0;//炸弹积累时间 30秒一次
//s 8019 D 8260 A7745 W 4439 G8775
int main() {
    timeBeginPeriod(1);
    srand(time(0));
    //所需变量声明区
    Block bck;
    int Type, rorate, p_Type,y,draw_y;
    LARGE_INTEGER fall_time;
    LARGE_INTEGER tool_time;
    ExMessage msg;
    void(*drawUI)(int y) = drawEliminateUI;
    scoreHigh = getHighScore();
    // --- 帧率控制 ---
    constexpr double Frame_rate = 60.0;//目标帧率
    constexpr double Frame_interval = 1.0 / Frame_rate;//目标间隔
    LARGE_INTEGER frequency, current_time, next_draw_time;//频率 当前时间 下一帧时间
    QueryPerformanceFrequency(&frequency);

    // 设置第一次时间和第一次帧率绘制时间
    QueryPerformanceCounter(&current_time);
    next_draw_time.QuadPart = current_time.QuadPart + (LONGLONG)(Frame_interval * frequency.QuadPart);

    //绘制开始动画
    initgraph(878, 1072, 1);
    //加载所需图片
    loadSerImage();
    //播放音频
    PlaySound(L"./res/bg.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    //开始动画
    while (1) {
        peekmessage(&msg, EX_MOUSE);
        drawMainImage(msg, flag);
        if (flag == 1) {
            flag = 0;
            break;
        }
    }
    closegraph();
    //绘制开始界面
    initgraph(938, 896, 1);
    //开始界面
    while (1) {
        cleardevice();
        drawStartUI();
        //判断是否开始游戏
        while (1) {
            peekmessage(&msg, EX_MOUSE);
            while (JudgeTerrior(msg)) {
                if (peekmessage(&msg, EX_MOUSE)) {
                    if (msg.message == WM_LBUTTONDOWN) {
                        flag = 1;
                        break;
                    }
                }
            }
            if (flag)break;
        }
        Sleep(500);
        //游戏前预处理
        QueryPerformanceCounter(&fall_time);
        QueryPerformanceCounter(&tool_time);
        bck.initBlock();

        //游戏主体
        while (state_flag != 2) {
            printf("%d\n", msg.ch);
            //所需变量存储
            Type = bck.getType();
            p_Type = bck.getp_type();
            rorate = bck.getRorate_index();
            y = bck.getY();
            draw_y = bck.getDrawY();
            flushmessage(EX_KEY);
            //---------------------------------------
            // 绘制操作 
            BeginBatchDraw();
            drawPicture(BlockData::inquireMatrix(Type, rorate), Block::static_block, bck.getX(), y, Type, rorate);
            drawSubPicture(BlockData::inquireMatrix(p_Type, 0), p_Type);
            drawTest(score, speed, scoreHigh, line__);
            drawBoom(boom_flag);
            if (draw_y - y > 2)drawShadow(BlockData::inquireMatrix(Type, rorate), bck.getX(), draw_y, Type, rorate);
            EndBatchDraw();
            // ------------------------------------
            //  //更新当前时间
            QueryPerformanceCounter(&current_time);
            // 等待直到到达下一次渲染的时间点
            while (current_time.QuadPart < next_draw_time.QuadPart) {
                // 非阻塞等待一小段时间，防止CPU占用过高
                Sleep(1);
                QueryPerformanceCounter(&current_time);
            }
            // 为下一帧计算目标时间点
            next_draw_time.QuadPart += (LONGLONG)(Frame_interval * frequency.QuadPart);

            //--------------------------------------------
            //游戏状态为2就退出
            if (state_flag == 2)break;
            //游戏状态为1手动消除和置空状态
            if (state_flag == 1) {  bck.HandyEliminate(drawUI, score, speed, line__);flushmessage(EX_KEY); line__ = bck.getline(); state_flag = 0;}
            //逻辑运算
            if (peekmessage(&msg, EX_KEY));
            //判断是否按下空格
            if(draw_y-y>2)bck.interactionSpace(msg,state_flag);
            if (state_flag == 1) { Sleep(100); flushmessage(EX_KEY); continue; }
            //判断是否按下
            bck.interactionDown(msg, fall_time, state_flag);
            //判断是否按左右
            bck.interactionLeftRight(msg);
            //判断是否按上
            bck.interactionUp(msg);
            //判断是否使用工具
            bck.interactionTool(msg, tool_time, tool_accumulate_time, state_flag, boom_flag, drawUI, line__);
            //---------------------------------------------
        }
        mciSendString(L"play res/over.wav", 0, 0, 0);
        drawOver();
        Sleep(1000);
        //存储当前分数
        store_score(score);
        //获取最高分
        scoreHigh = getHighScore();
        //设置UI标志为空
        flag = 0;
        drawEndUI(score, scoreHigh);
        //结束界面
        while (1) {
            peekmessage(&msg);
            while (JudgeTerrior1(msg)) {
                if (peekmessage(&msg, EX_MOUSE)) {
                    if (msg.message == WM_LBUTTONDOWN) {
                        flag = 2;
                        break;
                    }
                }
            }
            while (JudgeTerrior2(msg)) {
                if (peekmessage(&msg, EX_MOUSE)) {
                    if (msg.message == WM_LBUTTONDOWN) {
                        flag = 3;
                        break;
                    }
                }
            }
            if (flag)break;
        }
        if (flag == 2)break;
        bck.creatGame();
        initGame();
    }
    closegraph();
    timeEndPeriod(1);
    std::cout << "作者：鑨" << "\n感谢游玩" << "\n版本：1.1.0" << std::endl;
    Sleep(3000);
    return 0;
}
void store_score(int score) {
    FILE* fstream;
    fstream = fopen("./score.txt", "a");
    fprintf(fstream, "%d ", score);
    fclose(fstream);
}
int getHighScore() {
    int temp = 0, max = 0;
    FILE* fstream;
    fstream = fopen("./score.txt", "r");
    while (fscanf(fstream, "%d", &temp) == 1) {
        if (max < temp)max = temp;
    }
    fclose(fstream);
    return max;
}
bool JudgeTerrior(ExMessage& msg) {
    return msg.x > BUTTON_X && msg.y > BUTTON_Y && msg.x < BUTTON_X + BUTTON_WIDE && msg.y < BUTTON_Y + BUTTON_HIGH;
}
bool JudgeTerrior1(ExMessage& msg) {
    return msg.x > BUTTON_X1 && msg.y > BUTTON_Y1 && msg.x < BUTTON_X1 + BUTTON_WIDE1 && msg.y < BUTTON_Y1 + BUTTON_HIGH1;
}
bool JudgeTerrior2(ExMessage& msg) {
    return msg.x > BUTTON_X2 && msg.y > BUTTON_Y2 && msg.x < BUTTON_X2 + BUTTON_WIDE2 && msg.y < BUTTON_Y2 + BUTTON_HIGH2;
}
void initGame() {
    flag = 0;
    score = 0;
    speed = 1;
    line__ = 0;
    state_flag = 1;
    boom_flag = 0;
    tool_accumulate_time = 0.0;
}
