#include "board.hpp"
#include "movegen.hpp"
#include "tests.hpp"
#include <iostream>

int main() {
	Board board;
	// White has an extra knight (b1), otherwise equal material. White to move.
	testEvaluate("4k3/8/8/8/8/8/8/1N2K3 w - - 0 1", 3);

	// Same position, but Black to move — should now read -3
	// (from Black's perspective, White's material advantage is a disadvantage for the mover)
	testEvaluate("4k3/8/8/8/8/8/8/1N2K3 b - - 0 1", -3);	
	return 0;
}
