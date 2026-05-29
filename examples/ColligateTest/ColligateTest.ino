/*
   Created by DIYables

   This example code is in the public domain

   Comprehensive graphics benchmark - tests most drawing functions
   and reports timing for each operation via Serial Monitor.

   Product page:
   - https://diyables.io/tft-touch-shield
   - https://www.amazon.com/dp/B0DQ3NQ3LW

   Credit: This example is based on the colligate_test example from the LCDWIKI library
*/

#include <DIYables_TFT_Touch_Shield.h>

// NOTE: Choose the class that matches the driver IC printed on the shield's package/label:
//   - RM68140 driver  -> use DIYables_TFT_RM68140_Shield
//   - HX8357D driver  -> use DIYables_TFT_HX8357D_Shield
// For Arduino Uno R3, Uno R4, Mega, Due, Giga, DIYables STEM V3/V4 (default touch pins: XP=6, YP=A1, XM=A2, YM=7)
DIYables_TFT_RM68140_Shield TFT_display;
// DIYables_TFT_HX8357D_Shield  TFT_display;
// For DIYables ESP32-S3 Uno-form factor (https://diyables.io/esp32-s3-uno), touch pins: XP=3, YP=1, XM=7, YM=14
// DIYables_TFT_RM68140_Shield TFT_display(3, 1, 7, 14);
// DIYables_TFT_HX8357D_Shield TFT_display(3, 1, 7, 14);

uint16_t SCREEN_WIDTH;
uint16_t SCREEN_HEIGHT;

// Color definitions (RGB565)
#define BLACK     DIYables_TFT::colorRGB(0, 0, 0)
#define WHITE     DIYables_TFT::colorRGB(255, 255, 255)
#define RED       DIYables_TFT::colorRGB(255, 0, 0)
#define GREEN     DIYables_TFT::colorRGB(0, 255, 0)
#define BLUE      DIYables_TFT::colorRGB(0, 0, 255)
#define CYAN      DIYables_TFT::colorRGB(0, 255, 255)
#define MAGENTA   DIYables_TFT::colorRGB(255, 0, 255)
#define YELLOW    DIYables_TFT::colorRGB(255, 255, 0)
#define ORANGE    DIYables_TFT::colorRGB(255, 165, 0)

// Display main surface with header/footer
unsigned long show_text(void)
{
    unsigned long time_start = micros();
    TFT_display.fillRect(0, 0, SCREEN_WIDTH, 15, DIYables_TFT::colorRGB(32, 0, 255));
    TFT_display.setTextColor(GREEN);
    TFT_display.setTextSize(1);
    TFT_display.setCursor((SCREEN_WIDTH - 240) / 2, 3);
    TFT_display.print(F("* DIYables 3.5\" TFT Touch Shield For Arduino *"));

    TFT_display.fillRect(0, SCREEN_HEIGHT - 15, SCREEN_WIDTH, 15, DIYables_TFT::colorRGB(128, 128, 128));
    TFT_display.setTextColor(WHITE);
    TFT_display.setTextSize(1);
    TFT_display.setCursor((SCREEN_WIDTH - 210) / 2, SCREEN_HEIGHT - 11);
    TFT_display.print(F("https://diyables.io/tft-touch-shield"));

    TFT_display.drawRect(0, 15, SCREEN_WIDTH, SCREEN_HEIGHT - 31, RED);
    return micros() - time_start;
}

