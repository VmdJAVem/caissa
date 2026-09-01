#include "board.hpp"
#include "evaluation.hpp"
#include "types.hpp"

constexpr int mirrorSquare(int index)
{
	int file = index % 8;
	int rank = index / 8;
	int mirroredRank = 7 - rank;
	return mirroredRank * 8 + file;
}

int evaluate(const Board &board)
{
	auto color = board.getSideToMove();
	auto enemyColor = color == Color::White ? Color::Black : Color::White;

	int result = 0;

	constexpr std::array<int, 6> pieceValues = {
	    100, // Pawn
	    300, // Knight
	    300, // Bishop
	    500, // Rook
	    900, // Queen
	    0 // King
	};

	for (auto &p : allPieces) {
		auto pieces = board.getPieces(color, p);
		auto enemyPieces = board.getPieces(enemyColor, p);
		while (pieces) {
			int index = std::countr_zero(pieces);
			int pstIndex = (color == Color::White) ?
					   index :
					   mirrorSquare(index);

			int pstBonus =
			    pieceSquareTables[static_cast<int>(p)][pstIndex];

			result += pieceValues[static_cast<int>(p)] + pstBonus;
			pieces &= pieces - 1;
		}
		while (enemyPieces) {
			int index = std::countr_zero(enemyPieces);
			int pstIndex = (enemyColor == Color::White) ?
					   index :
					   mirrorSquare(index);

			int pstBonus =
			    pieceSquareTables[static_cast<int>(p)][pstIndex];

			result -= pieceValues[static_cast<int>(p)] + pstBonus;
			enemyPieces &= enemyPieces - 1;
		}
	}

	return result;
}
