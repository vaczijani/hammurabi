// Hammurabi: simple resource management strategy game
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define GOV_YEARS 10

#define POP_START 95  // starting population
#define IMM_START 5  // first ingress of people

#define FIRST_YIELD 3  // first year acre yield
#define MAX_YIELD 5

#define MAX_RATS 5
#define MAX_BABIES 5

#define MIN_PRICE 17
#define MAX_PRICE 10

#define FIRST_HARVEST 3000
#define STORE_START 2800

#define SEED 0.5  // bushel/acre
#define WORKFORCE 10  // acres/person
#define FEED 20  // bushel/person
#define STARVATION_THRESHOLD 45 // percent

bool plague() { return (rand() % 100) < 15; }

// returns 0..5
int fifth(double value, double min, double max) {
	double range = max - min;
	double pos = value - min;
	int ret = (int)((pos*5) / range);
	return (ret > 4) ? 4 : ret;
}

const char* much(double value, double min, double max) {
	switch(fifth(value, min, max)) {
	default:
	case 0: return "negligible";
	case 1: return "little";
	case 2: return "average";
	case 3: return "much";
	case 4: return "loads";
	}
}

const char* good(double value, double min, double max) {
	switch(fifth(value, min, max)) {
	default:
	case 0: return "worst";
	case 1: return "bad";
	case 2: return "average";
	case 3: return "good";
	case 4: return "excellent";
	}
}

const char* expensive(double value, double min, double max) {
	switch(fifth(value, min, max)) {
	default:
	case 0: return "cheap";
	case 1: return "moderate";
	case 2: return "average";
	case 3: return "high";
	case 4: return "expensive";
	}
}

int main() {
	srand(time(NULL));
	printf("Try your hand at governing ancient Sumeria for a %d year term of office.\n", GOV_YEARS);
	// extend with statistics
	int nplagues = 0;
	int plague_toll = 0;
	int nharvested = 0;
	int nstarved = 0;
	int nrats = 0;

	int year = 1; // Z
	int starved = 0;  // D
	int immigrants = IMM_START;  // I
	int store = STORE_START;  // S
	int population = POP_START;  // P
	int harvest = FIRST_HARVEST;  // H
	int yield = FIRST_YIELD; // Y
	int acres = harvest / yield; // A = H/Y
	int error = harvest - store; // E = H-S
	while (true) {
		printf("\nHammurabi: I beg to report to you,\n");
		nstarved += starved;
		printf("in year %d, %d people starved, %d came to the city.\n", year, starved, immigrants);
		population += immigrants;
		if (plague()) {
			printf("\nA horrible plague struck! Half the people died.\n\n");
			++nplagues;
			plague_toll += population - population/2;
			population /= 2;
		}
		printf("Population is now %d.\n", population);
		printf("The city now owns %d acres.\n", acres);
		printf("You harvested %d bushels per acre. (%s)\n", yield, good(yield, 1, MAX_YIELD));
		nrats += error;
		printf("Rats ate %d bushels.\n", error);
		printf("You now have %d bushels in store.\n\n", store);
		nharvested += harvest;
		if (starved > (STARVATION_THRESHOLD * population) / 100) {
			printf("\nYou starved %d people in one year!\n", starved);
			printf("Due to this extreme mismanagement you have not only\n");
			printf("been impeached and thrown out of office but you have\n");
			printf("also been declared national flink!\n");
			break;
		}
		if (year >= GOV_YEARS) {
			printf("\nWell done!\nYou've admirably served your %d year period.\n", GOV_YEARS);
			break;
		}
		++year;
		int price = (rand() % MAX_PRICE) + MIN_PRICE;
		printf("Land is trading at %d bushels per acre. (%s)\n", price, expensive(price, MIN_PRICE, MIN_PRICE + MAX_PRICE));

		int quantity;
		// user input: buy
		do {
			printf("How many acres do you wish to buy? ");
			scanf("%d", &quantity);
		} while (quantity < 0 || quantity*price > store);
		if (quantity == 0) {
			// user input: sell
			do {
				printf("How many acres do you wish to sell? ");
				scanf("%d", &quantity);
			} while (quantity < 0 || quantity > acres);
			if (quantity > 0) {
				acres -= quantity;
				store += quantity * price;
				printf("The city now owns %d acres\n", acres);
				printf("You now have %d bushels in store.\n", store);
			}
		} else {
			acres += quantity;
			store -= quantity * price;
			printf("The city now owns %d acres\n", acres);
			printf("You now have %d bushels in store.\n", store);
		}
		// user input: feed
		int feed;
		do {
			printf("How many bushels do you wish to feed your people? (%d per person) ", FEED);
			scanf("%d", &feed);
		} while (feed < 0 || feed > store);
		store -= feed;
		// user input: plant
		do {
			printf("How many acres do you wish to plant with seed? (%.1f bushels per acre, %d acres per person) ", SEED, WORKFORCE);
			scanf("%d", &quantity);
			if (SEED * quantity > store) {
				printf("Hammurabi: Think again. You have only\n%d bushels of grain. Now then,\n", store);
				quantity = -1;
				continue;
			}
			if (quantity > WORKFORCE * population) {
				printf("Hammurabi: Think again. You have only\n%d people to tend fields. Now then,\n", population);
				quantity = -1;
				continue;
			}
		} while (quantity < 0 || quantity > acres);
		// simulation: tend the land
		store -= SEED * quantity;
		printf("%d acres were tended, costed %.0f bushels of grain.\n", quantity, SEED * quantity);
		// simulation: harvest
		yield = (rand() % MAX_YIELD) + 1;
		harvest = quantity * yield;
		printf("Yield was %d bushels per acre. Harvested %d bushels of grain.\n", yield, harvest);
		// simulation: rats
		error = 0;
		int rats = (rand() % MAX_RATS) + 1;
		if (rats % 2 != 0) {
			error = store / rats;
		}
		store -= error;
		store += harvest;
		printf("Rats ate %d bushels of grain. Store now has %d bushels.\n", error, store);
		// simulation: babies
		immigrants = ((rand() % MAX_BABIES) + 1) * (20 * acres + store) / population / 100 + 1;
		// simulation: feed people
		int full = feed / FEED;
		if (full < population) {
			// starvation
			starved = population - full;
			population -= starved;
		}
	}
	printf("In your %d years in office:\n", year);
	printf("You starved %d people to death.\n", nstarved);
	printf("You survived %d plagues and lost %d people.\n", nplagues, plague_toll);
	printf("Harvested %d bushels of grain.\n", nharvested);
	printf("Rats ate %d bushels of your grain.\n", nrats);
	return 0;
}
