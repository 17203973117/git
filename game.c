#include "game.h"

/* 清空输入缓冲区：把这一行没读完的字符全部吃掉，防止程序死循环 */
void FlushInput()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) // 一直读到换行符或文件结束
        ;
}

/* 初始化棋盘：把 row*col 范围内每个格子都填成 ret */
void Initboard(char board[ROWS][COLS], int row, int col, char ret)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < row; i++)       // 外层循环：每一行
    {
        for (j = 0; j < col; j++)   // 内层循环：每一列
        {
            board[i][j] = ret;      // 填入初始字符（雷盘填'0'，玩家盘填'*'）
        }
    }
}

/* 打印棋盘
   show_mine == 1 时用于展示最终答案，'1' 显示成 '@'（雷） */
void DisplayBoard(char board[ROWS][COLS], int row, int col, int show_mine)
{
    int i = 0;
    int j = 0;

    printf("   ");                  // 先打印 3 个空格，占住左上角
    for (j = 1; j <= col; j++)
    {
        printf("%2d ", j);          // 打印每一列的编号
    }
    printf("\n");

    for (i = 1; i <= row; i++)      // 从第 1 行打印到第 row 行
    {
        printf("%2d ", i);          // 打印行号
        for (j = 1; j <= col; j++)
        {
            char c = board[i][j];
            if (show_mine && c == '1')
            {
                c = '@';            // 答案模式下，'1' 表示雷，显示成 @
            }
            else if (c == '0')
            {
                c = ' ';            // 数字 0 显示成空格，界面更干净
            }
            printf(" %c ", c);      // 每个格子固定占 3 个字符宽度，行列对齐
        }
        printf("\n");
    }
}

/* 随机布雷：在 1~row、1~col 里随机放 mine_count 颗雷，雷用 '1' 表示 */
void Set(char board[ROWS][COLS], int row, int col, int mine_count)
{
    int count = mine_count;         // 还需要放 count 颗雷
    while (count > 0)
    {
        int x = rand() % row + 1;   // rand()%row 得到 0~row-1，+1 变成 1~row
        int y = rand() % col + 1;   // 列号同理
        if (board[x][y] == '0')     // 这个格子还是空的
        {
            board[x][y] = '1';      // 放一颗雷
            count--;                // 还差一颗
        }
    }
}

/* 统计 (x,y) 周围 3x3 区域里有多少颗雷 */
int CountMine(char board[ROWS][COLS], int x, int y)
{
    int i = 0;
    int j = 0;
    int cnt = 0;                    // 雷数计数器
    for (i = x - 1; i <= x + 1; i++)// 行从 x-1 到 x+1
    {
        for (j = y - 1; j <= y + 1; j++) // 列从 y-1 到 y+1
        {
            if (board[i][j] == '1') // 发现一颗雷
            {
                cnt++;              // 计数 +1
            }
        }
    }
    return cnt;                     // 返回周围雷数
}

/* 统计玩家已经插了多少面旗（'!'） */
static int CountFlags(char show[ROWS][COLS], int row, int col)
{
    int i = 0;
    int j = 0;
    int cnt = 0;
    for (i = 1; i <= row; i++)
    {
        for (j = 1; j <= col; j++)
        {
            if (show[i][j] == '!')  // 这个格子插了旗
            {
                cnt++;
            }
        }
    }
    return cnt;
}

/* 翻开 (x,y)：
   如果它周围没有雷（是'0'），就递归地把周围 8 格一起翻开，形成连片展开 */
void Open(char board[ROWS][COLS], char show[ROWS][COLS],
          int row, int col, int x, int y, int* win)
{
    int dx = 0;
    int dy = 0;

    if (x < 1 || x > row || y < 1 || y > col) // 坐标越界就返回
    {
        return;
    }
    if (show[x][y] != '*')          // 已翻开或已插旗的格子不再处理
    {
        return;
    }
    if (board[x][y] == '1')         // 雷不在这里展开（踩雷由上层处理）
    {
        return;
    }

    int cnt = CountMine(board, x, y);   // 先算好周围有几颗雷
    show[x][y] = cnt + '0';             // 把周围雷数转成字符写进玩家盘
    (*win)++;                           // 已翻开的格子数 +1

    if (cnt == 0)                   // 周围没有雷，才继续向四周扩散
    {
        for (dx = -1; dx <= 1; dx++)// 遍历周围 3x3
        {
            for (dy = -1; dy <= 1; dy++)
            {
                if (dx == 0 && dy == 0) // 跳过自己这一格
                {
                    continue;
                }
                Open(board, show, row, col, x + dx, y + dy, win); // 递归翻开邻居
            }
        }
    }
}

