#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>

extern "C" {
  #include "lcd.h"
}

extern "C" {
  #include "usart.h"
}

#define sensor_dp PD2
#define sensor_fp PD3
#define led_fp   PB6
#define led_dp   PB7

#define L0   PL0
#define L1   PL1
#define L2   PL2
#define L3   PL3

#define C0   PC0
#define C1   PC1
#define C2   PC2

#define loop_30ms   30
#define timeout_15s   15000
#define timeout_5s   5010

#define ciclos500_de30ms  ( timeout_15s / loop_30ms )
#define ciclos167_de30ms (timeout_5s / loop_30ms)

#define num_pins_validos  2

const uint16_t PULSO_ABRIR  = 1000;
const uint16_t PULSO_FECHAR = 2800;
volatile uint8_t contador_5ms = 0;

const char teclado[4][3] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

char    pin_buffer[4];
uint8_t pin_index = 0;
uint16_t ciclos30ms_desde_1digito = 0;

const char pins_validos[num_pins_validos][4] = {
  {'1','2','3','4'},
  {'2','5','8','0'}
};

volatile uint8_t flag_30ms = 0;
volatile uint8_t contador_ms = 0;

typedef enum {
  FECHADA,
  AGUARDA_VEICULO,
  AGUARDA_CODIGO,
  CODIGO_VALIDO,
  CODIGO_INVALIDO,
  ABRIR_ENTRADA,
  AGUARDA_PASSAGEM_E,
  FECHAR_ENTRADA,
  ABRIR_SAIDA,
  AGUARDA_PASSAGEM_S,
  FECHAR_SAIDA
} estado_t;

estado_t ultimo_estado_lcd = (estado_t)255;

/* ===================== SERVO ===================== */

void abrir_cancela(void) // Abre a cancela movendo o servo para a posição (+90 graus)
{
  OCR1A = PULSO_ABRIR;
}

void fechar_cancela(void) // Fecha a cancela movendo o servo para a posição (0 graus)
{
  OCR1A = PULSO_FECHAR;
}

/* ===================== SENSORES IR ===================== */

uint8_t sensor_dp_detetado(void) // Verifica se o sensor dentro do parque deteta um veículo
{
  if ((PIND & (1 << sensor_dp)) == 0) // verifica se PD2 está a 0
  { 
    return 1; //deteta
  }
  else
  {
    return 0; //não deteta
  }
}

uint8_t sensor_fp_detetado(void) // Verifica se o sensor fora do parque deteta um veículo
{
  if ((PIND & (1 << sensor_fp)) == 0) //verifica se PD3 está a 0
  {
    return 1; //deteta
  }
  else
  { 
    return 0; //não deteta
  }
}

/* ===================== LEDS ===================== */

void atualiza_leds(void) // Atualiza o estado dos LEDs conforme os sensores IR
{
  if (sensor_fp_detetado()==1)
  {
    PORTB |= (1 << led_fp); //põe PB6 a 1 (ligado)
  }
  else
  {
    PORTB &= ~(1 << led_fp); 
  }
  if (sensor_dp_detetado()==1)
  {
    PORTB |= (1 << led_dp); //põe PB7 a 1 (ligado)
  }
  else
  {
    PORTB &= ~(1 << led_dp); 
  }
}

/* ===================== TECLADO ===================== */

inline void linhas_desligadas(void) // Desliga todas as linhas do teclado colocando-as a HIGH
{
  PORTL |= (1<<L0) | (1<<L1) | (1<<L2) | (1<<L3); // põe PL0,PL1,PL2,PL3 a 1 (desligadas)
}

void linha_ligada(uint8_t l) // Ativa apenas uma linha do teclado colocando-a a LOW
{
  linhas_desligadas();

  switch (l)
  {
    case 0:
      PORTL &= ~(1 << L0); //põe PL0 a 0 (ligada)
      break;

    case 1:
      PORTL &= ~(1 << L1);
      break;

    case 2:
      PORTL &= ~(1 << L2);
      break;

    case 3:
      PORTL &= ~(1 << L3);
      break;

    default: // valor inválido nenhuma linha ativa
      break;
  }
}

uint8_t ler_colunas(void) // Lê qual coluna do teclado está ativa (pressionada)
{
  uint8_t p = PINC;

  if ((p & (1 << C0)) == 0) //verifica se PC0 está a 0 (pressionada)
  {
    return 0;
  }
  if ((p & (1 << C1)) == 0)
  {
    return 1;
  }
  if ((p & (1 << C2)) == 0)
  {
    return 2;
  }

  return 0xFF; //nenhuma pressionada
}

