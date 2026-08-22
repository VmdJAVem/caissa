#include "board.hpp"
#include "types.hpp"

int evaluate(const Board &board) {
	auto color = board.getSideToMove();
	auto enemyColor = color == Color::White ? Color::Black : Color::White;

	int result = 0;

	constexpr std::array<int, 6> pieceValues = {
	    1, // Pawn
	    3, // Knight
	    3, // Bishop
	    5, // Rook
	    9, // Queen
	    0  // King
	};
	

	for (auto &p : allPieces) {
		auto pieces = board.getPieces(color, p);
		auto enemyPieces = board.getPieces(enemyColor, p);
		result += (pieceValues[static_cast<int>(p)] * std::popcount(pieces));
		result -= (pieceValues[static_cast<int>(p)] * std::popcount(enemyPieces));
	}
	
	return result;
}
