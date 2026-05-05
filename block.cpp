#include"block.h"
Static_block Block::static_block[TETRIS_HIGH][TETRIS_WIDE];
Block::Block() :x(3), y(-3), rorate_index(0), p_x(x), p_y(y)
{
	p_type = rand() % 7;
	type = 0;
}

void Block::initBlock()
{
	x = 3, y = -3;
	p_x = x; p_y = y;
	type = p_type;
	p_type = ((rand()%13)*(rand()%19))% 7;
}
void Block::interactionDown(ExMessage& msg, LARGE_INTEGER& main_time,int &flag)
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = getBlockShape(type, rorate_index);
	if (msg.message == WM_KEYDOWN) {
		if (msg.ch == 8019||msg.ch==20520) {
			isAccelerate = true;
			isDownPress = true;
		}
	}
	else if (msg.message == WM_KEYUP) {
		msg.ch = 0;
		isAccelerate = false;
		isDownPress = false;
	}
	LARGE_INTEGER now_time;
	QueryPerformanceCounter(&now_time);
	LARGE_INTEGER CPU_fre;
	QueryPerformanceFrequency(&CPU_fre);
	double elapsed_time = (double)(now_time.QuadPart - main_time.QuadPart) / CPU_fre.QuadPart;
	double down_elapsed_time = isAccelerate ? Fast_speed : Normal_speed;
	if (elapsed_time > down_elapsed_time) {
		p_y++;
		if (checkCollisionBlock()) {
			for (int i = 0; i < Matrix_Size; i++) {
				for (int j = 0; j < Matrix_Size; j++) {
					if (MatrixBlock[i][j] == 0)continue;
					int logic_x = j + x;
					int logic_y = i + y;
					if (logic_y >= 0) {
						Block::static_block[logic_y][logic_x].state = true;
						Block::static_block[logic_y][logic_x].color = type + 1;
					}
				}
			}
			if (JudgeFull()) {
				flag = 2;//代表游戏结束
				return;
			}
			//生成新方块
			initBlock();
			flag = 1;//该绘制小窗口了
		}
		else {
			y = p_y;
		}
		main_time = now_time;
	}
}

void Block::interactionLeftRight(ExMessage& msg)
{
	if (msg.ch == 7745||msg.ch==19237) {
		p_x--;
		if (checkCollisionWall() || checkCollisionBlock()) {
			p_x++;
		}
		else {
			x = p_x;
		}
		msg.ch = 0;
	}
	else if (msg.ch == 8260||msg.ch==19751) {
		p_x++;
		if (checkCollisionWall() || checkCollisionBlock()) {
			p_x--;
		}
		else {
			x = p_x;
		}
		msg.ch = 0;
	}
	return;
}

//void Block::interactionUp(ExMessage& msg)
//{	
//	int rorate = (rorate_index + 1) % Block_State;
//	if (msg.ch == 4439) {
//		msg.ch = 0;
//		if (checkCollisionBlock(rorate))return;
//		if (checkCollisionWall(rorate)) {
//			p_x--;
//			if (checkCollisionBlock(rorate)) {
//				p_x++;
//			}
//			else {
//				if (checkCollisionWall(rorate)) {
//					p_x++;
//				}
//				else {
//					x = p_x;
//					rorate_index = rorate;
//					return;
//				}
//			}
//			p_x++;
//			if (checkCollisionBlock(rorate)) {
//				p_x--;
//			}
//			else {
//				if (checkCollisionWall(rorate)) {
//					p_x--;
//				}
//				else {
//					x = p_x;
//					rorate_index = rorate;
//					return;
//				}
//			}
//		}
//		else {
//			rorate_index = rorate;
//			return;
//		}
//	}
//	return;
//}
void Block::interactionUp(ExMessage& msg)
{
	int rorate = (rorate_index + 1) % Block_State;
	if (msg.ch == 4439||msg.ch==18470) {
		msg.ch = 0;
		if (!checkCollisionBlock(rorate) && !checkCollisionWall(rorate)) {
			rorate_index = rorate;
			return;
		}
		else {
			p_x++;
			if (!checkCollisionBlock(rorate) && !checkCollisionWall(rorate)) {
				x = p_x;
				rorate_index = rorate;
				return;
			}
			else {
				p_x--;
			}
			p_x--;
			if (!checkCollisionBlock(rorate) && !checkCollisionWall(rorate)) {
				x = p_x;
				rorate_index = rorate;
				return;
			}
			else {
				p_x++;
			}
		}
		/*if (checkCollisionBlock(rorate)) {
			p_x--;
			if (checkCollisionBlock(rorate)) {
				p_x++;
			}
			else {
				if (checkCollisionWall(rorate)) {
					p_x++;
				}
				else {
					x = p_x;
					rorate_index = rorate;
					return;
				}
			}
			p_x++;
			if (checkCollisionBlock(rorate)) {
				p_x--;
			}
			else {
				if (checkCollisionWall(rorate)) {
					p_x--;
				}
				else {
					x = p_x;
					rorate_index = rorate;
					return;
				}
			}
		}
		if (checkCollisionWall(rorate)) {
			p_x--;
			if (checkCollisionBlock(rorate)) {
				p_x++;
			}
			else {
				if (checkCollisionWall(rorate)) {
					p_x++;
				}
				else {
					x = p_x;
					rorate_index = rorate;
					return;
				}
			}
			p_x++;
			if (checkCollisionBlock(rorate)) {
				p_x--;
			}
			else {
				if (checkCollisionWall(rorate)) {
					p_x--;
				}
				else {
					x = p_x;
					rorate_index = rorate;
					return;
				}
			}
		}
		else {
			rorate_index = rorate;
			return;
		}*/
	}
	return;
}

