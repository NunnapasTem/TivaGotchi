/*
 *  threads.c
 *  - Define functions of background threads, periodic threads, and aperiodic threads
 *  Created on: Nov 29, 2024
 *  Author: Nina Temridiwong
 *
 */

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"
#include "./MultimodDrivers/multimod_dac.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "driverlib/timer.h"
#include <driverlib/adc.h>
#include "Pixel.h"
#include "Tama_Struct.h"

#define MAX_NUM_SAMPLES (200)
#define SIGNAL_STEPS (2)
#define SPRITE_OFFSET_X 14
#define SPRITE_OFFSET_Y 22

/*********************************Global Variables**********************************/
uint16_t dac_step = 0;
int16_t dac_signal[SIGNAL_STEPS] = {0x001, 0x000};
int16_t current_volume = 0xFFF;

// Display
uint8_t menu_index = 0;
uint32_t tama_time = 0;
uint8_t current_menu = 0;

// Tamagotchi
tama_t *Tama;
bool game_started = false;
/*********************************Global Variables**********************************/

/********************************Public Functions***********************************/

void Initialize_Tama()
{
    tama_time = 0;
    current_menu = 0;
    //    static tama_t new_tama = {0, 0, 0, Kuchipatchi_L, 10, 10, 10, 10, 10, false, false};
    //    Tama = &new_tama;
    Tama = malloc(sizeof(tama_t));
    Tama->age = 0;
    Tama->weight = 0;
    Tama->growth_stage = 0;
    Tama->sprite_L = Kuchipatchi_L;
    Tama->sprite_R = Kuchipatchi_R;
    Tama->hunger = 10;
    Tama->discipline = 10;
    Tama->poop = 2;
    Tama->happiness = 10;
    Tama->health = 10;
    Tama->sleeping = false;
    Tama->dead = false;
    game_started = true;
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_Fill(ST7789_BLACK);
    G8RTOS_SignalSemaphore(&sem_SPIA);
}

// Draw a 32x32 px sprite.
void Draw_Sprite(const uint32_t *sprite, uint8_t pos_x, uint8_t pos_y, uint16_t color, uint8_t size)
{
    uint16_t x_sprite;
    uint16_t y_sprite;

    for (int i = 0; i < 1024; i++)
    {
        if (sprite[i] != 0)
        {
            x_sprite = (i % 32) + pos_x;
            y_sprite = (i / 32) + pos_y;
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(x_sprite * size, y_sprite * size, size, size, color);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }
    }
}

void Draw_Menu()
{
    menu_index++;

    switch (menu_index)
    {
    case FEED:
        Draw_Sprite(Food_Icon, 82, 0, ST7789_BLUE, 2);
        break;
    case LIGHT:
        Draw_Sprite(Food_Icon, 82, 0, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 58, 0, ST7789_BLUE, 2);
        break;
    case PLAY:
        Draw_Sprite(Food_Icon, 58, 0, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 33, 0, ST7789_BLUE, 2);
        break;
    case MEDICINE:
        Draw_Sprite(Food_Icon, 33, 0, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 8, 0, ST7789_BLUE, 2);
        break;
    case DUCK:
        Draw_Sprite(Food_Icon, 8, 0, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 82, 110, ST7789_BLUE, 2);
        break;
    case HEALTH_METER:
        Draw_Sprite(Food_Icon, 82, 110, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 58, 110, ST7789_BLUE, 2);
        break;
    case DISCIPLINE:
        Draw_Sprite(Food_Icon, 58, 110, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 33, 110, ST7789_BLUE, 2);
        break;
    case ATTENTION:
        Draw_Sprite(Food_Icon, 33, 110, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 8, 110, ST7789_BLUE, 2);
        break;
    case OTHER:
        Draw_Sprite(Food_Icon, 8, 110, ST7789_BLACK, 2);
        menu_index = NONE;
        break;
    }
}

// TODO
void Draw_Food()
{
}

void Flush_Poop()
{
    // Reset poop timer
    Tama->poop = 10;
    // Clear screen
    current_menu = OTHER;
}

