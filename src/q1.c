#include "q1.h"
#include "../uart/uart0.h"
#include "mbox.h"

#define NULL 0

int my_strncmp(const char *command, const char *compare, int index)
{

    for (int i = 0; i < index; i++)
    {
        if (command[i] != compare[i])
        {
            return 0;
        }
    }
    return 1;
}

char *my_strstr(const char *haystack, const char *needle)
{
    if (*needle == '\0')
    {
        return (char *)haystack;
    }

    for (const char *h = haystack; *h != '\0'; h++)
    {
        const char *n = needle;
        const char *h2 = h;

        while (*n != '\0' && *h2 == *n)
        {
            h2++;
            n++;
        }

        if (*n == '\0')
        {
            return (char *)h;
        }
    }

    return "f";
}

int my_strlen(const char *str)
{
    int length = 0;
    while (*str != '\0')
    {
        str++;
        length++;
    }
    return length;
}

void printHelp(const char *command)
{
    const char *helpCommand = my_strstr(command, "help");

    if (my_strlen(command) == 4)
    {
        uart_puts(" -hlep: Show brief informaion of all commands\n");
        uart_puts(" -help <command_name>: Show full information of the command\n");
        uart_puts(" -clear: Clear screen (in the terminal it will scroll down to current position of the cursor\n");
        uart_puts(" -setcolor: set text color, and/or background color of the consolt to one of the following colors: BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE\n");
        uart_puts(" Examples: setcolor -t yellow \n");
        uart_puts("          setcolor -b yellow -t white \n");
        uart_puts(" -showinfo: Show board revision and board MAC address in correct format/ meaningful information\n");
        uart_puts(" -change baudrates: The changerble boundary for the baudrates are 9600, 19200, 38400, 57600, 115200 \n");
        uart_puts(" -change databits: The changerble boundary for the databits are 8, 7, 6, 5 \n");
        uart_puts(" -change paritybits: The changerble boundary for the parity bits are none, odd, even \n");
        uart_puts(" -change stopbits: The changerble boundary for the stop bits are 1, and 2\n");
        uart_puts(" -change handshaking: The changerble boundary for the handshaking are CTS or RTS\n");
    }
    else
    {
        if (my_strncmp(helpCommand, "f", 1) != 1)
        {
            helpCommand += 5;

            if (my_strncmp(helpCommand, "clear", 5) == 1)
            {
                uart_puts(" -clear: Clear screen (in the terminal it will scroll down to current position of the cursor\n");
            }
            else if (my_strncmp(helpCommand, "setcolor", 8) == 1)
            {
                uart_puts(" -setcolor: set text color, and/or background color of the consolt to one of the following colors: BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE\n");
                uart_puts("  Examples: setcolor -t yellow \n");
                uart_puts("            setcolor -b yellow -t white \n");
            }
            else if (my_strncmp(helpCommand, "showinfo", 8) == 1)
            {
                uart_puts(" -showinfo: Show board revision and board MAC address in correct format/ meaningful information\n");
            }
            else if (my_strncmp(helpCommand, "help", 4) == 1)
            {
                uart_puts(" -hlep: Show brief informaion of all commands\n");
            }
            else if (my_strncmp(helpCommand, "<command>", 9) == 1)
            {
                uart_puts(" -help <command_name>: Show full information of the command\n");
            }
            else if (my_strncmp(helpCommand, "change", 6) == 1)
            {
                uart_puts(" -change baudrates: The changerble boundary for the baudrates are 9600, 19200, 38400, 57600, 115200 \n");
                uart_puts(" -change databits: The changerble boundary for the databits are 8, 7, 6, 5 \n");
                uart_puts(" -change paritybits: The changerble boundary for the parity bits are none, odd, even \n");
                uart_puts(" -change stopbits: The changerble boundary for the stop bits are 1, and 2\n");
                uart_puts(" -change handshaking: The changerble boundary for the handshaking are CTS or RTS\n");
            }
        }
        else
        {
            uart_puts("NULL \n");
        }
    }
}

