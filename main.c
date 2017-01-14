// **********************************************************
// Arvinder Basi & Kelvin Lu
// EEC 172 - Embedded Systems
// Lab Assignment 2: Serial Interfacing Using SPI and UART
// 1/21/2016
// **********************************************************

// Standard includes
#include <string.h>
#include <stdlib.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"
#include "uart.h"
#include "interrupt.h"

// Common interface includes
#include "uart_if.h"
#include "pin_mux_config.h"

#include "Adafruit_SSD1351.h"
#include "glcdfont.h"
#include "Adafruit_GFX.h"
#include "test.h"

#define APPLICATION_VERSION     "1.1.1"
#define SPI_IF_BIT_RATE  100000
#define TR_BUFF_SIZE     100

#define CONSOLE              		UARTA0_BASE
#define PAIRDEV              		UARTA1_BASE
#define PAIRDEV_PERIPH  			PRCM_UARTA1
#define UartConsoleGetChar()        MAP_UARTCharGet(CONSOLE)
#define UartConsolePutChar(c)       MAP_UARTCharPut(CONSOLE,c)
#define UartPairDevGetChar()        MAP_UARTCharGet(PAIRDEV)
#define UartPairDevPutChar(c)       MAP_UARTCharPut(PAIRDEV,c)

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//*****************************************************************************

// Test program that mimics the Arduino example.
void DemoRoutine()
{
	// Finding the length of the font array.
	unsigned int fontLength = sizeof(font) / sizeof(font[0]);

	unsigned int i;
	unsigned int x;
	unsigned int y;

	Message("Adafruit Demo Routine\n\r");

	while(1)
	{
		fillScreen(BLACK); // Clear screen.

		// Print the full character-set found in the font table.
		x = 0;
		y = 0;
		for(i = 0; i < fontLength; i++)
		{
			drawChar(x, y, font[i], YELLOW, BLACK, (unsigned char) 1);
			x += 6;
			if (x > 120) {
				y += 8;
				x = 0;
			}
		}
		MAP_UtilsDelay(10000000);

		// Print the string "Hello world!" to display.
		setCursor(0,0);
		fillScreen(BLACK);
		Outstr("Hello world!");
		MAP_UtilsDelay(40000000);

		// Display 8 horizontal bands of different colors.
		lcdTestPattern();
		MAP_UtilsDelay(40000000);

		// Display 8 vertical bands of different colors.
		lcdTestPattern2();
		MAP_UtilsDelay(40000000);

		// Display diagonal lines.
		testlines(RED);
		MAP_UtilsDelay(40000000);

		// Display a rectagular grid pattern.
		testfastlines(BLUE, GREEN);
		MAP_UtilsDelay(40000000);

		// Draw a pattern of rectangles.
		testdrawrects(YELLOW);
		MAP_UtilsDelay(40000000);

		// Call the testfilllrects() function.
		testfillrects(WHITE, BLACK);
		MAP_UtilsDelay(40000000);

		// Draw a pattern of circles.
		testdrawcircles(20,WHITE);
		MAP_UtilsDelay(40000000);

		// Call the testfillcircles() function.
		testfillcircles(20, BLUE);
		MAP_UtilsDelay(40000000);

		// Call the testroundrects() function.
		testroundrects();
		MAP_UtilsDelay(40000000);

		// Call the testtriangles() function.
		testtriangles();
		MAP_UtilsDelay(40000000);
	}
}

