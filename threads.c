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
#include <math.h>

#include "driverlib/timer.h"
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include "Pixel.h"
#include "Tama_Struct.h"

#define MAX_NUM_SAMPLES (200)
#define SIGNAL_STEPS (2)
#define SPRITE_OFFSET_X 14
#define SPRITE_OFFSET_Y 22

/*********************************Global Variables**********************************/

// Display
uint8_t menu_index = 0;
uint32_t tama_time = 0;
uint8_t current_menu = 0;
bool burger = true;
bool food_menu_on = false;
bool health_menu_on = false;

// Tamagotchi
tama_t *Tama;
bool game_started = false;

/*********************************Global Variables**********************************/

/********************************Public Functions***********************************/

void Initialize_Tama()
{
    // Set default tama values
    tama_time = 0;
    current_menu = 0;
    Tama = malloc(sizeof(tama_t));
    Tama->age = 0;
    Tama->weight = 0;
    Tama->sprite_L = Kuchipatchi_L;
    Tama->sprite_R = Kuchipatchi_R;
    Tama->hunger = 10;
    Tama->discipline = 10;
    Tama->poop = 10;
    Tama->happiness = 10;
    Tama->health = 10;
    Tama->sleep_deficit = 0;
    Tama->sleeping = false;
    Tama->dead = false;
    game_started = true;

    // Clear screen
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_Fill(ST7789_BLACK);
    G8RTOS_SignalSemaphore(&sem_SPIA);
    sleep(80);

    // Set default thread ID
    ID_walking_tama = G8RTOS_AddThread(&Walking_Tama_Thread, 2, "walking");
    ID_virus_thread = -1;
    ID_sleepy_thread = -1;
}

/*
 * Draw_Sprite()
 * Draw a 32x32 sprite with a specific color and pixel size.
 */
void Draw_Sprite(const uint32_t *sprite, int8_t pos_x, int8_t pos_y, uint16_t color, uint8_t size)
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
        Draw_Sprite(Food_Icon, 89, 10, ST7789_BLUE, 2);
        break;
    case LIGHT:
        Draw_Sprite(Food_Icon, 89, 10, ST7789_BLACK, 2);
        Draw_Sprite(Light_Icon, 66, 10, ST7789_BLUE, 2);
        break;
    case PLAY:
        Draw_Sprite(Light_Icon, 66, 10, ST7789_BLACK, 2);
        Draw_Sprite(Game_Icon, 43, 10, ST7789_BLUE, 2);
        break;
    case MEDICINE:
        Draw_Sprite(Game_Icon, 43, 10, ST7789_BLACK, 2);
        Draw_Sprite(Medicine_Icon, 20, 10, ST7789_BLUE, 2);
        break;
    case DUCK:
        Draw_Sprite(Medicine_Icon, 20, 10, ST7789_BLACK, 2);
        Draw_Sprite(Duck_Icon, 89, 120, ST7789_BLUE, 2);
        break;
    case HEALTH_METER:
        Draw_Sprite(Duck_Icon, 89, 120, ST7789_BLACK, 2);
        Draw_Sprite(Health_Icon, 66, 120, ST7789_BLUE, 2);
        break;
    case DISCIPLINE:
        Draw_Sprite(Health_Icon, 66, 120, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 43, 120, ST7789_BLUE, 2);
        break;
    case ATTENTION:
        Draw_Sprite(Food_Icon, 43, 120, ST7789_BLACK, 2);
        Draw_Sprite(Food_Icon, 20, 120, ST7789_BLUE, 2);
        break;
    case OTHER:
        Draw_Sprite(Food_Icon, 20, 120, ST7789_BLACK, 2);
        menu_index = NONE;
        break;
    }
}

