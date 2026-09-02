#include "board.hpp"
#include <cassert>

UndoInfo Board::makeMove(Move move)
{
	UndoInfo undo = {.previousCastlingRights = m_castlingRights,
			 .previousEnPassantTarget = m_enPassantTarget,
			 .previousHalfMoveClock = m_halfMoveClock};
	Color opposite =
	    m_sideToMove == Color::White ? Color::Black : Color::White;
	m_enPassantTarget = Square::None;

	if (move.capturedPiece != Piece::None && !move.isEnPassant) {
		removePiece(opposite, move.capturedPiece, move.to);
		m_halfMoveClock = 0;
	} else if (move.isEnPassant) {
		int capturedPawnIndex = (m_sideToMove == Color::White) ?
					    static_cast<int>(move.to) - 8 :
					    static_cast<int>(move.to) + 8;
		removePiece(opposite, Piece::Pawn,
			    static_cast<Square>(capturedPawnIndex));
		m_halfMoveClock = 0;
	} else if (move.piece == Piece::Pawn) {
		m_halfMoveClock = 0;
		int toIndex = static_cast<int>(move.to);
		int fromIndex = static_cast<int>(move.from);
		int diff = toIndex - fromIndex;
		if (diff == 16 || diff == -16) {
			int enPassantIndex = (m_sideToMove == Color::White) ?
						 toIndex - 8 :
						 toIndex + 8;
			m_enPassantTarget = static_cast<Square>(enPassantIndex);
		}
	} else {
		m_halfMoveClock++;
	}

	if (move.isCastling) {
		switch (move.to) {
		case Square::G1:
			placePiece(m_sideToMove, Piece::Rook, Square::F1);
			removePiece(m_sideToMove, Piece::Rook, Square::H1);
			break;
		case Square::C1:
			placePiece(m_sideToMove, Piece::Rook, Square::D1);
			removePiece(m_sideToMove, Piece::Rook, Square::A1);
			break;
		case Square::G8:
			placePiece(m_sideToMove, Piece::Rook, Square::F8);
			removePiece(m_sideToMove, Piece::Rook, Square::H8);
			break;
		case Square::C8:
			placePiece(m_sideToMove, Piece::Rook, Square::D8);
			removePiece(m_sideToMove, Piece::Rook, Square::A8);
			break;
		default:
			assert(false);
			break;
		}
	}

	if (move.piece == Piece::King) {
		if (m_sideToMove == Color::White)
			m_castlingRights &= ~(CastlingRights::WhiteKingside |
					      CastlingRights::WhiteQueenside);
		else
			m_castlingRights &= ~(CastlingRights::BlackKingside |
					      CastlingRights::BlackQueenside);
	} else if (move.piece == Piece::Rook) {
		switch (move.from) {
		case Square::A1:
			m_castlingRights &= ~CastlingRights::WhiteQueenside;
			break;
		case Square::H1:
			m_castlingRights &= ~CastlingRights::WhiteKingside;
			break;
		case Square::A8:
			m_castlingRights &= ~CastlingRights::BlackQueenside;
			break;
		case Square::H8:
			m_castlingRights &= ~CastlingRights::BlackKingside;
			break;
		default:
			break;
		}
	}

	if (move.capturedPiece == Piece::Rook) {
		switch (move.to) {
		case Square::A1:
			m_castlingRights &= ~CastlingRights::WhiteQueenside;
			break;
		case Square::H1:
			m_castlingRights &= ~CastlingRights::WhiteKingside;
			break;
		case Square::A8:
			m_castlingRights &= ~CastlingRights::BlackQueenside;
			break;
		case Square::H8:
			m_castlingRights &= ~CastlingRights::BlackKingside;
			break;
		default:
			break;
		}
	}

	if (move.promotionPiece != Piece::None) {
		placePiece(m_sideToMove, move.promotionPiece, move.to);
	} else
		placePiece(m_sideToMove, move.piece, move.to);
	removePiece(m_sideToMove, move.piece, move.from);

	if (m_sideToMove == Color::Black)
		m_fullMoveNumber++;

	m_sideToMove = opposite;
	return undo;
}
void Board::unmakeMove(Move move, UndoInfo undo)
{
	Color moverColor =
		m_sideToMove == Color::White ? Color::Black : Color::White;
	
	if (move.promotionPiece != Piece::None)
		removePiece(moverColor, move.promotionPiece, move.to);
	else
		removePiece(moverColor, move.piece, move.to);
	placePiece(moverColor, move.piece, move.from);

	if (move.capturedPiece != Piece::None && !move.isEnPassant) {
		placePiece(m_sideToMove, move.capturedPiece, move.to);
	} else if (move.isEnPassant) {
		int capturedPawnIndex = (moverColor == Color::White) ?
					static_cast<int>(move.to) - 8 :
					static_cast<int>(move.to) + 8;
		placePiece(m_sideToMove, Piece::Pawn,
			   static_cast<Square>(capturedPawnIndex));
	}

	if (move.isCastling) {
		switch (move.to) {
		case Square::G1:
			removePiece(moverColor, Piece::Rook, Square::F1);
			placePiece(moverColor, Piece::Rook, Square::H1);
			break;
		case Square::C1:
			removePiece(moverColor, Piece::Rook, Square::D1);
			placePiece(moverColor, Piece::Rook, Square::A1);
			break;
		case Square::G8:
			removePiece(moverColor, Piece::Rook, Square::F8);
			placePiece(moverColor, Piece::Rook, Square::H8);
			break;
		case Square::C8:
			placePiece(moverColor, Piece::Rook, Square::A8);
			removePiece(moverColor, Piece::Rook, Square::D8);
			break;
		default:
			assert(false);
			break;
		}
	}

	if (moverColor == Color::Black)
		--m_fullMoveNumber;

	m_enPassantTarget = undo.previousEnPassantTarget;
	m_halfMoveClock = undo.previousHalfMoveClock;
	m_castlingRights = undo.previousCastlingRights;
	m_sideToMove = moverColor;
}
