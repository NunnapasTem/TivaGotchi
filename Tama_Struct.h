/*
 * Tama_Struct.h
 *
 *  Created on: Nov 29, 2024
 *  Author: Nina Temridiwong
 */

#ifndef TAMA_STRUCT_H_
#define TAMA_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>
#include <float.h>

typedef struct tama_t
{
    uint16_t age;
    float weight;
    uint8_t growth_stage;
    const uint32_t *sprite_L;
    const uint32_t *sprite_R;
    int8_t hunger;
    int8_t happiness;
    int8_t health;
    int8_t discipline;
    int8_t poop;

    // Tama Status
    bool sleeping;
    bool dead;
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
