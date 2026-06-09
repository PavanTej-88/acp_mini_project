#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>

#define ROWS        22
#define COLS        60
#define MAX_SHAPES  20

#define SHAPE_CIRCLE    'C'
#define SHAPE_RECT      'R'
#define SHAPE_LINE      'L'
#define SHAPE_TRIANGLE  'T'

#define FILL_CHAR   '*'
#define BORDER_CHAR '_'

#define MENU_WIDTH   22

WINDOW *win1;
WINDOW *win2;

typedef struct {
    int  num;
    char kind;
    int  a, b;
    int  p, q;
    char symbol;
    int  flag;
} Shape;

char grid[ROWS][COLS];
Shape list[MAX_SHAPES];
int total = 0;
int counter = 1;

void clear_canvas(void) {
    int i, j;
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++)
            grid[i][j] = ' ';
}

void plot(int r, int c, char ch) {
    if (r >= 0 && r < ROWS && c >= 0 && c < COLS)
        grid[r][c] = ch;
}

void display_canvas_ncurses(void) {
    werase(win2);
    box(win2, 0, 0);
    mvwprintw(win2, 0, 2, " CANVAS ");

    int i, j;
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++)
            mvwaddch(win2, i + 1, j + 1, grid[i][j]);

    wrefresh(win2);
}

void draw_circle(int cx, int cy, int r, char ch) {
    int row, col;
    for (row = cy - r; row <= cy + r; row++) {
        for (col = cx - r * 2; col <= cx + r * 2; col++) {
            double dx = (col - cx) * 0.5;
            double dy = (row - cy);
            double d = sqrt(dx * dx + dy * dy);
            if (fabs(d - r) < 0.6)
                plot(row, col, ch);
        }
    }
}

void draw_rect(int x, int y, int w, int h, char ch) {
    int i;
    for (i = y; i < y + h; i++) {
        plot(i, x, ch);
        plot(i, x + w - 1, ch);
    }
    for (i = x; i < x + w; i++) {
        plot(y, i, BORDER_CHAR);
        plot(y + h - 1, i, BORDER_CHAR);
    }
}

void draw_line(int x1, int y1, int x2, int y2, char ch) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int e = dx - dy;

    while (1) {
        plot(y1, x1, ch);
        if (x1 == x2 && y1 == y2) break;
        int temp = 2 * e;
        if (temp > -dy) { e -= dy; x1 += sx; }
        if (temp < dx)  { e += dx; y1 += sy; }
    }
}

void draw_triangle(int tx, int ty, int base, char ch) {
    int half = base / 2;
    int i;
    for (i = 0; i <= half; i++) {
        plot(ty + i, tx - i, ch);
        plot(ty + i, tx + i, ch);
    }
    for (i = tx - half; i <= tx + half; i++)
        plot(ty + half, i, BORDER_CHAR);
}

void redraw_all(void) {
    clear_canvas();
    int i;
    for (i = 0; i < total; i++) {
        if (!list[i].flag) continue;
        Shape *s = &list[i];
        switch (s->kind) {
            case SHAPE_CIRCLE:
                draw_circle(s->a, s->b, s->p, s->symbol);
                break;
            case SHAPE_RECT:
                draw_rect(s->a, s->b, s->p, s->q, s->symbol);
                break;
            case SHAPE_LINE:
                draw_line(s->a, s->b, s->p, s->q, s->symbol);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(s->a, s->b, s->p, s->symbol);
                break;
        }
    }
}

int find_shape(int id) {
    int i;
    for (i = 0; i < total; i++)
        if (list[i].flag && list[i].num == id)
            return i;
    return -1;
}

void add_shape(char kind, int a, int b, int p, int q) {
    if (total >= MAX_SHAPES) return;
    Shape *s = &list[total++];
    s->num    = counter++;
    s->kind   = kind;
    s->a      = a;
    s->b      = b;
    s->p      = p;
    s->q      = q;
    s->symbol = FILL_CHAR;
    s->flag   = 1;
    redraw_all();
}

