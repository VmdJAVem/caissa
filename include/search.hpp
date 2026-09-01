#pragma once

#include "board.hpp"
#include "moves.hpp"
#include <atomic>
#include <stop_token>

std::optional<Move> negamax(Board &board, int depth,
			    std::stop_token shouldStop);
std::optional<Move> iterativeNegaMax(Board &board, int maxDepth,
				     std::stop_token shouldStop);
