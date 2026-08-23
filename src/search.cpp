#include "moves.hpp"
#include "board.hpp"
#include "movegen.hpp"
#include "evaluation.hpp"
#include <climits>

constexpr int checkmateScore = -32000;

int recursiveNegamax(Board &board, int depth) {
	auto moves = generateAllLegalMoves(board);

	if (moves.empty()) {
		auto color = board.getSideToMove();
		auto oppositeColor = color == Color::White ? Color::Black : Color::White;
		Square king = findKing(board, color);
		
		if (isSquareAttacked(board, king, oppositeColor)) {
			return checkmateScore;
		} else return 0;
	} else if (depth == 0) 
		return evaluate(board);
	

	int bestScore = checkmateScore;

	for (auto &move : moves) {
		UndoInfo undo = board.makeMove(move);
		int score = -recursiveNegamax(board, depth - 1);
		board.unmakeMove(move, undo);
		if (score > bestScore) bestScore = score;
	}
	return bestScore;
}

std::optional<Move> negamax(Board &board, int depth) {
	auto moves = generateAllLegalMoves(board);

	if (depth == 0 || moves.empty()) {
		return std::nullopt;
	}

	int bestScore = checkmateScore;
	Move bestMove;

	for (auto &move : moves) {
		auto undo = board.makeMove(move);
		int score = -recursiveNegamax(board, depth - 1);
		board.unmakeMove(move, undo);
		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
		}
	}
	
	return bestMove;
}
