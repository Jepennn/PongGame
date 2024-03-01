/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "pic32mx.h"  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


// Skapa en instans av bracket och ball
ball ball1 = {74, 16, 1, 1};          // x-position, y-position, x-velocity, y-velocity
bracket my_bracket = {1, 12, 6};      // x-position, y-position, height
bracket my_bracket2 = {126, 12, 6};   // x-position, y-position, height

//Globala variabler för LED
int countled = 0;

//Variabel som bestämmer vinnare
int winner;

//Räknare för LED
int leds = 0;                         

//Start hastigheten på bollen som kommer att öka under spelets gång
int velocity_b = 20;          


/* Interrupt Service Routine */ 
void user_isr( void )
{
  if(IFS(0) & 0x100)  //Kontrollerar om timer flaggan är satt. Ett icke nollvärde anses vara sant i C.
  {
    IFSCLR(0) = 0x100;                        //Nollställer timer flaggan

    int direction_R = (getbtns() & 0x3);      //styr den högra bracketen med
    int direction_L = (getbtns() & 0xc);      

    move_bracket(&my_bracket, direction_L, single_map);
    move_bracket(&my_bracket2, direction_R, single_map);    

  }
}

/*Initialisera allt som behövs för spelet PONG nedan*/
void proj_init( void )
{
  //Sätter upp switchar och knappar till input
  TRISDSET = 0xf00;
  TRISFSET = 0x2; 
  TRISDSET = 0xe0;

  //Sätter upp LED till output
  TRISECLR = 0xff;

  //Initierar displayen
  display_init(); 

  //Visa PONG_GAME png i 10s
  display_image(0, PONG_GAME);
	delay(6000); 

  //Sätter upp min timer, prescaler 256 (15625 cykler), tid 0.05s, 
  T2CONCLR = 0x0;           //Resetar timer 2
  T2CON = 0x70;             //Prescaler 1:256
  TMR2 = 0x0;               //Nollställer räknaren
  PR2 = 15625;              /*Sätter perioden till 0.05s, vilket ger en frekvens på 20 Hz (Korrigeras för bracketens velocity senare)*/

  //Sätter upp interrupt för timer2
  IECSET(0) = 0x100;            //Sätter upp interrupt för timer 2
  IPCSET(2) = 0x1f;             //Sätter prioritet och subprioritet till 7
  
  T2CONSET = 0x8000;            //Startar timer 2

  enable_interrupt();           //Aktiverar globala interrupter (funktionen finns i labwork.S)

}

/* Gameplaying function*/
void labwork( void )
{
  draw_bracket(my_bracket, single_map);
  draw_bracket(my_bracket2, single_map);   
  draw_ball(ball1, single_map);
  move_ball(&ball1, single_map);
  display_image(0, single_map);
}
  