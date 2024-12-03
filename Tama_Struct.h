/*
 * Tama_Struct.h
 *
 *  Created on: Nov 29, 2024
 *      Author: Nina Temridiwong
 */

#ifndef TAMA_STRUCT_H_
#define TAMA_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>
#include <float.h>

typedef struct tama_t {

    uint16_t age;
    float weight;
    uint8_t growth_stage;
    const uint32_t* sprite_L;
    const uint32_t* sprite_R;
    int8_t hunger;
    int8_t happiness;
    int8_t health;
    int8_t discipline;
    int8_t poop;

    // Tama Status
    bool sleeping;
    bool dead;
//    uint16_t age = 0;
//    float weight = 1;
//    uint8_t growth_stage = 0;
//    static const uint32_t* sprite;
//
//    int8_t hunger = 10;
//    int8_t happiness = 10;
//    int8_t health = 10;
//    int8_t discipline = 10;
//    int8_t poop = 10;
//
//    // Tama Status
//    bool sleeping = false;
//    bool dead = false;
} tama_t;

typedef enum
{
    NONE = 0,
    FEED = 1,
    LIGHT = 2,
    PLAY = 3,
    MEDICINE = 4,
    DUCK = 5,
    HEALTH_METER = 6,
    DISCIPLINE = 7,
    ATTENTION = 8,
    OTHER = 9
} menu_t;

#endif /* TAMA_STRUCT_H_ */
