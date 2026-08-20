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
				if ((color == Color::White) ? (targetIndex >= static_cast<int>(Square::A8) && targetIndex <= static_cast<int>(Square::H8))
							    : (targetIndex >= static_cast<int>(Square::A1) && targetIndex <= static_cast<int>(Square::H1))) {
					for (auto &p : {Piece::Knight, Piece::Bishop, Piece::Rook, Piece::Queen}) {
						moves.push_back(
						    Move{.from = sq, .to = target, .piece = Piece::Pawn, .promotionPiece = p, .isEnPassant = false});
					}
				} else {
					moves.push_back(Move{.from = sq, .to = target, .piece = Piece::Pawn, .isEnPassant = false});
					bool onStartRank = (color == Color::White)
							       ? (index >= static_cast<int>(Square::A2) && index <= static_cast<int>(Square::H2))
							       : (index >= static_cast<int>(Square::A7) && index <= static_cast<int>(Square::H7));

					if (onStartRank) {
						int doubleTargetIndex = (color == Color::White) ? index + 16 : index - 16;
						auto doubleTarget = static_cast<Square>(doubleTargetIndex);
						if (!board.pieceAt(doubleTarget)) {
							moves.push_back(Move{.from = sq, .to = doubleTarget, .piece = Piece::Pawn, .isEnPassant = false});
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
				moves.push_back(Move{.from = sq, .to = targetSquare, .piece = Piece::Pawn, .capturedPiece = Piece::Pawn, .isEnPassant = true});
			} else if (targetPiece) {
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
						moves.push_back(Move{.from = sq,
								     .to = targetSquare,
								     .piece = Piece::Pawn,
								     .capturedPiece = targetPiece->piece,
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
					});
				}
			} else {
				moves.push_back(Move{
				    .from = sq,
				    .to = attackedSquare,
				    .piece = Piece::Knight,
				});
			}

			attackTargets &= attackTargets - 1;
		}
		knights &= knights - 1;
	}
	return moves;
}
Bitboard rookAttacksFrom(const Board &board, Square sq) {
	Bitboard attacks = 0;
	int file = static_cast<int>(sq) % 8;
	int rank = static_cast<int>(sq) / 8;
	constexpr std::array<std::pair<int, int>, 4> directions = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

	for (auto &[fileOffset, rankOffset] : directions) {
		int newFile = file + fileOffset;
		int newRank = rank + rankOffset;
		while (newFile >= 0 && newFile <= 7 && newRank >= 0 && newRank <= 7) {
			Square newSquare = static_cast<Square>(newRank * 8 + newFile);
			attacks |= squareToBitboard(newSquare);
			if (board.pieceAt(newSquare))
				break;
			newFile += fileOffset;
			newRank += rankOffset;
		}
	}
	return attacks;
}

// TODO: Bishop & Queen
bool isSquareAttacked(const Board &board, Square sq, Color byColor) {
	if (sq == Square::None)
		return false; // not sure about what to do
	Bitboard enemyKnights = board.getPieces(byColor, Piece::Knight);
	if (knightAttackTable[static_cast<int>(sq)] & enemyKnights)
		return true;
	Bitboard enemyKing = board.getPieces(byColor, Piece::King);
	if (kingAttackTable[static_cast<int>(sq)] & enemyKing)
		return true;

	Bitboard enemyPawns = board.getPieces(byColor, Piece::Pawn);
	auto opposite = byColor == Color::White ? Color::Black : Color::White;
	if (pawnCaptureTable[static_cast<int>(opposite)][static_cast<int>(sq)] & enemyPawns)
		return true;
	
	Bitboard enemyRooks = board.getPieces(byColor, Piece::Rook);
	if (rookAttacksFrom(board, sq) & enemyRooks)
		return true;
	

	return false;
}

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
					});
				}
			} else {
				moves.push_back(Move{
				    .from = sq,
				    .to = attackedSquare,
				    .piece = Piece::King,
				});
			}

			attackTargets &= attackTargets - 1;
		}
		kings &= kings - 1;
	}
	auto castlingRights = board.getCastlingRights();
	Color enemyColor = (color == Color::White) ? Color::Black : Color::White;
	if (color == Color::White) {
		if ((castlingRights & CastlingRights::WhiteKingside) != CastlingRights::None) {
			if (!board.pieceAt(Square::F1) && !board.pieceAt(Square::G1) && !isSquareAttacked(board, Square::E1, enemyColor) &&
			    !isSquareAttacked(board, Square::F1, enemyColor) && !isSquareAttacked(board, Square::G1, enemyColor)) {
				moves.push_back(Move{/* ... king e1->g1, some way to flag
							"this is a castle" ... */
						     .from = Square::E1,
						     .to = Square::G1,
						     .piece = Piece::King,
						     .isCastling = true});
			}
		}
		if ((castlingRights & CastlingRights::WhiteQueenside) != CastlingRights::None) {
			if (!board.pieceAt(Square::B1) && !board.pieceAt(Square::C1) && !board.pieceAt(Square::D1) &&
			    !isSquareAttacked(board, Square::E1, enemyColor) && !isSquareAttacked(board, Square::D1, enemyColor) &&
			    !isSquareAttacked(board, Square::C1, enemyColor)) {
				moves.push_back(Move{.from = Square::E1, .to = Square::C1, .piece = Piece::King, .isCastling = true});
			}
		}
	} else {
		if ((castlingRights & CastlingRights::BlackKingside) != CastlingRights::None) {
			if (!board.pieceAt(Square::F8) && !board.pieceAt(Square::G8) && !isSquareAttacked(board, Square::E8, enemyColor) &&
			    !isSquareAttacked(board, Square::F8, enemyColor) && !isSquareAttacked(board, Square::G8, enemyColor)) {
				moves.push_back(Move{/* ... king e8->g8, some way to flag
						"this is a castle" ... */
						     .from = Square::E8,
						     .to = Square::G8,
						     .piece = Piece::King,
						     .isCastling = true});
			}
		}

		if ((castlingRights & CastlingRights::BlackQueenside) != CastlingRights::None) {
			if (!board.pieceAt(Square::B8) && !board.pieceAt(Square::C8) && !board.pieceAt(Square::D8) &&
			    !isSquareAttacked(board, Square::E8, enemyColor) && !isSquareAttacked(board, Square::D8, enemyColor) &&
			    !isSquareAttacked(board, Square::C8, enemyColor)) {
				moves.push_back(Move{.from = Square::E8, .to = Square::C8, .piece = Piece::King, .isCastling = true});
			}
		}
	}

	return moves;
}

std::vector<Move> generateRookMoves(const Board &board, Color color) {
	std::vector<Move> moves;
	Bitboard rooks = board.getPieces(color, Piece::Rook);

	while (rooks) {
		int index = std::countr_zero(rooks);
		auto from = static_cast<Square>(index);
		Bitboard reacheable = rookAttacksFrom(board, from);

		while (reacheable) {
			int targetIndex = std::countr_zero(reacheable);
			Square target = static_cast<Square>(targetIndex);
			auto atSquare = board.pieceAt(target);
			if (!atSquare) {
				moves.push_back(Move{
				    .from = from,
				    .to = target,
				    .piece = Piece::Rook
				});
			} else if (atSquare->color != color) {
				moves.push_back(Move{
				    .from = from,
				    .to = target,
				    .piece = Piece::Rook,
				    .capturedPiece = atSquare->piece
				});
			}
			reacheable &= reacheable - 1;
		}

		rooks &= rooks - 1;
	}
	return moves;
}
