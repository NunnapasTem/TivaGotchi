// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include "./G8RTOS/G8RTOS.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define BUTTONS_FIFO        0
#define JOYSTICK_FIFO       1
#define FREQ1_FIFO          2
#define FREQ2_FIFO          3
#define DISPLAY_FIFO        4
#define MIC_DAC_FIFO        5

/*************************************Defines***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_UART;
semaphore_t sem_I2CA;
semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;
semaphore_t sem_Joystick_Debounce;
semaphore_t sem_KillCube;

/***********************************Semaphores**************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/

void Draw_Sprite(const uint32_t* sprite, uint8_t pos_x, uint8_t pos_y, uint16_t color, uint8_t size);
/*******************************Background Threads**********************************/

void Idle_Thread(void);
void Read_Buttons(void);
void test(void);


/*******************************Background Threads**********************************/

/********************************Periodic Threads***********************************/
void Display_Thread(void);
void Clock_Thread(void);

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/

void Button_Handler(void);
void DAC_Timer_Handler(void);

/*******************************Aperiodic Threads***********************************/


#endif /* THREADS_H_ */