/* 游戏主流程：循环让玩家输入，直到翻开所有安全格或踩雷 */
void Find(char board[ROWS][COLS], char show[ROWS][COLS],
          int row, int col, int mine_count)
{
    int x = 0;
    int y = 0;
    int win = 0;                    // 已翻开的格子数
    int first = 1;                  // 是否第一次点击（1 是，0 否）
    int safe = row * col - mine_count; // 非雷格子总数，全部翻开就赢
    char cmd[16] = { 0 };           // 存放玩家输入的命令字符串
    time_t start = time(NULL);      // 记录开局时间，用于计时

    while (win < safe)              // 只要没翻完安全格就一直循环
    {
        printf("\n");
        DisplayBoard(show, row, col, 0); // 打印玩家看到的棋盘
        printf("已翻开 %d/%d 格，剩余雷数 %d\n",
               win, safe, mine_count - CountFlags(show, row, col));

        printf("输入\"行 列\"翻开（如 3 5）；输入 f 行 列 标记/取消标记；输入 q 返回菜单\n");
        printf("请选择：");
        if (scanf("%15s", cmd) != 1) // 读不到内容说明输入异常
        {
            FlushInput();           // 清掉坏输入
            printf("输入无效，请重新输入\n");
            continue;               // 回到循环开头重来
        }

        if (cmd[0] == 'q' || cmd[0] == 'Q') // 输入 q 退出本局
        {
            printf("已返回菜单\n");
            return;
        }

        /* 插旗 / 取消插旗 */
        if (cmd[0] == 'f' || cmd[0] == 'F')
        {
            if (scanf("%d %d", &x, &y) != 2) // f 后面必须跟两个数字
            {
                FlushInput();
                printf("标记格式应为：f 行 列\n");
                continue;
            }
            if (x < 1 || x > row || y < 1 || y > col) // 坐标越界
            {
                printf("坐标越界，请重新输入\n");
                continue;
            }
            if (show[x][y] == '*')  // 没翻开的格子才能插旗
            {
                show[x][y] = '!';   // 插上一面旗
            }
            else if (show[x][y] == '!') // 已经插了旗
            {
                show[x][y] = '*';   // 再输入一次就取消旗
            }
            else
            {
                printf("该位置已翻开，不能标记\n");
            }
            continue;               // 标记处理完，回到循环开头
        }

        /* 翻开：cmd 里存的是行号 */
        x = atoi(cmd);              // 把字符串 "3" 转成数字 3
        if (x == 0 && cmd[0] != '0') // 转出来是 0 又不是 "0"，说明不是数字
        {
            FlushInput();
            printf("无法识别的指令，请重新输入\n");
            continue;
        }
        if (scanf("%d", &y) != 1)   // 再读一个列号
        {
            FlushInput();
            printf("翻开格式应为：行 列\n");
            continue;
        }

        if (x < 1 || x > row || y < 1 || y > col) // 坐标越界检查
        {
            printf("坐标越界，请重新输入\n");
            continue;
        }
        if (show[x][y] == '!')      // 插了旗的格子不能直接翻开
        {
            printf("该位置已标记，如需翻开请先取消标记\n");
            continue;
        }
        if (show[x][y] != '*')      // 已经翻过的格子
        {
            printf("该位置已翻开\n");
            continue;
        }

        /* 第一次点击保护：如果首点踩雷，把雷移走，保证首开安全 */
        if (first)
        {
            first = 0;              // 之后不再是第一次了
            if (board[x][y] == '1') // 首点恰好是雷
            {
                int nx = 0;
                int ny = 0;
                board[x][y] = '0';  // 先把这颗雷去掉
                do
                {
                    nx = rand() % row + 1; // 随机找一个新位置
                    ny = rand() % col + 1;
                } while (board[nx][ny] == '1' || (nx == x && ny == y)); // 直到该格没雷且不是刚才那格
                board[nx][ny] = '1'; // 把雷放到新位置
            }
        }

        /* 踩雷：游戏结束 */
        if (board[x][y] == '1')
        {
            printf("\n很遗憾，你踩到雷了！用时 %.0f 秒\n",
                   difftime(time(NULL), start));
            DisplayBoard(board, row, col, 1); // 展示答案（@ 是雷）
            return;
        }

        /* 安全：翻开这格，并递归展开周围的空白区域 */
        Open(board, show, row, col, x, y, &win);
    }

    /* 循环结束说明所有安全格都翻开了，玩家获胜 */
    printf("\n恭喜你，排雷成功！用时 %.0f 秒\n", difftime(time(NULL), start));
    DisplayBoard(board, row, col, 1); // 展示答案
}