void delete_shape(int id) {
    int idx = find_shape(id);
    if (idx == -1) return;
    list[idx].flag = 0;
    redraw_all();
}

void modify_shape(int id, int a, int b, int p, int q) {
    int idx = find_shape(id);
    if (idx == -1) return;
    list[idx].a = a;
    list[idx].b = b;
    list[idx].p = p;
    list[idx].q = q;
    redraw_all();
}

void draw_menu(int hl) {
    const char *items[] = {
        "1. Add Circle",
        "2. Add Rectangle",
        "3. Add Line",
        "4. Add Triangle",
        "5. Delete Shape",
        "6. Modify Shape",
        "7. List Shapes",
        "8. Clear All",
        "9. Exit"
    };
    int n = 9;

    werase(win1);
    box(win1, 0, 0);
    mvwprintw(win1, 0, 2, " MENU ");

    int i;
    for (i = 0; i < n; i++) {
        if (i + 1 == hl) {
            wattron(win1, A_REVERSE);
            mvwprintw(win1, i + 2, 2, "%s", items[i]);
            wattroff(win1, A_REVERSE);
        } else {
            mvwprintw(win1, i + 2, 2, "%s", items[i]);
        }
    }
    mvwprintw(win1, n + 3, 1, "UP/DOWN: navigate");
    mvwprintw(win1, n + 4, 1, "ENTER  : select  ");
    wrefresh(win1);
}

int prompt_int(const char *msg) {
    echo();
    int h = ROWS + 3;
    mvwprintw(win1, h - 3, 1, "%-18s", msg);
    mvwprintw(win1, h - 2, 1, "> ");
    wrefresh(win1);
    int val = 0;
    wscanw(win1, "%d", &val);
    noecho();
    return val;
}

void status_msg(const char *msg) {
    mvwprintw(win2, ROWS + 1, 2, "%-56s", msg);
    wrefresh(win2);
}

void show_shapes_list(void) {
    werase(win2);
    box(win2, 0, 0);
    mvwprintw(win2, 0, 2, " SHAPES ");
    mvwprintw(win2, 1, 2, "ID   Type       Parameters");
    mvwprintw(win2, 2, 2, "---  ---------  --------------------");
    int row = 3;
    int found = 0;
    int i;
    for (i = 0; i < total && row < ROWS; i++) {
        if (!list[i].flag) continue;
        found = 1;
        Shape *s = &list[i];
        switch (s->kind) {
            case SHAPE_CIRCLE:
                mvwprintw(win2, row++, 2,
                    "%-4d Circle     cx=%d cy=%d r=%d",
                    s->num, s->a, s->b, s->p);
                break;
            case SHAPE_RECT:
                mvwprintw(win2, row++, 2,
                    "%-4d Rectangle  x=%d y=%d w=%d h=%d",
                    s->num, s->a, s->b, s->p, s->q);
                break;
            case SHAPE_LINE:
                mvwprintw(win2, row++, 2,
                    "%-4d Line       (%d,%d)->(%d,%d)",
                    s->num, s->a, s->b, s->p, s->q);
                break;
            case SHAPE_TRIANGLE:
                mvwprintw(win2, row++, 2,
                    "%-4d Triangle   tip=(%d,%d) base=%d",
                    s->num, s->a, s->b, s->p);
                break;
        }
    }
    if (!found)
        mvwprintw(win2, row, 2, "(no shapes yet)");
    mvwprintw(win2, ROWS + 1, 2, "Press any key to go back...");
    wrefresh(win2);
    getch();
}

