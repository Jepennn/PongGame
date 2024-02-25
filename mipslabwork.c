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

int prime = 1234567;
int mytime = 0x5957;
int countled = 0;

int timeoutcount = 0; //counts antalet gånger som timeout inträffat

//char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
  if(IFS(0) & 0x100)  //Kontrollerar om timer flaggan är satt. Ett icke nollvärde anses vara sant i C.
  {
    timeoutcount++;
    IFSCLR(0) = 0x100;    //Nollställer timer flaggan

    /*if(timeoutcount == 10)  //Om timeoutcount är 10, dvs 1s, så uppdateras tiden och skrivs ut.(Klockan fungerar som den ska)
    {
      time2string( textstring, mytime );
      display_string( 3, textstring );
      display_update();
      tick(&mytime);
     timeoutcount = 0;
    }*/
  }
}

/* Project initialization goes here */
void proj_init( void )
{
  //Sätter upp switchar och knappar till input
  TRISDSET = 0xf00;

  TRISFSET = 0x2; 
  TRISDSET = 0xe0;

  //Initierar displayen
  display_init();  


  //Sätter upp min timer, prescaler 256 (31250 cykler), tid 0.1s, 
  T2CONCLR = 0x0;       //Resetar timer 2
  T2CON = 0x70;          //Prescaler 1:256
  TMR2 = 0x0;           //Nollställer räknaren
  PR2 = 31250;          //Sätter perioden till 0.1s

  //Sätter upp interrupt för timer2
  IECSET(0) = 0x100;     //Sätter upp interrupt för timer 2
  IPCSET(2) = 0x1f;      //Sätter prioritet och subprioritet till 7
  
  T2CONSET = 0x8000;    //Startar timer 2

  enable_interrupt();   //Aktiverar globala interrupter (funktionen finns i labwork.S)
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  add_objects_to_screen(&ball, &single_map, &bracket);
}
  