// Display triangle functions (sin, cos, tan, cot)
unsigned long show_triangle_function(void)
{
    uint16_t i;
    unsigned long time_start = micros();

    // Draw crosshairs
    TFT_display.drawFastVLine(SCREEN_WIDTH / 2 - 1, 16, SCREEN_HEIGHT - 32, BLUE);
    TFT_display.drawFastHLine(1, SCREEN_HEIGHT / 2 - 1, SCREEN_WIDTH - 2, BLUE);

    for (i = 1; i <= (SCREEN_HEIGHT - 32) / 2 / 10; i++) {
        TFT_display.drawFastHLine(SCREEN_WIDTH / 2 - 3, SCREEN_HEIGHT / 2 - 1 - i * 10, 5, BLUE);
        TFT_display.drawFastHLine(SCREEN_WIDTH / 2 - 3, SCREEN_HEIGHT / 2 - 1 + i * 10, 5, BLUE);
    }
    for (i = 1; i <= (SCREEN_WIDTH - 2) / 2 / 10; i++) {
        TFT_display.drawFastVLine(SCREEN_WIDTH / 2 - 1 - i * 10, SCREEN_HEIGHT / 2 - 3, 5, BLUE);
        TFT_display.drawFastVLine(SCREEN_WIDTH / 2 - 1 + i * 10, SCREEN_HEIGHT / 2 - 3, 5, BLUE);
    }

    // Draw sin
    TFT_display.setTextColor(CYAN);
    TFT_display.setTextSize(1);
    TFT_display.setCursor(5, 17);
    TFT_display.print(F("sin"));
    for (i = 1; i < SCREEN_WIDTH - 2; i++) {
        TFT_display.drawPixel(i, SCREEN_HEIGHT / 2 - 1 + (sin(((i * 1.13) * 3.14) / 180) * 95), CYAN);
    }

    // Draw cos
    TFT_display.setTextColor(GREEN);
    TFT_display.setCursor(5, 25);
    TFT_display.print(F("cos"));
    for (i = 1; i < SCREEN_WIDTH - 2; i++) {
        TFT_display.drawPixel(i, SCREEN_HEIGHT / 2 - 1 + (cos(((i * 1.13) * 3.14) / 180) * 95), GREEN);
    }

    // Draw tan
    TFT_display.setTextColor(YELLOW);
    TFT_display.setCursor(5, 33);
    TFT_display.print(F("tan"));
    for (i = 1; i < SCREEN_WIDTH - 2; i++) {
        TFT_display.drawPixel(i, SCREEN_HEIGHT / 2 - 1 + (tan(((i * 1.13) * 3.14) / 180) * 10), YELLOW);
    }

    // Draw cot
    TFT_display.setTextColor(RED);
    TFT_display.setCursor(5, 41);
    TFT_display.print(F("cot"));
    for (i = 1; i < SCREEN_WIDTH - 2; i++) {
        TFT_display.drawPixel(i, SCREEN_HEIGHT / 2 - 1 + 1 / (tan(((i * 1.13) * 3.14) / 180) * 0.1), RED);
    }
    return micros() - time_start;
}

// Draw a moving sinewave
unsigned long show_sinewave(void)
{
    uint16_t buf[480], x = 1, i, y;
    unsigned long time_start = micros();
    int16_t t;
    float k;
    if (SCREEN_WIDTH == 320) {
        t = 20; k = 1.1;
    } else {
        t = 15; k = 0.7;
    }

    TFT_display.drawFastVLine(SCREEN_WIDTH / 2 - 1, 16, SCREEN_HEIGHT - 32, BLUE);
    TFT_display.drawFastHLine(1, SCREEN_HEIGHT / 2 - 1, SCREEN_WIDTH - 2, BLUE);

    for (i = 1; i < ((SCREEN_WIDTH - 2) * t); i++) {
        x++;
        if (x == SCREEN_WIDTH - 1) x = 1;
        if (i > SCREEN_WIDTH - 1) {
            if ((x == SCREEN_WIDTH / 2 - 1) || (buf[x - 1] == SCREEN_HEIGHT / 2 - 1)) {
                TFT_display.drawPixel(x, buf[x - 1], BLUE);
            } else {
                TFT_display.drawPixel(x, buf[x - 1], BLACK);
            }
        }
        y = SCREEN_HEIGHT / 2 - 1 + (sin(((i * k) * 3.14) / 180) * (90 - (i / 100)));
        TFT_display.drawPixel(x, y, DIYables_TFT::colorRGB(255, 64, 255));
        buf[x - 1] = y;
    }
    return micros() - time_start;
}

