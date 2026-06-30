#define F_CPU 8000000L

#include <avr/io.h>
#include "lcd_control.h"
#include <util/delay.h>


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

    //char tab[10] = "test";
    lcd_init();
    lcd_str("start");

    //dodano nowy komentarz
    DDRD |= (1<<PD5);
    while(1)
    {
        PORTD ^= (1<<PD5);
        lcd_cls();
        lcd_str("czesc");
        _delay_ms(1500);
        lcd_cls();
        lcd_str("czy");
        _delay_ms(1500);
        lcd_cls();
        lcd_str("dasz sie");
        _delay_ms(1500);
        lcd_cls();
        lcd_str("zaprosic");
        _delay_ms(1500);
        lcd_cls();
        lcd_str("na maly trip");
        _delay_ms(1500);
        lcd_cls();
        lcd_str("w czwartek?");
        _delay_ms(1500);
    }
}