#define F_CPU 8000000L

#include <avr/io.h>
#include "lcd_control.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>


static inline void data_dir_in()
{
    //ustawienie kierunku pinow portu C
    DDR(LCD_D4PORT) &= ~(1<<LCD_D4);
    DDR(LCD_D6PORT) &= ~(1<<LCD_D6);
    DDR(LCD_D5PORT) &= ~(1<<LCD_D5);
    DDR(LCD_D7PORT) &= ~(1<<LCD_D7);
}


static inline void data_dir_out()
{
    //ustawienie kierunku pinow portu C
    DDR(LCD_D4PORT) |= (1<<LCD_D4);
    DDR(LCD_D6PORT) |= (1<<LCD_D6);
    DDR(LCD_D5PORT) |= (1<<LCD_D5);
    DDR(LCD_D7PORT) |= (1<<LCD_D7);
}


//funckja do odczytu polowki bajtu z wyswietlacza LCD
//bedziemy wykorzystywac ta polowke glownie do odczytu busy flag
#if USE_RW == 1
static inline uint8_t lcd_readHalf(void)
{
    uint8_t result = 0;
    if(PIN(LCD_D4PORT) & (1<<LCD_D4)) result |= (1<<0);
    if(PIN(LCD_D5PORT) & (1<<LCD_D5)) result |= (1<<1);
    if(PIN(LCD_D6PORT) & (1<<LCD_D6)) result |= (1<<2);
    if(PIN(LCD_D7PORT) & (1<<LCD_D7)) result |= (1<<3);
    return result;
}
#endif



#if USE_RW == 1
uint8_t _lcd_read_byte(void)
{
    uint8_t result = 0;
    data_dir_in();

    SET_RW;
    SET_E;

    //doczyt starszej czesci bajtu z LCD D7-D4
    result |= (lcd_readHalf() << 4);
    CLR_E;
    
    SET_E;

    //odczyt mlodszej czesci bajtu z LCD D3-D0
    result |= lcd_readHalf();
    CLR_E;

    return result;
}
#endif


//funckja zwraca batj, jesli RW jest 1, sluzy do odczytywania flagi zajetosci z bitu 7
#if USE_RW == 1
uint8_t check_BF(void)
{
    CLR_RS;
    return _lcd_read_byte();
}
#endif

//funckcja wysylajaca odpowiednie polowki bajtu na odpowiednie piny linii wyswietlacza
static inline void lcd_sendHalf(uint8_t data)
{
    if(data&(1<<0))
    {
        PORT(LCD_D4PORT) |= (1<<LCD_D4); //ustawia bit jesli w data bit 4 jest 1
    }
    else
    {
        PORT(LCD_D4PORT) &= ~(1<<LCD_D4);//zeruje bit jesli w data bit 4 jest 0 (czyszczenie po poprzednim stanie)
    }

    if(data&(1<<1))
    {
        PORT(LCD_D5PORT) |= (1<<LCD_D5);
    }
    else
    {
        PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
    }

    if(data&(1<<2))
    {
        PORT(LCD_D6PORT) |= (1<<LCD_D6);
    }
    else
    {
        PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
    }

    if(data&(1<<3))
    {
        PORT(LCD_D7PORT) |= (1<<LCD_D7);
    }
    else
    {
        PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
    }
}


void _lcd_write_byte(unsigned char _data)
{
    //ustawienie pinow lcd d4-d7 jako wyjscia
    data_dir_out();

    //zerujemy RW jesli jest na stale do masy
    #if USE_RW == 1
        CLR_RW;
    #endif

    lcd_sendHalf(_data>>4); //wyslanie starszej czesci bajtu d7-d4
    SET_E;
    _delay_us(2);
    CLR_E;

    lcd_sendHalf(_data);
    SET_E;
    _delay_us(2);
    CLR_E;

    #if USE_RW == 1
        while(check_BF() & (1<<7));  //z tego co rozumiem ta petla sluzy do wyczekania idealnego czasu na flage zajetosci sterownika LCD
    #else
        _delay_us(120); //w przeciwnym razie, jesli RW jest na stale do GND, nie mamy jak sprawdzac flagi zajetosci to w nocie katalogowej mamy podany minimalny czas 120 us
    #endif
}

//przygotowujemy funckje okrelajace stan linii RS
//mowi on czy przyslamy komende do sterownika czy kod znaku do wyswietlenia

