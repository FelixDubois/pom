#include "pomme.cpp"

#define WIDTH 150
#define HEIGHT 150

#define BACKGROUND_COLOR 0xFF2B2D42

#define RED_COLOR 0xFFFF0000
#define GREEN_COLOR 0xFF00FF00
#define BLUE_COLOR 0xFF0000FF
#define WHITE_COLOR 0xFFFFFFFF

int main() {
    uint32_t pixels[WIDTH * HEIGHT];

    Pomme::fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    //Pomme::draw_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 25, RED_COLOR);
    //Pomme::draw_circle(pixels, WIDTH, HEIGHT, 25, 50, 25, GREEN_COLOR);

    //Pomme::draw_line(pixels, WIDTH, HEIGHT, WIDTH/2, 0, WIDTH/2, HEIGHT, BLUE_COLOR);

    Pomme::draw_triangle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 100, RED_COLOR, true);
    Pomme::draw_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 5, GREEN_COLOR);

    Pomme::draw_x_axis(pixels, WIDTH, HEIGHT, 25, WHITE_COLOR);

    Pomme::save_to_ppm("image.ppm", pixels, WIDTH, HEIGHT);

    return 0;
}