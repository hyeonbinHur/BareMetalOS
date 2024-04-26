#include "uart0.h"
#include "../src/mbox.h"
#include "../src/q1.h"
/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */

int currentBaud = 115200;
int currentDataBits = 8;
int currentStopBits = 1;
int currentParityBits = 0;
int currentHandshaking = 0;

void uart_init(const char *baud, const char *stop, const char *data, const char *parity, const char *handshaking)
{
	unsigned int r;

	/* Turn off UART0 */
	UART0_CR = 0x0;

	/* NEW: set up UART clock for consistent divisor values
	--> may not work with QEMU, but will work with real board */
	mBuf[0] = 9 * 4;
	mBuf[1] = MBOX_REQUEST;
	mBuf[2] = MBOX_TAG_SETCLKRATE; // set clock rate
	mBuf[3] = 12;				   // Value buffer size in bytes
	mBuf[4] = 0;				   // REQUEST CODE = 0
	mBuf[5] = 2;				   // clock id: UART clock
	mBuf[6] = 48000000;			   // rate: 48Mhz
	mBuf[7] = 0;				   // clear turbo
	mBuf[8] = MBOX_TAG_LAST;
	mbox_call(ADDR(mBuf), MBOX_CH_PROP);

	/* Setup GPIO pins 14 and 15 */

	/* Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0    */
	r = GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));		// clear bits 17-12 (FSEL15, FSEL14)
	r |= (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
	GPFSEL1 = r;

	/* enable GPIO 14, 15 */
#ifdef RPI3	   // RBP3
	GPPUD = 0; // No pull up/down control
	// Toogle clock to flush GPIO setup
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}								   // waiting 150 cycles
	GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}			   // waiting 150 cycles
	GPPUDCLK0 = 0; // flush GPIO setup

#else // RPI4
	r = GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
	GPIO_PUP_PDN_CNTRL_REG0 = r;

#endif

	/* Mask all interrupts. */
	UART0_IMSC = 0;

	/* Clear pending interrupts. */
	UART0_ICR = 0x7FF;

	uart_BaudRate(baud);
	

	int devider = 16 * currentBaud;
	double devider2 = (48000000.0 / (float)devider);
	int intPart = (int)devider2 / 1;
	float frPart = ((devider2 - (float)intPart) * 64) + 0.5;

	UART0_IBRD = intPart;
	UART0_FBRD = frPart;

	UART0_LCRH = (1 << 4);

	uart_StopBit(stop);
	uart_DataBit(data);
	uart_Parity(parity);
	uart_HandShaking(handshaking);

	/* Enable UART0, receive, and transmit */
	UART0_CR = 0x301; // enable Tx, Rx, FIFO
}

/**
 * Send a character
 */
void uart_sendc(char c)
{

	/* Check Flags Register */
	/* And wait until transmitter is not full */
	do
	{
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_TXFF);

	/* Write our data byte out to the data register */
	UART0_DR = c;
}

/**
 * Receive a character
 */
