#include <vector>
#include "movegen.hpp"

std::vector<Move> generateWhitePawnPushes(const Board& board) {
	std::vector<Move> moves;
	Bitboard pawns = board.getPieces(Color::White, Piece::Pawn);

	while (pawns) {
		    int index = std::countr_zero(pawns);
		    Square sq = static_cast<Square>(index);
		    auto p = board.pieceAt(sq);
		    if (p) {
			    if (p.value().color == Color::White) {
				    if (index + 8 < 63) {
					    Square target = static_cast<Square>(index + 8);
					    moves.push_back(Move{sq, target});
				    }
			    }
		    }
		    pawns &= pawns - 1;
	}
	return moves;
}
