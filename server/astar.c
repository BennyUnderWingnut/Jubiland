#include "astar.h"

#define STARTNODE    1
#define ENDNODE        2
#define BARRIER        3

typedef struct AStarNode {
    int s_y;
    int s_x;    // 坐标(最终输出路径需要)
    int s_g;    // 起点到此点的距离( 由g和h可以得到f，此处f省略，f=g+h )
    int s_h;    // 启发函数预测的此点到终点的距离
    int s_style;// 结点类型：起始点，终点，障碍物
    struct AStarNode *s_parent;    // 父节点
    int s_is_in_closetable;        // 是否在close表中
    int s_is_in_opentable;        // 是否在open表中
} AStarNode, *pAStarNode;

AStarNode map_maze[AI_SEARCH_LIMIT][AI_SEARCH_LIMIT];        // 结点数组
pAStarNode open_table[AI_SEARCH_LIMIT * AI_SEARCH_LIMIT];        // open表
pAStarNode close_table[AI_SEARCH_LIMIT * AI_SEARCH_LIMIT];        // close表
int open_node_count;// open表中节点数量
int close_node_count;     // close表中结点数量

// 交换两个元素
//
void swap(int idx1, int idx2) {
    pAStarNode tmp = open_table[idx1];
    open_table[idx1] = open_table[idx2];
    open_table[idx2] = tmp;
}

// 堆调整
//
void adjust_heap(int /*i*/nIndex) {
    int curr = nIndex;
    int child = curr * 2 + 1;    // 得到左孩子idx( 下标从0开始，所有做孩子是curr*2+1 )
    int parent = (curr - 1) / 2;    // 得到双亲idx

    if (nIndex < 0 || nIndex >= open_node_count) {
        return;
    }

    // 往下调整( 要比较左右孩子和cuur parent )
    //
    while (child < open_node_count) {
        // 小根堆是双亲值小于孩子值
        //
        if (child + 1 < open_node_count &&
            open_table[child]->s_g + open_table[child]->s_h > open_table[child + 1]->s_g + open_table[child + 1]->s_h) {
            ++child;// 判断左右孩子大小
        }

        if (open_table[curr]->s_g + open_table[curr]->s_h <= open_table[child]->s_g + open_table[child]->s_h) {
            break;
        } else {
            swap(child, curr);            // 交换节点
            curr = child;                // 再判断当前孩子节点
            child = curr * 2 + 1;            // 再判断左孩子
        }
    }

    if (curr != nIndex) {
        return;
    }

    // 往上调整( 只需要比较cuur child和parent )
    //
    while (curr != 0) {
        if (open_table[curr]->s_g + open_table[curr]->s_h >= open_table[parent]->s_g + open_table[parent]->s_h) {
            break;
        } else {
            swap(curr, parent);
            curr = parent;
            parent = (curr - 1) / 2;
        }
    }
}

// 判断邻居点是否可以进入open表
//
void insert_to_opentable(int x, int y, pAStarNode curr_node, pAStarNode end_node, int w) {
    int i;

    if (map_maze[y][x].s_style != BARRIER)        // 不是障碍物
    {
        if (!map_maze[y][x].s_is_in_closetable)    // 不在闭表中
        {
            if (map_maze[y][x].s_is_in_opentable)    // 在open表中
            {
                // 需要判断是否是一条更优化的路径
                //
                if (map_maze[y][x].s_g > curr_node->s_g + w)    // 如果更优化
                {
                    map_maze[y][x].s_g = curr_node->s_g + w;
                    map_maze[y][x].s_parent = curr_node;

                    for (i = 0; i < open_node_count; ++i) {
                        if (open_table[i]->s_x == map_maze[y][x].s_x && open_table[i]->s_y == map_maze[y][x].s_y) {
                            break;
                        }
                    }

                    adjust_heap(i);                    // 下面调整点
                }
            } else                                    // 不在open中
            {
                map_maze[y][x].s_g = curr_node->s_g + w;
                map_maze[y][x].s_h = abs(end_node->s_x - x) + abs(end_node->s_y - y);
                map_maze[y][x].s_parent = curr_node;
                map_maze[y][x].s_is_in_opentable = 1;
                open_table[open_node_count++] = &(map_maze[y][x]);
            }
        }
    }
}

