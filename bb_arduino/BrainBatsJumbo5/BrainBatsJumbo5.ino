#include "GiantLOL.h"

//**************************************************************//
//  Name    : Brain Pong for Arduino/Neurosky Headsets / Charlieplexing                 //
//  Author  : Benjamin Sonntag http://benjamin.sonntag.fr/      //
//  Modified: Matt Mets http://cibomahto.com                    //
//  Modified: David Reeves http://hackmanhattan.com
//  Date    : 28 May 2010                                       //
//  Version : 0.2                                               //
//  Notes   : Uses Charlieplexing library to light up           //
//          : a matrix of 126 LEDs in a 9x14 grid               //
//          : from Jimmie P Rodgers www.jimmieprodgers.com      //
//**************************************************************//

// Modified by David Reeves Apr 2012
// Accepts serial input from pong.py to control Player 2 via MindWave EEG headset
// Elevated attention values (> 50) are required to move the bat


/* ---------------------------------------------------------------------------*/
/** The figures from 0 to 9 encoded in 7 lines of 5 bits :
 */
uint16_t figures[][7] = { 
{14,17,17,17,17,17,14},
{4,6,4,4,4,4,14},
{14,17,16,14,1,1,31},
{14,17,16,14,16,17,14},
{8,12,10,9,31,8,8},
{31,1,1,15,16,16,15},
{14,17,1,15,17,17,14},
{31,16,8,8,4,4,4},
{14,17,17,14,17,17,14},
{14,17,17,30,16,16,15},
};

  
int x,y,dx,dy;
int sh1y,sh2y,s1,s2;

int inByte = 0;               // for serial input

/* ---------------------------------------------------------------------------*/
/* Arduino setup func
 */
void setup() {
  LedSign::Init(DOUBLE_BUFFER);
  
  x = 3;
  y = 7;
  sh1y=5;
  sh2y=5;
  dx = 1;
  dy = 1;
  s1 = 0;
  s2 = 0;
  
  Serial.begin(9600);
  
  randomSeed(analogRead(0));
  drawscores();
}


/* ---------------------------------------------------------------------------*/
/* Arduino main loop 
 */
void loop() {
  int ct1,ct2,randommove;


  // The Ball shall bounce on the walls : 
  if (x==14 || x==1) {
    dx=-dx;
    // Collision detection
    if (x==1) {
      // check the first ship (left side)
      if (sh1y!=y && sh1y+1!=y) {
        s2++;
        drawscores();
        checkscores();
      }
    } else {
// check the second ship (right side)
      if (sh2y!=y && sh2y+1!=y) {
        s1++;
        drawscores();
        checkscores();
      }
}
  }
  if (y==10 || y==0) dy=-dy;
 
  
  // Clear the non-active screen
  LedSign::Clear();

  // Move the BALL : 
  x=x+dx; 
  y=y+dy;
  
  // Draw the ball :
  LedSign::Set(x,y,1);

  // Draw the Ship
  LedSign::Set(0, sh1y, 1);
  LedSign::Set(0, sh1y+1, 1);
  LedSign::Set(15, sh2y, 1);
  LedSign::Set(15, sh2y+1, 1);
  
  // The ships moves when the ball go in their direction. They follow it magically ;) :   
  /* This code is too smart, in fact he is perfekt :) 
  if (dx<0) {
    if (sh1y>y) {
      sh1y--;
    }
    if (sh1y<y) {
      sh1y++;
    }
  }
  // let's code a dummy one 
  */
  // Player 1 (Computer)
  if (dx>0) {
    // the ball goes away from me, let's move randomly
    randommove=random(0,3);
    if (randommove==0) {
      sh1y--;
    }
    if (randommove==1) {
      sh1y++;
    }
  } else {
    if (sh1y>y && (random(0,12)<9 || x<3)) {
      sh1y--;
    }
    if (sh1y<y && (random(0,12)<9 || x<3)) {
      sh1y++;
    }
    if (random(0,8)==0) {
      if (sh1y>y) {
        sh1y++;
      }
      if (sh1y<y) {
        sh1y--;
      }      
    }
  }
    // Player 2 (Human connected via EEG headset)
    while (Serial.available() > 0) {
      inByte = Serial.read();
      Serial.print((inByte - 48));
    }
    
//    inByte = 70;

    if (sh2y>y && (inByte - 48) > 4) {
      sh2y--;
    }
    if (sh2y<y && (inByte - 48) > 4) {
      sh2y++;
    }
   
     
    
  
  // Human Player 
  // 1/4 of the variator is used. If we use it fully, it's too hard to play.
  // To use it fully replace 36 by 146
  //sh2y=analogRead(5)/72; 



  // Sanity checks for the ships : 
  if (sh1y>9) sh1y=9;
  if (sh2y>9) sh2y=9;
  if (sh1y<0) sh1y=0;
  if (sh2y<0) sh2y=0;

  // swap the screens ;) (sometime we may need this double-buffer algorithm...
  // of course, as of today it's a little bit overkill ...)

  LedSign::Flip();

  // Display the bitmap some times 
  delay(400);
  
  // loop :) 
}


/* ---------------------------------------------------------------------------*/
/** Check if a player won !
 * If one of the players won, let's show a funny animation ;) 
 */
void checkscores() {  
  // TODO : DO the animation ;)
}
  
  
/* ---------------------------------------------------------------------------*/
/** Draw the scores in a lovely scrolling :) 
 * Use the current active screen brutally ...
 */
void drawscores() {
  int i,j,ps1,ps2,ct2;
  
  for(ps1=0;ps1<9;ps1++) {
    LedSign::Clear();  // Clear the active screen

    LedSign::Set(7,5,1); // dash between the scores
    LedSign::Set(8,5,1);
    
    // Fill it with both scores : 
    // Left score goes up>down
    i=ps1; j=6;
    while (i>=0 && j>=0) {
      for(uint8_t k = 0; k < 5; k++) {
        LedSign::Set(k+1,i,(figures[s1][j] >> k) & 1 );
      }
      
      i--; j--;
    }
    // Right score goes down>up
    ps2=10-ps1;
    i=ps2; j=0;
    while (i<=10 && j<=6) {
      for(uint8_t k = 0; k < 5; k++) {
        LedSign::Set(k+10,i,(figures[s2][j] >> k) & 1 );
      }

      i++; j++;
    }    

    LedSign::Flip();
    delay(200);
  }
  
  delay(3000);
  
  LedSign::Clear(0);

  if (s1==5 || s2==5) {
    for(ps1=0;ps1<3;ps1++) {
      LedSign::Flip();      
      delay(300);
      
      LedSign::Flip();
      delay(600);
      
    }
    delay(3000);

    s1=0; s2=0;
    drawscores();
  }
}