void Flush_Poop()
{
    // Reset poop timer
    if (Tama->poop <= 0)
        Tama->poop = 10;
    // Clear screen
    G8RTOS_KillThread(ID_walking_tama);
    // Draw flush poop
    for (int i = -6; i < 36; i++)
    {
        Draw_Sprite(Sprite_Flush, i - 2, 20, ST7789_BLACK, 4);
        Draw_Sprite(Sprite_Flush, i, 20, ST7789_BLUE, 4);
        sleep(40);
    }
    // Back to previous activity
    if (Tama->sleep_deficit > 0)
        ID_sleepy_thread = G8RTOS_AddThread(Sleepy_Tama_Thread, 2, "Sleep");
    else
        ID_walking_tama = G8RTOS_AddThread(Walking_Tama_Thread, 2, "walk");
    // Clear screen
    current_menu = OTHER;
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

/*************************************Threads***************************************/

void Idle_Thread(void)
{
    while (1)
        ;
}

/*
 * Blinking_LED()
 * Background thread to notify when a tamagotchi needs attention.
 */
void Blinking_LED(void)
{
    //    PCA9956b_SetAllOff();
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 5; i++)
        {
            PCA9556b_SetLED(LEDOUT0 + 2 + i, 0xFF, 0xFF);
            PCA9556b_SetLED(LEDOUT0 + 3 + i, 0xFF, 0xFF);
            PCA9556b_SetLED(LEDOUT0 + 4 + i, 0xFF, 0xFF);
            PCA9556b_SetLED(LEDOUT0 + 5 + i, 0xFF, 0xFF);
            sleep(40);
            PCA9556b_SetLED(LEDOUT0 + 2 + i, 0x00, 0xFF);
            PCA9556b_SetLED(LEDOUT0 + 3 + i, 0x00, 0xFF);
            PCA9556b_SetLED(LEDOUT0 + 4 + i, 0x00, 0xFF);
            PCA9556b_SetLED(LEDOUT0 + 5 + i, 0x00, 0xFF);
        }
        sleep(80);
    }
    G8RTOS_KillSelf();
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
            if (game_started)
            {
                if (food_menu_on)
                    burger = !burger;
                else
                    Draw_Menu();
            }
        }

        // Button B - Select
        else if (((buttons_data >> 3) & 1) == 0)
        {
            // Update current menu selection
            if (game_started)
            {
                if (food_menu_on)
                {
                    if (Tama->hunger < 10)
                    {
                        Tama->hunger++;
                        if (!burger)
                            Tama->weight++;
                    }
                }
                else
                {
                    current_menu = menu_index;
                }
            }
            // Hatch new tama
            else
            {
                G8RTOS_KillThread(ID_start_thread);
                Initialize_Tama();
            }
        }

        // Button C - Exit
        else if (((buttons_data >> 2) & 1) == 0)
        {
            if (game_started)
            {
                // Kill current menu page
                if (food_menu_on)
                {
                    food_menu_on = false;
                    G8RTOS_KillThread(ID_food_thread);
                }
                else if (health_menu_on)
                {
                    health_menu_on = false;
                    G8RTOS_KillThread(ID_health_thread);
                }
                // Clear board
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(10, 76, 220, 132, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                // Back to main
                if (Tama->sleep_deficit > 0)
                    Draw_Sprite(Sleepy_Tama, 21, 27, ST7789_BLUE, 4);
                else
                    ID_walking_tama = G8RTOS_AddThread(Walking_Tama_Thread, 2, "Walk");
            }
        }

        // clear button interrupt
        GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
        GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    }
}

void Menu_Thread(void)
{

    while (1)
    {
        if (game_started)
        {
            switch (current_menu)
            {
            case NONE:
                break;
            case FEED:
                G8RTOS_KillThread(ID_walking_tama);

                current_menu = NONE;
                food_menu_on = true;
                ID_food_thread = G8RTOS_AddThread(Food_Thread, 2, "food");
                break;
            case LIGHT:
                if (Tama->sleeping)
                {
                    G8RTOS_KillThread(ID_light_thread);
                    Tama->sleeping = false;
                    if (Tama->sleep_deficit > 0)
                        ID_sleepy_thread = G8RTOS_AddThread(Sleepy_Tama_Thread, 2, "Sleepy");
                    else
                        ID_walking_tama = G8RTOS_AddThread(Walking_Tama_Thread, 2, "walk");
                }
                else
                {
                    G8RTOS_KillThread(ID_sleepy_thread);
                    G8RTOS_KillThread(ID_walking_tama);
                    Tama->sleeping = true;
                    ID_light_thread = G8RTOS_AddThread(Light_Off_Thread, 2, "Light");
                }
                current_menu = NONE;
                break;
            case MEDICINE:
                // Reset health score & kill virus
                Tama->health = 10;
                current_menu = NONE;
                if (ID_virus_thread)
                {
                    G8RTOS_KillThread(ID_virus_thread);
                    Draw_Sprite(Sprite_Virus, 7, 18, ST7789_BLACK, 4);
                }
                break;
            case DUCK:
                Flush_Poop();
                break;
            case HEALTH_METER:
                G8RTOS_KillThread(ID_walking_tama);
                current_menu = NONE;
                health_menu_on = true;
                ID_health_thread = G8RTOS_AddThread(Draw_Health_Thread, 2, "health");
                break;
            case OTHER:
                current_menu = NONE;
                break;
            }
        }
    }
}

