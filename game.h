#ifndef GAME_H                     // 防止头文件被重复包含
#define GAME_H

#include <stdio.h>                 // printf / scanf 等输入输出函数
#include <time.h>                  // time / difftime（计时用）
#include <stdlib.h>                // rand / srand / atoi

/* 棋盘最大尺寸（含雷区外圈的边框），实际行/列在运行时确定 */
#define MAX_ROW 32                 // 支持的最大行数
#define MAX_COL 32                 // 支持的最大列数
#define ROWS MAX_ROW + 2           // 数组总行数 = 最大行 + 上下两条边框
#define COLS MAX_COL + 2           // 数组总列数 = 最大列 + 左右两条边框

/* 默认难度：简单 9x9，10 雷 */
#define ROW 9                      // 默认行数
#define COL 9                      // 默认列数
#define EASY_COUNT 10              // 默认雷数

/* 难度配置 */
typedef struct Difficulty
{
    int row;                       // 行数
    int col;                       
    int mine_count;                // 雷数
} Difficulty;

void Initboard(char board[ROWS][COLS], int row, int col, char ret); // 把棋盘全部填成 ret
void DisplayBoard(char board[ROWS][COLS], int row, int col, int show_mine); // 打印棋盘
void Set(char board[ROWS][COLS], int row, int col, int mine_count); // 随机布雷
int CountMine(char board[ROWS][COLS], int x, int y); // 统计某格周围有几颗雷
void Open(char board[ROWS][COLS], char show[ROWS][COLS],
          int row, int col, int x, int y, int* win); // 翻开并递归展开空白区域
void Find(char board[ROWS][COLS], char show[ROWS][COLS],
          int row, int col, int mine_count); // 游戏主流程
void FlushInput();                 // 清空输入缓冲区

#endif