// Draw some filled rectangles
unsigned long show_fill_rectangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t side_len = (SCREEN_HEIGHT - 40) / 5;
    uint16_t x_spec = (SCREEN_WIDTH - 5 * side_len) / 2;
    uint16_t y_spec = (SCREEN_HEIGHT - 5 * side_len) / 2;
    uint16_t colors[] = {MAGENTA, RED, GREEN, BLUE, YELLOW};
    for (i = 0; i < 5; i++) {
        TFT_display.fillRect(x_spec + i * side_len, y_spec + i * side_len, side_len, side_len, colors[i]);
        TFT_display.fillRect(x_spec + i * side_len, y_spec + (4 - i) * side_len, side_len, side_len, colors[i]);
    }
    return micros() - time_start;
}

// Draw some filled round rectangles
unsigned long show_fill_round_rectangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t side_len = (SCREEN_HEIGHT - 40) / 5;
    uint16_t x_spec = (SCREEN_WIDTH - 5 * side_len) / 2;
    uint16_t y_spec = (SCREEN_HEIGHT - 5 * side_len) / 2;
    uint16_t colors[] = {MAGENTA, RED, GREEN, BLUE, YELLOW};
    for (i = 0; i < 5; i++) {
        TFT_display.fillRoundRect(x_spec + i * side_len, y_spec + i * side_len, side_len, side_len, 10, colors[i]);
        TFT_display.fillRoundRect(x_spec + i * side_len, y_spec + (4 - i) * side_len, side_len, side_len, 10, colors[i]);
    }
    return micros() - time_start;
}

// Draw some filled circles
unsigned long show_fill_circle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t r_len = (SCREEN_HEIGHT - 40) / 5 / 2;
    uint16_t x_spec = (SCREEN_WIDTH - 5 * r_len * 2) / 2;
    uint16_t y_spec = (SCREEN_HEIGHT - 5 * r_len * 2) / 2;
    uint16_t colors[] = {MAGENTA, RED, GREEN, BLUE, YELLOW};
    for (i = 0; i < 5; i++) {
        TFT_display.fillCircle(x_spec + r_len + i * r_len * 2, y_spec + r_len + i * r_len * 2, r_len, colors[i]);
        TFT_display.fillCircle(x_spec + r_len + i * r_len * 2, y_spec + (5 - i) * r_len * 2 - r_len, r_len, colors[i]);
    }
    return micros() - time_start;
}

// Draw some filled triangles
unsigned long show_fill_triangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t h_len = (SCREEN_HEIGHT - 40) / 5;
    uint16_t side_len = (h_len * 115) / 100;
    uint16_t x_spec = (SCREEN_WIDTH - 5 * side_len) / 2;
    uint16_t y_spec = (SCREEN_HEIGHT - 5 * h_len) / 2;
    uint16_t colors[] = {MAGENTA, RED, GREEN, BLUE, YELLOW};
    for (i = 0; i < 5; i++) {
        TFT_display.fillTriangle(
            x_spec + i * side_len, y_spec + (i + 1) * h_len,
            x_spec + side_len / 2 + i * side_len, y_spec + i * h_len,
            x_spec + (i + 1) * side_len, y_spec + (i + 1) * h_len, colors[i]);
        TFT_display.fillTriangle(
            x_spec + i * side_len, y_spec + (5 - i) * h_len,
            x_spec + side_len / 2 + i * side_len, y_spec + (4 - i) * h_len,
            x_spec + (i + 1) * side_len, y_spec + (5 - i) * h_len, colors[i]);
    }
    return micros() - time_start;
}

// Draw grid lines pattern
unsigned long show_grid_lines(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    float k = (SCREEN_WIDTH == 320) ? 1.44 : 1.6;

    for (i = 16; i < SCREEN_HEIGHT - 17; i += 5) {
        TFT_display.drawLine(1, i, (i * k) - 10, SCREEN_HEIGHT - 17, RED);
    }
    for (i = SCREEN_HEIGHT - 17; i > 16; i -= 5) {
        TFT_display.drawLine(SCREEN_WIDTH - 2, i, (i * k) - 11, 16, RED);
    }
    for (i = SCREEN_HEIGHT - 16; i > 16; i -= 5) {
        TFT_display.drawLine(1, i, (SCREEN_HEIGHT - 17) * k + 10 - (i * k), 16, CYAN);
    }
    for (i = 15; i < SCREEN_HEIGHT - 17; i += 5) {
        TFT_display.drawLine(SCREEN_WIDTH - 2, i, (SCREEN_HEIGHT - 17) * k + 10 - (i * k), SCREEN_HEIGHT - 17, CYAN);
    }
    return micros() - time_start;
}

