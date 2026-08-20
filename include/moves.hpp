#pragma once
#include "board.hpp"


struct Move {
	Square from;
	Square to;
	Piece piece;
	Piece capturedPiece = Piece::None;
	Piece promotionPiece = Piece::None;
	bool isEnPassant = false;
	bool isCastling = false;
};
