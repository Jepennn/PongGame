/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <string.h>
#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}


/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/   
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 128; j++)
			spi_send_recv(data[i*128 + j]); // add (~) för att 0:or som på och 1:or som av.
	}
}

void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

/*
 * nextprime
 * 
 * Return the first prime number larger than the integer
 * given as a parameter. The integer must be positive.
 */
#define PRIME_FALSE   0     /* Constant to help readability. */
#define PRIME_TRUE    1     /* Constant to help readability. */
int nextprime( int inval )
{
   register int perhapsprime = 0; /* Holds a tentative prime while we check it. */
   register int testfactor; /* Holds various factors for which we test perhapsprime. */
   register int found;      /* Flag, false until we find a prime. */

   if (inval < 3 )          /* Initial sanity check of parameter. */
   {
     if(inval <= 0) return(1);  /* Return 1 for zero or negative input. */
     if(inval == 1) return(2);  /* Easy special case. */
     if(inval == 2) return(3);  /* Easy special case. */
   }
   else
   {
     /* Testing an even number for primeness is pointless, since
      * all even numbers are divisible by 2. Therefore, we make sure
      * that perhapsprime is larger than the parameter, and odd. */
     perhapsprime = ( inval + 1 ) | 1 ;
   }
   /* While prime not found, loop. */
   for( found = PRIME_FALSE; found != PRIME_TRUE; perhapsprime += 2 )
   {
     /* Check factors from 3 up to perhapsprime/2. */
     for( testfactor = 3; testfactor <= (perhapsprime >> 1) + 1; testfactor += 1 )
     {
       found = PRIME_TRUE;      /* Assume we will find a prime. */
       if( (perhapsprime % testfactor) == 0 ) /* If testfactor divides perhapsprime... */
       {
         found = PRIME_FALSE;   /* ...then, perhapsprime was non-prime. */
         goto check_next_prime; /* Break the inner loop, go test a new perhapsprime. */
       }
     }
     check_next_prime:;         /* This label is used to break the inner loop. */
     if( found == PRIME_TRUE )  /* If the loop ended normally, we found a prime. */
     {
       return( perhapsprime );  /* Return the prime we found. */
     } 
   }
   return( perhapsprime );      /* When the loop ends, perhapsprime is a real prime. */
} 

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}


/*#############################################################################################*/


//Meny funktionen som visar menyn och låter användaren välja om den ska se high score eller spela spelet.
int show_menu(void)
{
    display_string(0, "			PONG-GAME!		");
    display_string(2, "1.Play-game");
    display_string(3, "2.Credentials");
    display_update();
    while(1)
    {
      int choice = getbtns();

      if(choice == 1)
      {
        return 1;
      }
      else if(choice == 2)
      {
        return 2;
      }
    }
}

//Visar skaparna av spelet
void credentials(void)
{
  display_string(0, "--CREDENTIALS--");
  display_string(1, "");
  display_string(2, "Jesper Sandberg");
  display_string(3, "Andia Mir");
  display_update();

  //Knapp 4 för att gå tillbaka till menyn
  while(1)
  {
    int choice = getbtns();
    if(choice == 8)
    {
      return;
    }
  }
}

//Rensar skärmen tom för att göra den redo at visa en ny bild.
void clear_screen(void)
{
  display_string(0, " ");
  display_string(1, " ");
  display_string(2, " ");
  display_string(3, " ");
  display_image(0, clr_screen);
  display_update();
}


/*Funktion för att rita en pixel på skärmen från vårt 2D kordinatsystem 
till arrayen som skärmen använder.*/
void draw_pixel(int x, int y, uint8_t* map, int value) 
{
  int page = y / 8;                   //Hittar vilken page vi befinner oss på i displayen.
  int pixel = y % 8;                  //Hittar vilken pixel vi befinner oss på i elementet
  int index = page * 128 + x;         //Hittar vilket element vi befinner oss på i arrayen
  if(value == 1)
  {
    map[index] |= 1 << pixel;
  }
  else
  {
    map[index] &= ~(1 << pixel);
  }
}

/*Funktioner för att hantera bracketen*/


//Rita ut bracketen på skärmen
 void draw_bracket(bracket br, uint8_t* map)
  {
  int i;
  for(i = 0; i < br.y_height; i++)
  {
    draw_pixel(br.x, br.y + i, map, 1);                
  }
  }

//Anvvänds för att rita bort den gamla bracket när man börjar spela efte game_over.
  void clear_bracket(bracket br, uint8_t* map)
  {
    int i;
    for(i = 0; i < br.y_height; i++)
    {
      draw_pixel(br.x, br.y + i, map, 0);
    }
  }


