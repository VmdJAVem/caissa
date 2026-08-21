#pragma once
#include "moves.hpp"
#include <string>
#include <vector>

void testMove(const std::string& startFen, Move move, const std::string& expectedFen);


struct RoundTripCase {
	std::string fen;
	Move move;
};
void testRoundTrip(const std::vector<RoundTripCase>& cases);
