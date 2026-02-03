#include "lcd.h"

// =========================================================

static void lcd_pulso_E(void)   
{
    PORTH |= (1 << PH1);    // E = 1
    _delay_us(1);
    PORTH &= ~(1 << PH1);    // E = 0
    _delay_us(50);
}

// =========================================================

static void lcd_escreve_byte(uint8_t valor, uint8_t RS)     //lcd envia 1 byte (8bits)
{
    if (RS != 0)
    {
        PORTH |= (1 << PH0);    // RS = 1 -> dados
    }
    else
    {
        PORTH &= ~(1 << PH0);    // RS = o -> comandos
    }
    PORTA = valor;    // D0-D7
    lcd_pulso_E();
}

// =========================================================

void lcd_comando(uint8_t cmd)    //função para enviar comandos
{
    lcd_escreve_byte(cmd, 0);
    _delay_us(50);
}

// =========================================================

void lcd_caracter(uint8_t dados)    //função para enviar dados
{
    lcd_escreve_byte(dados, 1);
    _delay_us(50);
}

// =========================================================

void lcd_limpar(void)    //função para limpar
{
    lcd_comando(0x01);
    _delay_ms(2);
}

// =========================================================

void lcd_sel_cursor(uint8_t linha, uint8_t coluna)    //função que seleciona o cursor
{
    uint8_t posicao;
    if (linha == 0)
    {
        posicao = coluna;
    }
    else
    {
        posicao = 0x40 + coluna;
    }
    lcd_comando(0x80 | posicao);
}

// =========================================================

void lcd_escreve_frase(const char *str)    //função para escrever frase
{
    while (*str != '\0')
    {
        lcd_caracter(*str);
        str++;
    }
}

// =========================================================

void lcd_limpar_linha(uint8_t linha)    //função para limpar a linha
{
    uint8_t i;
    lcd_sel_cursor(linha, 0);
    for (i = 0; i < 16; i++)
    {
        lcd_caracter(' ');
    }
    lcd_sel_cursor(linha, 0);
}

// =========================================================

void lcd_msg(const char *linha0, const char *linha1)    ////função para escrever nas duas linhas
{
    lcd_limpar();
    lcd_sel_cursor(0, 0);
    lcd_escreve_frase(linha0);
    lcd_sel_cursor(1, 0);
    lcd_escreve_frase(linha1);
}

// =========================================================

void lcd_inic(void)    //inicializações
{
    DDRA = 0xFF;  //portA tudo saída

    DDRH |= (1 << PH0) | (1 << PH1); 
    PORTH &= ~((1 << PH0) | (1 << PH1));

    _delay_ms(20);

    lcd_comando(0x38);    
    lcd_comando(0x08);    
    lcd_limpar();
    lcd_comando(0x06);    
    lcd_comando(0x0C);    
}