void changeColor(const char *command)
{
    char *textColor = my_strstr(command, "-t");

    char *bgColor = my_strstr(command, "-b");

    if (my_strncmp(textColor, "f", 5) != 1)
    {
        textColor += 3;

        if (my_strncmp(textColor, "black", 5) == 1)
        {
            uart_puts("\033[30m");
        }
        else if (my_strncmp(textColor, "red", 3) == 1)
        {
            uart_puts("\033[31m");
        }
        else if (my_strncmp(textColor, "green", 5) == 1)
        {

            uart_puts("\033[32m");
        }
        else if (my_strncmp(textColor, "yellow", 6) == 1)
        {

            uart_puts("\033[33m");
        }
        else if (my_strncmp(textColor, "blue", 4) == 1)
        {

            uart_puts("\033[34m");
        }
        else if (my_strncmp(textColor, "purple", 6) == 1)
        {

            uart_puts("\033[35m");
        }
        else if (my_strncmp(textColor, "cyan", 4) == 1)
        {

            uart_puts("\033[36m");
        }
        else if (my_strncmp(textColor, "white", 5) == 1)
        {
            uart_puts("\033[37m");
        }
    }

    if (my_strncmp(bgColor, "f", 1) != 1)
    {
        bgColor += 3;

        if (my_strncmp(bgColor, "black", 5) == 1)
        {
            uart_puts("\033[40m");
        }
        else if (my_strncmp(bgColor, "red", 3) == 1)
        {
            uart_puts("\033[41m");
        }
        else if (my_strncmp(bgColor, "green", 5) == 1)
        {
            uart_puts("\033[42m");
        }
        else if (my_strncmp(bgColor, "yellow", 6) == 1)
        {
            uart_puts("\033[43m");
        }
        else if (my_strncmp(bgColor, "blue", 4) == 1)
        {
            uart_puts("\033[44m");
        }
        else if (my_strncmp(bgColor, "purple", 6) == 1)
        {
            uart_puts("\033[45m");
        }
        else if (my_strncmp(bgColor, "cyan", 4) == 1)
        {
            uart_puts("\033[46m");
        }
        else if (my_strncmp(bgColor, "white", 5) == 1)
        {
            uart_puts("\033[47m");
        }
    }
}

void clearTerminal()
{
    uart_puts("\033[2J");
    uart_puts("\033[H");
}

void printInfo()
{
    mBuf[0] = 12 * 4;
    mBuf[1] = MBOX_REQUEST;
    // get board revision
    mBuf[2] = 0x00010002; // tag identifier : board revision
    mBuf[3] = 4;          // max response length
    mBuf[4] = 0;          // request code
    mBuf[5] = 0;          // clear

    mBuf[6] = 0x00010003; // tag identifier : MAC address
    mBuf[7] = 6;          // max response length
    mBuf[8] = 0;          // request code
    mBuf[9] = 0;          // clear
    mBuf[10] = 0;         // clear

    mBuf[11] = MBOX_TAG_LAST;

    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {

        uart_puts("\nBoard Revision: \n");
        printRecision(mBuf[5]);
        uart_puts("\n");
        uart_puts("MAC Address: \n");

        // uart_MAC(mBuf[9] ,mBuf[10]);
        uart_hex(mBuf[9]);
        uart_hex(mBuf[10]);

        uart_puts("\n");
    }
    else
    {

        // uart_puts("Unable to query!\n");
    }

    // get MAC address 0x00010003
}

