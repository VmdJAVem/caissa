#include "board.hpp"
#include "movegen.hpp"
#include "uci.hpp"
#include <iostream>

int main()
{
	Uci::loop();
	// Board board;
	// for (int i = 0; i <= 5; i++) {
	// 	std::cout << "=== DEPTH " << i << " ===" << "\n";
	// 	std::cout << perft(board, i) << "\n";
	// }
	return 0;
}
