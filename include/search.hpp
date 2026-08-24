#pragma once

#include "board.hpp"
#include "moves.hpp"

std::optional<Move> negamax(Board &board, int depth);
