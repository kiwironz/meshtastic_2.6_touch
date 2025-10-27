/*
 * Minimal CRC32 implementation compatible with ErriezCRC32.h
 * This file is a small fallback to provide crc32Buffer and friends
 * in case the ErriezCRC32 library isn't compiled/linked for this build.
 */

#include "ErriezCRC32.h"
#include <string.h>

uint32_t crc32Update(const void *buffer, size_t bufferLength, uint32_t crc)
{
    const uint8_t *p = (const uint8_t *)buffer;
    while (bufferLength--) {
        crc ^= (uint32_t)(*p++);
        for (int i = 0; i < 8; ++i) {
            if (crc & 1) {
                crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

uint32_t crc32Final(uint32_t crc)
{
    return crc ^ 0xFFFFFFFFUL;
}

uint32_t crc32Buffer(const void *buffer, size_t bufferLength)
{
    uint32_t crc = CRC32_INITIAL;
    crc = crc32Update(buffer, bufferLength, crc);
    return crc32Final(crc);
}

uint32_t crc32String(const char *buffer)
{
    return crc32Buffer(buffer, strlen(buffer));
}
