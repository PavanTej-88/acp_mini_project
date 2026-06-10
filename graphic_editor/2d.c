#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS 24
#define COLS 80
#define MAX_SHAPES 100

#define LINE     1
#define RECT     2
#define CIRCLE   3
#define TRIANGLE 4

typedef struct {
    int type;
    int active;
    int x1, y1, x2, y2;
    int x3, y3;
    int cx, cy, r;
} Shape;

char canvas[ROWS][COLS];
Shape shapes[MAX_SHAPES];
int shape_count = 0;

void clear_canvas() {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            canvas[r][c] = '_';
}

void plot(int r, int c, char ch) {
    if (r >= 0 && r < ROWS && c >= 0 && c < COLS)
        canvas[r][c] = ch;
}

void draw_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        plot(y1, x1, '*');
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

void draw_rect(int x1, int y1, int x2, int y2) {
    for (int c = x1; c <= x2; c++) {
        plot(y1, c, '*');
        plot(y2, c, '*');
    }
    for (int r = y1; r <= y2; r++) {
        plot(r, x1, '*');
        plot(r, x2, '*');
    }
}

void draw_circle(int cx, int cy, int r) {
    for (int row = cy - r; row <= cy + r; row++) {
        for (int col = cx - r; col <= cx + r; col++) {
            int dx = col - cx, dy = row - cy;
            double dist = sqrt((double)(dx*dx + dy*dy));
            if (fabs(dist - r) < 0.6)
                plot(row, col, '*');
        }
    }
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

void redraw_all() {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        if (!shapes[i].active) continue;
        Shape *s = &shapes[i];
        if (s->type == LINE)     draw_line(s->x1, s->y1, s->x2, s->y2);
        if (s->type == RECT)     draw_rect(s->x1, s->y1, s->x2, s->y2);
        if (s->type == CIRCLE)   draw_circle(s->cx, s->cy, s->r);
        if (s->type == TRIANGLE) draw_triangle(s->x1, s->y1, s->x2, s->y2, s->x3, s->y3);
    }
}

void display_canvas() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++)
            putchar(canvas[r][c]);
        putchar('\n');
    }
}

void print_menu() {
    printf("\n2D Graphics Editor\n");
    printf("Canvas size: %d x %d\n", COLS, ROWS);
    printf("1. Add object\n");
    printf("2. Delete object\n");
    printf("3. Modify object\n");
    printf("4. Display picture\n");
    printf("5. List objects\n");
    printf("0. Exit\n");
    printf("Enter choice: \n");
}

int main() {
    int choice;
    clear_canvas();

    while (1) {
        print_menu();
        scanf("%d", &choice);

        if (choice == 0) {
            printf("Goodbye.\n");
            break;
        }

        if (choice == 1) {
            int type;
            printf("Choose shape type:\n");
            printf("1. Line\n2. Rectangle\n3. Circle\n4. Triangle\n");
            printf("Enter shape type: ");
            scanf("%d", &type);

            Shape s;
            memset(&s, 0, sizeof(s));
            s.type = type;
            s.active = 1;

            if (type == LINE) {
                printf("Enter x1 y1 x2 y2: ");
                scanf("%d %d %d %d", &s.x1, &s.y1, &s.x2, &s.y2);
            } else if (type == RECT) {
                printf("Enter top-left x y and bottom-right x y: ");
                scanf("%d %d %d %d", &s.x1, &s.y1, &s.x2, &s.y2);
            } else if (type == CIRCLE) {
                printf("Enter center x y and radius: ");
                scanf("%d %d %d", &s.cx, &s.cy, &s.r);
            } else if (type == TRIANGLE) {
                printf("Enter x1 y1 x2 y2 x3 y3: ");
                scanf("%d %d %d %d %d %d", &s.x1, &s.y1, &s.x2, &s.y2, &s.x3, &s.y3);
            }

            shapes[shape_count] = s;
            printf("Object added with index %d.\n", shape_count);
            shape_count++;
            redraw_all();
        }

        else if (choice == 2) {
            int idx;
            printf("Enter index to delete: ");
            scanf("%d", &idx);
            if (idx >= 0 && idx < shape_count && shapes[idx].active) {
                shapes[idx].active = 0;
                printf("Object %d deleted.\n", idx);
                redraw_all();
            } else {
                printf("Invalid index.\n");
            }
        }

        else if (choice == 3) {
            int idx;
            printf("Enter index to modify: ");
            scanf("%d", &idx);
            if (idx >= 0 && idx < shape_count && shapes[idx].active) {
                Shape *s = &shapes[idx];
                if (s->type == LINE) {
                    printf("Enter new x1 y1 x2 y2: ");
                    scanf("%d %d %d %d", &s->x1, &s->y1, &s->x2, &s->y2);
                } else if (s->type == RECT) {
                    printf("Enter new top-left x y and bottom-right x y: ");
                    scanf("%d %d %d %d", &s->x1, &s->y1, &s->x2, &s->y2);
                } else if (s->type == CIRCLE) {
                    printf("Enter new center x y and radius: ");
                    scanf("%d %d %d", &s->cx, &s->cy, &s->r);
                } else if (s->type == TRIANGLE) {
                    printf("Enter new x1 y1 x2 y2 x3 y3: ");
                    scanf("%d %d %d %d %d %d", &s->x1, &s->y1, &s->x2, &s->y2, &s->x3, &s->y3);
                }
                printf("Object %d modified.\n", idx);
                redraw_all();
            } else {
                printf("Invalid index.\n");
            }
        }

        else if (choice == 4) {
            display_canvas();
        }

        else if (choice == 5) {
            for (int i = 0; i < shape_count; i++) {
                if (!shapes[i].active) continue;
                Shape *s = &shapes[i];
                if (s->type == LINE)
                    printf("%d: Line (%d,%d)-(%d,%d)\n", i, s->x1, s->y1, s->x2, s->y2);
                else if (s->type == RECT)
                    printf("%d: Rectangle (%d,%d)-(%d,%d)\n", i, s->x1, s->y1, s->x2, s->y2);
                else if (s->type == CIRCLE)
                    printf("%d: Circle center(%d,%d) r=%d\n", i, s->cx, s->cy, s->r);
                else if (s->type == TRIANGLE)
                    printf("%d: Triangle (%d,%d),(%d,%d),(%d,%d)\n", i, s->x1, s->y1, s->x2, s->y2, s->x3, s->y3);
            }
        }
    }

    return 0;
}