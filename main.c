/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/


#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc_device.h"
#include "mxc_sys.h"
#include "gcfr_regs.h"
#include "fcr_regs.h"
#include "icc.h"
#include "dma.h"
#include "led.h"
#include "tmr.h"
#include "pb.h"
#include "cnn.h"
#include "weights.h"
#include "gpio_regs.h"
#include "board.h"
#include "sampledata.h"
#include "mxc_delay.h"
#include "camera.h"
#ifdef BOARD_EVKIT_V1
#include "bitmap.h"
#include "tft_ssd2119.h"
#endif
#ifdef BOARD_FTHR_REVA
#include "tft_ili9341.h"
#include "road_edge_lines.h"
#endif

// Comment out USE_SAMPLEDATA to use Camera module
//#define USE_SAMPLEDATA

#define CAMERA_TO_LCD (1)
#define IMAGE_SIZE_X (64)
#define IMAGE_SIZE_Y (64)
#define CAMERA_FREQ (10 * 1000 * 1000)

#define TFT_BUFF_SIZE 35 // TFT buffer size
#define CNN_NUM_OUTPUTS 3 // number of classes

const uint32_t frame_interval_us = 2000000; // 2 second --> try different times
MXC_Delay(frame_interval_us);


#ifdef BOARD_EVKIT_V1


int image_bitmap_1 = ADI_256_bmp;
int image_bitmap_2 = logo_white_bg_darkgrey_bmp;
int font_1 = urw_gothic_13_white_bg_grey;
int font_2 = urw_gothic_12_white_bg_grey;
#endif
#ifdef BOARD_FTHR_REVA
int image_bitmap_1 = (int)&img_1_rgb565[0];
int image_bitmap_2 = (int)&logo_rgb565[0];
int font_1 = (int)&Liberation_Sans16x16[0];
int font_2 = (int)&Liberation_Sans16x16[0];
#endif

uint32_t input_0_camera[1024];
uint32_t input_1_camera[1024];
uint32_t input_2_camera[1024];
char buff[TFT_BUFF_SIZE];
// Define GPIO pins for deviations
#define RIGHT_DEVIATION_PIN  (MXC_GPIO_PIN_6) // P0_6
#define LEFT_DEVIATION_PIN   (MXC_GPIO_PIN_5) // P0_5
#define DEVIATION_PORT       (MXC_GPIO0)



void setupGPIO() {
    // Configure GPIO for Right Deviation
    mxc_gpio_cfg_t right_dev_cfg;
    right_dev_cfg.port = DEVIATION_PORT;
    right_dev_cfg.mask = RIGHT_DEVIATION_PIN;
    right_dev_cfg.pad = MXC_GPIO_PAD_NONE;
    right_dev_cfg.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&right_dev_cfg);

    // Configure GPIO for Left Deviation
    mxc_gpio_cfg_t left_dev_cfg;
    left_dev_cfg.port = DEVIATION_PORT;
    left_dev_cfg.mask = LEFT_DEVIATION_PIN;
    left_dev_cfg.pad = MXC_GPIO_PAD_NONE;
    left_dev_cfg.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&left_dev_cfg);

    // Set both pins LOW initially
    MXC_GPIO_OutClr(DEVIATION_PORT, RIGHT_DEVIATION_PIN);
    MXC_GPIO_OutClr(DEVIATION_PORT, LEFT_DEVIATION_PIN);
}

void fail(void)
{
    printf("\n*** FAIL ***\n\n");
    while (1) {}
}
uint8_t *frame_buffer;
unsigned int chobi[64][64];
void process_camera_img(uint32_t *data0, uint32_t *data1, uint32_t *data2)
{

    uint32_t imgLen;
    uint32_t w, h, x, y;
    uint8_t *ptr0;
    uint8_t *ptr1;
    uint8_t *ptr2;
    uint8_t *buffer;

    camera_get_image(&frame_buffer, &imgLen, &w, &h);
    ptr0 = (uint8_t *)data0;
    ptr1 = (uint8_t *)data1;
    ptr2 = (uint8_t *)data2;
    buffer = frame_buffer;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++, ptr0++, ptr1++, ptr2++) {
            *ptr0 = (*buffer);
            chobi[x][y] = *ptr0;
            buffer++;
            *ptr1 = (*buffer);
            buffer++;
            *ptr2 = (*buffer);
            buffer++;

            buffer++; //MSB is zero
        }
    }
}

/* **************************************************************************** */
void capture_camera_img(void)
{
    camera_start_capture_image();
    while (1) {
        if (camera_is_image_rcv()) {
            return;
        }
    }
}

