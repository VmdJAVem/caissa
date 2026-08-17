#pragma once
#include "board.hpp"
#include <optional>


struct Move {
	Square from;
	Square to;
	Piece piece;
	std::optional<Piece> capturedPiece;
	std::optional<Piece> promotionPiece;
};