void Block::creatGame()
{
	isDownPress = false;
	isAccelerate = false;
	Normal_speed = 0.5;
	rorate_index = 0;
	for (int i = 0; i < TETRIS_HIGH; i++) {
		for (int j = 0; j < TETRIS_WIDE; j++) {
			static_block[i][j].state = false;
			static_block[i][j].color = 0;
		}
	}
}

void Block::interactionTool(ExMessage&msg,LARGE_INTEGER&tool_time,double&accumulate_time,int&state_flag,int&boom_flag, void(*drawUI)(int y),int& line)
{	
	if (timeTick(tool_time,accumulate_time)) {
		boom_flag = 1;
		if (msg.ch == 8775) {
			Eliminate(drawUI,line);
			accumulate_time = 0.0;
			state_flag = 1;
			QueryPerformanceCounter(&tool_time);
			boom_flag = 0;
			return;
		}
		QueryPerformanceCounter(&tool_time);
		return;
	}
	boom_flag = 0;
	QueryPerformanceCounter(&tool_time);
}
void Block::interactionSpace(ExMessage& msg, int& flag)
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = BlockData::inquireMatrix(type, rorate_index);
	if (msg.ch==14624) {
		p_x = x;
		p_y = draw_y;
		x = x;
		y =	draw_y;
		msg.ch = 0;
		flushmessage(EX_KEY);
		for (int i = 0; i < Matrix_Size; i++) {
			for (int j = 0; j < Matrix_Size; j++) {
				if (MatrixBlock[i][j]) {
					int logic_x = j + x;
					int logic_y = i + y;
					static_block[logic_y][logic_x].state = true;
					static_block[logic_y][logic_x].color = type + 1;
				}
			}
		}
		if (JudgeFull())flag = 2;
		initBlock();
		flag = 1;
		mciSendString(L"play res/low4.wav", 0, 0, 0);
	}
}
void Block::HandyEliminate(void(*drawUI)(int y), int& score, int& speed, int& line)
{	
	Eliminate(drawUI,score,speed,line);
}
//void Block::useTool(void(*drawUI)(int y), int& score, int& speed, int& line)
//{	
//	static int flag = 0;
//	static LARGE_INTEGER current_time, last_time, frequency;
//	if (!flag) {
//		last_time.QuadPart = 0;
//		flag = 1;
//	}
//	QueryPerformanceFrequency(&frequency);
//	QueryPerformanceCounter(&current_time);
//	double replased_time = (double)(current_time.QuadPart - last_time.QuadPart) / frequency.QuadPart;
//	static double accumulate_time=0.0;
//	accumulate_time += replased_time;
//	last_time = current_time;
//	if (accumulate_time > 30.0) {
//		Eliminate(drawUI, score, speed, line, 1);
//		accumulate_time = 0.0;
//	}
//}
bool Block::JudgeFull()
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = getBlockShape(type, rorate_index);
	for (int i = 0; i < Matrix_Size; i++) {
		for (int j = 0; j < Matrix_Size; j++) {
			if (MatrixBlock[i][j]) {
				int logic_y = y + i;
				if (logic_y >= 0)return false;
				else return true;
			}
		}
	}
	return false;
}
bool Block::timeTick(LARGE_INTEGER& last_time,double& accumulate_time)
{
	LARGE_INTEGER current_time, frequency;
	QueryPerformanceCounter(&current_time);
	QueryPerformanceFrequency(&frequency);
	double elpased_time = (double)(current_time.QuadPart - last_time.QuadPart) / frequency.QuadPart;
	accumulate_time += elpased_time;
	if (accumulate_time >= 30)return true;
	return false;
}
int Block::getBottom(const int(&MatrixBlock)[Matrix_Size][Matrix_Size], int j)
{
	int low = -1;
	for (int i = 0; i < Matrix_Size; i++) {
		if (MatrixBlock[i][j]) {
			if (i > low)low = i;
		}
	}
	return low;
}
int Block::getX()
{
	return x;
}
int Block::getY()
{
	return y;
}
int Block::getType()
{
	return type;
}
int Block::getRorate_index() {
	return rorate_index;
}
int Block::getp_type()
{
	return p_type;
}
int Block::getline()
{
	int ret;
	for (int i = 0; i < TETRIS_HIGH; i++) {
		for (int j = 0; j < TETRIS_WIDE; j++) {
			if (static_block[i][j].state) {
				ret = 20 - i;
				return ret;
			}
		}
	}
	return 0;
}
int Block::getDrawX()
{
	return x;
}
int Block::getDrawY()
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = BlockData::inquireMatrix(type, rorate_index);
	int low_y = BlockData::inquireMatrixHigh(type, rorate_index);
	int p_draw_y = 0;
	//int bottom = 0;
	//for (int j = x; j <x+Matrix_Size; j++) {
	//	bottom = getBottom(MatrixBlock, j-x);
	//	if (bottom == -1)continue;
	//	for (int i = bottom + y; i < TETRIS_HIGH; i++) {
	//		if (static_block[i][j].state) {
	//			if (draw_y > i - 1)
	//				draw_y = i - 1;
	//				low_y = bottom;
	//				break;
	//		}
	//	}
	//}
	draw_y= p_draw_y - low_y;
	while (1) {
		if (checkCollisionBlock(x, draw_y) || checkCollisionWall(x, draw_y)) {
			draw_y--;
			return draw_y;
		}
		else {
			draw_y++;
		}
	}
	return draw_y;
}

