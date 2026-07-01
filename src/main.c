#define F_CPU 8000000L

#include <avr/io.h>
#include "lcd_control.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>


int main(void){
    //wylaczenie JTAG-a po korzystam z pinpow PC2 3 4 5
    uint8_t temp = MCUCSR | (1<<JTD);
    MCUCSR = temp;
    MCUCSR = temp;

    //inicjowanie pinow portow podlaczonych do wyswielacza
    //ustawienie wszystkich jako wyjscia
    DDR(LCD_D7PORT) |= (1<<LCD_D7);
    DDR(LCD_D6PORT) |= (1<<LCD_D6);
    DDR(LCD_D5PORT) |= (1<<LCD_D5);
    DDR(LCD_D4PORT) |= (1<<LCD_D4);
    DDR(LCD_RSPORT) |= (1<<LCD_RS);
    DDR(LCD_EPORT) |= (1<<LCD_E);
    #if USE_RW == 1
    DDR(LCD_RWPORT) |= (1<<LCD_RW);
    #endif

    //domyslne wyzerowanie wszystkich linii sterujacych na starcie
    PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
    PORT(LCD_EPORT) &= ~(1<<LCD_E);
    #if USE_RW == 1
    PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
    #endif


    //wyswietlanie napisu
    lcd_init();

    uint8_t tab1[] = {
        0x00,
        0x00,
        0x0A,
        0x15,
        0x11,
        0x0A,
        0x04,
        0x00
    };

    uint8_t celc_deg[] = {
        0x1C,
        0x14,
        0x18,
        0x03,
        0x04,
        0x04,
        0x04,
        0x03
    };


    uint8_t thermo_up[] = {
  0x04,
  0x0A,
  0x0A,
  0x0A,
  0x0A,
  0x0A,
  0x0A,
  0x0A
    };

    uint8_t thermo_bot[] = {
         0x0A,
  0x0A,
  0x0A,
  0x15,
  0x1B,
  0x11,
  0x0E,
  0x00
    };

    lcd_defchar(0x01, tab1);
    lcd_defchar(0x00, celc_deg);
    lcd_defchar(0x02, thermo_up);
    lcd_defchar(0x03, thermo_bot);
    lcd_write_data(0x02);
    lcd_locate(1,0);
    lcd_write_data(0x03);
    lcd_locate(0,1);
    lcd_str("RPM: ");

    
    uint16_t rpm = 0;

    while(1)
    {
        rpm+=40;
        lcd_write_data(0x02);
        lcd_locate(1,0);
        lcd_write_data(0x03);
        lcd_locate(0,1);
        lcd_str("RPM: ");
        lcd_int(rpm);
        _delay_ms(30);
        lcd_cls();
    }
}