void Walking_Tama_Thread(void)
{

    // Clear board
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(10, 76, 220, 132, ST7789_BLACK);
    G8RTOS_SignalSemaphore(&sem_SPIA);

    int8_t poop_space = 0;

    while (1)
    {
        // Check for poop space
        if (Tama->poop <= 0)
        {
            poop_space = 10;
            Draw_Sprite(Sprite_Poop, 0, SPRITE_OFFSET_Y, ST7789_BLUE, 4);
        }

        // Walk right
        for (int i = 12; i > -12 + poop_space; i--)
        {
            Draw_Sprite(Tama->sprite_R, SPRITE_OFFSET_X + i, SPRITE_OFFSET_Y, ST7789_BLACK, 4);
            if (current_menu != 0)
                return;
            Draw_Sprite(Tama->sprite_R, SPRITE_OFFSET_X + i - 1, SPRITE_OFFSET_Y, ST7789_BLUE, 4);
            sleep(80);
        }
        Draw_Sprite(Tama->sprite_R, SPRITE_OFFSET_X + -12 + poop_space, SPRITE_OFFSET_Y, ST7789_BLACK, 4);

        // Walk left
        for (int i = -12 + poop_space; i < 12; i++)
        {
            Draw_Sprite(Tama->sprite_L, SPRITE_OFFSET_X + i - 1, SPRITE_OFFSET_Y, ST7789_BLACK, 4);
            if (current_menu != 0)
                return;
            Draw_Sprite(Tama->sprite_L, SPRITE_OFFSET_X + i, SPRITE_OFFSET_Y, ST7789_BLUE, 4);
            sleep(80);
        }
        Draw_Sprite(Tama->sprite_L, SPRITE_OFFSET_X + 11, SPRITE_OFFSET_Y, ST7789_BLACK, 4);
    }
}

void Draw_Start_Thread()
{
    for (int i = 0; i < 1760; i++)
    {
        if (Start_Page[i] != 0)
        {
            uint16_t x_sprite = i % 55;
            uint16_t y_sprite = i / 55;
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle((x_sprite * 4) + 10, (y_sprite * 4) + 76, 4, 4, ST7789_BLUE);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }
    }

    while (1)
    {
        // Blink button B
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawRectangle(114, 140, 12, 12, ST7789_BLUE);
        G8RTOS_SignalSemaphore(&sem_SPIA);
        sleep(300);
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawRectangle(114, 140, 12, 12, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);
        sleep(300);
    }
}

void Virus_Thread()
{
    while (1)
    {
        Draw_Sprite(Sprite_Virus, 7, 18, ST7789_BLUE, 4);
        sleep(250);
        Draw_Sprite(Sprite_Virus, 7, 18, ST7789_BLACK, 4);
        sleep(250);
    }
}

void Sleepy_Tama_Thread()
{
    uint32_t start_time = tama_time;
    // Clear screen
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(10, 76, 220, 128, ST7789_BLACK);
    G8RTOS_SignalSemaphore(&sem_SPIA);
    // Re-draw poop
    if (Tama->poop <= 0)
        Draw_Sprite(Sprite_Poop, 0, SPRITE_OFFSET_Y, ST7789_BLUE, 4);
    // Draw sleepy tama
    Draw_Sprite(Sleepy_Tama, 21, 27, ST7789_BLUE, 4);
    while (1)
    {
        // TODO
        Tama->sleep_deficit = tama_time - start_time;
    }
}

void Light_Off_Thread()
{
    uint32_t start_time = tama_time;
    // Clear screen
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(10, 76, 220, 128, ST7789_BLUE);
    G8RTOS_SignalSemaphore(&sem_SPIA);

    while (1)
    {
        // TODO
        if (tama_time - start_time > 10)
        {
            // Clear screen
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(10, 76, 220, 128, ST7789_BLACK);
            G8RTOS_SignalSemaphore(&sem_SPIA);
            // Re-draw poop
            if (Tama->poop <= 0)
                Draw_Sprite(Sprite_Poop, 0, SPRITE_OFFSET_Y, ST7789_BLUE, 4);

            // Kill thread to turn the light on
            Tama->sleep_deficit = 0;
            current_menu = NONE;
            ID_walking_tama = G8RTOS_AddThread(Walking_Tama_Thread, 2, "Walk");
            G8RTOS_KillSelf();
        }
    }
}