char uart_getc()
{
	char c = 0;

	/* Check Flags Register */
	/* Wait until Receiver is not empty
	 * (at least one byte data in receive fifo)*/
	do
	{
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_RXFE);

	/* read it and return */
	c = (unsigned char)(UART0_DR);

	/* convert carriage return to newline */
	return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(char *s)
{
	while (*s)
	{
		/* convert newline to carriage return + newline */
		if (*s == '\n')
			uart_sendc('\r');
		uart_sendc(*s++);
	}
}

/**
 * Display a value in hexadecimal format
 */
void uart_hex(unsigned int num)
{
	uart_puts("0x");
	for (int pos = 28; pos >= 0; pos = pos - 4)
	{

		// Get highest 4-bit nibble
		char digit = (num >> pos) & 0xF;

		/* Convert to ASCII code */
		// 0-9 => '0'-'9', 10-15 => 'A'-'F'
		digit += (digit > 9) ? (-10 + 'A') : '0';
		uart_sendc(digit);
	}
}

/*
**
* Display a value in decimal format
*/
void uart_dec(int num)
{
	// A string to store the digit characters
	char str[33] = "";

	// Calculate the number of digits
	int len = 1;
	int temp = num;
	while (temp >= 10)
	{
		len++;
		temp = temp / 10;
	}

	// Store into the string and print out
	for (int i = 0; i < len; i++)
	{
		int digit = num % 10; // get last digit
		num = num / 10;		  // remove last digit from the number
		str[len - (i + 1)] = digit + '0';
	}
	str[len] = '\0';

	uart_puts(str);
}

void uart_displayMAC(unsigned int high, unsigned int low) {
    // Start by processing the lower part first to match the original output order
    for (int shift = 12; shift >= 0; shift -= 4) {
        // Extract the highest 4-bit segment
        char hex_char = (low >> shift) & 0xF;
        // Convert numeric value to hexadecimal ASCII character
        hex_char += (hex_char > 9) ? ('A' - 10) : '0';
        uart_sendc(hex_char);

        // Insert dash at every 8 bits, except at the end
        if (shift % 8 == 0 && shift != 0) {
            uart_sendc(':');
        }
    }

    // Insert final dash between high and low parts
    uart_sendc(':');

    // Then process the high part
    for (int shift = 28; shift >= 0; shift -= 4) {
        // Extract the highest 4-bit segment
        char hex_char = (high >> shift) & 0xF;
        // Convert numeric value to hexadecimal ASCII character
        hex_char += (hex_char > 9) ? ('A' - 10) : '0';
        uart_sendc(hex_char);

        // Insert dash every 8 bits
        if (shift % 8 == 0 && shift != 0) {
            uart_sendc(':');
        }
    }
}

void uart_BaudRate(const char *baud)
{
	if (my_strncmp(baud, "9600", 4) == 1)
	{
		currentBaud = 9600;
	}
	else if (my_strncmp(baud, "19200", 5) == 1)
	{
		currentBaud = 19200;
	}
	else if (my_strncmp(baud, "38400", 5) == 1)
	{
		currentBaud = 38400;
	}
	else if (my_strncmp(baud, "57600", 5) == 1)
	{
		currentBaud = 57600;
	}
	else if (my_strncmp(baud, "115200", 6) == 1)
	{
		currentBaud = 115200;
	}

}

void uart_DataBit(const char *data)
{

	if (my_strncmp(data, "8", 1) == 1)
	{
		currentDataBits = 8;
		UART0_LCRH &= ~(0b11 << 5);
		UART0_LCRH |= UART0_LCRH_WLEN_8BIT;
	}
	else if (my_strncmp(data, "7", 1) == 1)
	{
		currentDataBits = 7;
		UART0_LCRH &= ~(0b11 << 5);
		UART0_LCRH |= UART0_LCRH_WLEN_7BIT;
	}
	else if (my_strncmp(data, "6", 1) == 1)
	{
		currentDataBits = 6;
		UART0_LCRH &= ~(0b11 << 5);
		UART0_LCRH |= UART0_LCRH_WLEN_6BIT;
	}
	else if (my_strncmp(data, "5", 1) == 1)
	{
		currentDataBits = 5;
		UART0_LCRH |= UART0_LCRH_WLEN_5BIT;
	}
	else
	{
		if (currentDataBits == 8)
		{
			UART0_LCRH |= UART0_LCRH_WLEN_8BIT;
		}
		else if (currentDataBits == 7)
		{
			UART0_LCRH |= UART0_LCRH_WLEN_7BIT;
		}
		else if (currentDataBits == 6)
		{
			UART0_LCRH |= UART0_LCRH_WLEN_6BIT;
		}
		else if (currentDataBits == 5)
		{

			UART0_LCRH |= UART0_LCRH_WLEN_5BIT;
		}
	}
}
void uart_StopBit(const char *stop)
{
	if (my_strncmp(stop, "1", 1) == 1)
	{
		currentStopBits = 1;
		UART0_LCRH &= ~(0b1 << 3);
	}
	else if (my_strncmp(stop, "2", 1) == 1)
	{
		currentStopBits = 2;
		UART0_LCRH &= ~(0b1 << 3);
		UART0_LCRH |= UART0_LCRH_STP2;
	}
	else
	{
		if (currentStopBits == 1)
		{
			UART0_LCRH &= ~(0b1 << 3);
		}
		else if (currentStopBits == 2)
		{
			UART0_LCRH &= ~(0b1 << 3);
			UART0_LCRH |= UART0_LCRH_STP2;
		}
		
	}
}
void uart_Parity(const char *parity)
{

	if (my_strncmp(parity, "none", 4) == 1)
	{
		currentParityBits = 0;
		UART0_LCRH &= ~UART0_LCRH_PEN;
	}
	else if (my_strncmp(parity, "odd", 3) == 1)
	{
		currentParityBits = 1;
		UART0_LCRH |= UART0_LCRH_PEN;
		UART0_LCRH &= ~UART0_LCRH_EPS;
	}
	else if (my_strncmp(parity, "even", 4) == 1)
	{
		currentParityBits = 2;
		UART0_LCRH |= UART0_LCRH_PEN;
		UART0_LCRH |= UART0_LCRH_EPS;
	}
	else
	{
		if (currentParityBits == 1)
		{
			UART0_LCRH |= UART0_LCRH_PEN;
			UART0_LCRH &= ~UART0_LCRH_EPS;
		}
		else if (currentParityBits == 2)
		{
			UART0_LCRH |= UART0_LCRH_PEN;
			UART0_LCRH |= UART0_LCRH_EPS;
		}
		else if (currentParityBits == 0)
		{

			UART0_LCRH &= ~UART0_LCRH_PEN;
		}
	}
}
void uart_HandShaking(const char *hand)
{

	if (my_strncmp(hand, "CTS", 3) == 1)
	{
		currentHandshaking = 1;
		UART0_CR &= ~(0b1 << 15);
		UART0_CR |= (0b1 << 15);
	}
	else if (my_strncmp(hand, "RTS", 3) == 1)
	{
		currentHandshaking = 2;
		UART0_CR &= ~(0b1 << 14);
		UART0_CR |= (0b1 << 14);
	}
	
	else if (my_strncmp(hand, "NONE", 4) == 1)
	{
		currentHandshaking = 0;
		UART0_CR &= ~(0b1 << 15);
		UART0_CR &= ~(0b1 << 14);
	}
	else
	{
		if (currentHandshaking == 0)
		{ // no hand shaking
			UART0_CR &= ~(0b1 << 15);
			UART0_CR &= ~(0b1 << 14);
		}
		else if (currentHandshaking == 1)
		{ // only CTS
			UART0_CR &= ~(0b1 << 15);
			UART0_CR |= (0b1 << 15);
		}
		else if (currentHandshaking == 2)
		{ // only RTS
			UART0_CR &= ~(0b1 << 14);
			UART0_CR |= (0b1 << 14);
		}

	}
}
void uart_setting(){
	if(currentBaud == 9600){
		uart_puts("Baudrates : 9600 \n");
	}else if(currentBaud == 19200 ){
		uart_puts("Baudrates : 19200 \n");
	}else if(currentBaud == 38400){
		uart_puts("Baudrates : 38400 \n");
	}else if(currentBaud == 57600){
		uart_puts("Baudrates : 57600 \n");
	}else if(currentBaud == 115200){
		uart_puts("Baudrates : 115200 \n");
	}

	if(currentDataBits == 8){
		uart_puts("Databits : 8 \n");
	}else if(currentDataBits == 7){
		uart_puts("Databits : 7 \n");
	}else if(currentDataBits == 6){
		uart_puts("Databits : 6 \n");
	}else if(currentDataBits == 5){
		uart_puts("Databits : 5 \n");
	}

	if(currentStopBits == 1){
		uart_puts("Stop bit  : 1 \n");

	}else if(currentStopBits == 2){
		uart_puts("Stop bit  : 2 \n");
	}

	if(currentParityBits == 0){
		uart_puts("Parity bit : NONE \n");
	}else if(currentParityBits == 1){
		uart_puts("Parity bit : odd \n");
	}else if(currentParityBits == 2){
		uart_puts("Parity bit : even \n");
	}

	if(currentHandshaking == 0){
		uart_puts("handshaking : none \n");

	}else if(currentHandshaking == 1){
		uart_puts("handshaking : cts \n");

	}else if (currentHandshaking == 2){
		uart_puts("handshaking : rts \n");

	}
}