// Draw some random pixels
unsigned long show_random_pixels(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for (i = 0; i < 10000; i++) {
        TFT_display.drawPixel(
            2 + random(SCREEN_WIDTH - 4),
            17 + random(SCREEN_HEIGHT - 34),
            DIYables_TFT::colorRGB(random(255), random(255), random(255)));
    }
    return micros() - time_start;
}

// Draw some random lines
unsigned long show_random_lines(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for (i = 0; i < 300; i++) {
        TFT_display.drawLine(
            2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
            2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
            DIYables_TFT::colorRGB(random(255), random(255), random(255)));
    }
    return micros() - time_start;
}

// Draw some random rectangles
unsigned long show_random_rectangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for (i = 0; i < 150; i++) {
        int16_t x1 = 2 + random(SCREEN_WIDTH - 4);
        int16_t y1 = 17 + random(SCREEN_HEIGHT - 34);
        int16_t x2 = 2 + random(SCREEN_WIDTH - 4);
        int16_t y2 = 17 + random(SCREEN_HEIGHT - 34);
        int16_t rx = min(x1, x2);
        int16_t ry = min(y1, y2);
        TFT_display.drawRect(rx, ry, abs(x2 - x1), abs(y2 - y1),
            DIYables_TFT::colorRGB(random(255), random(255), random(255)));
    }
    return micros() - time_start;
}

// Draw some random round rectangles
unsigned long show_random_round_rectangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for (i = 0; i < 150; i++) {
        int16_t x1 = 2 + random(SCREEN_WIDTH - 4);
        int16_t y1 = 17 + random(SCREEN_HEIGHT - 34);
        int16_t x2 = 2 + random(SCREEN_WIDTH - 4);
        int16_t y2 = 17 + random(SCREEN_HEIGHT - 34);
        int16_t rx = min(x1, x2);
        int16_t ry = min(y1, y2);
        TFT_display.drawRoundRect(rx, ry, abs(x2 - x1), abs(y2 - y1), 5,
            DIYables_TFT::colorRGB(random(255), random(255), random(255)));
    }
    return micros() - time_start;
}

// Draw some random circles
unsigned long show_random_circles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for (i = 0; i < 150; i++) {
        TFT_display.drawCircle(
            41 + random(SCREEN_WIDTH - 82),
            56 + random(SCREEN_HEIGHT - 112),
            random(40),
            DIYables_TFT::colorRGB(random(255), random(255), random(255)));
    }
    return micros() - time_start;
}

// Draw some random triangles
unsigned long show_random_triangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for (i = 0; i < 150; i++) {
        TFT_display.drawTriangle(
            2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
            2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
            2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
            DIYables_TFT::colorRGB(random(255), random(255), random(255)));
    }
    return micros() - time_start;
}

// Clear the drawing area
void clear_screen(void)
{
    delay(2000);
    TFT_display.fillRect(1, 16, SCREEN_WIDTH - 2, SCREEN_HEIGHT - 32, BLACK);
}

// Function pointer array and labels
typedef unsigned long (*ShowFunc)(void);
ShowFunc show_function[] = {
    show_text,
    show_triangle_function,
    show_sinewave,
    show_fill_rectangle,
    show_fill_round_rectangle,
    show_fill_circle,
    show_fill_triangle,
    show_grid_lines,
    show_random_pixels,
    show_random_lines,
    show_random_rectangles,
    show_random_round_rectangles,
    show_random_circles,
    show_random_triangles,
};