// 查找邻居
// 对上下左右8个邻居进行查找
//
void get_neighbors(pAStarNode curr_node, pAStarNode end_node) {
    int x = curr_node->s_x;
    int y = curr_node->s_y;

    // 下面对于8个邻居进行处理！
    //
    if ((x + 1) >= 0 && (x + 1) < AI_SEARCH_LIMIT && y >= 0 && y < AI_SEARCH_LIMIT) {
        insert_to_opentable(x + 1, y, curr_node, end_node, 10);
    }

    if ((x - 1) >= 0 && (x - 1) < AI_SEARCH_LIMIT && y >= 0 && y < AI_SEARCH_LIMIT) {
        insert_to_opentable(x - 1, y, curr_node, end_node, 10);
    }

    if (x >= 0 && x < AI_SEARCH_LIMIT && (y + 1) >= 0 && (y + 1) < AI_SEARCH_LIMIT) {
        insert_to_opentable(x, y + 1, curr_node, end_node, 10);
    }

    if (x >= 0 && x < AI_SEARCH_LIMIT && (y - 1) >= 0 && (y - 1) < AI_SEARCH_LIMIT) {
        insert_to_opentable(x, y - 1, curr_node, end_node, 10);
    }

    if ((x + 1) >= 0 && (x + 1) < AI_SEARCH_LIMIT && (y + 1) >= 0 && (y + 1) < AI_SEARCH_LIMIT) {
        insert_to_opentable(x + 1, y + 1, curr_node, end_node, 14);
    }

    if ((x + 1) >= 0 && (x + 1) < AI_SEARCH_LIMIT && (y - 1) >= 0 && (y - 1) < AI_SEARCH_LIMIT) {
        insert_to_opentable(x + 1, y - 1, curr_node, end_node, 14);
    }

    if ((x - 1) >= 0 && (x - 1) < AI_SEARCH_LIMIT && (y + 1) >= 0 && (y + 1) < AI_SEARCH_LIMIT) {
        insert_to_opentable(x - 1, y + 1, curr_node, end_node, 14);
    }

    if ((x - 1) >= 0 && (x - 1) < AI_SEARCH_LIMIT && (y - 1) >= 0 && (y - 1) < AI_SEARCH_LIMIT) {
        insert_to_opentable(x - 1, y - 1, curr_node, end_node, 14);
    }
}


int get_next_pos(int start_y, int start_x, int end_y, int end_x, int *next_y, int *next_x) {
    AStarNode *start_node;            // 起始点
    AStarNode *end_node;            // 结束点
    AStarNode *curr_node;            // 当前点
    int is_found = 0;            // 是否找到路径
    int maze[AI_SEARCH_LIMIT][AI_SEARCH_LIMIT];
    int i, j;
    int search_start_x = start_x - AI_SEARCH_LIMIT / 2;
    int search_start_y = start_y - AI_SEARCH_LIMIT / 2;

    open_node_count = 0;
    close_node_count = 0;

    for (i = 0; i < AI_SEARCH_LIMIT; i++)
        for (j = 0; j < AI_SEARCH_LIMIT; j++)
            if (search_start_y + i == start_y && search_start_x + j == start_x)
                maze[i][j] = STARTNODE;
            else if (search_start_y + i == end_y && search_start_x + j == end_x)
                maze[i][j] = ENDNODE;
            else if (is_move_okay(search_start_y + i, search_start_x + j))
                maze[i][j] = 0;
            else maze[i][j] = BARRIER;

    for (i = 0; i < AI_SEARCH_LIMIT; ++i) {
        for (j = 0; j < AI_SEARCH_LIMIT; ++j) {
            map_maze[i][j].s_g = 0;
            map_maze[i][j].s_h = 0;
            map_maze[i][j].s_is_in_closetable = 0;
            map_maze[i][j].s_is_in_opentable = 0;
            map_maze[i][j].s_style = maze[i][j];
            map_maze[i][j].s_y = i;
            map_maze[i][j].s_x = j;
            map_maze[i][j].s_parent = NULL;

            if (map_maze[i][j].s_style == STARTNODE)    // 起点
            {
                start_node = &(map_maze[i][j]);
            } else if (map_maze[i][j].s_style == ENDNODE)    // 终点
            {
                end_node = &(map_maze[i][j]);
            }
        }
    }

    // 下面使用A*算法得到路径
    //
    open_table[open_node_count++] = start_node;            // 起始点加入open表

    start_node->s_is_in_opentable = 1;                // 加入open表
    start_node->s_g = 0;
    start_node->s_h = abs(end_node->s_x - start_node->s_x) + abs(end_node->s_y - start_node->s_y);
    start_node->s_parent = NULL;


    if (start_node->s_x == end_node->s_x && start_node->s_y == end_node->s_y) {
        return 0;
    }

    while (1) {
        curr_node = open_table[0];        // open表的第一个点一定是f值最小的点(通过堆排序得到的)
        open_table[0] = open_table[--open_node_count];    // 最后一个点放到第一个点，然后进行堆调整
        adjust_heap(0);                // 调整堆

        close_table[close_node_count++] = curr_node;    // 当前点加入close表
        curr_node->s_is_in_closetable = 1;        // 已经在close表中了

        if (curr_node->s_x == end_node->s_x && curr_node->s_y == end_node->s_y)// 终点在close中，结束
        {
            is_found = 1;
            break;
        }

        get_neighbors(curr_node, end_node);            // 对邻居的处理

        if (open_node_count == 0)                // 没有路径到达
        {
            is_found = 0;
            break;
        }
    }

    if (is_found) {
        curr_node = end_node;
        while (curr_node->s_parent->s_parent) {
            curr_node = curr_node->s_parent;
        }
        *next_y = search_start_y + curr_node->s_y;
        *next_x = search_start_x + curr_node->s_x;
    } else {
        return -1;
    }
    return 0;
}