void printRecision(int revision)
{
    if (revision == 0x00a22042)
    {
        uart_puts("The Board is Pi 2 Model B v1.2 with 1GB");
    }
    else if (revision == 0x00900092)
    {
        uart_puts("The Board is Pi zero v1.2 with 512MB");
    }
    else if (revision == 0x00900093)
    {
        uart_puts("The Board is Pi zero v1.3 with 512MB");
    }
    else if (revision == 0x009000C1)
    {
        uart_puts("The Board is Pi zero W with 512MB");
    }
    else if (revision == 0x00a02082)
    {
        uart_puts("The Board is Pi 3 Model B v1.2 with 1GB Sony, UK");
    }
    else if (revision == 0x00a22082)
    {
        uart_puts("The Board is Pi 3 Model B v1.2 with 1GB Embest, China");
    }
    else if (revision == 0x00a020d3)
    {
        uart_puts("The Board is Pi 3 Model B+ v1.2 with 1GB Sony, UK");
    }
    else if (revision == 0x00a03111)
    {
        uart_puts("The Board is Pi 4 with 1GB Sony, UK");
    }
    else if (revision == 0x00b03111)
    {
        uart_puts("The Board is Pi 4 with 2GB Sony UK");
    }
    else if (revision == 0x00b03112)
    {
        uart_puts("The Board is Pi 4 with 2GB Sony UK");
    }
    else if (revision == 0x00b03114)
    {
        uart_puts("The Board is Pi 4 with 2GB Sony UK");
    }
    else if (revision == 0x00c03111)
    {
        uart_puts("The Board is Pi 4 with 4GB Sony UK");
    }
    else if (revision == 0x00c03112)
    {
        uart_puts("The Board is Pi 4 with 4GB Sony UK");
    }
    else if (revision == 0x00c03114)
    {
        uart_puts("The Board is Pi 4 with 4GB Sony UK");
    }
    else if (revision == 0x00d03114)
    {
        uart_puts("The Board is Pi 4 with 8GB Sony UK");
    }
    else if (revision == 0x00c03130)
    {
        uart_puts("The Board is Pi 400 with 4GB Sony UK");
    }
    else if (revision == 0x00902120)
    {
        uart_puts("The Board is Pi zero 2 W with 1GB Sony UK");
    }
    else
    {
        uart_puts("Can not find corresponding revision");
    }
}

void changeUartSetting(const char *command)
{
    char *baudRate = my_strstr(command, "baudrates");
    char *databit = my_strstr(command, "databits");
    char *paritybit = my_strstr(command, "paritybits");
    char *stopbit = my_strstr(command, "stopbits");
    char *handShaking = my_strstr(command, "handshaking");

    if (
        my_strncmp(baudRate, "f", 1) != 1)
    {
        baudRate += 10;
    }

    if (
        my_strncmp(databit, "f", 1) != 1)
    {
        databit += 9;
        // uart_DataBit(databit);
    }
    if (
        my_strncmp(paritybit, "f", 1) != 1)
    {
        paritybit += 11;
        // uart_Parity(paritybit);
    }
    if (
        my_strncmp(stopbit, "f", 1) != 1)
    {
        stopbit += 9;
        // uart_StopBit(stopbit);
    }
    if (
        my_strncmp(handShaking, "f", 1) != 1)
    {
        handShaking += 12;
    }

    uart_init(baudRate, stopbit, databit, paritybit, handShaking);
}

void my_strcpy(char *str1, const char *str2)
{
    while (1)
    {
        if (*str2 == '\0')
            break;
        *str1 = *str2;
        str1++;
        str2++;
    }
    *str1 = '\0';
    return;
}

void my_memset(void *str, int startIndex, int strLength)
{
    unsigned char *p = (unsigned char *)str;
    while (strLength--)
    {
        *p++ = (unsigned char)startIndex;
    }
}

// void *tab_complete(char* currentCommand){
//     const char *commandList[] = {
//         "help","clear","selector","showinfo"
//     };
//     int commandLength = my_strlen(currentCommand);

//     for(int i = 0; i < 4; i++){
//         if(my_strncmp(currentCommand,commandList[i],commandLength) == 1){
//             while(1){
//                 if(commandLength == 0) break;
//                 uart_sendc('\b'); // Move cursor back
// 				uart_sendc(' ');  // Clear character
// 				uart_sendc('\b'); // Move cursor back again
// 				commandLength--;
//             }
//         }
//     }
// }

char *tab_complete(char currentCommand[], int index)
{
    const char *commandList[] = {
        "help", "clear", "selector", "showinfo", "change"};

    for (int i = 0; i < 4; i++)
    {
        if (my_strncmp(currentCommand, commandList[i], index) == 1)
        { // Here using 0 for match
            // Clear the current input in the UART
            for (int j = index; j > 0; j--)
            {
                uart_sendc('\b');
                uart_sendc(' ');
                uart_sendc('\b');
            }
            // Return the matching command
            return (char *)commandList[i];
        }
    }

    return NULL; // No match found
}