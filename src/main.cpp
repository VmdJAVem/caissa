#include <iostream>

#include "board.hpp"
#include "movegen.hpp"

int main(void) {
	Board b;

	std::cout << b.toString();

	auto moves = generateWhitePawnPushes(b);
	std::cout << "White pawn pushes: " << moves.size() << "\n";
	for (auto& m : moves) {
		std::cout << "  from square " << static_cast<int>(m.from)
                << " to " << static_cast<int>(m.to) << "\n";
	}
}
