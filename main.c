// TivaGotchi, uP2 Fall 2024
// TivaGotchi - A Tamagotchi Clone
// Created: 2024-11-29
// By: Nina Temridiwong


/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>
#include "Pixel.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/************************************MAIN*******************************************/

int main(void  )
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    // You might want a delay here (~10 ms) to make sure the display has powered up
    SysCtlDelay(SysCtlClockGet()/100);

    // Initialize the G8RTOS framework
    G8RTOS_Init();
    multimod_init();

    // Initialize semaphore
    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 1);
    G8RTOS_InitSemaphore(&sem_I2CA, 1);
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_UART, 1);

    // Background Thread
    G8RTOS_AddThread(Idle_Thread, 255, "Idle");
    G8RTOS_AddThread(Read_Buttons, 1, "Buttons");
    G8RTOS_AddThread(Display_Thread, 2, "Display");

    // Aperiodic Thread
    G8RTOS_Add_APeriodicEvent(Button_Handler, 1, 20);

    // Periodic Thread
    G8RTOS_Add_PeriodicEvent(Clock_Thread, 1000, 0);
//    Draw_Sprite(Age_Icon, 31, 9, ST7789_BLUE, 4);
//    Draw_Sprite(Scale_Icon, 32, 30, ST7789_BLUE, 4);
//    Draw_Sprite(Sprite_One, 18, 10, ST7789_BLUE, 4);
//    Draw_Sprite(Sprite_Two, 12, 10, ST7789_BLUE, 4);
//    Draw_Sprite(Sprite_Zero, 6, 10, ST7789_BLUE, 4);
//    Draw_Sprite(Sprite_Year, 3, 12, ST7789_BLUE, 4);
//    Draw_Sprite(Sprite_Ounce, 3, 32, ST7789_BLUE, 4);

    G8RTOS_Launch();
    while (1);
}

/************************************MAIN*******************************************/