void Food_Thread()
{
    // Clear screen
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(10, 76, 220, 128, ST7789_BLACK);
    G8RTOS_SignalSemaphore(&sem_SPIA);
    // Draw food
    Draw_Sprite(Sprite_Food, 18, 26, ST7789_BLUE, 4);
    // Draw Arrow
    Draw_Sprite(Sprite_Arrow, 40, 26, ST7789_BLUE, 4);
    while (1)
    {
        if (burger)
        {
            Draw_Sprite(Sprite_Arrow, 40, 38, ST7789_BLACK, 4);
            Draw_Sprite(Sprite_Arrow, 40, 26, ST7789_BLUE, 4);
        }
        else
        {
            Draw_Sprite(Sprite_Arrow, 40, 26, ST7789_BLACK, 4);
            Draw_Sprite(Sprite_Arrow, 40, 38, ST7789_BLUE, 4);
        }
    }
}

void Draw_Health_Thread()
{
    // Clear board
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(10, 76, 220, 132, ST7789_BLACK);
    G8RTOS_SignalSemaphore(&sem_SPIA);
    Draw_Sprite(Age_Icon, 31, 9, ST7789_BLUE, 4);
    Draw_Sprite(Scale_Icon, 32, 30, ST7789_BLUE, 4);
    Draw_Sprite(Sprite_Year, 3, 12, ST7789_BLUE, 4);
    Draw_Sprite(Sprite_Ounce, 3, 32, ST7789_BLUE, 4);
    Draw_Digits(Tama->age, 6, 10);
    while (1)
    {
    }
}

/********************************Periodic Threads***********************************/

// keep the clock running and keep track of status
void Clock_Thread()
{
    if (game_started && !Tama->dead)
    {
        tama_time++;

        if (tama_time % 30 == 0)
        {
            Tama->age++;
            if (Tama->age % 2 == 0)
            {
                Tama->weight += 5;
            }
        }
        // Decrement hunger
        if (tama_time % 5 == 2)
        {
            Tama->hunger--;
        }
        // Decrement health
        if (tama_time % 3 == 0)
        {
            Tama->health--;
            if (Tama->health == 0)
            {
                ID_virus_thread = G8RTOS_AddThread(Virus_Thread, 2, "virus");
                G8RTOS_AddThread(Blinking_LED, 2, "LED");
            }
        }
        // Decrement poop
        if (tama_time % 5 == 0)
        {
            Tama->poop--;
            if (Tama->poop == 0)
            {
                Draw_Sprite(Sprite_Poop, 0, SPRITE_OFFSET_Y, ST7789_BLUE, 4);
                G8RTOS_AddThread(Blinking_LED, 2, "LED");
            }
        }
        if (!Tama->sleeping && (tama_time % 40 == 0))
        {
            G8RTOS_KillThread(ID_walking_tama);
            ID_sleepy_thread = G8RTOS_AddThread(Sleepy_Tama_Thread, 2, "Sleepy");
            G8RTOS_AddThread(Blinking_LED, 2, "LED");
        }
        // Check tama's status need to changed to dead
        if (Tama->hunger < -2 || Tama->health < -2 || Tama->poop < -2 || Tama->sleep_deficit > 15)
        {
            Tama->dead = true;
            tama_time = 0;
            game_started = false;
            G8RTOS_KillThread(ID_walking_tama);
            G8RTOS_KillThread(ID_sleepy_thread);
            G8RTOS_KillThread(ID_virus_thread);
            G8RTOS_KillThread(ID_light_thread);
            sleep(80);

            // Clear board
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_Fill(ST7789_BLACK);
            G8RTOS_SignalSemaphore(&sem_SPIA);

            // Draw end page

            for (int i = 0; i < 1760; i++)
            {
                if (End_Page[i] != 0)
                {
                    uint16_t x_sprite = i % 55;
                    uint16_t y_sprite = i / 55;
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle((x_sprite * 4) + 10, (y_sprite * 4) + 76, 4, 4, ST7789_BLUE);
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }
            }

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