#include "game.h"

/* 打印主菜单 */
void menu()
{
    printf("******************************\n");
    printf("********** 扫雷游戏 **********\n");
    printf("******** 1. 开始游戏 *********\n");
    printf("******** 2. 选择难度 *********\n");
    printf("******** 0. 退出游戏 *********\n");
    printf("******************************\n");
}

/* 让玩家选择难度，返回选中的难度配置 */
Difficulty ChooseDifficulty()
{
    int n = 0;
    Difficulty d = { ROW, COL, EASY_COUNT }; // 默认先给简单难度

    printf("\n请选择难度：\n");
    printf("1. 简单：9x9，10 雷\n");
    printf("2. 中等：16x16，40 雷\n");
    printf("3. 困难：16x30，99 雷\n");
    printf("请输入数字：");
    if (scanf("%d", &n) != 1)       // 输入的不是数字
    {
        FlushInput();               // 清掉坏输入
        printf("输入无效，保持当前难度\n");
        return d;
    }

    switch (n)
    {
    case 2:                         // 选 2：中等
        d.row = 16;
        d.col = 16;
        d.mine_count = 40;
        break;
    case 3:                         // 选 3：困难
        d.row = 16;
        d.col = 30;
        d.mine_count = 99;
        break;
    default:                        // 其他数字（包括 1）都按简单处理
        d.row = ROW;
        d.col = COL;
        d.mine_count = EASY_COUNT;
        break;
    }
    printf("已选择难度：%dx%d，%d 雷\n", d.row, d.col, d.mine_count);
    return d;
}

/* 开始一局游戏 */
void game(Difficulty d)
{
    char board[ROWS][COLS] = { 0 }; // 雷盘：'0' 空、'1' 雷
    char show[ROWS][COLS] = { 0 };  // 玩家盘：'*' 未翻、'!' 旗、数字表示周围雷数

    Initboard(board, ROWS, COLS, '0'); // 雷盘全部填 0
    Initboard(show, ROWS, COLS, '*');  // 玩家盘全部填 *
    Set(board, d.row, d.col, d.mine_count); // 随机布雷
    Find(board, show, d.row, d.col, d.mine_count); // 进入游戏主流程
}

int main()
{
    int input = 0;
    Difficulty d = { ROW, COL, EASY_COUNT }; // 默认难度：简单

    srand((unsigned int)time(NULL)); // 用当前时间做随机种子，每次布雷都不同

    do
    {
        menu();
        printf("请输入数字：");
        if (scanf("%d", &input) != 1) // 输入的不是数字
        {
            FlushInput();           // 清掉坏输入
            printf("输入无效，请重新输入\n\n");
            input = -1;             // 设成非法值，让循环继续而不是退出
            continue;
        }

        switch (input)
        {
        case 1:
            game(d);                // 用当前难度开始游戏
            break;
        case 2:
            d = ChooseDifficulty(); // 更换难度
            break;
        case 0:
            printf("游戏结束，再见！\n");
            break;
        default:
            printf("输入错误，请重新输入\n\n");
            break;
        }
    } while (input != 0);           // 输入 0 才退出整个程序

    return 0;
}
