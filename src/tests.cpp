#include "tests.hpp"
#include "board.hpp"
#include "evaluation.hpp"
#include "movegen.hpp"
#include "search.hpp"
#include "types.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void testMove(const std::string &startFen, Move move,
	      const std::string &expectedFen)
{
	auto result = Board::fromFen(startFen);
	assert(result.has_value());
	Board board = result.value();
	board.makeMove(move);
	std::string actualFen = board.toFen();
	if (actualFen != expectedFen) {
		std::cout << "FAIL: " << startFen << "\n";
		std::cout << "  expected: " << expectedFen << "\n";
		std::cout << "  actual:   " << actualFen << "\n";
	} else {
		std::cout << "PASS: " << startFen << "\n";
	}
}

void testRoundTrip(const std::vector<RoundTripCase> &cases)
{
	int passed = 0;
	for (const auto &c : cases) {
		auto result = Board::fromFen(c.fen);
		if (!result.has_value()) {
			std::cout << "FAIL (bad FEN): " << c.fen << " -- "
				  << result.error() << "\n";
			continue;
		}
		Board board = result.value();
		std::string before = board.toFen();

		UndoInfo undo = board.makeMove(c.move);
		board.unmakeMove(c.move, undo);

		std::string after = board.toFen();

		if (before == after) {
			std::cout << "PASS: " << c.fen << "\n";
			++passed;
		} else {
			std::cout << "FAIL: " << c.fen << "\n";
			std::cout << "  before: " << before << "\n";
			std::cout << "  after:  " << after << "\n";
		}
	}
	std::cout << passed << "/" << cases.size() << " passed\n";
}
void testLegalMoves(const std::string &fen,
		    const std::vector<std::pair<Square, Square>> &expected)
{
	auto result = Board::fromFen(fen);
	if (!result.has_value()) {
		std::cout << "FAIL (bad FEN): " << result.error() << "\n";
		return;
	}
	Board board = result.value();
	auto legalMoves = generateAllLegalMoves(board);

	std::cout << "FEN: " << fen << "\n";
	std::cout << "Legal moves found: " << legalMoves.size() << " (expected "
		  << expected.size() << ")\n";
	for (auto &m : legalMoves) {
		std::cout << "  " << squareName(m.from) << " -> "
			  << squareName(m.to) << "\n";
	}

	if (legalMoves.size() != expected.size()) {
		std::cout << "FAIL: count mismatch\n";
		return;
	}
	for (auto &[from, to] : expected) {
		bool found = false;
		for (auto &m : legalMoves) {
			if (m.from == from && m.to == to) {
				found = true;
				break;
			}
		}
		if (!found) {
			std::cout << "FAIL: expected move " << squareName(from)
				  << " -> " << squareName(to) << " not found\n";
			return;
		}
	}
	std::cout << "PASS\n";
}
void testEvaluate(const std::string &fen, int expected)
{
	auto result = Board::fromFen(fen);
	if (!result) {
		std::cout << "FAIL (bad FEN): " << result.error() << "\n";
		return;
	}
	Board board = result.value();
	int actual = evaluate(board);
	std::cout << "FEN: " << fen << "\n";
	std::cout << "  expected: " << expected << ", actual: " << actual;
	std::cout << (actual == expected ? "  PASS" : "  FAIL") << "\n";
}
void testNegamax(const std::string &fen, int depth, Square expectedFrom,
		 Square expectedTo)
{
	auto result = Board::fromFen(fen);
	if (!result.has_value()) {
		std::cout << "FAIL (bad FEN): " << result.error() << "\n";
		return;
	}
	Board board = result.value();
	auto bestMove = negamax(board, depth);

	if (!bestMove.has_value()) {
		std::cout << "FAIL: no move returned\n";
		return;
	}

	std::cout << "FEN: " << fen << "\n";
	std::cout << "  chosen: " << squareName(bestMove->from) << " -> "
		  << squareName(bestMove->to) << "\n";

	if (bestMove->from == expectedFrom && bestMove->to == expectedTo) {
		std::cout << "  PASS\n";
	} else {
		std::cout << "  FAIL: expected " << squareName(expectedFrom)
			  << " -> " << squareName(expectedTo) << "\n";
	}
}
