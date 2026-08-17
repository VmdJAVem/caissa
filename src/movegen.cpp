#include <vector>
#include "movegen.hpp"
#include "board.hpp"

// TODO: En passant & promotion
std::vector<Move> generatePawnMoves(const Board& board, Color color) {
	std::vector<Move> moves;
	Bitboard pawns = board.getPieces(color, Piece::Pawn);

	while (pawns) {
		int index = std::countr_zero(pawns);
		Square sq = static_cast<Square>(index);
		Bitboard targetBB = pawnPushTable[static_cast<int>(color)][index];

		if (targetBB != 0) {
			Square target = bitboardToSquare(targetBB);
			
			if (!board.pieceAt(target)) {   // only push if target is empty
				moves.push_back(Move{
					.from = sq,
					.to = target,
					.piece = Piece::Pawn,
					.capturedPiece = std::nullopt});
				bool onStartRank = (color == Color::White)
						   ? (index >= static_cast<int>(Square::A2) && index <= static_cast<int>(Square::H2))
						   : (index >= static_cast<int>(Square::A7) && index <= static_cast<int>(Square::H7));

				if (onStartRank) {
					int doubleTargetIndex = (color == Color::White) ? index + 16 : index - 16;
					auto doubleTarget = static_cast<Square>(doubleTargetIndex);
					if (!board.pieceAt(doubleTarget)) {
						moves.push_back(Move{
							.from = sq,
							.to = doubleTarget,
							.piece = Piece::Pawn,
							.capturedPiece = std::nullopt
						});
					}
				}
			}		
			pawns &= pawns - 1;
		}
		Bitboard possibleCaptures = pawnCaptureTable[static_cast<int>(color)][index];
		while (possibleCaptures) {
			int targetIndex = std::countr_zero(possibleCaptures);
			Square targetSquare = static_cast<Square>(targetIndex);

			auto targetPiece = board.pieceAt(targetSquare);

			if (targetPiece) {
				if (targetPiece->color != color) {
					moves.push_back(Move{
						.from = sq,
						.to = targetSquare,
						.piece = Piece::Pawn,
						.capturedPiece = targetPiece->piece
					});
				}
			}
			possibleCaptures &= possibleCaptures - 1;
		}
	}
	return moves;
}
