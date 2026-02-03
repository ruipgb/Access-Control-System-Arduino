#include "usart.h"

// =========================================================

void usart_inic(void)    // inicialização
{
    // BAUD = 9600, F_CPU = 16 MHz, modo assíncrono normal (U2X0 = 0),UBRR0 = 103
    UCSR0A &= ~(1 << U2X0);
    UBRR0H = 0;
    UBRR0L = 103;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << TXEN0);
}

// =========================================================

void usart_caracter(char caracter)    // função para enviar um caracter
{
    while ( (UCSR0A & (1 << UDRE0)) == 0 )
    {
    }
    UDR0 = (uint8_t)caracter;
}

// =========================================================

void usart_frase(const char *letra)    // função para enviar frase
{
    while (*letra != '\0')
    {
        usart_caracter(*letra);
        letra++;
    }
}

// =========================================================

void usart_linha(const char *frase)    // função para enviar frase com \n
{
    usart_frase(frase);
    usart_caracter('\n');
}
