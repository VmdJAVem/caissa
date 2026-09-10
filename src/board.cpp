#include "board.hpp"
#include "movegen.hpp"
#include "types.hpp"
#include <cassert>
#include <cctype>
#include <expected>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

void Board::setPieces(Color c, Piece p, Bitboard value)
{
	Bitboard oldValue = m_bitboards[static_cast<size_t>(c)][static_cast<size_t>(p)];
	Bitboard changed = oldValue ^ value;
	while (changed) {
		int index = std::countr_zero(changed);
		bool nowSet = (value >> index) & 1ULL;
		if (nowSet) {
			m_mailbox[index] = PieceOnSquare{c, p};
		} else {
			m_mailbox[index] = PieceOnSquare{c, Piece::None};
		}
		changed &= changed - 1;
	}
	m_bitboards[static_cast<size_t>(c)][static_cast<size_t>(p)] = value;
}
Bitboard Board::getPieces(Color c, Piece p) const
{
	return m_bitboards[static_cast<size_t>(c)][static_cast<size_t>(p)];
}
Color Board::getSideToMove() const
{
	return m_sideToMove;
}
Square Board::getEnPassantTarget() const
{
	return m_enPassantTarget;
}
CastlingRights Board::getCastlingRights() const
{
	return m_castlingRights;
}
Board::Board()
{
	for (int i = static_cast<int>(Square::A2);
	     i <= static_cast<int>(Square::H2); ++i) {
		Square sq = static_cast<Square>(i);
		setPieces(Color::White, Piece::Pawn,
			  getPieces(Color::White, Piece::Pawn) |
			      squareToBitboard(sq));
	}
	for (int i = static_cast<int>(Square::A7);
	     i <= static_cast<int>(Square::H7); ++i) {
		Square sq = static_cast<Square>(i);
		setPieces(Color::Black, Piece::Pawn,
			  getPieces(Color::Black, Piece::Pawn) |
			      squareToBitboard(sq));
	}

	setPieces(Color::White, Piece::Knight,
		  squareToBitboard(Square::B1) | squareToBitboard(Square::G1));
	setPieces(Color::Black, Piece::Knight,
		  squareToBitboard(Square::B8) | squareToBitboard(Square::G8));

	setPieces(Color::White, Piece::Bishop,
		  squareToBitboard(Square::C1) | squareToBitboard(Square::F1));
	setPieces(Color::Black, Piece::Bishop,
		  squareToBitboard(Square::C8) | squareToBitboard(Square::F8));

	setPieces(Color::White, Piece::Rook,
		  squareToBitboard(Square::A1) | squareToBitboard(Square::H1));
	setPieces(Color::Black, Piece::Rook,
		  squareToBitboard(Square::A8) | squareToBitboard(Square::H8));

	setPieces(Color::White, Piece::Queen, squareToBitboard(Square::D1));
	setPieces(Color::Black, Piece::Queen, squareToBitboard(Square::D8));

	setPieces(Color::White, Piece::King, squareToBitboard(Square::E1));
	setPieces(Color::Black, Piece::King, squareToBitboard(Square::E8));
}

PieceOnSquare Board::pieceAt(Square sq) const
{
	if (sq == Square::None || m_mailbox[static_cast<int>(sq)].piece == Piece::None)
		return PieceOnSquare{.color = Color::White, .piece = Piece::None};
	else
		return m_mailbox[static_cast<int>(sq)];
}

std::string Board::toString() const
{
	std::string out;
	for (int rank = 7; rank >= 0; --rank) {
		for (int file = 0; file < 8; ++file) {
			int square = rank * 8 + file;
			auto p = pieceAt(static_cast<Square>(square));
			char cp = '.';
			if (p) {
				switch (p.piece) {
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
				default:
					break;
				}
				if (p.color == Color::White) {
					cp = static_cast<char>(toupper(cp));
				}
			}
			out += cp;
		}
		out += '\n';
	}
	return out;
}

Board Board::empty()
{
	Board b;
	for (Color c : allColors) {
		for (Piece p : allPieces) {
			b.setPieces(c, p, 0);
		}
	}
	return b;
}

void Board::placePiece(Color c, Piece p, Square sq)
{
	setPieces(c, p, getPieces(c, p) | squareToBitboard(sq));
}
void Board::removePiece(Color c, Piece p, Square sq)
{
	setPieces(c, p, getPieces(c, p) & ~squareToBitboard(sq));
}