void Draw_Walking_Tama()
{

    int8_t poop_space = 0;
    if (Tama->poop <= 0)
    {
        poop_space = 10;
        Draw_Sprite(Sprite_Poop, 0, SPRITE_OFFSET_Y, ST7789_BLUE, 4);
    }

    for (int i = 12; i > -12 + poop_space; i--)
    {
        Draw_Sprite(Tama->sprite_R, SPRITE_OFFSET_X + i, SPRITE_OFFSET_Y, ST7789_BLACK, 4);
        if (current_menu != 0)
            return;
        Draw_Sprite(Tama->sprite_R, SPRITE_OFFSET_X + i - 1, SPRITE_OFFSET_Y, ST7789_BLUE, 4);
        sleep(80);
    }

    Draw_Sprite(Tama->sprite_R, SPRITE_OFFSET_X + -12 + poop_space, SPRITE_OFFSET_Y, ST7789_BLACK, 4);
    sleep(80);

    for (int i = -12 + poop_space; i < 12; i++)
    {
        Draw_Sprite(Tama->sprite_L, SPRITE_OFFSET_X + i - 1, SPRITE_OFFSET_Y, ST7789_BLACK, 4);
        if (current_menu != 0)
            return;
        Draw_Sprite(Tama->sprite_L, SPRITE_OFFSET_X + i, SPRITE_OFFSET_Y, ST7789_BLUE, 4);
        sleep(80);
    }

    Draw_Sprite(Tama->sprite_L, SPRITE_OFFSET_X + 11, SPRITE_OFFSET_Y, ST7789_BLACK, 4);
    sleep(80);
}

void Draw_Digits(uint16_t number, uint16_t x, uint16_t y)
{
    for (int i = 0; i < 3; i++)
    {
        int digit = number % 10;
        Draw_Sprite(Digit_Sprites[digit], x, y, ST7789_BLUE, 4);
        number = number / 10;
        x += 6;
    }
}

void Draw_Health()
{
    Draw_Sprite(Age_Icon, 31, 9, ST7789_BLUE, 4);
    Draw_Sprite(Scale_Icon, 32, 30, ST7789_BLUE, 4);
    Draw_Sprite(Sprite_Year, 3, 12, ST7789_BLUE, 4);
    Draw_Sprite(Sprite_Ounce, 3, 32, ST7789_BLUE, 4);
    Draw_Digits(Tama->age, 6, 10);
}

/*************************************Threads***************************************/

void Idle_Thread(void)
{
    while (1)
        ;
}

/*
 * Button A - Move through options
 * Button B - Select an option
 * Button C - Cancel or exit a menu
 */
void Read_Buttons(void)
{
    // Initialize / declare any variables here
    uint8_t buttons_data;

    while (1)
    {
        // wait for button semaphore
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
        // Get buttons
        G8RTOS_WaitSemaphore(&sem_I2CA);
        buttons_data = MultimodButtons_Get();
        G8RTOS_SignalSemaphore(&sem_I2CA);
        // debounce buttons
        sleep(15);

        // Button A
        if (((buttons_data >> 4) & 1) == 0)
        {
            Draw_Menu();
        }
        // Button B - Select
        else if (((buttons_data >> 3) & 1) == 0)
        {
            if (game_started)
            {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(10, 76, 220, 128, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);
                current_menu = menu_index;
                // Select_Menu();
            }
            else
            {
                Initialize_Tama();
            }
        }
        // Button C
        else if (((buttons_data >> 2) & 1) == 0)
        {
            current_menu = OTHER;
        }

        // clear button interrupt
        GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
        GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    }
}

void Display_Thread(void)
{
    while (1)
    {
        if (game_started)
        {
            switch (current_menu)
            {
            case NONE:
                Draw_Walking_Tama();
                break;
            case DUCK:
                Flush_Poop();
                break;
            case HEALTH_METER:
                Draw_Health();
                break;
            case OTHER:
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(10, 76, 220, 132, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);
                current_menu = NONE;
                break;
            }
        }
        else
        {
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(10, 76, 220, 132, ST7789_BLUE);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }
    }
}

/********************************Periodic Threads***********************************/

// keep the clock running and keep track of status
void Clock_Thread()
{
    if (game_started && !Tama->dead)
    {
        tama_time++;

        // Baby to teen tamagotchi
        if (tama_time == 10)
        {
            // ST7789_Fill(ST7789_BLACK);
            // Tama->sprite = Kuchipatchi_R;
        }
        else if (tama_time % 30 == 0)
        {
            Tama->age++;
        }

        // Decrement hunger

        // Decrement happiness

        // Decrement health

        // Decrement discipline

        // Decrement poop
        if (tama_time % 5 == 0)
        {
            Tama->poop--;
        }

        // Check tama's status need to changed to dead
        if (Tama->happiness < -2 || Tama->hunger < -2 || Tama->health < -2 || Tama->poop < -2)
        {
            Tama->dead = true;
            tama_time = 0;
            game_started = false;
            free(Tama);
        }
    }
}

/*******************************Aperiodic Threads***********************************/

void Button_Handler()
{
    // disable interrupt and signal semaphore
    GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

void DAC_Timer_Handler()
{
    // clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    // read next output sample
    uint32_t output = (current_volume) * (dac_signal[dac_step++ % SIGNAL_STEPS]);
    // write the output value to the dac
    MutimodDAC_Write(DAC_OUT_REG, output);
}
