#include "nfr_can_driver.h"

// Populates a RawCanSignal with data, given a start bit, lenght, factor, and offset
void populateRawMessage(RawCanSignal *signal, float raw_data, int length, float factor, float offset)
{
    // offset and factor data
    uint64_t raw_value = (uint64_t)((raw_data - offset) / factor);

    // copy data into RawCanSignal
    memcpy(&(signal->raw_data), &raw_value, sizeof(raw_value));
    signal->length = length;

}

// Helper function to printBytes of a RawCanSignal during debugging
void printBytes(RawCanSignal *data)
{
    printf("Bytes: ");
    for (int i = 0; i < 8; i++)
    {
        printf("%02X ", data->raw_data.bytes[i]);
    }
    printf("\n");
}

// Generates a mask for given position and length
uint64_t generateMask(int pos, int len)
{
    return (0xFFFFFFFFFFFFFFFFull << (64 - len) >> (64 - len - pos));
}

// Encodes a CAN msg with number of sub-messages
void encodeSignals(uint8_t *data, int count, ...)
{
    va_list args;
    va_start(args, count);
    uint64_t buffer = 0;

    int len_ptr = 0;
    for (size_t i = 0; i < count; i++)
    {
        RawCanSignal temp = va_arg(args, RawCanSignal);

        // shift data over and apply mask then add to buffer
        uint64_t mask = generateMask(len_ptr, temp.length);
        buffer |= (temp.raw_data.as_uint64 << len_ptr) & mask;

        // update length
        len_ptr += temp.length;
    }

    // fill in msg
    for (size_t i = 0; i < 8; i++)
    {
        data[i] = (buffer >> (i * 8)) & 0xFF;
    }
}