//funckja mowi sterownikowi ze wysylamy komende i przesyla ta komende
void lcd_write_cmd(uint8_t cmd)
{
    CLR_RS;
    _lcd_write_byte(cmd);
}

//funckja mowi sterownikowi ze wysylamy znak i przesyla ten znak
void lcd_write_data(uint8_t data)
{
    SET_RS;
    _lcd_write_byte(data);
}

//odczekiwanie 4,9ms po wykonaniu polecenia, ale tylko jezeli RW jest na stale do GND
void lcd_cls(void)
{
    lcd_write_cmd(LCDC_CLS);
    #if USE_RW == 0
    _delay_ms(4.9);
    #endif
}

//funckja inicjalizujaca prace wyswietlacza LCD w trybie 4 bitowym
void lcd_init(void)
{
    //wszystkie linii jako wyjscia
    data_dir_out();
    DDR(LCD_RSPORT) |= (1<<LCD_RS);
    DDR(LCD_EPORT) |= (1<<LCD_E);
    #if USE_RW == 1
        DDR(LCD_RWPORT) |= (1<<LCD_RW);
    #endif


    //ustawienie 1 na wyjsciach
    PORT(LCD_RSPORT) |= (1<<LCD_RS);
    PORT(LCD_EPORT) |= (1<<LCD_E);
    #if USE_RW == 1
        PORT(LCD_RWPORT) |= (1<<LCD_RW);
    #endif

    _delay_ms(15);
    PORT(LCD_EPORT) &= ~(1<<LCD_E);
    PORT(LCD_RSPORT) &= ~(1<<LCD_RS);

    #if USE_RW == 1
    PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
    #endif
    //jeszcze nie mozna odczytac busy flag

    lcd_sendHalf(0x03); //tryb 8 bitowy DL = 1
    SET_E;
    _delay_us(2);
    CLR_E;
    _delay_ms(4.1);

    lcd_sendHalf(0x03); //tryb 8 bitowy DL = 1
    SET_E;
    _delay_us(2);
    CLR_E;
    _delay_us(100);

    lcd_sendHalf(0x03); //tryb 8 bitowy DL = 1
    SET_E;
    _delay_us(2);
    CLR_E;
    _delay_us(100);

    lcd_sendHalf(0x02); //tryb 4 bitowy DL = 0
    SET_E;
    _delay_us(2);
    CLR_E;
    _delay_us(100);
    //w tym momencie mozna uzywac juz busy flag - uzywamy juz funckji do wysylania komend
    
    //tryb 4 bitowy, 2 wiersze, znak 5x7 px
    lcd_write_cmd(LCDC_FUNC |LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7); //tutaj musze makra dodac

    //wylaczenie kursora
    lcd_write_cmd(LCDC_ONOFF|LCDC_CURSOROFF);

    //wlaczenie wyswietlacza
    lcd_write_cmd(LCDC_ONOFF|LCDC_DISPLAYON);

    //przesuwanie kursora w prawo bez przesuwania zawartosci ekranu
    lcd_write_cmd(LCDC_ENTRY|LCDC_ENTRYR);

    //wyczyszczenie ekranu
    lcd_cls();
}



//funckja przenosi kursos w pozycje home (0,0)
#if USE_LCD_CURSOR_HOME == 1
void lcd_home(void)
{
    lcd_wrtie_cmd(LCDC_CLS|LCDC_HOME);

    #if USE_RW == 0
    _delady_ms(4.9)
    #endif
}
#endif

//funckja wlacza kursor i wyswietlacz
#if USE_LCD_CURSOR_ON == 1
void lcd_cursor_on(void)
{
    lcd_write_cmd(LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON);
}

//funckja wylacza kursor (chyba)
void lcd_cursor_off(void)
{
    lcd_write_cmd(LCDC_ONOFF|LCDC_DISPLAYON)
}
#endif

#if USE_LCD_CURSOR_BLINK == 1
//wlaczenie migania kursora
void lcd_blink_on(void)
{
    lcd_write_cmd(LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON|LCDC_BLINKON);
}

