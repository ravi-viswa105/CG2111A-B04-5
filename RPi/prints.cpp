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
		printw("speed is already 0");
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
		printf("speed is already 0");
	}else{
		speed -=5;
		printf("speed is decreased to %d\n" , speed);
	}
}

void refresh_screen(){
	if(commands_sent % 9 == 0){
		clear();
		refresh();
	}
}
