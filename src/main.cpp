#include <iostream>

#include "board.hpp"
#include "movegen.hpp"

int main(void) {

/*	std::string line;
	std::getline(std::cin, line);
*/
//	auto eb = Board::fromFen(line);
	Board b;
	/*
		if (eb.has_value()) {
			std::cout << "FEN STRING: " << line << "\n\n\n";
			b = eb.value();
		} else {
			std::cout << "FEN error: " << eb.error() << "\n";
			return -1;
}
*/

	std::cout << b.toString();
	auto moves = generateAllMoves(b);
	std::cout << "Moves:\n";
	 for (auto& m : moves) {
	 	std::cout << "  " << squareName(m.from) << " -> " << squareName(m.to);
	 	if (m.capturedPiece != Piece::None) std::cout << " (capture)";
	 	std::cout << "\n";
	 }
}
