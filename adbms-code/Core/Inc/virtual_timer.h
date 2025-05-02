#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "ad_system_prams.h"

typedef void (*Callback)();

typedef struct
{
    uint32_t tickstart;
    uint32_t durations_ms;
    Callback cb;
} timer_;

typedef struct
{
    timer_ timers[NUM_TIMERS]; // Array of timers
} timer_group_;

timer_ CreateTimer(uint32_t duration_ms, Callback cb);

timer_group_ *CreateTimerGroup(timer_ timers[NUM_TIMERS]);

void TickTimerGroup(timer_group_ *tg);

void TickChargerTimer(timer_group_ *tg);