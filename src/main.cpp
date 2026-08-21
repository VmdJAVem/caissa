#include <iostream>
#include <random>

#include "board.hpp"
#include "movegen.hpp"

int main(void) {

	std::string line;
	std::getline(std::cin, line);

	auto eb = Board::fromFen(line);
	Board b;

	if (eb.has_value()) {
		std::cout << "FEN STRING: " << line << "\n";
		b = eb.value();
	} else {
		std::cout << "FEN error: " << eb.error() << "\n";
		return -1;
	}

	if (b.toFen() != line) {
		std::cout << "toFen() is wrong\n";
		return 1;
	} else {
		std::cout << "toFen() is right\n";
		return 0;
	}
	
	/*

	std::cout << b.toString();
	auto moves = generateAllMoves(b);

	std::random_device rd;                          // seed source (true randomness from the OS, when available)
	std::mt19937 gen(rd());                          // a good general-purpose PRNG (Mersenne Twister), seeded once
	std::uniform_int_distribution<int> dist(0, moves.size() - 1); // inclusive range [0, x-1]

	int r = dist(gen);
	auto move = moves[r];
	auto x = b.makeMove(move);
	std::cout << b.toString();
	*/
}
