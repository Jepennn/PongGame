#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

// Hämtar värdet från switcharna 1 till 4
int getsw(void)
{
    int sw = PORTD & 0X000000f00;           //nollar alla bitsen till switcharna
    sw = sw >> 8; 
    return sw;
}

 //Hämtar värdet från knapparna 1 till 4
int getbtns(void)
{
    int btns = (PORTD & 0x00000e0) >> 4;          //Hämtar värdet BTN2-4. Samt flyttar till bit-index 3 till 1. 

    int btn1 = (PORTF & 0x2) >> 1;                //Hämtar värdet från BTN1 och flyttar till bit-index 0

    btns = btns | btn1;                          //Lägger ihop värdet från BTN1 och BTN2-4                                             
    return btns;                                //Returnerar värdet
}
