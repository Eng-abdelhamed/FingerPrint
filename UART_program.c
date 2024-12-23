#include <xc.h>
#include "UART_interface.h"
#include "UART_Register.h"
#include "avr/interrupt.h"

// Global Variables


// USART Receive Complete Interrupt Service Routine
ISR(USART_RXC_vect)
{
    // Read the data from buffer
    rcvData[cont++] = UDR;
}

// Function to clear a character array
void clearArray(unsigned char *str)
{
    while (*str)
    {
        *str = 0;
        str++;
    }
}

// UART Initialization (Polling Mode)
void UART_Init(unsigned long baud)
{
    unsigned int UBRR = (F_CPU / (16 * baud)) - 1; // Baud rate calculation
    UBRRH = (unsigned char)(UBRR >> 8);           // Set high byte of UBRR
    UBRRL = (unsigned char)UBRR;                 // Set low byte of UBRR

    // Enable transmitter and receiver
    UCSRB = (1 << RXEN) | (1 << TXEN);

    // Asynchronous mode, 8-bit data, 1 stop bit
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

// UART Initialization (Interrupt Mode)
void UART_InterrptInit(unsigned long baud)
{
    unsigned int UBRR =  F_CPU / 16 / baud - 1;
// Baud rate calculation
    UBRRH = (unsigned char)(UBRR >> 8);           // Set high byte of UBRR
    UBRRL = (unsigned char)UBRR;                 // Set low byte of UBRR

    // Enable receiver interrupt, transmitter, and receiver
    UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);

    // Asynchronous mode, 8-bit data, 1 stop bit
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

    // Enable global interrupts
    sei();
}

// UART Receive (Polling Mode)
unsigned char UART_Receive()
{
    while ((UCSRA & (1 << RXC)) == 0); // Wait till data is received
    return UDR;                        // Return the received byte
}

// UART Transmit
void UART_Transmit(unsigned char data)
{
    while (!(UCSRA & (1 << UDRE))); // Wait until the buffer is empty
    UDR = data;                     // Put the data into the buffer
}

// Transmit Array
void UART_Array(const char *data, int size)
{
    for (int i = 0; i < size; i++)
    {
        UART_Transmit(data[i]);
    }
}

// Transmit String
void UART_String(const char *data)
{
    while (*data)
    {
        UART_Transmit(*data++);
    }
}
