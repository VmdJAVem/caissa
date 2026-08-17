#pragma once
#include "board.hpp"
#include <optional>


struct Move {
	Square from;
	Square to;
	Piece piece;
	Piece capturedPiece;
	Piece promotionPiece;
	bool isEnPassant;
};
