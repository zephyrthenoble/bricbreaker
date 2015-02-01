#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define X_COORD 0
#define Y_COORD 1

#define BLANK_STR " "
#define BALL_STR "o"

#define PADDLE_COLOR 1
#define BALL_COLOR 2
#define ROW_0 3
#define ROW_1 4
#define ROW_2 5

#define DELAY 10000
#define PADDLE_SPEED 3
#define BALL_DELAY 9
#define NUM_ROWS 3

void move_right();
void move_left();
void check_collisions();

int paddle_left[] = {0, 0}, paddle_mid[] = {0, 0}, paddle_right[] = {0, 0};
float ball[] = {0, 0};
float ball_vel[] = {1.0, -1.0};
int ball_pix[] = {0, 0};

int scr_height = 0, scr_width = 0;

int main(int argc, char *argv[]) {
    WINDOW *window;
    if ((window = initscr()) == NULL) {
        fprintf(stderr, "Error initializing ncurses.\n");
        exit(EXIT_FAILURE);
    }
    getmaxyx(window, scr_height, scr_width);

    bool brick_grid[NUM_ROWS][scr_width];

    int i, j;
    for (i = 0; i < NUM_ROWS; i++) {
        for (j = 0; j < scr_width; j++) {
            brick_grid[i][j] = TRUE;
        }
    }

    noecho();
    keypad(window, TRUE);
    nodelay(window, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(PADDLE_COLOR, COLOR_BLUE, COLOR_BLUE);
        init_pair(BALL_COLOR, COLOR_WHITE, COLOR_BLACK);
        init_pair(ROW_0, COLOR_RED, COLOR_RED);
        init_pair(ROW_1, COLOR_YELLOW, COLOR_YELLOW);
        init_pair(ROW_2, COLOR_GREEN, COLOR_GREEN);
    }

    int mid_x = scr_width / 2;
    paddle_left[X_COORD] = mid_x - 1;
    paddle_left[Y_COORD] = scr_height - 2;
    paddle_mid[X_COORD] = mid_x;
    paddle_mid[Y_COORD] = scr_height - 2;
    paddle_right[X_COORD] = mid_x + 1;
    paddle_right[Y_COORD] = scr_height - 2;
    ball[X_COORD] = mid_x;
    ball[Y_COORD] = scr_height - 4;
    ball_pix[X_COORD] = (int)ball[X_COORD];
    ball_pix[Y_COORD] = (int)ball[Y_COORD];

    int key_press;
    while (TRUE) {
        clear();

        key_press = getch();
        if (key_press == KEY_BACKSPACE)
            break;
        switch (key_press) {
            case KEY_RIGHT:
                move_right();
                break;
            case KEY_LEFT:
                move_left();
                break;
            default:
                break;
        }

        check_collisions();

        for (i = 0; i < NUM_ROWS; i++) {
            for (j = 0; j < scr_width; j++) {
                if (brick_grid[i][j] == TRUE) {
                    if (ball_pix[X_COORD] == j && ball_pix[Y_COORD] == i) {
                        ball_vel[Y_COORD] *= -1;
                        brick_grid[i][j] = FALSE;
                    }
                }
            }
        }

        ball[X_COORD] += ball_vel[X_COORD] / BALL_DELAY;
        ball[Y_COORD] += ball_vel[Y_COORD] / BALL_DELAY;

        ball_pix[X_COORD] = (int)ball[X_COORD];
        ball_pix[Y_COORD] = (int)ball[Y_COORD];

        attrset(COLOR_PAIR(PADDLE_COLOR));
        mvprintw(paddle_left[Y_COORD], paddle_left[X_COORD], BLANK_STR);
        mvprintw(paddle_mid[Y_COORD], paddle_mid[X_COORD], BLANK_STR);
        mvprintw(paddle_right[Y_COORD], paddle_right[X_COORD], BLANK_STR);

        attrset(COLOR_PAIR(BALL_COLOR));
        mvprintw(ball_pix[Y_COORD], ball_pix[X_COORD], BALL_STR);

        for (i = 0; i < NUM_ROWS; i++) {
            for (j = 0; j < scr_width; j++) {
                if (brick_grid[i][j] == TRUE) {
                    int cur_color;
                    switch (i) {
                        case 0:
                            cur_color = ROW_0;
                            break;
                        case 1:
                            cur_color = ROW_1;
                            break;
                        case 2:
                            cur_color = ROW_2;
                            break;
                        default:
                            cur_color = ROW_0;
                            break;
                    }
                    attrset(COLOR_PAIR(cur_color));
                    mvprintw(i, j, BLANK_STR);
                }
            }
        }

        refresh();
        usleep(DELAY);
    }

    endwin();

    return 0;
}

void move_right() {
    if (paddle_right[X_COORD] > scr_width - 1 - PADDLE_SPEED) {
        paddle_left[X_COORD] = scr_width - 1;
        paddle_mid[X_COORD] = scr_width - 1 - 1;
        paddle_right[X_COORD] = scr_width - 1 - 2;
        return;
    }
    paddle_left[X_COORD] += PADDLE_SPEED;
    paddle_mid[X_COORD] += PADDLE_SPEED;
    paddle_right[X_COORD] += PADDLE_SPEED;
}

void move_left() {
    if (paddle_left[X_COORD] < PADDLE_SPEED + 2) {
        paddle_left[X_COORD] = 0;
        paddle_mid[X_COORD] = 1;
        paddle_right[X_COORD] = 2;
        return;
    }
    paddle_left[X_COORD] -= PADDLE_SPEED;
    paddle_mid[X_COORD] -= PADDLE_SPEED;
    paddle_right[X_COORD] -= PADDLE_SPEED;
}

void check_collisions() {
    if (ball_pix[X_COORD] == scr_width - 1 || ball_pix[X_COORD] == 0)
        ball_vel[X_COORD] *= -1;
    if (ball_pix[Y_COORD] == 0)
        ball_vel[Y_COORD] *= -1;
    if (ball_pix[X_COORD] == paddle_left[X_COORD] && ball_pix[Y_COORD] == paddle_left[Y_COORD]) {
        if (ball_vel[X_COORD] < 0)
            ball_vel[X_COORD] *= -1;
        ball_vel[Y_COORD] *= -1;
    } else if (ball_pix[X_COORD] == paddle_mid[X_COORD] && ball_pix[Y_COORD] == paddle_mid[Y_COORD]) {
        ball_vel[Y_COORD] *= -1;
    } else if (ball_pix[X_COORD] == paddle_right[X_COORD] && ball_pix[Y_COORD] == paddle_right[Y_COORD]) {
        if (ball_vel[X_COORD] > 0)
            ball_vel[X_COORD] *= -1;
        ball_vel[Y_COORD] *= -1;
    }
}