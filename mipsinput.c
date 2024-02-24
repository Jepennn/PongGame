#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

//switcharna 1 till 4
int getsw(void)
{
    int sw = PORTD & 0X000000f00;           //nollar alla bitsen till switcharna
    sw = sw >> 8; 
    return sw;
}

//Knapp 2 till 4
int getbtns(void)
{
    int btns = PORTD & 0X00000e0;           //Hämtar värdet från knapparna och nollar alla andra bits
    btns = btns >> 5;                       // Flyttar bitarn så de lägger till rätt position(LSB)
    return btns;
}


//Knapp 1 ligger på RF1 (Kolla refrensesheeten)