#ifdef LCD_DRIVER_H
#error Already included
#else
#define LCD_DRIVER_H

#define LINE_0  (0U)
#define LINE_1  (1U)

extern void lcd_driver_init(void);
extern void lcd_write_str(uint8_t line, const char *str);
extern void lcd_write_char(char c);
extern void lcd_clear(void);

#endif

