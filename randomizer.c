#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define CARDNUMBERS 18 // 6, 18, 32
#define CARDRAND CARDNUMBERS/2

struct CardData
{
   short  position;
};

struct CardData card_data[CARDNUMBERS];

void cleaner(void)
{
   int xcnt;
	
   for (xcnt = 0; xcnt < CARDNUMBERS; xcnt++)
   {
      card_data[xcnt].position = 0;
   }
}

void randomizer(void)
{
   int xcnt;
   int xnum;
   int xval;
   char paired;
   
   srand(time(NULL));
   for (xcnt=0; xcnt < CARDNUMBERS; xcnt++)
   {
      do
	  {
	     xval = (rand()%CARDRAND)+1;
	     paired = 0;
	  
	     for (xnum=0; xnum < xcnt; xnum++)
	     {
            if (card_data[xnum].position == xval)
            {
               paired++;   
		    }
	     }
	  
	     if (paired < 2) 
	        card_data[xcnt].position = xval;

	  } while(paired >= 2);
	  
	  printf("%02d : %d\n", xcnt, card_data[xcnt].position);
   }
}


int main ()
{
   cleaner();
   randomizer();
   return 0;
}