#include "pom.cpp"

#define WIDTH 150
#define HEIGHT 150

#define BACKGROUND_COLOR 0xFF2B2D42

#define RED_COLOR 0xFFFF0000
#define GREEN_COLOR 0xFF00FF00
#define BLUE_COLOR 0xFF0000FF
#define WHITE_COLOR 0xFFFFFFFF
#define BLACK_COLOR 0xFF000000

int main() {
    uint32_t pixels[WIDTH * HEIGHT];

    Pom::fill(pixels, WIDTH, HEIGHT, WHITE_COLOR);

    //Pom::draw_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 25, RED_COLOR);
    //Pom::draw_circle(pixels, WIDTH, HEIGHT, 25, 50, 25, GREEN_COLOR);

    //Pom::draw_line(pixels, WIDTH, HEIGHT, WIDTH/2, 0, WIDTH/2, HEIGHT, BLUE_COLOR);

    //Pom::draw_triangle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 100, RED_COLOR, true);
    //Pom::draw_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 20, RED_COLOR);

    //Pom::draw_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 50, 25, RED_COLOR);
    Pom::draw_rotated_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 50, 25, 45, RED_COLOR);
    //Pom::draw_rotated_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 50, 25, 0, RED_COLOR);
    //Pom::draw_rotated_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 50, 25, 45 + 90, RED_COLOR);
    //Pom::draw_rotated_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 45, 20, 45, WHITE_COLOR);
    //Pom::draw_rotated_ellipse(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 45, 20, 45 + 90, WHITE_COLOR);

    //Pom::draw_rotated_triangle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 50, 45, BLACK_COLOR, true);
    //Pom::draw_rotated_triangle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 45, 45, WHITE_COLOR, false);

    //Pom::draw_x_axis(pixels, WIDTH, HEIGHT, 25, WHITE_COLOR);

    Pom::save_to_ppm("image.ppm", pixels, WIDTH, HEIGHT);

    return 0;
}