const char str_0[]  PROGMEM = "show text                    :";
const char str_1[]  PROGMEM = "show triangle function       :";
const char str_2[]  PROGMEM = "show sinewave                :";
const char str_3[]  PROGMEM = "show fill rectangle          :";
const char str_4[]  PROGMEM = "show fill round rectangle    :";
const char str_5[]  PROGMEM = "show fill circle             :";
const char str_6[]  PROGMEM = "show fill triangle           :";
const char str_7[]  PROGMEM = "show grid lines              :";
const char str_8[]  PROGMEM = "show random pixels           :";
const char str_9[]  PROGMEM = "show random lines            :";
const char str_10[] PROGMEM = "show random rectangles       :";
const char str_11[] PROGMEM = "show random round rectangles :";
const char str_12[] PROGMEM = "show random circles          :";
const char str_13[] PROGMEM = "show random triangles        :";

const char * const show_str[] PROGMEM = {
    str_0, str_1, str_2, str_3, str_4, str_5, str_6,
    str_7, str_8, str_9, str_10, str_11, str_12, str_13
};

#define NUM_TESTS 14

// Run all tests and display timing results
unsigned long show_total_time(void)
{
    uint16_t i;
    unsigned long buf[NUM_TESTS];
    unsigned long time_start = micros();
    char strbuf[35];

    for (i = 0; i < NUM_TESTS; i++) {
        buf[i] = show_function[i]();
        clear_screen();
    }

    int16_t x_off = (SCREEN_WIDTH - 260) / 2;
    int16_t y_off = (SCREEN_HEIGHT - NUM_TESTS * 10) / 2;

    for (i = 0; i < NUM_TESTS; i++) {
        strcpy_P(strbuf, (char *)pgm_read_ptr(&show_str[i]));
        TFT_display.setTextColor(ORANGE);
        TFT_display.setTextSize(1);
        TFT_display.setCursor(x_off, y_off + i * 10);
        TFT_display.print(strbuf);
        TFT_display.setTextColor(GREEN);
        TFT_display.setCursor(x_off + 200, y_off + i * 10);
        TFT_display.print(buf[i]);

        // Also print to Serial for easy comparison
        Serial.print(strbuf);
        Serial.print(F(" "));
        Serial.println(buf[i]);
    }

    // Print total
    unsigned long total = 0;
    for (i = 0; i < NUM_TESTS; i++) total += buf[i];
    Serial.print(F("Total (tests only):              "));
    Serial.print(total);
    Serial.println(F(" us"));
    Serial.println();

    delay(5000);
    return micros() - time_start;
}

// Display ending screen with total time
void show_end(unsigned long run_time)
{
    TFT_display.fillScreen(CYAN);
    TFT_display.fillRoundRect(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 60,
                              240, 120, 5, RED);
    TFT_display.setTextColor(CYAN);
    TFT_display.setTextSize(1);

    int16_t cx = SCREEN_WIDTH / 2 - 40;
    int16_t cy = SCREEN_HEIGHT / 2 - 40;
    TFT_display.setCursor(cx, cy);
    TFT_display.println(F("Running over!"));
    TFT_display.setCursor(cx, cy + 10);
    TFT_display.println(F("That's ok!"));
    TFT_display.setCursor(cx - 20, cy + 20);
    TFT_display.println(F("After a few seconds,"));
    TFT_display.setCursor(cx - 10, cy + 30);
    TFT_display.println(F("it will restart."));
    TFT_display.setCursor(cx - 5, cy + 40);
    TFT_display.println(F("Please wait ..."));

    TFT_display.setTextColor(YELLOW);
    TFT_display.setCursor(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2 + 40);
    TFT_display.print(F("Total runtime(us): "));
    TFT_display.setTextColor(GREEN);
    TFT_display.print(run_time);
    delay(10000);
}

void setup()
{
    Serial.begin(9600);
    TFT_display.begin();
    TFT_display.fillScreen(BLACK);
    TFT_display.setRotation(1); // Landscape: 480x320

    SCREEN_WIDTH  = TFT_display.width();
    SCREEN_HEIGHT = TFT_display.height();
}

void loop()
{
    unsigned long total_time;
    TFT_display.fillScreen(BLACK);
    total_time = show_total_time();
    show_end(total_time);
}
