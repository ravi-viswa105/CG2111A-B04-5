float getPercentDiff(uint32_t a, uint32_t b) {
	uint32_t diff = (a > b) ? a-b : b-a; // abs diff
	uint32_t large = (a > b) ? a : b; // max
	return (float) diff / large * 100.0;
}

void handleColor(TPacket *packet) {
	uint32_t red = packet->params[0];
	uint32_t green = packet->params[1];
	uint32_t blue = packet->params[2];

	printf("\n --------- ALEX COLOR SENSOR --------- \n\n");
	printf("Red (R) frequency:\t%d\n", red);
	printf("Green (G) frequency:\t%d\n", green);
	printf("Blue (B) frequency:\t%d\n", blue);

	// Determine color
	const int COLOR_THRESHOLD = 10;
	const int RED_THRESHOLD = 1000;
	const int GREEN_THRESHOLD = 25;

	float redGreenDiff = getPercentDiff(red, green);
	float blueGreenDiff = getPercentDiff(blue, green);

	printf( "Red Green diff:\t\t%0.2lf%\n", redGreenDiff);
	printf( "Blue Green diff:\t%0.2lf%\n", blueGreenDiff);

	if (redGreenDiff >= COLOR_THRESHOLD) {
		if (red < green) {
			if (green > RED_THRESHOLD) printf("\nRED!\n");
			else printf("\nORANGE!\n");
		} else {
			if (blueGreenDiff < GREEN_THRESHOLD) printf("\nGREEN!\n");
			else printf("\nBLUE!\n");
		}
	} else printf("\nNo color detected!\n");
	printf("\n--------------------------------------\n\n");
}

void handleDistance(TPacket *packet) {
	uint32_t distance = packet->params[0];
	printf("Ultrasonic Distance:\t\t%d cm\n", distance);
	
	const int DIST_THRESHOLD = 25;
	if (distance < DIST_THRESHOLD) printf("WALL NEARBY! SLOW DOWN!\n");
}