char leitura_feita(void) // Faz o varrimento do teclado e devolve a tecla pressionada
{
  uint8_t l;
  for (l = 0; l < 4; l++)  // corre as 4 linhas
  {
    linha_ligada(l);
    _delay_us(10);

    uint8_t c = ler_colunas();
    if (c != 0xFF) // se tiver coluna pressionada 
    {
      linhas_desligadas(); 
      return teclado[l][c]; //devolve tecla
    }
  }

  linhas_desligadas();
  return 0; // nenhuma tecla pressionada
}

char leitura_completa(void) // Devolve a tecla apenas uma vez por pressão (anti-repetição)
{
  static char ultima_tecla = 0;
  char k = leitura_feita();

  if (k != 0) // se tiver tecla pressionada
  {
    if (k != ultima_tecla) // se não for a mesma que a anterior
    {
      ultima_tecla = k; 
      return k; // devolve tecla
    }
    else
    {
      return 0; // devolve nada 
    }
  }
  else
  {
    ultima_tecla = 0;
    return 0; //devolve nada
  }
}

/* ===================== PIN ===================== */

void pin_reset(void) // Reinicia o PIN e o contador de timeout
{
  pin_index = 0;
  ciclos30ms_desde_1digito = 0;
}

bool pin_e_valido(void) // Verifica se o PIN introduzido corresponde a um PIN válido
{
  uint8_t i;
  uint8_t j;

  for (i = 0; i < num_pins_validos; i++) //corre os pins validos
  {
    bool igual = true;

    for (j = 0; j < 4; j++) //corre os 4 digitos do pin
    {
      if (pin_buffer[j] != pins_validos[i][j]) // verifica se é valido
      {
        igual = false;
        break;
      }
    }

    if (igual == true)
    {
      return true; // devolve pin valido
    }
  }

  return false; // devolve pin invalido
}

void lcd_mostra_pin_progresso(void) // Mostra no LCD o progresso do PIN com asteriscos
{
  uint8_t i;

  lcd_limpar_linha(1);
  lcd_sel_cursor(1, 0);
  lcd_escreve_frase("PIN: ");

  for (i = 0; i < pin_index; i++) //corre pin_index vezes
  {
    lcd_caracter('*');
  }
}

void usart_mostra_pin_progresso(void) // Envia os dígitos do PIN para a porta série
{
  uint8_t i;

  for (i = 0; i < pin_index; i++) //corre pin_index vezes
  {
    usart_caracter(pin_buffer[i]);
  }

  usart_caracter('\n');
}

int8_t trata_tecla(char k) // Processa uma tecla pressionada e gere a introdução do PIN
{
  if (k == '*' && pin_index < 4) // se tecla for * e o pin ainda não tiver concluido
  {
    pin_reset(); // limpa pin
    lcd_msg("PIN limpo", "");
    usart_linha("Pin apagado");
    lcd_mostra_pin_progresso();
    //usart_mostra_pin_progresso();
    return -1; // continuar a espera de pin 
  }

  if (pin_index < 4 && k != '*') //se pin não estiver completo
  {
    pin_buffer[pin_index] = k; //atualiza o pin_buffer
    pin_index ++;
    lcd_mostra_pin_progresso();
    usart_mostra_pin_progresso();
  }

  if (pin_index == 4) // se pin completo
  {
    bool valido = pin_e_valido();
    int8_t resultado;

    if (valido == true)  // se pin válido
    {
      lcd_msg("PIN correto", "");
      usart_linha("Pin correto");
      resultado = 1;  // válido
    }
    else
    {
      lcd_msg("PIN errado", "");
      usart_linha("Pin incorreto");
      resultado = 0; // inválido
    }

    pin_reset();
    lcd_mostra_pin_progresso();
    //usart_mostra_pin_progresso();

    return resultado; // devolve se é valido ou não
  }

  return -1; // pin incompleto
}

