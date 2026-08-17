#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <optional>

using Bitboard = std::uint64_t;

enum class Color {
	White,
	Black
};

constexpr std::array<Color, 2> allColors = {
	Color::White, Color::Black
};

enum class Piece {
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};
constexpr std::array<Piece, 6> allPieces = {
	Piece::Pawn, Piece::Knight, Piece::Bishop,
	Piece::Rook, Piece::Queen, Piece::King
};

enum class Square : std::uint8_t {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,

	None
};

struct PieceOnSquare {
	Color color;
	Piece piece;
};

class Board {
public:
	Board();
	std::string toString() const;
	Bitboard getPieces(Color c, Piece p) const;
	void setPieces(Color c, Piece p, Bitboard value);
	Color sideToMove() const;
	std::optional<PieceOnSquare> pieceAt(Square sq) const;
private:
	std::array<std::array<Bitboard, 6>, 2> m_bitboards;
	Color m_sideToMove = Color::White;
};

constexpr Bitboard squareToBitboard(Square sq) {
	return 1ULL << static_cast<int>(sq);
}
constexpr Square bitboardToSquare(Bitboard bb) {
	return static_cast<Square>(std::countr_zero(bb));
}
