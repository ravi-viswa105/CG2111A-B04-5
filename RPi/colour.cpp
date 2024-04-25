#include "colour.h"
#include "prints.h"
#define SHROUDTOULTRA 7

float getPercentDiff(uint32_t a, uint32_t b) { // returns absolute difference between the two colours
	uint32_t diff = (a > b) ? a-b : b-a; // abs diff
	uint32_t large = (a > b) ? a : b; // max
	return (float) diff / large * 100.0;
}

void handleColor(TPacket *packet) {
	uint32_t red = packet->params[0];
	uint32_t green = packet->params[1];
	uint32_t blue = packet->params[2];
	float redGreenDiff = getPercentDiff(red, green);
	float redBlueDiff = getPercentDiff(red, blue);
	float blueGreenDiff = getPercentDiff(blue, green);
	if(keyboardMode == 1 || keyboardMode == 2){
		print_colourf(red , green , blue , redGreenDiff , redBlueDiff, blueGreenDiff);	//for mode 1 , 2
	}else{
		print_colourw(red , green , blue , redGreenDiff , redBlueDiff, blueGreenDiff);	//mode 3 , 4
	}
}

void handleDistance(TPacket *packet) {
	uint32_t distance = packet->params[0];
	printf("Ultrasonic Distance:\t\t%d cm\n", distance); // returns distance from ultrasonic to object
	printf("Actual distance from shroud is \t\t%d cm\n" , distance - SHROUDTOULTRA); // return distance of shroud from object
	const int DIST_THRESHOLD = 25;
	if (distance < DIST_THRESHOLD) printf("WALL NEARBY! SLOW DOWN!\n");
}