uint8_t codigo_valido(void) // Função bloqueante que aguarda e valida a introdução completa do PIN
{
  uint8_t ciclos30ms_5s_sem_tecla;
  char tecla;
  int8_t resultado_pin;

  //pin_reset();
  lcd_msg("Insira o codigo", "");
  usart_linha("Inserir o codigo");
  usart_linha("PIN:");
  lcd_mostra_pin_progresso();
  usart_mostra_pin_progresso();

  ciclos30ms_5s_sem_tecla = 0;
  ciclos30ms_desde_1digito = 0;

  while (1) // loop pin 4 digitos vaLido/inválido
  {
    if (flag_30ms == 1) // se passaram 30 ms
    {
      flag_30ms = 0; 

      atualiza_leds();
      tecla = leitura_completa();

      if (tecla != 0)  // se existe tecla
      {
        ciclos30ms_5s_sem_tecla = 0; // reseta contador 5s

        resultado_pin = trata_tecla(tecla);
        if (resultado_pin == 1)  // se válido
        {
          _delay_ms(500);
          return 1; // pin aceite 
        }
        else if (resultado_pin == 0) // se inválido
        {
          _delay_ms(500);
          return 0; // pin rejeitado
        }
      }
      else // se não existe tecla
      {
        if (pin_index == 0)
        {
          if (ciclos30ms_5s_sem_tecla >= ciclos167_de30ms) // se os ciclos de 30ms = 167 
          {
            lcd_msg("Tempo limite", "");
            usart_linha("Atingiu o tempo limite sem digito");
            pin_reset();
            _delay_ms(500);
            return 0; // devolve timeout
          }
          else
          {
            ciclos30ms_5s_sem_tecla++;
          }
        }
      }

      if (pin_index == 0)
      {
        ciclos30ms_desde_1digito = 0; // reseta contador 15s
      }
      else // se tiver 1 digito ou mais
      {
        if (ciclos30ms_desde_1digito >= ciclos500_de30ms) // se os ciclos de 30 ms = 500
        {
          lcd_msg("Tempo limite", "");
          usart_linha("Atingiu o tempo limite para inserir o codigo");
          pin_reset();
          _delay_ms(500);
          return 0; // devolve timeout
        }
        else
        {
          ciclos30ms_desde_1digito++; 
        }
      }
    }
  }
}

/* ===================== LCD/USART ESTADO ===================== */

void lcd_estado(estado_t estado) // Atualiza o LCD e USART conforme o estado atual da máquina de estados
{
  if (estado == ultimo_estado_lcd){ // se o estado atual é igual ao ultimo estado 
    return;
  }
  ultimo_estado_lcd = estado; // atualiza ultimo estado

  switch (estado)  // atualiza usart e lcd 
  {
    case FECHADA:
      usart_linha("---------Ciclo---------");
      break;

    case AGUARDA_VEICULO:
      lcd_msg("Aguardando veicu", "lo");
      usart_linha("A espera de carro");
      break;

    case AGUARDA_CODIGO:
      lcd_msg("Insira o codigo", "");
      lcd_mostra_pin_progresso();
      usart_linha("veículo no sensor de fora");
      break;

    case CODIGO_VALIDO:
      break;

    case CODIGO_INVALIDO:
      break;

    case ABRIR_ENTRADA:
      usart_linha("Cancela abriu");
      break;

    case AGUARDA_PASSAGEM_E:
      lcd_msg("Aguarda passagem", "A entrar");
      usart_linha("Veículo a entrar");
      break;

    case FECHAR_ENTRADA:
      usart_linha("Cancela fechou, veiculo entrou");
      break;

    case ABRIR_SAIDA:
      usart_linha("Veículo no sensor de dentro, cancela abriu");
      break;

    case AGUARDA_PASSAGEM_S:
      lcd_msg("Aguarda passagem", "A sair");
      usart_linha("Veículo a sair");
      break;

    case FECHAR_SAIDA:
      usart_linha("Cancela fechou, veiculo saiu");
      break;
  }
}

/* ===================== TIMER ===================== */

void timer0e2_inic(void) // Configura o Timer0 para 1ms e o Timer2 para ~5ms
{
  TCCR0A = (1 << WGM01); // timer0 em modo CTC, t= 1ms, prescaler 64, interrupção ativa
  TCCR0B = 0;
  OCR0A = 249;
  TIMSK0 = (1 << OCIE0A);
  TCCR0B |= (1 << CS01) | (1 << CS00);

  DDRB |= (1 << PB4);  // led_pisca
  PORTB &= ~ (1 << PB4); //led desligado

  TCCR2A = (1 << WGM21); // timer2 em modo CTC, t= 5ms, prescaler 1024, interrupção ativa
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
  OCR2A = 77;
  TIMSK2 = (1 << OCIE2A);
}

