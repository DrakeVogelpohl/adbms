#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

/* Purpose of the simple CAN library is to encode / decode CAN signals so that it aligns
   with the 8 8-bit frames that's being sent and received by the CAN.
*/

typedef struct
{
    union
    {
        uint64_t as_uint64;
        uint8_t bytes[8];
    } raw_data;
    int length;
} RawCanSignal;

void populateRawMessage(RawCanSignal *signal, float raw_data, int length, float factor, float offset);

void printBytes(RawCanSignal *data);

uint64_t generateMask(int pos, int len);

void encodeSignals(uint8_t *data, int count, ...);
