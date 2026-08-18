#include "movegen.hpp"
#include "board.hpp"
#include <vector>

std::vector<Move> generatePawnMoves(const Board &board, Color color) {
	std::vector<Move> moves;
	Bitboard pawns = board.getPieces(color, Piece::Pawn);

	while (pawns) {
		int index = std::countr_zero(pawns);
		Square sq = static_cast<Square>(index);
		Bitboard targetBitboard = pawnPushTable[static_cast<int>(color)][index];

		if (targetBitboard != 0) {
			Square target = bitboardToSquare(targetBitboard);
			if (!board.pieceAt(target)) {
				auto targetIndex = static_cast<int>(target);
				if ((color == Color::White)
				    ? (targetIndex >= static_cast<int>(Square::A8) && targetIndex <= static_cast<int>(Square::H8))
				    : (targetIndex >= static_cast<int>(Square::A1) && targetIndex <= static_cast<int>(Square::H1))) {
					for (auto &p : {Piece::Knight, Piece::Bishop, Piece::Rook, Piece::Queen}) {
						moves.push_back(Move{
							.from = sq,
							.to = target,
							.piece = Piece::Pawn,
							.capturedPiece = Piece::None,
							.promotionPiece = p,
							.isEnPassant = false});
					}
				} else {
					moves.push_back(Move{
						.from = sq,
						.to = target,
						.piece = Piece::Pawn,
						.capturedPiece = Piece::None,
						.promotionPiece = Piece::None,
						.isEnPassant = false});
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
								.capturedPiece = Piece::None,
								.promotionPiece = Piece::None,
								.isEnPassant = false});
						}
					}
				}
			}
		}
		Bitboard possibleCaptures = pawnCaptureTable[static_cast<int>(color)][index];
		Square enPassantTarget = board.getEnPassantTarget();
		while (possibleCaptures) {
			int targetIndex = std::countr_zero(possibleCaptures);
			Square targetSquare = static_cast<Square>(targetIndex);

			auto targetPiece = board.pieceAt(targetSquare);
			if (targetSquare == enPassantTarget) {
				moves.push_back(Move{
					.from = sq,
					.to = targetSquare,
					.piece = Piece::Pawn,
					.capturedPiece = Piece::Pawn,
					.promotionPiece = Piece::None,
					.isEnPassant = true});
			}
			else if (targetPiece) {
				if (targetPiece->color != color) {
					if ((color == Color::White)
					    ? (targetIndex >= static_cast<int>(Square::A8) && targetIndex <= static_cast<int>(Square::H8))
					    : (targetIndex >= static_cast<int>(Square::A1) && targetIndex <= static_cast<int>(Square::H1))) {
						for (auto &p : {Piece::Knight, Piece::Bishop, Piece::Rook, Piece::Queen}) {
							moves.push_back(Move{
								.from = sq,
								.to = targetSquare,
								.piece = Piece::Pawn,
								.capturedPiece = targetPiece->piece,
								.promotionPiece = p,
								.isEnPassant = false,
							});
						}
					} else {
						moves.push_back(Move{
							.from = sq,
							.to = targetSquare,
							.piece = Piece::Pawn,
							.capturedPiece = targetPiece->piece,
							.promotionPiece = Piece::None,
							.isEnPassant = false});
					}
				}
			}
			possibleCaptures &= possibleCaptures - 1;
		}
		pawns &= pawns - 1;
	}
	return moves;
}
std::vector<Move> generateKnightMoves(const Board &board, Color color) {
	std::vector<Move> moves{};
	Bitboard knights = board.getPieces(color, Piece::Knight);
	while (knights) {
		int index = std::countr_zero(knights);
		Square sq = static_cast<Square>(index);
		Bitboard attackTargets = knightAttackTable[index];
		while (attackTargets) {
			int attackedIndex = std::countr_zero(attackTargets);
			Square attackedSquare = static_cast<Square>(attackedIndex);
			auto attackedPiece = board.pieceAt(attackedSquare);

			if (attackedPiece) {
				if (attackedPiece->color != color) {
					moves.push_back(Move{
					    .from = sq,
					    .to = attackedSquare,
					    .piece = Piece::Knight,
					    .capturedPiece = attackedPiece->piece,
					    .promotionPiece = Piece::None,
					    .isEnPassant = false
					});
				}
			} else {
				moves.push_back(Move{
					.from = sq,
					.to = attackedSquare,
					.piece = Piece::Knight,
					.capturedPiece = Piece::None,
					.promotionPiece = Piece::None,
					.isEnPassant = false
				});
			}
			
			attackTargets &= attackTargets - 1;
		}
		knights &= knights - 1;
	}
	return moves;
}
// TODO: Castling
std::vector<Move> generateKingMoves(const Board &board, Color color) {
	std::vector<Move> moves{};
	Bitboard kings = board.getPieces(color, Piece::King);
	while (kings) {
		int index = std::countr_zero(kings);
		Square sq = static_cast<Square>(index);
		Bitboard attackTargets = kingAttackTable[index];
		while (attackTargets) {
			int attackedIndex = std::countr_zero(attackTargets);
			Square attackedSquare = static_cast<Square>(attackedIndex);
			auto attackedPiece = board.pieceAt(attackedSquare);

			if (attackedPiece) {
				if (attackedPiece->color != color) {
					moves.push_back(Move{
						.from = sq,
						.to = attackedSquare,
						.piece = Piece::King,
						.capturedPiece = attackedPiece->piece,
						.promotionPiece = Piece::None,
						.isEnPassant = false
					});
				}
			} else {
				moves.push_back(Move{
					.from = sq,
					.to = attackedSquare,
					.piece = Piece::King,
					.capturedPiece = Piece::None,
					.promotionPiece = Piece::None,
					.isEnPassant = false
				});
			}
			
			attackTargets &= attackTargets - 1;
		}
		kings &= kings - 1;
	}
	return moves;
}
