#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "lcd_driver.h"

#define KEY_MASK    (0x1FU)

/**
 * @brief Sets corresponding GPIO for specified key row
 * @param row Row to activate
 */
static void key_row_activate(uint8_t row)
{
    switch (row)
    {
        case 0U:
            PORTE = 0x00U;
            PORTC = _BV(PC5);
            break;
        case 1U:
            PORTE = 0x00U;
            PORTC = _BV(PC6);
            break;
        case 2U:
            PORTE = 0x00U;
            PORTC = _BV(PC7);
            break;
        case 3U:
            PORTE = _BV(PE2);
            PORTC = 0x00U;
            break;
        case 4U:
            PORTE = _BV(PE1);
            PORTC = 0x00U;
            break;
        default:
            PORTE = 0x00U;
            PORTC = 0x00U;
            break;
    }
}

/**
 * @brief  Deactivates all key rows
 */
static inline void key_row_off(void)
{
    PORTE = 0x00U;
    PORTC = 0x00U;
}

/**
 * @brief Returns bitmask of pressed keys
 * @return uint8_t Bitmask of pressed keys
 * @see KEY_MASK
 */
static inline uint8_t key_row_get(void)
{
    return (PINC & KEY_MASK);
}

/**
 * @brief Returns key position from roh key bitmask
 * @param mask Bitmask with pressed key
 * @return uint8_t Key position
 */
static uint8_t key_position(uint8_t mask)
{
    uint8_t cnt = 0U;

    if (mask == 0x00U)
    {
        return 0U;
    }

    while (mask != 0x01U)
    {
        mask >>= 1U;
        ++cnt;
    }

    return cnt;
}

/**
 * @brief Computes key code from active key row and key
 * @param row Key row
 * @param key Pressed key in the row
 * @return uint8_t Key code
 */
static inline uint8_t compute_keycode(uint8_t row, uint8_t key)
{
    return ((5U * row) + key);
}

/**
 * @brief Handles key pressed event
 * @param key_code Key code of pressed key
 */
static void key_pressed_event(uint8_t key_code)
{
    switch (key_code)
    {
        case 2U:
            lcd_write_char('0');
            break;
        case 6U:
            lcd_write_char('1');
            break;
        case 7U:
            lcd_write_char('2');
            break;
        case 8U:
            lcd_write_char('3');
            break;
        case 11U:
            lcd_write_char('4');
            break;
        case 12U:
            lcd_write_char('5');
            break;
        case 13U:
            lcd_write_char('6');
            break;
        case 16U:
            lcd_write_char('7');
            break;
        case 17U:
            lcd_write_char('8');
            break;
        case 18U:
            lcd_write_char('9');
            break;

        case 24U:
            lcd_clear();
            break;
        default:
            /* Unknown key code, do nothing */
            break;
    }
}

static uint8_t key_row = 0U;
static uint8_t key_mask = 0U;

/**
 * @brief Function main
 * @return int Always 0, but normally never returns
 */
int main(void)
{
    /* Disable interrupts */
    cli();

    /* Initialize GPIOs for key rows and columns */
    {
        DDRC = _BV(PC5) | _BV(PC6) | _BV(PC7);
        PORTC = 0x00U;

        DDRE = _BV(PE2) | _BV(PE1);
        PORTE = 0x00U;
    }

    lcd_driver_init();

    lcd_write_str(LINE_0, "calc started");
    _delay_ms(2000U);

    lcd_clear();

    while (1)
    {
        uint8_t cnt = 0U;

        /* Activate row of keys and get key mask of pressed key(s) */
        key_row_activate(key_row);

        if (0U != key_row_get())
        {
            while (cnt < 100U)
            {
                key_mask = key_row_get();

                if (key_mask != 0U)
                {
                    // Some key pressed
                    ++cnt;
                    _delay_ms(1U);
                    continue;
                }

                break;
            }
        }
        else
        {
            key_mask = 0U;
        }

        /* Check, if one and only one key is pressed */
        if ((key_mask != 0U) && ((key_mask & (key_mask - 1U)) == 0U))
        {
            const uint8_t key_code = compute_keycode(key_row, key_position(key_mask));

            key_pressed_event(key_code);

            cnt = 0U;

            while (cnt < 100U)
            {
                if (0 == key_row_get())
                {
                    ++cnt;
                }
                else
                {
                    cnt = 0U;
                }
                _delay_ms(1U);
            }
        }

        key_row_off();

        /* Next row */
        key_row = (key_row > 4U) ? 0U : (key_row + 1U);
    }

    return 0;
}