int main(void)
{
    int i, dma_channel;
    int digs, tens;  //couldn't find any use for this
    int ret = 0;

#if defined(BOARD_FTHR_REVA)
    // Wait for PMIC 1.8V to become available, about 180ms after power up.
    MXC_Delay(200000);
    /* Enable camera power */
    Camera_Power(POWER_ON);
    printf("\n\nRPS Feather Demo\n");
#else
    printf("\n\nRPS Evkit Demo\n");
#endif

    MXC_ICC_Enable(MXC_ICC0); // Enable cache

    // Switch to 100 MHz clock
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
    SystemCoreClockUpdate();

    printf("Waiting...\n");

    // DO NOT DELETE THIS LINE:
    MXC_Delay(SEC(2)); // Let debugger interrupt if needed

#ifdef TFT_ENABLE
    // Initialize TFT display.
    printf("Init LCD.\n");
#ifdef BOARD_EVKIT_V1
    MXC_TFT_Init();
    MXC_TFT_ClearScreen();
    // Rotate screen 180 degree
    MXC_TFT_SetRotation(SCREEN_FLIP);
    MXC_TFT_ShowImage(0, 0, image_bitmap_1);
#endif
#ifdef BOARD_FTHR_REVA
    /* Initialize TFT display */
    MXC_TFT_Init(MXC_SPI0, 1, NULL, NULL);
    MXC_TFT_SetRotation(ROTATE_270);
    MXC_TFT_ShowImage(0, 0, image_bitmap_1);
    MXC_TFT_SetForeGroundColor(WHITE); // set chars to white
#endif
    MXC_Delay(1000000);
#endif

    // Initialize DMA for camera interface
    MXC_DMA_Init();
    dma_channel = MXC_DMA_AcquireChannel();

    // Initialize camera.
    printf("Init Camera.\n");
    camera_init(CAMERA_FREQ);

    ret = camera_setup(IMAGE_SIZE_X, IMAGE_SIZE_Y, PIXFORMAT_RGB888, FIFO_THREE_BYTE, USE_DMA,
                       dma_channel);
    if (ret != STATUS_OK) {
        printf("Error returned from setting up camera. Error %d\n", ret);
        return -1;
    }

#ifdef TFT_ENABLE
    MXC_TFT_SetPalette(image_bitmap_2);
    MXC_TFT_SetBackGroundColor(4);
    //MXC_TFT_ShowImage(1, 1, image_bitmap_2);
    memset(buff, ' ', TFT_BUFF_SIZE);
    TFT_Print(buff, 55, 50, font_1, snprintf(buff, sizeof(buff), "ANALOG DEVICES"));
    TFT_Print(buff, 30, 90, font_2, snprintf(buff, sizeof(buff), "Rock-Paper-Scissors Game"));
    TFT_Print(buff, 10, 130, font_1, snprintf(buff, sizeof(buff), "PRESS PB1(SW1) TO START!"));
#endif

    int frame = 0;

    while (1) {
#ifdef TFT_ENABLE
        MXC_TFT_ClearScreen();
        //MXC_TFT_ShowImage(1, 1, image_bitmap_2);
        TFT_Print(buff, 55, 110, font_2, snprintf(buff, sizeof(buff), "CAPTURING IMAGE...."));
#endif

#ifdef USE_SAMPLEDATA
        // Copy the sampledata reference to the camera buffer as a test.
        printf("\nCapturing sampledata %d times\n", ++frame);
        memcpy32(input_0_camera, input_0, 1024);
        memcpy32(input_1_camera, input_1, 1024);
        memcpy32(input_2_camera, input_2, 1024);
        convert_img_signed_to_unsigned(input_0_camera, input_1_camera, input_2_camera);
#else
        // Capture a single camera frame.
        printf("\nCapture a camera frame %d\n", ++frame);
        capture_camera_img();
        // Copy the image data to the CNN input arrays.
        printf("Copy camera frame to CNN input buffers.\n");
        process_camera_img(input_0_camera, input_1_camera, input_2_camera);
        float ou=Detect_Lines(chobi);
        printf("%f\n ", ou);
        void detectDeviation(ou) {
            // Clear both pins initially
            MXC_GPIO_OutClr(DEVIATION_PORT, RIGHT_DEVIATION_PIN);
            MXC_GPIO_OutClr(DEVIATION_PORT, LEFT_DEVIATION_PIN);

           // if (f <= 0.7 && f >= -0.7) {
                // Normal
             //   printf("No Deviation\n");
             if (ou <= 10 && ou > 0.7) {
                // Right deviation
                printf("Right Deviation\n");
                MXC_GPIO_OutSet(DEVIATION_PORT, RIGHT_DEVIATION_PIN); // Turn P0_6 HIGH
            } else if (ou < -0.7 && ou >= -10) {
                // Left deviation
                printf("Left Deviation\n");
                MXC_GPIO_OutSet(DEVIATION_PORT, LEFT_DEVIATION_PIN); // Turn P0_5 HIGH
            }// else {
               // printf("Error in scanning road edge lines\n");
            //}
        }
        detectDeviation(ou);

        #endif

#ifdef TFT_ENABLE
        // Show the input data on the lcd.
        MXC_TFT_ClearScreen();
        //MXC_TFT_ShowImage(1, 1, image_bitmap_2);
        printf("Show camera frame on LCD.\n");
        TFT_Print(buff, 10, 30, font_2, snprintf(buff, sizeof(buff), "User Move"));
        TFT_Print(buff, 152, 30, font_2, snprintf(buff, sizeof(buff), "Computer Move"));
#endif


    }

    return 0;
}
