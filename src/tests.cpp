#include <cassert>
#include <iostream>
#include <vector>
#include "types.hpp"
#include "board.hpp"
#include "tests.hpp"

void testMove(const std::string& startFen, Move move, const std::string& expectedFen) {
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

void testRoundTrip(const std::vector<RoundTripCase>& cases) {
	int passed = 0;
	for (const auto& c : cases) {
		auto result = Board::fromFen(c.fen);
		if (!result.has_value()) {
			std::cout << "FAIL (bad FEN): " << c.fen << " -- " << result.error() << "\n";
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
