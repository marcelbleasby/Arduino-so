#include <LiquidCrystal.h>


typedef char (*ptrFunDrv)(void *parameters);

typedef struct {
    char drv_id;
    ptrFunDrv *functions;
    ptrFunDrv initFunc;
} driver;


typedef enum {
    LCD_INIT,
    LCD_UPDATE,
    LCD_CLEAR,
    d_END
} availableFunctions;

char lcdInit(void *parameters);
char lcdUpdate(void *parameters);
char lcdClear(void *parameters);

driver *getLcdDriver();


#define RS 7
#define EN 8
#define D4 9
#define D5 10
#define D6 11
#define D7 12

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);


ptrFunDrv lcdFunctions[] = {
    lcdInit,
    lcdUpdate,
    lcdClear
};


driver lcdDriver = {
    .drv_id = 1,
    .functions = lcdFunctions,
    .initFunc = lcdInit
};

char lcdInit(void *parameters) {
    lcd.begin(16, 2);
    return 1;
}

char lcdUpdate(void *parameters) {
    char *text = (char *)parameters;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(text);
    return 1;
}

char lcdClear(void *parameters) {
    lcd.clear();
    return 1;
}

driver *getLcdDriver() {
    return &lcdDriver;
}