void run(void) {
    int hl = 1;
    int choice = 0;
    int ch;

    clear_canvas();
    draw_menu(hl);
    display_canvas_ncurses();

    while (1) {
        ch = wgetch(win1);

        if (ch == KEY_UP)   { if (hl > 1) hl--; }
        if (ch == KEY_DOWN) { if (hl < 9) hl++; }
        if (ch >= '1' && ch <= '9') hl = ch - '0';

        draw_menu(hl);

        if (ch == '\n' || ch == KEY_ENTER) {
            choice = hl;

            int x, y, p1, p2, id;

            switch (choice) {
                case 1:
                    x  = prompt_int("Centre X (0-59):");
                    y  = prompt_int("Centre Y (0-21):");
                    p1 = prompt_int("Radius:");
                    add_shape(SHAPE_CIRCLE, x, y, p1, 0);
                    display_canvas_ncurses();
                    status_msg("Circle added.");
                    break;

                case 2:
                    x  = prompt_int("Top-left X:");
                    y  = prompt_int("Top-left Y:");
                    p1 = prompt_int("Width:");
                    p2 = prompt_int("Height:");
                    add_shape(SHAPE_RECT, x, y, p1, p2);
                    display_canvas_ncurses();
                    status_msg("Rectangle added.");
                    break;

                case 3:
                    x  = prompt_int("Start X (x1):");
                    y  = prompt_int("Start Y (y1):");
                    p1 = prompt_int("End X (x2):");
                    p2 = prompt_int("End Y (y2):");
                    add_shape(SHAPE_LINE, x, y, p1, p2);
                    display_canvas_ncurses();
                    status_msg("Line added.");
                    break;

                case 4:
                    x  = prompt_int("Tip X:");
                    y  = prompt_int("Tip Y:");
                    p1 = prompt_int("Base width:");
                    add_shape(SHAPE_TRIANGLE, x, y, p1, 0);
                    display_canvas_ncurses();
                    status_msg("Triangle added.");
                    break;

                case 5:
                    show_shapes_list();
                    id = prompt_int("Shape ID to delete:");
                    if (find_shape(id) == -1) {
                        status_msg("ID not found!");
                    } else {
                        delete_shape(id);
                        display_canvas_ncurses();
                        status_msg("Shape deleted.");
                    }
                    break;

                case 6: {
                    show_shapes_list();
                    id = prompt_int("Shape ID to modify:");
                    int idx = find_shape(id);
                    if (idx == -1) {
                        status_msg("ID not found!");
                        break;
                    }
                    Shape *s = &list[idx];
                    switch (s->kind) {
                        case SHAPE_CIRCLE:
                            x  = prompt_int("New centre X:");
                            y  = prompt_int("New centre Y:");
                            p1 = prompt_int("New radius:");
                            modify_shape(id, x, y, p1, 0);
                            break;
                        case SHAPE_RECT:
                            x  = prompt_int("New top-left X:");
                            y  = prompt_int("New top-left Y:");
                            p1 = prompt_int("New width:");
                            p2 = prompt_int("New height:");
                            modify_shape(id, x, y, p1, p2);
                            break;
                        case SHAPE_LINE:
                            x  = prompt_int("New x1:");
                            y  = prompt_int("New y1:");
                            p1 = prompt_int("New x2:");
                            p2 = prompt_int("New y2:");
                            modify_shape(id, x, y, p1, p2);
                            break;
                        case SHAPE_TRIANGLE:
                            x  = prompt_int("New tip X:");
                            y  = prompt_int("New tip Y:");
                            p1 = prompt_int("New base width:");
                            modify_shape(id, x, y, p1, 0);
                            break;
                    }
                    display_canvas_ncurses();
                    status_msg("Shape modified.");
                    break;
                }

                case 7:
                    show_shapes_list();
                    display_canvas_ncurses();
                    break;

                case 8:
                    clear_canvas();
                    total = 0;
                    counter = 1;
                    display_canvas_ncurses();
                    status_msg("Canvas cleared.");
                    break;

                case 9:
                    return;
            }

            draw_menu(hl);
        }
    }
}

int main(void) {
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    int h = ROWS + 3;
    int w = COLS + 2;

    win1 = newwin(h, MENU_WIDTH, 0, 0);
    win2 = newwin(h, w, 0, MENU_WIDTH);

    keypad(win1, TRUE);

    run();

    delwin(win1);
    delwin(win2);
    endwin();

    printf("Goodbye!\n");
    return 0;
}