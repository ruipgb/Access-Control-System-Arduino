#ifndef USART_H
#define USART_H

#include <avr/io.h>
#include <stdint.h>

void usart_inic(void);
void usart_caracter(char caracter);
void usart_frase(const char *letra);
void usart_linha(const char *frase);

#endif
