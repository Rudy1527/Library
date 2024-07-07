/*----------------------------------------
 * lcd1602.h
 *
 *  Created on: 20 dicember 2021
 *      Author: Rudy
 -----------------------------------------*/

#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_


void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);

void lcd_return_home(void);

void lcd_OnOff(char DisplayOnOff,char CursorOnOff, char CursorBlinkOnOff);

#endif /* INC_LCD1602_H_ */