bool Block::checkCollisionWall(int rorate)
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = getBlockShape(type, rorate);
	for (int i = 0; i < Matrix_Size; i++) {
		for (int j = 0; j < Matrix_Size; j++) {
			if (MatrixBlock[i][j]) {
				int logic_x = j + p_x;
				int logic_y = i + p_y;
				if (logic_x < 0 || logic_x >= TETRIS_WIDE)return true;
				if (logic_y >= TETRIS_HIGH)return true;
			}
		}
	}
	return false;
}

bool Block::checkCollisionWall()
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = getBlockShape(type, rorate_index);
	for (int i = 0; i < Matrix_Size; i++) {
		for (int j = 0; j < Matrix_Size; j++) {
			if (MatrixBlock[i][j]) {
				int logic_x = j + p_x;
				int logic_y = i + p_y;
				if (logic_x < 0 || logic_x >= TETRIS_WIDE)return true;
				if (logic_y >= TETRIS_HIGH)return true;
			}
		}
	}
	return false;
}

bool Block::checkCollisionWall(int p_x,int p_y)
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = getBlockShape(type, rorate_index);
	for (int i = 0; i < Matrix_Size; i++) {
		for (int j = 0; j < Matrix_Size; j++) {
			if (MatrixBlock[i][j]) {
				int logic_x = j + p_x;
				int logic_y = i + p_y;
				if (logic_y >= TETRIS_HIGH)return true;
			}
		}
	}
	return false;
}
bool Block::checkCollisionBlock()
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = getBlockShape(type, rorate_index);
	for (int i = 0; i < Matrix_Size; i++) {
		for (int j = 0; j < Matrix_Size; j++) {
			if (MatrixBlock[i][j]) {
				int logic_x = j + p_x;
				int logic_y = i + p_y;
				if (logic_y == TETRIS_HIGH)return true;
				if (logic_y >= 0 && Block::static_block[logic_y][logic_x].state)return true;
			}
		}
	}
	return false;
}
bool Block::checkCollisionBlock(int p_x,int p_y)
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = getBlockShape(type, rorate_index);
	for (int i = 0; i < Matrix_Size; i++) {
		for (int j = 0; j < Matrix_Size; j++) {
			if (MatrixBlock[i][j]) {
				int logic_x = j + p_x;
				int logic_y = i + p_y;
				if (logic_y >= 0 && Block::static_block[logic_y][logic_x].state)return true;
			}
		}
	}
	return false;
}
bool Block::checkCollisionBlock(int rorate)
{
	const int(&MatrixBlock)[Matrix_Size][Matrix_Size] = getBlockShape(type, rorate);
	for (int i = 0; i < Matrix_Size; i++) {
		for (int j = 0; j < Matrix_Size; j++) {
			if (MatrixBlock[i][j]) {
				int logic_x = j + p_x;
				int logic_y = i + p_y;
				if (logic_y == TETRIS_HIGH)return true;
				if (logic_y >= 0 && Block::static_block[logic_y][logic_x].state)return true;
			}
		}
	}
	return false;
}

