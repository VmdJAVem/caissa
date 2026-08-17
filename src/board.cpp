#include "board.hpp"
#include <optional>
#include <cctype>

void Board::setPieces(Color c, Piece p, Bitboard value) {
	m_bitboards[static_cast<size_t>(c)][static_cast<size_t>(p)] = value;
}
Bitboard Board::getPieces(Color c, Piece p) const {
        return m_bitboards[static_cast<size_t>(c)][static_cast<size_t>(p)];
}
Color Board::sideToMove() const {
	return m_sideToMove;
}
Board::Board() {
	for (int i = static_cast<int>(Square::A2); i <= static_cast<int>(Square::H2); ++i) {
		Square sq = static_cast<Square>(i);
		setPieces(Color::White, Piece::Pawn, getPieces(Color::White, Piece::Pawn) | squareToBitboard(sq));
	}
	for (int i = static_cast<int>(Square::A7); i <= static_cast<int>(Square::H7); ++i) {
		Square sq = static_cast<Square>(i);
		setPieces(Color::Black, Piece::Pawn, getPieces(Color::Black, Piece::Pawn) | squareToBitboard(sq));
	}

	setPieces(Color::White, Piece::Knight, squareToBitboard(Square::B1) | squareToBitboard(Square::G1));
	setPieces(Color::Black, Piece::Knight, squareToBitboard(Square::B8) | squareToBitboard(Square::G8));

	setPieces(Color::White, Piece::Bishop, squareToBitboard(Square::C1) | squareToBitboard(Square::F1));
	setPieces(Color::Black, Piece::Bishop, squareToBitboard(Square::C8) | squareToBitboard(Square::F8));
	
	setPieces(Color::White, Piece::Rook, squareToBitboard(Square::A1) | squareToBitboard(Square::H1));
	setPieces(Color::Black, Piece::Rook, squareToBitboard(Square::A8) | squareToBitboard(Square::H8));
	
	setPieces(Color::White, Piece::Queen, squareToBitboard(Square::D1));
	setPieces(Color::Black, Piece::Queen, squareToBitboard(Square::D8));

	setPieces(Color::White, Piece::King, squareToBitboard(Square::E1));
	setPieces(Color::Black, Piece::King, squareToBitboard(Square::E8));
}

std::optional<PieceOnSquare> Board::pieceAt(Square sq) const {
	if (sq == Square::None) return std::nullopt;
	Bitboard mask = squareToBitboard(sq);

	for (Color c : allColors) {
		for (Piece p : allPieces) {
			if (getPieces(c, p) & mask) {
				return PieceOnSquare{c, p};
			}
		}
	}
	return std::nullopt;  // empty square	
}

std::string Board::toString() const {
	std::string out;
	for (int rank = 7; rank >= 0; --rank) {
		for (int file = 0; file < 8; ++file) {
			int square = rank * 8 + file;
			auto p = pieceAt(static_cast<Square>(square));
			char cp = '.';
			if (p) {
				switch (p.value().piece) {
				case Piece::Pawn:
					cp = 'p';					
					break;
				case Piece::Knight:
					cp = 'n';
					break;
				case Piece::Bishop:
					cp = 'b';
					break;
				case Piece::Rook:
					cp = 'r';
					break;
				case Piece::Queen:
					cp = 'q';
					break;
				case Piece::King:
					cp = 'k';					
					break;
				}
				if (p.value().color == Color::White) {
					cp = static_cast<char>(toupper(cp));
				}
				
			}
			out += cp;

		}
		out += '\n';
	}
	return out;
}
