#include <iostream>

#include "board.hpp"
#include "movegen.hpp"

int main(void) {
	Board b;

	std::cout << b.toString();

	auto moves = generatePawnMoves(b, Color::White);
	std::cout << "White pawn pushes: " << moves.size() << "\n";
	for (auto& m : moves) {
		std::cout << "  from square " << static_cast<int>(m.from)
                << " to " << static_cast<int>(m.to) << "\n";
	}
	auto blackMoves = generatePawnMoves(b, Color::Black);
	std::cout << "Black pawn pushes:" << blackMoves.size() << "\n";
	for (auto& m : blackMoves) {
		std::cout << "  from square " << static_cast<int>(m.from) << " to " << static_cast<int>(m.to) << "\n";
	}
}