const int(&Block::getBlockShape(int Type, int State)const)[Matrix_Size][Matrix_Size]
{
	return BlockData::Block_Shape[Type][State];
}

void Block::Eliminate(void(*drawUI)(int y), int& score, int& speed, int& line)
{
	int eliminate_count = 0;//记录消除的行数量
	int eliminate_remerber[TETRIS_HIGH];//记录需要消除的行数（从大到小顺序）
	//从底部开始判断，如果一行都是true就说明要消除
	for (int i = TETRIS_HIGH - 1; i >= 0; i--) {
		for (int j = 0; j < TETRIS_WIDE; j++) {
			if (!(Block::static_block[i][j].state))goto loop;
		}
		eliminate_remerber[eliminate_count++] = i;
	loop: int m = 0;
	}
	if (eliminate_count) {
		if (Normal_speed > 0.30)Normal_speed -= 0.01 * eliminate_count;
		score += 10 * eliminate_count;
		speed = score / 100 + 1;
		//先把记录的行给消除
		for (int i = 0; i < eliminate_count; i++) {
			for (int j = 0; j < TETRIS_WIDE; j++) {
				Block::static_block[eliminate_remerber[i]][j].state = false;
				Block::static_block[eliminate_remerber[i]][j].color = 0;
			}
		}
		//模拟物理，对有方块的格子进行下降模拟处理
		int eliminate = 0;
		for (int i = 0; i < eliminate_count; i++) {
			int temp = eliminate_remerber[i] + eliminate;
			for (int j = temp; j > 0; j--) {
				for (int t = 0; t < TETRIS_WIDE; t++) {
					Block::static_block[j][t] = Block::static_block[j - 1][t];
				}
			}
			eliminate++;
		}
		for (int i = 0; i < eliminate_count; i++) {
			int seed = rand() % 2;
			switch (seed) {
			case 0:
				mciSendString(L"play res/xiaochu1.wav", 0, 0, 0);
				break;
			case 1:
				mciSendString(L"play res/xiaochu2.wav", 0, 0, 0);
				break;
			}
			drawUI(eliminate_remerber[i]);
		}
	}
	return;
}
void Block::Eliminate(void(*drawUI)(int y),int& line)
{
	int eliminate_count = 0;//记录消除的行数量
	int eliminate_remerber[TETRIS_HIGH];//记录需要消除的行数（从大到小顺序）
	if (line != 0) {
		eliminate_count = 1;
		int RandomLine = TETRIS_HIGH - (rand() % line + 1);
		eliminate_remerber[0] = RandomLine;
	}
	if (eliminate_count) {
		//先把记录的行给消除
		for (int i = 0; i < eliminate_count; i++) {
		for (int j = 0; j < TETRIS_WIDE; j++) {
			Block::static_block[eliminate_remerber[i]][j].state = false;
			Block::static_block[eliminate_remerber[i]][j].color = 0;
		}
	}
	//模拟物理，对有方块的格子进行下降模拟处理
	int eliminate = 0;
	for (int i = 0; i < eliminate_count; i++) {
		int temp = eliminate_remerber[i] + eliminate;
		for (int j = temp; j > 0; j--) {
			for (int t = 0; t < TETRIS_WIDE; t++) {
				Block::static_block[j][t] = Block::static_block[j - 1][t];
			}
		}
		eliminate++;
	}
	for (int i = 0; i < eliminate_count; i++) {
		int seed = rand() % 2;
		//播放特效
		switch (seed) {
		case 0:
			mciSendString(L"play res/xiaochu1.wav", 0, 0, 0);
			break;
		case 1:
			mciSendString(L"play res/xiaochu2.wav", 0, 0, 0);
			break;
		}
		//绘制UI
		drawUI(eliminate_remerber[i]);
		}
	}
	return;
}
const int(&BlockData::inquireMatrix(int Type, int State))[Matrix_Size][Matrix_Size]
{
	return Block_Shape[Type][State];
}

int BlockData::inquireMatrixHigh(int Type, int State)
{
	return Block_High[Type][State];
}
