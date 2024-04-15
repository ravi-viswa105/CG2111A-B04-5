#include "prints.h"

void printf_commands(){
	
	printf("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats v= colour q=exit)\n");
}

void printw_commands(){
	
	printw("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats v= colour q=exit)\n");
}

void increase_speedw(){
	if(speed+5 <= 100){
		speed += 5;
		printw("speed is increased to %d\n" , speed);
	}else{
		speed = 100;
		printw("speed is maxed at 100\n");
	}
}

void decrease_speedw(){
	if(speed == 0){
		printw("speed is already 0\n");
	}else{
		speed -=5;
		printw("speed is decreased to %d\n" , speed);
	}
}

void increase_speedf(){
	if(speed+5 <= 100){
		speed += 5;
		printf("speed is increased to %d\n" , speed);
	}else{
		speed = 100;
		printf("speed is maxed at 100\n");
	}
}

void decrease_speedf(){
	if(speed == 0){
		printf("speed is already 0\n");
	}else{
		speed -=5;
		printf("speed is decreased to %d\n" , speed);
	}
}

void print_colourf(uint32_t red , uint32_t green , uint32_t blue , float redGreenDiff , float blueGreenDiff){
	printf("\n --------- ALEX COLOR SENSOR --------- \n\n");
	printf("Red (R) frequency:\t%d\n", red);
	printf("Green (G) frequency:\t%d\n", green);
	printf("Blue (B) frequency:\t%d\n", blue);
	printf( "Red Green diff:\t\t%0.2lf%\n", redGreenDiff);
	printf( "Blue Green diff:\t%0.2lf%\n", blueGreenDiff);
/*
	if ((red+5 <= green) && (green > 15)) 
	{
		printf("\nRED!\n");
	}
	else if (blue < 10) 
	{
		printf("\nNEITHER!\n");
	}
	else 
	{
		printf("\nGREEN!\n");
	}*/
	if((green > 60 && green <= 100 && blue > 70 && blue < 130) || (red > 120 && green <= 180))
  {
   	printf("GREEN\n");
  }
    else if((red < 120 && green > 105 && blue > 70) || (red < 180 && green >= 250))
  {
    printf("RED\n");
  }
  else
  {
    printf("NEITHER\n");
  }
}

void print_colourw(uint32_t red , uint32_t green , uint32_t blue , float redGreenDiff , float blueGreenDiff){
	printw("\n --------- ALEX COLOR SENSOR --------- \n\n");
	printw("Red (R) frequency:\t%d\n", red);
	printw("Green (G) frequency:\t%d\n", green);
	printw("Blue (B) frequency:\t%d\n", blue);
	printw( "Red Green diff:\t\t%0.2lf%\n", redGreenDiff);
	printw( "Blue Green diff:\t%0.2lf%\n", blueGreenDiff);

	if ((red+5 <= green) && (green > 15)) 
	{
		printw("\nRED!\n");
	}
	else if (blue < 10) 
	{
		printw("\nNEITHER!\n");
	}
	else 
	{
		printw("\nGREEN!\n");
	}
}

void refresh_screen(){
	if(commands_sent != 0 && commands_sent % 9 == 0){
		clear();
		refresh();
	}
}
