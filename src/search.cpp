#include "board.hpp"
#include "evaluation.hpp"
#include "movegen.hpp"
#include "moves.hpp"
#include <stop_token>

constexpr int checkmateScore = -32000;

int recursiveNegamax(Board &board, int depth, int alpha, int beta, std::stop_token shouldStop)
{
	if (shouldStop.stop_requested()) {
		return 0;
	}
	auto moves = generateAllLegalMoves(board);

	if (moves.empty()) {
		auto color = board.getSideToMove();
		auto oppositeColor =
		    color == Color::White ? Color::Black : Color::White;
		Square king = findKing(board, color);

		if (isSquareAttacked(board, king, oppositeColor)) {
			return checkmateScore;
		} else
			return 0;
	} else if (depth == 0)
		return evaluate(board);

	for (auto &move : moves) {
		UndoInfo undo = board.makeMove(move);
		int score = -recursiveNegamax(board, depth - 1, -beta, -alpha, shouldStop);
		board.unmakeMove(move, undo);
		
		if (score >= beta) 
			return beta;

		
		
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

std::optional<Move> negamax(Board &board, int depth,
			    std::stop_token shouldStop)
{
	auto moves = generateAllLegalMoves(board);

	if (depth == 0 || moves.empty() || shouldStop.stop_requested()) {
		return std::nullopt;
	}

	int bestScore = checkmateScore;
	Move bestMove;

	for (auto &move : moves) {
		auto undo = board.makeMove(move);
		int score = -recursiveNegamax(board, depth - 1, checkmateScore, -checkmateScore, shouldStop);
		board.unmakeMove(move, undo);

		if (shouldStop.stop_requested())
			return std::nullopt;

		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
		}
	}

	return bestMove;
}
std::optional<Move> iterativeNegaMax(Board &board, int maxDepth,
				     std::stop_token shouldStop)
{
	std::optional<Move> bestMoveSoFar = std::nullopt;

	for (int i = 1; i <= maxDepth; i++) {
		if (shouldStop.stop_requested())
			break;
		
		auto result = negamax(board, i, shouldStop);
		if (result.has_value()) {
			bestMoveSoFar = result;
		} else {
			break;
		}
	}

	return bestMoveSoFar;
}
