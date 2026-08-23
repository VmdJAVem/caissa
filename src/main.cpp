#include "board.hpp"
#include "movegen.hpp"
#include "tests.hpp"
#include <iostream>

int main() {
	Board board;
	testNegamax("rnbqkbnr/pppp1ppp/8/4p3/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1", 2, Square::D4, Square::E5);
	return 0;
}
