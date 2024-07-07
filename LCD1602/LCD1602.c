/*
 * LCD1602.c
 *
 *  Created on: 20 dicember 2021
 *      Author: Rudy
 */

#include <LCD1602.h>

#include "stm32f1xx_hal.h"

/*********** Define the LCD PINS below ****************/

#define RS_Pin GPIO_PIN_0
#define RS_GPIO_Port GPIOB
#define RW_Pin GPIO_PIN_3
#define RW_GPIO_Port GPIOB
#define EN_Pin GPIO_PIN_1
#define EN_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_4
#define D4_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOB
#define D7_Pin GPIO_PIN_7
#define D7_GPIO_Port GPIOB

/****************** define the timer handler below  **************/
#define timer htim1


extern TIM_HandleTypeDef timer;
void delay (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&timer, 0);
	while (__HAL_TIM_GET_COUNTER(&timer) < us);
}

/****************************************************************************************************************************************************************/

void send_to_lcd (char data, int rs) /*RP*/
{
	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, rs);  // rs = 1 for data, rs=0 for command

	/* write the data to the respective pin */
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, ((data>>3)&0x01));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, ((data>>2)&0x01));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, ((data>>1)&0x01));
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, ((data>>0)&0x01));

	/* Toggle EN PIN to send the data
	 * if the HCLK > 100 MHz, use the  20 us delay
	 * if the LCD still doesn't work, increase the delay to 50, 80 or 100..
	 */
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	delay (20);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
	delay (20);
}

void lcd_send_cmd (char cmd) /*RP*/
{
    char datatosend;

    /* send upper nibble first */
    datatosend = ((cmd>>4)&0x0f);
    send_to_lcd(datatosend,0);  // RS must be 0 while sending command

    /* send Lower Nibble */
    datatosend = ((cmd)&0x0f);
	send_to_lcd(datatosend, 0);
}

void lcd_send_data (char data) /*RP*/
{
	char datatosend;

	/* send 4 most significant bits*/

	datatosend = ((data>>4)&0x0f);
	send_to_lcd(datatosend, 1);  // rs =1 for sending data

	/* send 4 least significant bits */

	datatosend = ((data)&0x0f);
	send_to_lcd(datatosend, 1);
}

void lcd_clear (void) /*RP*/
{
	lcd_send_cmd(0x01);
	HAL_Delay(2);
}

void lcd_put_cur(int row, int col) /*RP*/
{
    switch (row)
    {
        case 0:
            col |= 0x80;			//0x80 force curse at position row=0
            break;
        case 1:
            col |= 0xC0;			//0xC0 force curse at position row=1
            break;
    }

    lcd_send_cmd (col);
}


void lcd_init (void) /*RP*/
{
	// dislay initialisation to 4 bit 
	
	//Secuence:
	// step 1:
	HAL_Delay(50);  		// wait for >15ms
	// step 2:
	lcd_send_cmd (0x30);
	// step 3:
	HAL_Delay(5);  			// wait for >4.1ms
	// step 4:
	lcd_send_cmd (0x30);
	// step 5:
	HAL_Delay(1);  			// wait for >100us
	// step 6:
	lcd_send_cmd (0x30);
	// step 7:
	HAL_Delay(1);			//Delay not especificate in the datasheet
	lcd_send_cmd (0x20);  	// 4bit mode
	// step 8:
	HAL_Delay(1);			//Delay not especificate in the datasheet
	lcd_send_cmd (0x20);  	// 4bit mode
	/* ---------------------------------------------------------------------------
	Function set --> (Rs-R/W-D7-D6-D5-D4)=(0-0-N-F-X-X) 
	DL=0 (4 bit mode),DL=1 (8 bit mode)
	N = 1 (2 line display), N=0 (1 line display), 
	F = 0 (5x7 Style), F=1 (5x10 Style)
	-----------------------------------------------------------------------------*/
	HAL_Delay(1);			//Delay not especificate in the datasheet
	lcd_send_cmd (0x28); 	// Function set

	// step 9:
	HAL_Delay(1);			//Delay not especificate in the datasheet
	lcd_send_cmd (0x00); 	
	HAL_Delay(1);			//Delay not especificate in the datasheet
	lcd_send_cmd (0x08); 	//Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);			//Delay not especificate in the datasheet

	// step 10:
	
	lcd_send_cmd (0x00); 	
	HAL_Delay(1);			//Delay not especificate in the datasheet
	lcd_send_cmd (0x01);  	// clear display
	HAL_Delay(1);			//Delay not especificate in the datasheet

	// step 11:
	lcd_send_cmd (0x00); 	
	HAL_Delay(1);			//Delay not especificate in the datasheet
	lcd_send_cmd (0x06); 	//Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);			//Delay not especificate in the datasheet
	// step 12:
	lcd_send_cmd (0x0C); 	//Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	HAL_Delay(1);			//Delay not especificate in the datasheet
	
	
	
	
	
}

void lcd_send_string (char *str) /*RP*/
{
	while (*str) 
	{
		lcd_send_data (*str++);	
	}
}

void lcd_return_home(void)	/*RP*/
{
	lcd_send_cmd(0x02);
	HAL_Delay(2);
}

void lcd_OnOff(char DisplayOnOff,char CursorOnOff, char BlinkOnOff) /*RP*/
{
	
	/*-------------------------------------------------------------

	cmd=0x00001DCB

	D == DisplayOnOff ----> 1 Display ON
					  ----> 0 Display OFF
	C == CursorOnOff  ----> 1 Cursor is displayed
					  ----> 0 Cursor isn't displayed
	B == BlinkOnOff   ----> 1 Cursor in actual position blink 
					  ----> 0 Cursor in actual position no blink 

	-------------------------------------------------------------*/
	char cmd=0;
	cmd = 0x08 | (DisplayOnOff<<2) | (CursorOnOff<<1) | BlinkOnOff;
	lcd_send_cmd(cmd);
	HAL_Delay(2);
}