void move_bracket(bracket *br, int d, uint8_t* map)
{

  if(d == 1) //Flytta bracketen uppåt(BTN1)
  {
    if(br->y > 0)
    {
      // Släck den nedersta pixeln
      draw_pixel(br->x, br->y + br->y_height - 1, map, 0);

      // Tänd den översta pixeln
      draw_pixel(br->x, br->y - 1, map, 1);

      // Uppdatera y-koordinaten
      br->y -= 1;
    }
  }

  if(d == 2) //Flytta bracketen nedåt(BTN2)
  {
    if((br->y + br->y_height) < 32)
    {
      // Släck den översta pixeln
      draw_pixel(br->x, br->y, map, 0);

      // Tänd den nedersta pixeln
      draw_pixel(br->x, br->y + br->y_height, map, 1);

      // Uppdatera y-koordinaten
      br->y += 1;
    }
  }


  if(d == 4) //Flytta bracketen nedåt(BTN3)
  {
    if((br->y + br->y_height) < 32)
    {
      // Släck den översta pixeln
      draw_pixel(br->x, br->y, map, 0);
      // Tänd den nedersta pixeln
      draw_pixel(br->x, br->y + br->y_height, map, 1);
      // Uppdatera y-koordinaten
      br->y += 1;
    }
  }


  if(d == 8) //Flytta bracketen uppåt(BTN4)
  {
    if(br->y > 0)
    {
      // Släck den nedersta pixeln
      draw_pixel(br->x, br->y + br->y_height - 1, map, 0);
      // Tänd den översta pixeln
      draw_pixel(br->x, br->y - 1, map, 1);
      // Uppdatera y-koordinaten
      br->y -= 1;
    }
  }
}

/*Funktioner för att hantera bollen*/


//Rita ut bollen på skärmen
void draw_ball(ball b, uint8_t* map)
{
  draw_pixel(b.x, b.y, map, 1);
}

//Flytta bollen på skärmen
void move_ball(ball *b, uint8_t* map)
{
  static int counter = 0;      //Räknare för att flytta bollen långsammare
  
  if(leds == 10)
  {
    velocity_b = 13;
  }

  if(leds == 20)
  {
    velocity_b = 10;
  }

  if (counter == 0) {

    // Radera nuvarande position
    draw_pixel(b->x, b->y, map, 0);

    //Uppdatera position
    b->x += b->x_speed;               //Flytta bollen i x-led
    b->y += b->y_speed;               //Flytta bollen i y-led

    //Kontrollera om någon spelare har vunnit
    if(b->x == 0)
    {
      winner = 2;
      game_over();
    }

    if(b->x == 127)
    {
      winner = 1;
      game_over();
    }

    //Kontrollera kollision med brackets
    if(is_pixel_on(b->x, b->y, map)) 
    {
      leds++;                       //Lägg till 1 på leds om bollen kolliderar med bracketen
      PORTE = leds;                 //Visa poängen på LED

      b->x_speed *= -1;                 //Byt riktning i x-led om bollen kolliderar med kanterna. (x_speed = -x_speed)
    }

    //Kontrollerar kollision med tak och golv 
    if(b->y <= 0 || b->y >= 31)
    {
      b->y_speed *= -1;                 //Byt riktning i y-led om bollen kolliderar med kanterna. (y_speed = -y_speed)
    }

    // Rita bollen på sin nya position
    draw_pixel(b->x, b->y, map, 1);

    counter = velocity_b;             // Bollen kommer att flyttas olika snabbt beroende på velocity_b

  } else {
    counter--;
  }
}


/*Kontrollera om en pixel är tänd eller släkt kan användas 
för att kolla om bollen kolliderar med bracketen.*/
int is_pixel_on(int x, int y, uint8_t map[]) 
{
  int page = y / 8;
  int pixel = y % 8;
  int index = page * 128 + x;

  if(map[index] && 0x1)
    return 1;
  else
    return 0;
}


//Funktion för game over och börja spela om
void game_over(void)
{
  clear_screen();

  if(winner == 1)
  {
    display_string(1, " Left player win");
    display_update();
  }

  if(winner == 2)
  {
    display_string(1, "Right player win");
    display_update();
  }

  delay(5000);
  reset_game();
  game_play();
}

//Funktionen för att nollställa spelet.
void reset_game(void) 
{
  clear_bracket(my_bracket, single_map);
  clear_bracket(my_bracket2, single_map);
  clear_screen();

  my_bracket.x = 1;
  my_bracket.y = 12;
  my_bracket.y_height = 6;

  my_bracket2.x = 126;
  my_bracket2.y = 12;
  my_bracket2.y_height = 6;

  ball1.x = 74;
  ball1.y = 16;
  ball1.x_speed = 1;
  ball1.y_speed = 1;

  //Nollställer poängen
  leds = 0;
  PORTE = 0;
  winner = 0;         //Nollställer vinnaren
  velocity_b = 20;    //återställer hastigheten på bollen till starthastigheten
}


void game_play(void)
{
	int val = show_menu();

	while(1)
	{
		switch (val)
		{
		case 1: 						      //Play game
      labwork();		
			break;
		case 2:							      //show credentials	
			clear_screen();
			credentials();				
			val = show_menu();			//Går tillbaka till menyn igen
			break;
		}
	}
}











  


