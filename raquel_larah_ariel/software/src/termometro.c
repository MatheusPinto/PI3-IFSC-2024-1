#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "termometro.h"

#define TERMO_SKIP_ROM 0xCC
#define TERMO_CONVERT 0x44
#define TERMO_READ_SP 0xBE

#define OW_FORCE_HIGH()           \
    do                            \
    {                             \
        TERMO_PORT |= TERMO_MASK; \
        TERMO_DDR |= TERMO_MASK;  \
    } while (0)

#define OW_FORCE_LOW()             \
    do                             \
    {                              \
        OW_FORCE_HIGH();           \
        TERMO_PORT &= ~TERMO_MASK; \
    } while (0)

#define OW_RELEASE()               \
    do                             \
    {                              \
        TERMO_PORT &= ~TERMO_MASK; \
        TERMO_DDR &= ~TERMO_MASK;  \
    } while (0)

static uint8_t oneWire_reset();
static uint8_t oneWire_read();
static void oneWire_write(uint8_t);

static termo_result status = TERMO_UNINIT;
static volatile uint16_t timer;

termo_result termo_conv()
{
    if (status == TERMO_UNINIT || status == TERMO_READY)
    {
        if (oneWire_reset())
            status = TERMO_ERROR_COM;
        else
        {
            oneWire_write(TERMO_SKIP_ROM);
            oneWire_write(TERMO_CONVERT);
            status = TERMO_BUSY;
            timer = 1000;
        }
    }
    return status;
}

uint8_t ds18b20crc8(uint8_t *data, uint8_t length)
{
    // Generate 8bit CRC for given data (Maxim/Dallas)

    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t mix = 0;
    uint8_t crc = 0;
    uint8_t byte = 0;

    for (i = 0; i < length; i++)
    {
        byte = data[i];

        for (j = 0; j < 8; j++)
        {
            mix = (crc ^ byte) & 0x01;
            crc >>= 1;
            if (mix)
                crc ^= 0x8C;
            byte >>= 1;
        }
    }
    return crc;
}

termo_result termo_read(int16_t *temperatura)
{
    if (status == TERMO_BUSY && timer == 0)
    {
        if (oneWire_reset())
            status = TERMO_ERROR_COM;
        else
        {
            oneWire_write(TERMO_SKIP_ROM);
            oneWire_write(TERMO_READ_SP);
            uint8_t scratch[9];
            uint8_t all = 0;
            for (uint8_t i = 0; i < 9; i++)
            {
                scratch[i] = oneWire_read();
                all |= scratch[i];
            }
            if (all == 0)
            {
                status = TERMO_ERROR_PULL;
            }
            else if (scratch[8] != ds18b20crc8(scratch, 8))
            {
                status = TERMO_ERROR_CRC;
            }
            else
            {
                *temperatura = scratch[1];
                *temperatura <<= 8;
                *temperatura |= scratch[0];
                *temperatura >>= 4;
                status = TERMO_READY;
            }
        }
    }
    return status;
}

void termo_update()
{
    if (status == TERMO_BUSY && timer > 0)
    {
        timer -= 1;
    }
}

uint8_t oneWire_reset()
{
    uint8_t sreg = SREG; // Store status register
    cli();
    OW_FORCE_LOW();
    _delay_us(600);
    OW_RELEASE();
    _delay_us(70);
    uint8_t response = TERMO_PORTIN & TERMO_MASK;
    while ((TERMO_PORTIN & TERMO_MASK) == 0)
        ; // espera o barramento ficar livre
    OW_FORCE_HIGH();
    _delay_us(600);
    SREG = sreg; // Restore status register
    return response;
}

static void oneWire_write0()
{
    OW_FORCE_LOW();
    _delay_us(80);
    OW_FORCE_HIGH();
    _delay_us(2);
}

static void oneWire_write1()
{
    OW_FORCE_LOW();
    _delay_us(8);
    OW_FORCE_HIGH();
    _delay_us(80);
}

static void oneWire_write(uint8_t byte)
{
    uint8_t sreg = SREG;
    cli();
    for (uint8_t i = 0; i < 8; i++)
    {
        if ((byte & 1) == 0)
        {
            oneWire_write0();
        }
        else
        {
            oneWire_write1();
        }
        byte >>= 1;
    }
    SREG = sreg;
}

static uint8_t oneWire_read_bit()
{
    OW_FORCE_LOW();
    _delay_us(2);
    OW_RELEASE();
    _delay_us(5);
    uint8_t bit = TERMO_PORTIN & TERMO_MASK;
    _delay_us(60);
    return bit;
}

static uint8_t oneWire_read()
{
    uint8_t sreg = SREG;
    cli();
    uint8_t data = 0;

    for (uint8_t i = 1; i != 0; i <<= 1)
        data |= oneWire_read_bit() * i;

    SREG = sreg;
    return data;
}
