// -----------------------------------main.c -------------------------------------

// #include "../uart/uart1.h"
#include "../uart/uart0.h"

#include "q1.h"
int historyIndex = 0;
int checkHistoryIndex = 0;

char history[100][100];
// char history[100];

void welComeMessage()
{
    uart_puts("\n \n \n \n"

              "  8888888888 8888888888 8888888888 88888888888     .d8888b.      d8888   .d8888b.   .d8888b.\n"
              "  888        888        888            888        d88P  Y88b    d8P888  d88P  Y88b d88P  Y88b \n"
              "  888        888        888            888               888   d8P 888  888    888 888    888 \n"
              "  8888888    8888888    8888888        888             .d88P  d8P  888  Y88b. d888 888    888 \n"
              "  888        888        888            888         .od888P\"  d88   888   \"Y888P888 888    888 \n"
              "  888        888        888            888        d88P\"      8888888888        888 888    888 \n"
              "  888        888        888            888        888\"             888  Y88b  d88P Y88b  d88P \n"
              "  8888888888 8888888888 8888888888     888        888888888        888   \"Y8888P\"   \"Y8888P\" \n"
              "\n"
              "  888888b.         d8888 8888888b.  8888888888     .d88888b.   .d8888b.\n"
              "  888  \"88b       d88888 888   Y88b 888           d88P\" \"Y88b d88P  Y88b \n"
              "  888  .88P      d88P888 888    888 888           888     888 Y88b. \n"
              "  8888888K.     d88P 888 888   d88P 8888888       888     888  \"Y888b.\n"
              "  888  \"Y88b   d88P  888 8888888P\"  888           888     888     \"Y88b.\n"
              "  888    888  d88P   888 888 T88b   888           888     888       \"888\n"
              "  888   d88P d8888888888 888  T88b  888           Y88b. .d88P Y88b  d88P \n"
              "  8888888P\" d88P     888 888   T88b 8888888888     \"Y88888P\"   \"Y8888P\n"
              " \n       Developed by <Hur Hyeon Bin> - <s3740878>"
              "\n\nMyOS> ");
}

void cil()
{
    static char currentCommand[100];
    static int charIndex = 0;

    char c = uart_getc();

    if (c != '\n')
    {
        if (c == '+')
        {
            if (historyIndex - 1 > checkHistoryIndex)
            {
                while (1)
                {
                    if (charIndex == 0)
                        break;
                    charIndex--;
                    uart_sendc('\b');
                    uart_sendc(' ');
                    uart_sendc('\b');
                }
                checkHistoryIndex++;
                char *historyCommand = history[checkHistoryIndex];
                uart_puts(historyCommand);
                charIndex = my_strlen(historyCommand);
                my_memset(currentCommand, 0, charIndex);
                my_strcpy(currentCommand, historyCommand);
            }
        }
        else if (c == '_')
        {
            if (checkHistoryIndex >= 1)
            {
                while (1)
                {
                    if (charIndex == 0)
                        break;
                    charIndex--;
                    uart_sendc('\b');
                    uart_sendc(' ');
                    uart_sendc('\b');
                }
                checkHistoryIndex--;
                char *historyCommand = history[checkHistoryIndex];
                uart_puts(historyCommand);
                charIndex = my_strlen(historyCommand);
                my_memset(currentCommand, 0, charIndex);
                my_strcpy(currentCommand, historyCommand);
            }
        }
        else if (c == '\t')
        { // press tab

            char *autocompletedCommand = tab_complete(currentCommand, charIndex);
            charIndex = my_strlen(autocompletedCommand);
            my_memset(currentCommand, 0, charIndex);
            my_strcpy(currentCommand, autocompletedCommand);
            uart_puts(autocompletedCommand);
        }
        else if (c == '\b')
        { // delete current command char

            if (charIndex > 0)
            {
                currentCommand[charIndex] = '\0';
                uart_sendc('\b');
                uart_sendc(' ');
                uart_sendc('\b');
                charIndex--;
            }
        }

        else
        {
            uart_sendc(c);
            currentCommand[charIndex] = c;
            charIndex++;
        }
    }
    else if (c == '\n')
    {
        // currentCommand[charIndex] = c;
        // charIndex++;
        currentCommand[charIndex] = '\0';
        for (int i = 0; i < charIndex; i++)
        { // save user's current command into the history
            history[historyIndex][i] = currentCommand[i];
        }
        // history[historyIndex] = currentCommand;
        historyIndex++;
        checkHistoryIndex = historyIndex;
        uart_puts("\n");
        charIndex = 0;
        if (my_strncmp(currentCommand, "help", 4) == 1)
        {
            printHelp(currentCommand);
        }
        else if (my_strncmp(currentCommand, "clear", 5) == 1)
        {
            clearTerminal();
        }
        else if (my_strncmp(currentCommand, "setcolor", 8) == 1)
        {
            changeColor(currentCommand);
        }
        else if (my_strncmp(currentCommand, "showinfo", 8) == 1)
        {
            printInfo();
        }
        else if (my_strncmp(currentCommand, "change", 6) == 1)
        {
            changeUartSetting(currentCommand);
        }
        else
        {
            uart_puts(" unexpected command has been inputted please check the command \n");
        }

        uart_puts("MyOS> ");
    }
}

void main()
{
    uart_init("f", "f", "f", "f", "f");

    welComeMessage();
    while (1)
    {
        cil();
    }
}
