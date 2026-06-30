//makra upraszczajace dostep do portow
//port
#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)
//pin
#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)
//ddr
#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)


#define LCD_Y 2 //liczba wierszy wyswietlacza
#define LCD_X 16 //liczba kolumn wyswietlacza

//ustawiamy za pomoca zero lub 1 czy sterujemy programowo pinem RW
#define USE_RW 0 //0 - pin  na stale do gnd, 1 - pin do uC

//konfiguracja portow i pinow, do ktorych dolaczone beda linie D7-D4 LCD
#define LCD_D7PORT C
#define LCD_D7 5
#define LCD_D6PORT C
#define LCD_D6 4
#define LCD_D5PORT C
#define LCD_D5 3
#define LCD_D4PORT C
#define LCD_D4 2
//definicja pinow procesora do ktorych beda podlaczone sygnaly RS, RW, E
#define LCD_RSPORT A
#define LCD_RS 0
#define LCD_RWPORT A
#define LCD_RW 1
#define LCD_EPORT A
#define LCD_E 2

//adresy w DRAM dla roznych wyswietlaczy
//inne beda dla 2-wierszowych a inne dla 4-wierszowych
#if( (LCD_Y == 4) && (LCD_X == 20))
#define LCD_LINE1 0x00 //adres 1 znaku 1 wiersza
#define LCD_LINE2 0x28 //1 znaku 2 wiersza
#define LCD_LINE3 0x14 //1znaku 3 wiersza
#define LCD_LINE4 0x54 //1znaku 4 wiersza
#else
#define LCD_LINE1 0x00 //adres 1 znaku 1 wiersza
#define LCD_LINE2 0x40 //1 znaku 2 wiersza
#define LCD_LINE3 0x10 //1znaku 3 wiersza
#define LCD_LINE4 0x50 //1znaku 4 wiersza
#endif

//inicjalizacja mikrokontrolera do wspolpracy z wyswietlaczem lcd
//void lcd_init(void);
void lcd_init(void);
void lcd_str(char* str);
void lcd_str_E(char* str);
void lcd_str_P(char* str);
void lcd_cls(void);
void lcd_write_cmd(uint8_t cmd);
void lcd_write_data(uint8_t data);


//makrodefinicje operacji na sygnalach serujacych RS, RW, E

//ustaw stan wysoki na linii RS
#define SET_RS PORT(LCD_RSPORT) |= (1<<LCD_RS)
//ustaw stan niski na linii RS 
#define CLR_RS PORT(LCD_RSPORT) &= ~(1<<LCD_RS)

//ustaw stan wyoski na linii RW - odczyt
#define SET_RW PORT(LCD_RWPORT) |= (1<<LCD_RW)
//ustaw stan niski na lini RW - zapis
#define CLR_RW PORT(LCD_RWPORT) &= ~(1<<LCD_RW)

//ustaw stan wyoski na linii E
#define SET_E PORT(LCD_EPORT) |= (1<<LCD_E)
//ustaw stan niski na linii E
#define CLR_E PORT(LCD_EPORT) &= ~(1<<LCD_E)


//makra upraszczajace wysylanie funkcji do sterownika wyswietlacza

//czyszczenie wyswietlacza
#define LCDC_CLS 0x01

//ustawienie pozycji kursora na 0,0
#define LCDC_HOME 0x02

//ustawienie funkcji (Function set)
#define LCDC_FUNC 0x20

//ustawienie wyswietlacza w tryb 4 bitowy (DL = 0)
#define LCDC_FUNC4B 0x00
//ustawienie wyswietlacza w tryb 8 bitowy (DL = 1)
#define LCDC_FUNC8B 0x10

//ustawienie wysw. w tryb 2 wierwszowy (N = 1)
#define LCDC_FUNC2L 0x08
//ustawienie wysw. w tryb 1 wierwszowy (N = 0)
#define LCDC_FUNC1L 0x00

//ustawienie rozmiaru znaku na 5x10 (F = 1)
#define LCDC_FUNC5x10 0x04
//ustawienie rozmiaru znaku na 5x7 (F = 0)
#define LCDC_FUNC5x7 0x00

//wlaczenie ustawianien wyswietlania
#define LCDC_ONOFF 0x08

//wylaczenie kursora
#define LCDC_CURSOROFF 0x00
//wlaczenie kursora
#define LCDC_CURSORON 0x02

//wylaczenie wyswietlacza
#define LCDC_DISPLAYOFF 0x00
//wlaczenie wyswietlacza
#define LCDC_DISPLAYON 0x04

//wylaczenie migania kursora
#define LCDC_BLINKOFF 0x00
//wlaczenie migania kursora
#define LCDC_BLINKON 0x01

//ustawienia pracy kursora przy wprowadzaniu (Entry mode set)
#define LCDC_ENTRY 0x04

//przesuwanie kursora w prawo
#define LCDC_ENTRYR 0x02
//przesuwanie kursora w lewo
#define LCDC_ENTRYL 0x00

//----------------------------------//

//
//ustawia kursor na wybranej pozycji
#define USE_LCD_LOCATE 1 

//wysyla string umieszczony w pamieci flash
#define USE_LCD_STR_P 0

//wysyla string umieszczony w pamieci EEPROM
#define USE_LCD_STR_E 0

//wyswietla liczbe dziesietna na LCD
#define USE_LCD_INT 0

//wyswietla liczbe szesntastkowa na LCD
#define USE_LCD_HEX 0

//wysyla zdefiniowany znak z pamieci RAM
#define USE_LCD_DEFCHAR 0
//wysyla zdefiniowany znak z pamieci FLASH
#define USE_LCD_DEFCHAR_P 0
//wysyla zdefiniowany znak z pamieci EEPROM
#define USE_LCD_DEFCHAR_E 0

//obsluga wl/wyl kursora
#define USE_LCD_CURSOR_ON 0
//obsluga wl/wyl migania kursora
#define USE_LCD_CURSOR_BLINK 0 
//ustawia kursor na pozycji poczatkowej
#define USE_LCD_CURSOR_HOME 0 