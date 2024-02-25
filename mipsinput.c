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

 /*Samlar ihop input från knappar för att se vilken
 som blivit tryckt. */
int getbtns(void)
{
    int btns = (PORTD & 0x00000e0) >> 4;          /*Hämtar värdet BTN2-4. Nollar alla andra bits.
                                                    samt flyttar till bit-index 1-3*/

    int btn1 = (PORTF & 0x2) >> 1;                //Hämtar värdet från BTN1 och flyttar till bit-index 0

    btns = btns | btn1;                           //Lägger ihop värdet från BTN1 och BTN2-4
                                                  
    return btns;
}