//wylaczenie migania kursora
void lcd_blink_off(void)
{
    lcd_write_cmd(LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif


//wyswietlenie napisu zdefiniowanego w RAM
void lcd_str(char* str)
{
    register char znak;
    while(znak =* (str++))
    {
        lcd_write_data((znak>= 0x80 && znak <= 0x87) ? (znak & 0x07) : znak);
    }
}

//odczyt znaku z pamieci FLASH
#if USE_LCD_STR_P == 1
void lcd_str_P(char* str)
{
    register char znak;
    while((znak = pgm_read_byte(str++)))
    {
        //jesli kod zawiera sie pomiedzy 0x80 a 0x87 to maskujmey 5 starszych bitow wartoscia 0x07
        lcd_write_data((znak>= 0x80 && znak <= 0x87) ? (znak & 0x07) : znak);
    }
}
#endif

//odczyt znaku z pamieci EEPROM
#if USE_LCD_STR_E == 1
void lcd_str_E(char* str)
{
    register char znak;
    while(1)
    {
        znak = eeprom_read_byte((uint8_t *) (str++));
        if(!znak || znak==0xff)
        {
            break;  //jesli znak jest 0 (0xff) to kocznymy wpisywanie
        }
        else
        {
            //jesli kod zawiera sie pomiedzy 0x80 a 0x87 to maskujmey 5 starszych bitow wartoscia 0x07
            lcd_write_data((znak>= 0x80 && znak <= 0x87) ? (znak & 0x07) : znak);
        }

    }
}
#endif


//wyswietlanie liczb calkowitych na wyswietlaczu
#if USE_LCD_INT == 1
void lcd_int(int val)
{
    //bufor musi byc nieduzy bo tworzy sie na stosie
    char bufor[17];
    //itoa() zwraca jako rezultat adres pierwszego el tablicy po konwersji z bufora
    lcd_str(itoa(val, bufor, 10));
}
#endif

#if USE_LCD_HEX == 1
void lcd_hex(int val)
{
    //bufor musi byc nieduzy bo tworzy sie na stosie
    char bufor(17);
    //itoa() zwraca jako rezultat adres pierwszego el tablicy po konwersji z bufora
    lcd_str(itoa(val, bufor, 16));
}
#endif

//funckja sluzaca definicji wlasnego znaku na LCD z pamieci RAM
#if USE_LCD_DEFCHAR == 1
//args: nr - kod znaku w pamieci CGRAM 0x80-0x87
// *def_znak - wskaznik do tablicy 7 bajtow definicujacych znak
void lcd_defchar(uint8_t nr, uint8_t* def_znak)
{
    register uint8_t i, c, cgram_address;
    cgram_address = 64+(nr*8);
    lcd_write_cmd(cgram_address);
    _delay_us(50);
    for(i = 0; i<8; i++)
    {
        c = *(def_znak++);
        lcd_write_data(c);
    }
    lcd_cls();
}
#endif

//funckja sluzaca definicji wlasnego znaku na LCD z pamieci FLASH
#if USE_LCD_DEFCHAR_P == 1
//args: nr - kod znaku w pamieci CGRAM 0x80-0x87
// *def_znak - wskaznik do tablicy 7 bajtow definicujacych znak
void lcd_defchar_P(uint8_t nr, uint8_t* def_znak)
{
    register uint8_t i, c;
    lcd_write_cmd( 64+((nr&0x07)*8) );
    for(i = 0; i<8; i++)
    {
        c = pgm_read_byte(def_znak++);
        lcd_write_data(c);
    }
    lcd_cls();
}
#endif

//funckja sluzaca definicji wlasnego znaku na LCD z pamieci EEPROM
#if USE_LCD_DEFCHAR_E == 1
//args: nr - kod znaku w pamieci CGRAM 0x80-0x87
// *def_znak - wskaznik do tablicy 7 bajtow definicujacych znak
void lcd_defchar_E(uint8_t nr, uint8_t* def_znak)
{
    register uint8_t i, c;
    lcd_write_cmd( 64+((nr&0x07)*8) );
    for(i = 0; i<8; i++)
    {
        c = eeprom_read_byte(def_znak++);
        lcd_write_data(c);
    }
    lcd_cls();
}
#endif


#if USE_LCD_LOCATE == 1
void lcd_locate(uint8_t y, uint8_t x)
{
    switch(y)
    {
        case 0: y = LCD_LINE1; break;
        #if (LCD_Y > 1)
        case 1: y = LCD_LINE2; break;
        #endif
        #if (LCD_Y > 2)
        case 2: y = LCD_LINE3; break;
        #endif
        #if (LCD_Y > 3)
        case 3: y = LCD_LINE4; break;
        #endif
    }

    lcd_write_cmd((0x80+y+x));
}
#endif