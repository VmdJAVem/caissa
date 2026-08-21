#include <iostream>
#include <string>
#include <vector>
#include "types.hpp"
#include "moves.hpp"
#include "tests.hpp"

int main() {
	std::vector<RoundTripCase> cases = {
		// 1. quiet move
		{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
		 Move{.from = Square::G1, .to = Square::F3, .piece = Piece::Knight}},

		// 2. capture
		{"rnbqkbnr/pppp1ppp/8/4p3/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 2",
		 Move{.from = Square::D4, .to = Square::E5, .piece = Piece::Pawn, .capturedPiece = Piece::Pawn}},

		// 3. en passant
		{"rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 2",
		 Move{.from = Square::D4, .to = Square::E3, .piece = Piece::Pawn, .capturedPiece = Piece::Pawn, .isEnPassant = true}},

		// 4. castling
		{"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",
		 Move{.from = Square::E1, .to = Square::G1, .piece = Piece::King, .isCastling = true}},

		// 5. promotion
		{"8/P7/8/8/4k3/8/8/4K3 w - - 0 1",
		 Move{.from = Square::A7, .to = Square::A8, .piece = Piece::Pawn, .promotionPiece = Piece::Queen}},

		// 6. rook capture affecting rights
		{"r3k3/8/8/8/8/8/8/R3K2R w KQq - 0 1",
		 Move{.from = Square::A1, .to = Square::A8, .piece = Piece::Rook, .capturedPiece = Piece::Rook}},
	};

	testRoundTrip(cases);
	return 0;
}
