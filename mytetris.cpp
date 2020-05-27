#include <ctime>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//define blocks. first eight numbers for coordinates, last two numbers for side lengths
int block[7][4][20]=
{
    {// I
        {0, 0, 0, 1, 0, 2, 0, 3, 0, 3},
        {0, 0, 1, 0, 2, 0, 3, 0, 3, 0},
        {0, 0, 0, 1, 0, 2, 0, 3, 0, 3},
        {0, 0, 1, 0, 2, 0, 3, 0, 3, 0}
    },
    {// S
        {0, 0, 1, 0, 1, 1, 2, 1, 2, 1},
        {1, 0, 1, 1, 0, 1, 0, 2, 1, 2},
        {0, 0, 1, 0, 1, 1, 2, 1, 2, 1},
        {1, 0, 1, 1, 0, 1, 0, 2, 1, 2},
    },
    {// O
        {0, 0, 0, 1, 1, 0, 1, 1, 1, 1},
        {0, 0, 0, 1, 1, 0, 1, 1, 1, 1},
        {0, 0, 0, 1, 1, 0, 1, 1, 1, 1},
        {0, 0, 0, 1, 1, 0, 1, 1, 1, 1},
    },
    {// Z
        {0, 1, 1, 1, 1, 0, 2, 0, 2, 1},
        {0, 0, 0, 1, 1, 1, 1, 2, 1, 2},
        {0, 1, 1, 1, 1, 0, 2, 0, 2, 1},
        {0, 0, 0, 1, 1, 1, 1, 2, 1, 2}
    },
    {// T
        {1, 0, 1, 1, 1, 2, 0, 1, 1, 2},
        {0, 0, 1, 0, 2, 0, 1, 1, 2, 1},
        {0, 0, 0, 1, 0, 2, 1, 1, 1, 2},
        {0, 1, 1, 1, 2, 1, 1, 0, 2, 1}
    },
    {// L
        {0, 0, 0, 1, 1, 1, 1, 2, 1, 2},
        {0, 1, 1, 1, 1, 0, 2, 0, 2, 1},
        {0, 0, 0, 1, 1, 1, 1, 2, 1, 2},
        {0, 1, 1, 1, 1, 0, 2, 0, 2, 1}
    },
    {// J
        {0, 0, 1, 0, 2, 0, 2, 1, 2, 1},
        {0, 0, 0, 1, 0, 2, 1, 0, 1, 2},
        {0, 0, 0, 1, 1, 1, 2, 1, 2, 1},
        {1, 0, 1, 1, 1, 2, 0, 2, 1, 2}
    }

};

int hold, last_block, now_block, next_block, interface[25][25];
int x, y, r, lx, ly, lr;
int score;

//generate a new block
void new_block()
{
    lx = x = 3;
    ly = y = 0;
    lr = r = 0;
    now_block = rand() % 7;
}

//set the state of the interface
void set_block(int x, int y, int r, int t)
{
    for (int i = 0; i < 4; i ++) 
        interface[block[now_block][r][i * 2] + y][block[now_block][r][i * 2 + 1] + x] = t;
}

//check if the block can move
bool check(int x, int y, int r)
{
    if (y + block[now_block][r][8] > 19)
        return false;
    set_block(lx, ly, lr, 0);
    bool flag = true;
    for (int i = 0; i < 4; i++) 
    {
        if(interface[y + block[now_block][r][i * 2]][x + block[now_block][r][i * 2 + 1]])
            flag = false;
    }
    set_block(lx, ly, lr, now_block + 1);
    return flag;
}

//remove filled rows
void remove_row() 
{
  for (int row = y; row <= y + block[now_block][r][8]; row++) 
  {
    bool flag = true;
    for (int i = 0; i < 10; i++)
        if(!interface[row][i])
            flag = false;
    if (!flag)
      continue;
    for (int i = row - 1; i > 0; i--) {
      memcpy(&interface[i + 1][0], &interface[i][0], 40);
    }
    memset(&interface[0][0], 0, 10);
    score++;
  }
}

//update the position of block in interface
void update() {
  set_block(lx, ly, lr, 0);
  set_block(lx = x, ly = y, lr = r, now_block + 1);
}

//block fall down
bool tick()
{
    static int t = 0;
    if(++t > 30)
    {
        t = 0;
        if(!check(x, y + 1, r))
        {
            if(!y)
                return false;
            remove_row();
            new_block();
        }
        else
        {
            y++;
            update();
        }
    }
    return true;
}

//draw the interface
void draw()
{
    for (int i = 1; i < 20; i++) 
    {
        move(1 + i, 1);
        for (int j = 0; j < 10; j++) 
        {
            if(interface[i][j])
                attron(COLOR_PAIR(interface[i][j]));
            printw("  ");
            if(interface[i][j])
                attroff(COLOR_PAIR(interface[i][j]));
        }
    }
    move(21, 1);
    printw("Score: %d", score);
    refresh();
}

void run()
{
    char opration;
    while(tick())
    {
        usleep(10000);
        if ((opration = getch()) == 'a' && x > 0 && check(x - 1, y, r)) 
            x--;
        if (opration == 'd' && x + block[now_block][r][9] < 9 && check(x + 1, y, r)) 
            x++;
        if (opration == 's') 
        {
            while (check(x, y + 1, r)) 
            {
                y++;
                update();
            }
            remove_row();
            new_block();
        }
        if (opration == 'w') 
        {
            ++r %= 4;
            while (x + block[now_block][r][9] > 9) 
                x--;// ensure the block won't be blocked by border
            if (!check(x, y, r)) 
            {
                x = lx;
                r = lr;
            }
        }
        if(opration == 'q') 
            return;
        update();
        draw();
    }
}

int main()
{
    srand(time(0));
    initscr();
    start_color();
    for(int i = 1; i <= 7; i++)
        init_pair(i, i, i);
    noecho();
    curs_set(0);
    timeout(0);
    resizeterm(22, 22);
    box(stdscr, 0, 0);
    new_block();
    run();
    endwin();
}