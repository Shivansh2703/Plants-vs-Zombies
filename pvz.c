// TODO: Start Page - check boxes for levels
// TODO: Get Images of everything
// TODO: Setting up grid
// TODO: Setting up Structs
// TODO: Function to determine collisions

/* This files provides address values that exist in the system */
#include <stdbool.h>
#include <images.h>
#include <stdint.h>
#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

#define image_x_size 40
#define image_y_size 20

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include "images.h"

// Begin part3.c code for Lab 7


volatile int pixel_buffer_start; // global variable
volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

void plot_pixel(int x, int y, short int line_color)
{
    *(int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void clear_screen(){
	for(int x = 0; x < RESOLUTION_X; x++){
		for(int y = 0; y < RESOLUTION_Y; y++){
			plot_pixel(x,y,0x0000);
			}
	}
}
// // code not shown for clear_screen() and draw_line() subroutines
// void draw_line(int x0, int x1, int y0, int y1, short int colour){
// 	bool is_steep = ABS(y1 - y0) > ABS(x1 - x0);
// 	if(is_steep){
// 		int temp = x0;
// 		x0 = y0;
// 		y0 = temp;
		
// 		temp = x1;
// 		x1 = y1;
// 		y1 = temp;
// 	}
	
// 	if(x0 > x1){
// 		int temp = x0;
// 		x0 = x1;
// 		x1 = temp;
		
// 		temp = y1;
// 		y1 = y0;
// 		y0 = temp;
// 	}
// 	int y_step = 0;
// 	int deltax = x1- x0;
// 	int deltay = ABS(y1 - y0);
// 	int error = -(deltax / 2);
// 	int y = y0;
// 	if (y0 < y1){
// 		y_step = 1;
// 	}
// 	else {
// 		y_step = -1;
// 	}
	
// 	for(int x = x0; x < x1; x++){
// 		if(is_steep){
// 			if(x>0 && y>0 && x<=RESOLUTION_Y && y<=RESOLUTION_X){
// 				plot_pixel(y,x,colour);
// 			}
// 		}
// 		else{
// 			if(x>0 && y>0 && x<=RESOLUTION_X && y<=RESOLUTION_Y){
// 				plot_pixel(x,y,colour);
// 			}
// 		}
// 		error =  error + deltay;
// 		if(error > 0){
// 			y = y + y_step;
// 			error = error - deltax;
// 		}
// 	}
// }

// void wait_for_vsync(){
// 	register int status; 
// 	*pixel_ctrl_ptr = 1; //start synchronization
// 	status = *(pixel_ctrl_ptr +3);
// 	while(status&0x01){
// 		status = *(pixel_ctrl_ptr +3);
// 	}
// }

int main(void)
{
    // volatile int * pixel_ctrl_ptr = (int *)0xFF203020;	
    // /* set front pixel buffer to start of FPGA On-chip memory */
    // *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
    //                                     // back buffer
    // /* now, swap the front/back buffers, to set the front buffer location */
    // wait_for_vsync();
    // /* initialize a pointer to the pixel buffer, used by drawing functions */
    // pixel_buffer_start = *pixel_ctrl_ptr;
    // clear_screen(); // pixel_buffer_start points to the pixel buffer
    // /* set back pixel buffer to start of SDRAM memory */
    // *(pixel_ctrl_ptr + 1) = 0xC0000000;
    // pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    // clear_screen(); // pixel_buffer_start points to the pixel buffer


    printf("%i", (int)sizeof(peashooter_map));

    int z = 0;
    for(int i = 10; i < 30; i++){
        for(int k = 0; k < 20; k++){
			uint8_t first = peashooter_map[z+1];
			uint8_t second = peashooter_map[z];
            plot_pixel(k, i, grass_map[z+1] << 8 | grass_map[z]);
			short int colour = first << 8 | second;
			if(first != 0xff && second != 0xff){
				if(first != 0xf7 && second != 0x9e){
					plot_pixel(k, i, colour);
				}
				
			}
			z+=2;
        }
    }

    wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

    // while (1)
    // {
	// 	//draw
	// 	//erase contents of back buffer
							
    //     wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    //     pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    // }
}

// code for subroutines (not shown)
