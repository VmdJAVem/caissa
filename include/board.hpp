#pragma once
#include <string>
#include <expected>
#include "moves.hpp"
#include <optional>
#include "types.hpp"

class Board {
public:
	Board();
	std::string toString() const;
	Bitboard getPieces(Color c, Piece p) const;
	void setPieces(Color c, Piece p, Bitboard value);
	Color sideToMove() const;
	void placePiece(Color c, Piece p, Square sq);
	void removePiece(Color c, Piece p, Square sq);
	CastlingRights getCastlingRights() const;
	std::optional<PieceOnSquare> pieceAt(Square sq) const;
	static Board empty();
	static std::expected<Board,std::string> fromFen(const std::string& fen);
	std::string toFen() const;
	Square getEnPassantTarget() const;
	UndoInfo makeMove(Move move);
private:
	std::array<std::array<Bitboard, 6>, 2> m_bitboards;
	Color m_sideToMove = Color::White;
	CastlingRights m_castlingRights = CastlingRights::All;
	Square m_enPassantTarget = Square::None;
	int m_halfMoveClock = 0;
	int m_fullMoveNumber = 1;
};

constexpr Bitboard squareToBitboard(Square sq) {
	return 1ULL << static_cast<int>(sq);
}
constexpr Square bitboardToSquare(Bitboard bb) {
	return static_cast<Square>(std::countr_zero(bb));
}
constexpr std::string squareName(Square sq) {
	if (sq == Square::None) {
		return "-";
	}
	auto index = static_cast<int>(sq);
	int file = index % 8;
	int rank = index / 8;

	char fileChar = static_cast<char>('a' + file);
	char rankChar = static_cast<char>('0' + rank + 1);

	return std::string(1, fileChar) + std::string(1, rankChar);
}
