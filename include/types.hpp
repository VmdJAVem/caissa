#pragma once
#include <array>
#include <cstdint>

using Bitboard = std::uint64_t;

enum class Color { White, Black };

constexpr std::array<Color, 2> allColors = {Color::White, Color::Black};

enum class Piece {
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King,

	None
};
constexpr std::array<Piece, 6> allPieces = {Piece::Pawn,   Piece::Knight,
					    Piece::Bishop, Piece::Rook,
					    Piece::Queen,  Piece::King};

enum class Square : std::uint8_t {
	A1,
	B1,
	C1,
	D1,
	E1,
	F1,
	G1,
	H1,
	A2,
	B2,
	C2,
	D2,
	E2,
	F2,
	G2,
	H2,
	A3,
	B3,
	C3,
	D3,
	E3,
	F3,
	G3,
	H3,
	A4,
	B4,
	C4,
	D4,
	E4,
	F4,
	G4,
	H4,
	A5,
	B5,
	C5,
	D5,
	E5,
	F5,
	G5,
	H5,
	A6,
	B6,
	C6,
	D6,
	E6,
	F6,
	G6,
	H6,
	A7,
	B7,
	C7,
	D7,
	E7,
	F7,
	G7,
	H7,
	A8,
	B8,
	C8,
	D8,
	E8,
	F8,
	G8,
	H8,

	None
};

struct PieceOnSquare {
	Color color;
	Piece piece = Piece::None;
};

enum class CastlingRights : std::uint8_t {
	None = 0,
	WhiteKingside = 1 << 0, // 0b0001
	WhiteQueenside = 1 << 1, // 0b0010
	BlackKingside = 1 << 2, // 0b0100n
	BlackQueenside = 1 << 3, // 0b1000
	All = WhiteKingside | WhiteQueenside | BlackKingside | BlackQueenside
};

constexpr CastlingRights operator|(CastlingRights a, CastlingRights b)
{
	return static_cast<CastlingRights>(static_cast<uint8_t>(a) |
					   static_cast<uint8_t>(b));
}
constexpr CastlingRights operator|=(CastlingRights &a, CastlingRights b)
{
	a = a | b;
	return a;
}
constexpr CastlingRights operator&(CastlingRights a, CastlingRights b)
{
	return static_cast<CastlingRights>(static_cast<uint8_t>(a) &
					   static_cast<uint8_t>(b));
}
constexpr CastlingRights operator~(CastlingRights a)
{
	return static_cast<CastlingRights>(
	    ~static_cast<uint8_t>(a) &
	    static_cast<uint8_t>(CastlingRights::All));
}
constexpr CastlingRights &operator&=(CastlingRights &a, CastlingRights b)
{
	a = a & b;
	return a;
}

struct UndoInfo {
	CastlingRights previousCastlingRights;
	Square previousEnPassantTarget;
	int previousHalfMoveClock;
};
enum class GameResult { InProgress, Checkmate, Stalemate };