void CommunicateRoutine()
{
	char cChar;

	Message("Communication Routine\n\r");
	Message("Please begin typing your messages below:\n\r> ");

	fillScreen(BLACK);
	setTextColor(RED,BLACK);

	setCursor(0, 0); Outstr("Kelvin Lu");
	setCursor(0, 8); Outstr("Arvinder Basi");
	setCursor(0, 16); Outstr("EEC 172");
	setCursor(0, 24); Outstr("Lab 2, 01/21/2016");
	setCursor(0, 40); Outstr("Communication Routine");
	MAP_UtilsDelay(20000000);

	fillScreen(BLACK);
	setCursor(0, 0); Outstr("  You may begin...");

	int y_cursor = 16;
	int x_cursor = 12;
	int newMessage = 1;

	// As you write a character to the terminal it will
	// immeditely be reflected on the recievers OLED display.
	while(1)
	{
		// Recieved a message.
		if (UARTCharsAvail(PAIRDEV))
		{
			cChar = UartPairDevGetChar();

			if(x_cursor > 120) // About to go off right edge.
			{
				x_cursor = 12;
				y_cursor += 8; // go to next line.
			}

			if(y_cursor > 120) // About to go off bottom edge.
			{
				fillScreen(BLACK); // clear screen.
				x_cursor = 12;
				y_cursor = 0; // start at top.
			}

			if(newMessage) // User is typing a new message.
			{
				// All new messages begin with "> ".
				drawChar(0, y_cursor, '>', WHITE, BLACK, (unsigned char) 1);
				drawChar(6, y_cursor, ' ', WHITE, BLACK, (unsigned char) 1);
				newMessage = 0;
			}

			if(cChar == '\r' || cChar == '\n') // end of message.
			{
				y_cursor += 8; 		// go to next line.
				x_cursor = 12;		// indent.

				newMessage = 1; 	// start a new message.
			}
			else if(cChar == '\b') 	// deleting a character.
			{
				x_cursor -= 6; 		// go back one.
				if(x_cursor < 0) 	// if we're about to go off screen.
				{
					y_cursor -= 8; 	// previous line.
					x_cursor = 120; // last character of line.

					if(y_cursor < 0) // Went off top edge.
					{
						newMessage = 1; // start a new message.
						x_cursor = 12;
						y_cursor = 0;
					}
				}

				drawChar(x_cursor, y_cursor, 32, WHITE, BLACK, (unsigned char) 1);
			}
			else
			{
				drawChar(x_cursor, y_cursor, cChar, WHITE, BLACK, (unsigned char) 1);
				x_cursor += 6;
			}
		}

		// Sending a message.
		if (UARTCharsAvail(CONSOLE))
		{
			cChar = UartConsoleGetChar();
			if(cChar == '\r' || cChar == '\n') // end of message.
			{
				UartConsolePutChar(cChar);
				Message("\n\r> ");
			}
			else if(cChar == '\b') // deleting a character.
			{
				// "Erase" character in console
				UartConsolePutChar('\b');
				UartConsolePutChar(' ');
				UartConsolePutChar('\b');
			}
			else
			{
				// Echo char back to console.
				UartConsolePutChar(cChar);
			}

			UartPairDevPutChar(cChar); // Send the character.
		}
	}
}

//*****************************************************************************
//
//! Main function for lab application
//!
//! \param none
//!
//! \return None.
//
//*****************************************************************************
void main()
{
    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Muxing UART and SPI lines.
    //
    PinMuxConfig();

    //
    // Enable the SPI module clock
    //
    PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    //
    // Initialising the Terminal.
    //
    InitTerm();

    //
    // Clearing the Terminal.
    //
    ClearTerm();

    //
    // Initialize the device on UART1
    //
    MAP_UARTConfigSetExpClk(PAIRDEV,MAP_PRCMPeripheralClockGet(PAIRDEV_PERIPH),
                      UART_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));

    //
    // Reset the SPI peripheral
    //
    PRCMPeripheralReset(PRCM_GSPI);

    // SPI reset, initialize
    SPIReset(GSPI_BASE);
    SPIFIFOEnable(GSPI_BASE, SPI_TX_FIFO || SPI_RX_FIFO);

    SPIConfigSetExpClk(GSPI_BASE, PRCMPeripheralClockGet(PRCM_GSPI),
    				   SPI_IF_BIT_RATE, SPI_MODE_MASTER, SPI_SUB_MODE_0,
					   (SPI_SW_CTRL_CS | SPI_4PIN_MODE | SPI_TURBO_OFF |
					    SPI_CS_ACTIVELOW | SPI_WL_8));

    SPIEnable(GSPI_BASE);

    // Adafruit initialize
    Adafruit_Init();

    DemoRoutine();
	//CommunicateRoutine();
}

