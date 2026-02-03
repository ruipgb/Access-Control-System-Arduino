#ifndef LCD_H
#define LCD_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

void lcd_inic(void);
void lcd_comando(uint8_t cmd);
void lcd_caracter(uint8_t dados);
void lcd_limpar(void);
void lcd_sel_cursor(uint8_t linha, uint8_t coluna);
void lcd_escreve_frase(const char *str);
void lcd_limpar_linha(uint8_t linha);
void lcd_msg(const char *linha0, const char *linha1);

#endif
