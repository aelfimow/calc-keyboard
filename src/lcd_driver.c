#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lcd_driver.h"

#define LCD__CLEAR_DISPLAY      (0x01U)
#define LCD__RETURN_HOME        (0x02U)
#define LCD__ENTRY_MODE_SET     (0x04U)
#define LCD__DISP_CONTROL       (0x08U)
#define LCD__CURSOR_DISP_SHIFT  (0x10U)
#define LCD__FUNCTION_SET       (0x20U)
#define LCD__SET_CGRAM_ADDR     (0x40U)
#define LCD__SET_DDRAM_ADDR     (0x80U)

#define LCD_RS  PB0
#define LCD_RW  PB1
#define LCD_E   PB2

#define T_DELAY     (1U)

static inline void lcd_rs(uint8_t value)
{
    if (0U == value)
    {
        PORTB &= (uint8_t)(~_BV(LCD_RS));
    }
    else
    {
        PORTB |= (uint8_t)(_BV(LCD_RS));
    }
}

static inline void lcd_rw(uint8_t value)
{
    if (0U == value)
    {
        PORTB &= (uint8_t)(~_BV(LCD_RW));
    }
    else
    {
        PORTB |= (uint8_t)(_BV(LCD_RW));
    }
}

static inline void lcd_e(uint8_t value)
{
    if (0U == value)
    {
        PORTB &= (uint8_t)(~_BV(LCD_E));
    }
    else
    {
        PORTB |= (uint8_t)(_BV(LCD_E));
    }
}

static inline void lcd_set_data(uint8_t data)
{
    DDRA = 0xFFU;
    PORTA = data;
}

static inline uint8_t lcd_get_data(void)
{
    DDRA = 0x00U;
    return PINA;
}

static inline void lcd_data_high_Z(void)
{
    DDRA = 0x00U;
    PORTA = 0x00U;
}

static void lcd_wait_ready(void)
{
    lcd_rs(0U);
    lcd_rw(1U);

    lcd_data_high_Z();

    while (1)
    {
        lcd_e(1U);
        _delay_us(T_DELAY);

        if (0 == (0x80U & lcd_get_data()))
        {
            break;
        }

        lcd_e(0U);
        _delay_us(T_DELAY);
    }

    lcd_e(0U);
    _delay_us(T_DELAY);

    lcd_rs(1U);
}

static void lcd_write_instr(uint8_t instr)
{
    lcd_rs(0U);
    lcd_rw(0U);

    lcd_set_data(instr);

    lcd_e(1U);
    _delay_us(T_DELAY);

    lcd_e(0U);
    _delay_us(T_DELAY);

    lcd_wait_ready();
}

static void lcd_write_data(uint8_t data)
{
    lcd_rs(1U);
    lcd_rw(0U);

    lcd_set_data(data);

    lcd_e(1U);
    _delay_us(T_DELAY);

    lcd_e(0U);
    _delay_us(T_DELAY);

    lcd_wait_ready();
}

void lcd_driver_init(void)
{
    DDRB |= (uint8_t)(_BV(LCD_RS));
    PORTB |= (uint8_t)(_BV(LCD_RS));

    DDRB |= (uint8_t)(_BV(LCD_RW));
    PORTB |= (uint8_t)(_BV(LCD_RW));

    DDRB |= (uint8_t)(_BV(LCD_E));
    PORTB |= (uint8_t)(_BV(LCD_E));

    DDRA = 0x00U;
    PORTA = 0x00U;

    lcd_wait_ready();

    // Function set
    lcd_write_instr(LCD__FUNCTION_SET | 0x10U | 0x08U);

    // Display on/off
    lcd_write_instr(LCD__DISP_CONTROL | 0x04U | 0x02U);

    // Clear display
    lcd_write_instr(LCD__CLEAR_DISPLAY);

    // Set mode
    lcd_write_instr(LCD__ENTRY_MODE_SET | 0x02U);

    // Return home
    lcd_write_instr(LCD__RETURN_HOME);
}

void lcd_write_str(uint8_t line, const char *str)
{
    const uint8_t addr = (line == LINE_0) ? 0x00U : 0x40U;

    lcd_write_instr(LCD__SET_DDRAM_ADDR | addr);

    while (str[0U] != 0U)
    {
        lcd_write_data(str[0U]);
        ++str;
    }
}

void lcd_write_char(char c)
{
    lcd_write_data(c);
}

void lcd_clear(void)
{
    // Clear display
    lcd_write_instr(LCD__CLEAR_DISPLAY);

    // Return home
    lcd_write_instr(LCD__RETURN_HOME);
}