std::expected<Board, std::string> Board::fromFen(const std::string &fen)
{
	std::istringstream iss(fen);

	std::string placement, sideToMove, castling, enPassant;
	int halfMove, fullMove;

	iss >> placement;
	if (!iss)
		return std::unexpected("placement field is malformed");
	iss >> sideToMove;
	if (!iss)
		return std::unexpected("sideToMove is malformed");
	iss >> castling;
	if (!iss)
		return std::unexpected("Castling is malformed");
	iss >> enPassant;
	if (!iss)
		return std::unexpected("enPassant is malformed");
	iss >> halfMove;
	if (!iss)
		return std::unexpected("halfMove is malformed");
	iss >> fullMove;
	if (!iss)
		return std::unexpected("fullMove is malformed");

	Board board = Board::empty();

	board.m_halfMoveClock = halfMove;
	board.m_fullMoveNumber = fullMove;

	int rank = 7;
	int file = 0;

	for (char c : placement) {
		if (c == '/') {
			--rank;
			file = 0;
		} else if (std::isdigit(c)) {
			int offset = c - '0';

			file += offset;
			if (file > 8) {
				return std::unexpected("placement is malformed "
						       "(A digit is too big)");
			}
		} else {
			Color color = isupper(c) ? Color::White : Color::Black;
			c = tolower(c); // make handling easier since we alredy
			// how the color

			Piece p;
			switch (c) {
			case 'p':
				p = Piece::Pawn;
				break;
			case 'n':
				p = Piece::Knight;
				break;
			case 'b':
				p = Piece::Bishop;
				break;
			case 'r':
				p = Piece::Rook;
				break;
			case 'q':
				p = Piece::Queen;
				break;
			case 'k':
				p = Piece::King;
				break;
			default:
				return std::unexpected(
				    "placement contains invalid piece letter");
			}
			Square sq = static_cast<Square>(rank * 8 + file);
			board.placePiece(color, p, sq);
			file++;
		}
	}
	if (sideToMove == "w") {
		board.m_sideToMove = Color::White;
	} else if (sideToMove == "b") {
		board.m_sideToMove = Color::Black;
	} else {
		return std::unexpected("sideToMove is invalid");
	}
	board.m_castlingRights = CastlingRights::None;
	for (char r : castling) {
		switch (r) {
		case 'K':
			board.m_castlingRights |= CastlingRights::WhiteKingside;
			break;
		case 'k':
			board.m_castlingRights |= CastlingRights::BlackKingside;
			break;
		case 'Q':
			board.m_castlingRights |=
			    CastlingRights::WhiteQueenside;
			break;
		case 'q':
			board.m_castlingRights |=
			    CastlingRights::BlackQueenside;
			break;
		case '-':
			break;
		default:
			return std::unexpected("castling is malformed");
		}
	}
	if (enPassant == "-")
		board.m_enPassantTarget = Square::None;
	else if (enPassant.length() == 2) {
		char fileChar = enPassant[0];
		char rankChar = enPassant[1];
		int fileIndex;
		int rankIndex = (rankChar - '0') - 1;

		switch (fileChar) {
		case 'a':
			fileIndex = 0;
			break;
		case 'b':
			fileIndex = 1;
			break;
		case 'c':
			fileIndex = 2;
			break;
		case 'd':
			fileIndex = 3;
			break;
		case 'e':
			fileIndex = 4;
			break;
		case 'f':
			fileIndex = 5;
			break;
		case 'g':
			fileIndex = 6;
			break;
		case 'h':
			fileIndex = 7;
			break;
		default:
			return std::unexpected(
			    "enPassant has invalid file letter");
		}
		Square enPassantSquare =
		    static_cast<Square>(rankIndex * 8 + fileIndex);
		board.m_enPassantTarget = enPassantSquare;
	} else {
		return std::unexpected("enPassant is malformed");
	}
	return board;
}
std::string Board::toFen() const
{
	int rank = 7;
	int file = 0;
	std::string result;
	while (rank >= 0) {
		int emptyCount = 0;
		while (file <= 7) {
			auto sq = static_cast<Square>(rank * 8 + file);

			auto sqab = pieceAt(sq);

			if (sqab) {
				if (emptyCount > 0) {
					result += std::to_string(emptyCount);
					emptyCount = 0;
				}
				char p;
				switch (sqab.piece) {
				case Piece::Pawn:
					p = 'p';
					break;
				case Piece::Knight:
					p = 'n';
					break;
				case Piece::Bishop:
					p = 'b';
					break;
				case Piece::Rook:
					p = 'r';
					break;
				case Piece::Queen:
					p = 'q';
					break;
				case Piece::King:
					p = 'k';
					break;
				default:
					break;
				}

				if (sqab.color == Color::White)
					p = static_cast<char>(toupper(p));
				result += p;
			} else {
				++emptyCount;
			}
			++file;
		}
		if (emptyCount > 0)
			result += std::to_string(emptyCount);
		if (rank != 0)
			result += '/';

		file = 0;
		--rank;
	}

	result += ' ';
	result += (m_sideToMove == Color::White) ? 'w' : 'b';

	result += ' ';

	bool anyRights = false;
	if ((m_castlingRights & CastlingRights::WhiteKingside) !=
	    CastlingRights::None) {
		result += 'K';
		anyRights = true;
	}
	if ((m_castlingRights & CastlingRights::WhiteQueenside) !=
	    CastlingRights::None) {
		result += 'Q';
		anyRights = true;
	}
	if ((m_castlingRights & CastlingRights::BlackKingside) !=
	    CastlingRights::None) {
		result += 'k';
		anyRights = true;
	}
	if ((m_castlingRights & CastlingRights::BlackQueenside) !=
	    CastlingRights::None) {
		result += 'q';
		anyRights = true;
	}
	if (!anyRights)
		result += '-';

	result += ' ';
	result += squareName(m_enPassantTarget);

	result += ' ' + std::to_string(m_halfMoveClock);
	result += ' ' + std::to_string(m_fullMoveNumber);

	return result;
}
