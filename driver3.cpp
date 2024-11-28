// to compile: g++ -o driver driver.cpp -l gpiod
// to run: ./driver


#include <iostream>
#include <gpiod.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include <algorithm>
#include <vector>


struct gpiod_chip *chip;
struct gpiod_line_request_config config;
struct gpiod_line_bulk lines;

unsigned int offsets_w[1];
unsigned int offsets_r[8];
unsigned int offsets_b[8];
unsigned int offsets_y[8];

int values_w[1];
int values_r[8];
int values_b[8];
int values_y[8];

int err;


void output_fade(char colour, int value=1)
{

    memset(&config, 0, sizeof(config));
    config.consumer = "blink";
    config.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
    config.flags = 0;


    float intensity = 0;
    float iintensity = 1-intensity; /* inverted intensity */
    float phase = 10;
    int dir = 1;


    int n;


    std::vector<unsigned int> offsets_fv {};
    std::vector<int> values_fv {};


    if (colour == 'w')
    {
        /* 1 white led */
        n = 1;
        for (int i = 0; i < n; i++) {
             offsets_fv.push_back(offsets_w[i]);
             values_fv.push_back(values_w[i]);
        }
    }
    else if (colour == 'r')
    {
        /* 8 red led's */
        n = 8;
        for (int i = 0; i < n; i++) {
             offsets_fv.push_back((unsigned int)offsets_r[i]);
             values_fv.push_back((int)values_r[i]);
        }
    }
    else if (colour == 'b')
    {
        /* 8 blue led's */
        n = 8;
        for (int i = 0; i < n; i++) {
             offsets_fv.push_back((unsigned int)offsets_b[i]);
             values_fv.push_back((int)values_b[i]);
        }
    }
    else if (colour = 'y')
    {
        /* 8 yellow led's */
        n = 8;
        for (int i = 0; i < n; i++) {
             offsets_fv.push_back((unsigned int)offsets_y[i]);
             values_fv.push_back((int)values_y[i]);
        }
    }
    else
    {
        return;
    }

    unsigned int* offsets_f = &offsets_fv[0];
    int* values_f = &values_fv[0];

    gpiod_chip_get_lines(chip, offsets_f, n, &lines);

    while (true)
    {
        if(dir==1)
        {
            intensity += 0.01;
            iintensity -= 0.01;
            if(iintensity<=0.04)
            {
                dir=0;
            }
        }
        else if(dir==0)
        {
            intensity -= 0.01;
            iintensity += 0.01;
            if(intensity<=0.04)
            {
                return;
            } 
        }

        for (int i = 0; i < n; i++) {
            values_f[i] = 1;
        }
            
        /* send updated value to line(s) */
        gpiod_line_request_bulk(&lines, &config, values_f);
        gpiod_line_release_bulk(&lines);
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(phase*intensity)));

        for (int i = 0; i < n; i++) {
            values_f[i] = 0;
        }

        gpiod_line_request_bulk(&lines, &config, values_f);
        gpiod_line_release_bulk(&lines);
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(phase*iintensity)));

        }

    return;

}


void output_solid(char colour, int value=1)
{

    memset(&config, 0, sizeof(config));
    config.consumer = "blink";
    config.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
    config.flags = 0;

    if (colour == 'w') {
        values_w[0] = value;
        /* send updated value to line(s) */
        gpiod_chip_get_lines(chip, offsets_w, 1, &lines);
        gpiod_line_request_bulk(&lines, &config, values_w);
    }
    else if (colour == 'r') {
        for (int i = 0; i < 8; i++) {
            values_r[i] = value;
        }
        /* send updated value to line(s) */
        gpiod_chip_get_lines(chip, offsets_r, 8, &lines);
        gpiod_line_request_bulk(&lines, &config, values_r);
    }
    else if (colour == 'b') {
        for (int i = 0; i < 8; i++) {
            values_b[i] = value;
        }
        /* send updated value to line(s) */
        gpiod_chip_get_lines(chip, offsets_b, 8, &lines);
        gpiod_line_request_bulk(&lines, &config, values_b);
    }
    else if (colour == 'y') {
        for (int i = 0; i < 8; i++) {
            values_y[i] = value;
        }
        /* send updated value to line(s) */
        gpiod_chip_get_lines(chip, offsets_y, 8, &lines);
        gpiod_line_request_bulk(&lines, &config, values_y);
    }
    else {
        return;
    }

    gpiod_line_release_bulk(&lines);

    return;

}



int main()
{

    std::cout << "Initializing christmas tree...\n";

    /*

    PIN LAYOUT:
    red:    1  /  6 /  8 / 21 /  2 / 17 /  5 / 12
    blue:   16 / 22 /  3 / 19 / 20 / 18 / 24 / 13
    yellow: 7 /  14 / 15 /  9 / 10 /  4 / 23 / 11
    white: 2

    */

    /* variables for loop */
    int n = 1000;
    int i = 0;

    chip = gpiod_chip_open("/dev/gpiochip4");
    if(!chip)
    {
        std::cout << "ERROR: unable to open GPIO chip!\n";
        goto cleanup;
    }


    // set pin numbers
    offsets_w[0] = 2;
    offsets_r[0] = 4; offsets_r[1] = 8; offsets_r[2] = 10; offsets_r[3] = 11; offsets_r[4] = 15;
    offsets_r[5] = 20; offsets_r[6] = 25; offsets_r[7] = 26;
    offsets_b[0] = 5; offsets_b[1] = 6; offsets_b[2] = 13; offsets_b[3] = 14; offsets_b[4] = 18;
    offsets_b[5] = 19; offsets_b[6] = 22; offsets_b[7] = 24;
    offsets_y[0] = 7; offsets_y[1] = 9; offsets_y[2] = 12; offsets_y[3] = 16; offsets_y[4] = 17;
    offsets_y[5] = 21; offsets_y[6] = 23; offsets_y[7] = 27;


    /* star stays lit */
    output_solid('w');

    /* loop through fading in and out coloured led's */
    while(true)
    {
        output_fade('r');
        output_fade('b');
        output_fade('y');
    }


    cleanup:
        std::cout << "Cleaning up...\n";
        gpiod_line_release_bulk(&lines);
        gpiod_chip_close(chip);


    return 0;


}