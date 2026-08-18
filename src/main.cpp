#include <iostream>

#include "board.hpp"
#include "movegen.hpp"

int main(void) {

	std::string line;
	std::getline(std::cin, line);

	auto eb = Board::fromFen(line);
	Board b;

	if (eb.has_value()) {
		std::cout << "FEN STRING: " << line << "\n\n\n";
		b = eb.value();
	} else {
		std::cout << "FEN error: " << eb.error() << "\n";
		return -1;
	}

	std::cout << b.toString();
	// auto moves = generatePawnMoves(b, Color::White);
	// std::cout << "White:\n";
	// for (auto& m : moves) {
	// 	std::cout << "  " << squareName(m.from) << " -> " << squareName(m.to);
	// 	if (m.capturedPiece != Piece::None) std::cout << " (capture)";
	// 	if (m.promotionPiece != Piece::None) std::cout << " (promotes)";
	// 	std::cout << "\n";
	// }
	// auto blackMoves = generatePawnMoves(b, Color::Black);
	// std::cout << "Black:\n";
	// for (auto& m : blackMoves) {
	// 	std::cout << "  " << squareName(m.from) << " -> " << squareName(m.to);
	// 	if (m.capturedPiece != Piece::None) std::cout << " (capture)";
	// 	if (m.promotionPiece != Piece::None) std::cout << " (promotes)";
	// 	std::cout << "\n";
	// }
	auto moves = generateKingMoves(b, Color::White);
	std::cout << "Kings:\n";
	 for (auto& m : moves) {
	 	std::cout << "  " << squareName(m.from) << " -> " << squareName(m.to);
	 	if (m.capturedPiece != Piece::None) std::cout << " (capture)";
	 	std::cout << "\n";
	 }
}
