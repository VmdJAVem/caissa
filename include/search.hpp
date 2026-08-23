#pragma once

#include "moves.hpp"
#include "board.hpp"

std::optional<Move> negamax(Board &board, int depth);
