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

void increase_delay(){
	delay_time += 50;
	printw("delay time is increased to %d\n" , delay_time);
}

void decrease_delay(){
	delay_time -= 50;
	if(delay_time < 0){
		delay_time = 0;
		printw("delay time is zero");
	}else{
		printw("delay time is decreased to %d\n" , delay_time);
	}
}

float mod(float number){
	if(number >= 0){
		return number;
	}
	return 0 - number;
}

void print_colourf(uint32_t red , uint32_t green , uint32_t blue , float redGreenDiff , float redBlueDiff, float blueGreenDiff){
	printf("\n --------- ALEX COLOR SENSOR --------- \n\n");
	printf("Red (R) frequency:\t%d\n", red);
	printf("Green (G) frequency:\t%d\n", green);
	printf("Blue (B) frequency:\t%d\n", blue);
	printf( "Red Green diff:\t\t%0.2lf%\n", redGreenDiff);
	printf( "Red Blue diff:\t\t%0.2lf%\n", redBlueDiff);
	printf( "Blue Green diff:\t%0.2lf%\n", blueGreenDiff);
	if(blueGreenDiff > 17.0){
		if(mod(redGreenDiff - blueGreenDiff) < 6){
			printf("RED\n");
		}else{
			printf("NEITHER\n");
		}
	}else{
		printf("GREEN!\n");
	}
	printf("\n");
}

float find_max(float redGreenDiff , float redBlueDiff , float blueGreenDiff){
	if(redGreenDiff > redBlueDiff){
		if(redGreenDiff > blueGreenDiff){
			return redGreenDiff;
		}
		return blueGreenDiff;
	}else{
		if(redBlueDiff > blueGreenDiff){
			return redBlueDiff;
		}
		return blueGreenDiff;
	}
}

void print_colourw(uint32_t red , uint32_t green , uint32_t blue , float redGreenDiff , float redBlueDiff, float blueGreenDiff){
	printw("\n --------- ALEX COLOR SENSOR --------- \n\n");
	printw("Red (R) frequency:\t%d\n", red);
	printw("Green (G) frequency:\t%d\n", green);
	printw("Blue (B) frequency:\t%d\n", blue);
	printw( "Red Green diff:\t\t%0.2lf%\n", redGreenDiff);
	printw( "Red Blue diff:\t\t%0.2lf%\n", redBlueDiff);
	printw( "Blue Green diff:\t%0.2lf%\n", blueGreenDiff);

	
	if ((redBlueDiff < redGreenDiff) && (redBlueDiff < blueGreenDiff)) 
	{
		printw("RED!\n");

	}else if(redGreenDiff < 4){
		
		printw("NEITHER!\n");
	} 
	else 
	{
		printw("GREEN!\n");
	}
	
}

void refresh_screen(){
	if(commands_sent != 0 && commands_sent % 5 == 0){
		clear();
		printw_commands();
		refresh();
	}
}
