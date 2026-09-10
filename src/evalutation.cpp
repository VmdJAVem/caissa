#include "board.hpp"
#include "evaluation.hpp"
#include "types.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

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

	for (auto &p : allPieces) {
		auto pieces = board.getPieces(color, p);
		auto enemyPieces = board.getPieces(enemyColor, p);
		while (pieces) {
			int index = std::countr_zero(pieces);
			int pstIndex = (color == Color::White) ? index : mirrorSquare(index);

			int pstBonus = pieceSquareTables[static_cast<int>(p)][pstIndex];

			result += pieceValues[static_cast<int>(p)] + pstBonus;
			pieces &= pieces - 1;
		}
		while (enemyPieces) {
			int index = std::countr_zero(enemyPieces);
			int pstIndex = (enemyColor == Color::White) ? index : mirrorSquare(index);

			int pstBonus = pieceSquareTables[static_cast<int>(p)][pstIndex];

			result -= pieceValues[static_cast<int>(p)] + pstBonus;
			enemyPieces &= enemyPieces - 1;
		}
	}

	return result;
}

bool compareCaptures(const Move &a, const Move &b, const Board &board)
{
	assert(a.capturedPiece != Piece::None && b.capturedPiece != Piece::None);
	// all moves here should be legal so we shit ourselves if they aren't.
	
	auto attackerA = board.pieceAt(a.from).piece;
	int aScore = pieceValues[static_cast<int>(a.capturedPiece)] - pieceValues[static_cast<int>(attackerA)];
	if (a.promotionPiece != Piece::None)
		aScore += pieceValues[static_cast<int>(a.promotionPiece)];

	auto attackerB = board.pieceAt(b.from).piece;
	int bScore = pieceValues[static_cast<int>(b.capturedPiece)] - pieceValues[static_cast<int>(attackerB)];

	if (b.promotionPiece != Piece::None)
		bScore += pieceValues[static_cast<int>(b.promotionPiece)];

	return aScore > bScore;
}

void sortAllMoves(std::vector<Move>& moves, const Board& board) {
	auto firstQuiet = std::stable_partition(moves.begin(), moves.end(),
						[](const Move& m) { return m.capturedPiece != Piece::None; });
	std::sort(moves.begin(), firstQuiet,
		  [&board](const Move& a, const Move& b) {
			  return compareCaptures(a, b, board);
		  });
}
