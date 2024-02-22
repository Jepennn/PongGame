#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void)
{
    int sw = PORTD & 0X000000f00;           //nollar alla bitsen till switcharna
    sw = sw >> 8; 
    return sw;
}

int getbtns(void)
{
    int btns = PORTD & 0X00000e0;           //Hämtar värdet från knapparna och noll alla andra bits
    btns = btns >> 5;                       // Flyttar bitarn så de lägger till rätt position(LSB)
    return btns;
}