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
void testLegalMoves(const std::string &fen, const std::vector<std::pair<Square, Square>> &expected);
void testEvaluate(const std::string &fen, int depth, int expected);
void testNegamax(const std::string& fen, int depth, Square expectedFrom, Square expectedTo);
