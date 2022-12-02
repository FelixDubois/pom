#include "pom.cpp"


#define WIDTH 300
#define HEIGHT 300

#define BACKGROUND_COLOR 0xFFFFFFFF

#define RED_COLOR 0xFFFF0000
#define GREEN_COLOR 0xFF00FF00
#define DARK_GREEN_COLOR 0xFF008000
#define BLUE_COLOR 0xFF0000FF
#define WHITE_COLOR 0xFFFFFFFF
#define BLACK_COLOR 0xFF000000

int main() {
    uint32_t pixels[WIDTH * HEIGHT];

    pom_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);
    
    pom_draw_grid(pixels, WIDTH, HEIGHT, 10, 5, BLUE_COLOR);
    pom_draw_x_axis(pixels, WIDTH, HEIGHT, 10, BLACK_COLOR);
    pom_draw_y_axis(pixels, WIDTH, HEIGHT, 20, BLACK_COLOR);

    pom_save_to_png("examples/grid_axis.png", pixels, WIDTH, HEIGHT);


    pom_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    pom_draw_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 100, RED_COLOR);
    pom_draw_rotated_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2 + 60, HEIGHT/2, 60, 50, 110, BACKGROUND_COLOR);
    pom_draw_rotated_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2 - 80, 50, 20, 60, GREEN_COLOR);
    pom_draw_rotated_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2 - 80, 40, 10, 60, DARK_GREEN_COLOR);

    pom_save_to_png("examples/logo.png", pixels, WIDTH, HEIGHT);


    pom_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    pom_draw_circle(pixels, WIDTH, HEIGHT, WIDTH/10, HEIGHT/2, 60, RED_COLOR);
    pom_draw_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/3, 150, DARK_GREEN_COLOR);
    pom_draw_circle(pixels, WIDTH, HEIGHT, WIDTH - 100, HEIGHT -50, 13, GREEN_COLOR);
    
    pom_save_to_png("examples/circles.png", pixels, WIDTH, HEIGHT);


    pom_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    pom_draw_line(pixels, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, RED_COLOR);
    pom_draw_line(pixels, WIDTH, HEIGHT, 0, HEIGHT, WIDTH, 0, GREEN_COLOR);
    pom_draw_line(pixels, WIDTH, HEIGHT, WIDTH/2, 50, WIDTH - 100, HEIGHT - 250, BLUE_COLOR);
    pom_draw_line(pixels, WIDTH, HEIGHT, 0, HEIGHT/2, WIDTH, HEIGHT/2, WHITE_COLOR);

    pom_save_to_png("examples/lines.png", pixels, WIDTH, HEIGHT);


    pom_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    pom_draw_triangle(pixels, WIDTH, HEIGHT, 10, 100, 50, 150, 20, 175, RED_COLOR);
    pom_draw_triangle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 100, GREEN_COLOR);

    pom_draw_rotated_triangle(pixels, WIDTH, HEIGHT, WIDTH - 10, HEIGHT/2, 100, 45, BLUE_COLOR, true);


    pom_save_to_png("examples/triangles.png", pixels, WIDTH, HEIGHT);


    pom_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    pom_draw_rect(pixels, WIDTH, HEIGHT, 70, 50, 150, 200, GREEN_COLOR, true);
    pom_draw_rect(pixels, WIDTH, HEIGHT, 50, 10, 100, 100, RED_COLOR);

    pom_save_to_png("examples/rect.png", pixels, WIDTH, HEIGHT);


    pom_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    pom_draw_rotated_ellipse(pixels, WIDTH, HEIGHT, 50, 50, 25, 50, 45, RED_COLOR);
    pom_draw_rotated_ellipse(pixels, WIDTH, HEIGHT, 150, 150, 150, 75, 45, GREEN_COLOR);
    pom_draw_ellipse(pixels, WIDTH, HEIGHT, 200, 175, 36, 12, BLUE_COLOR);

    pom_save_to_png("examples/ellipses.png", pixels, WIDTH, HEIGHT);

    return 0;
}