ISR(TIMER0_COMPA_vect) // interrupção do timer0, conta milissegundos e ativa flag a cada 30ms
{
  contador_ms++;
  if (contador_ms >= loop_30ms) // se fez 30ms
  {
    contador_ms = 0;
    flag_30ms = 1; // passaram 30ms 
  }
}

ISR(TIMER2_COMPA_vect) // interrupção do timer2, toogle o LED a cada 500ms
{
  contador_5ms++;
  if ( contador_5ms == 100) // se  500 ms
  {
    PORTB ^= (1 << PB4); // toggle ao PB4
    contador_5ms= 0;
  }
}

/* ===================== INICIALIZAÇÃO ===================== */

void inic(void) // Inicializa todos os componentes do sistema
{
  lcd_inic();
  usart_inic();

  ICR1 = 39999; // timer1 em modo Fast PWM, prescaler 8, ICR1=39999=t=20ms=50hz
  OCR1A = PULSO_FECHAR;
  TCNT1 = 0;
  TCCR1A = 0b10000010;
  TCCR1B = 0b00011010;
  TCCR1C = 0;

  DDRD  &= ~((1 << sensor_dp) | (1 << sensor_fp));
  PORTD |=  (1 << sensor_dp) | (1 << sensor_fp); // sensores com pull up ativo, desligados
  
  DDRB |= (1 << PB5);
  DDRB |= (1 << led_fp) | (1 << led_dp);
  PORTB &= ~((1 << led_fp) | (1 << led_dp)); // leds desligados

  DDRL  |= (1<<L0) | (1<<L1) | (1<<L2) | (1<<L3);
  PORTL |= (1<<L0) | (1<<L1) | (1<<L2) | (1<<L3); //linhas desligadas

  DDRC  &= ~((1<<C0) | (1<<C1) | (1<<C2));
  PORTC |=  (1<<C0) | (1<<C1) | (1<<C2); //colunas desligadas

  timer0e2_inic();
  sei(); // interrupções globais ativas
  lcd_msg("Sistema iniciado", "");
  _delay_ms(500);
}

/* ===================== MAIN ===================== */

int main(void) // função principal 
{
  estado_t estado;

  inic();
  fechar_cancela();

  estado = FECHADA;

  while (1) // loop 
  {
    lcd_estado(estado);
    atualiza_leds();

    switch (estado) // estados do programa
    {
      case FECHADA:
        estado = AGUARDA_VEICULO;
        break;

      case AGUARDA_VEICULO:
        if (sensor_fp_detetado() == 1) // se sensor de fora detetado 
        {
          estado = AGUARDA_CODIGO;
        }
        else if (sensor_dp_detetado() == 1) // se sensor de dentro detetado
        {
          estado = ABRIR_SAIDA;
        }
        break;

      case AGUARDA_CODIGO:
        if (codigo_valido() == 1) // se pin valido
        {
          estado = CODIGO_VALIDO;
        }
        else
        {
          estado = CODIGO_INVALIDO;
        }
        break;

      case CODIGO_VALIDO:
        estado = ABRIR_ENTRADA;
        break;

      case CODIGO_INVALIDO:
        estado = FECHADA;
        break;

      case ABRIR_ENTRADA:
        abrir_cancela();
        estado = AGUARDA_PASSAGEM_E;
        break;

      case AGUARDA_PASSAGEM_E:
        if (sensor_dp_detetado() == 1) // se sensor de dentro detetado
        {
          while (sensor_dp_detetado() == 1) // loop para cancela fechar só quando o sensor estiver livre
          {
            atualiza_leds();
            _delay_ms(5);
          }
          estado = FECHAR_ENTRADA;
        }
        break;

      case FECHAR_ENTRADA:
        fechar_cancela();
        estado = FECHADA;
        break;

      case ABRIR_SAIDA:
        abrir_cancela();
        estado = AGUARDA_PASSAGEM_S;
        break;

      case AGUARDA_PASSAGEM_S:
        if (sensor_fp_detetado() == 1) // se sensor de fora detetado
        {
          while (sensor_fp_detetado() == 1) // loop para cancela fechar só quando o sensor estiver livre
          {
            atualiza_leds();
            _delay_ms(5);
          }
          estado = FECHAR_SAIDA;
        }
        break;

      case FECHAR_SAIDA:
        fechar_cancela();
        estado = FECHADA;
        break;
    }
  }
}
