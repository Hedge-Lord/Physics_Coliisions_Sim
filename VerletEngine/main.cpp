#include "solver.h"


int main() {
	Solver solver(60, sf::Vector2i(350,1000));
	solver.setNumSteps(12);
	solver.setMaxNumObjects(345);
	solver.setGravity(sf::Vector2f(0, 1500));
	solver.run();
}