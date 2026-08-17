#pragma once

#include <vector>
#include "board.hpp"
#include "moves.hpp"

constexpr std::array<std::array<Bitboard, 64>, 2> computePawnPushTable() {
	std::array<std::array<Bitboard, 64>, 2> table{};
	for (int i = 0; i < 64; i++) {
		table[static_cast<int>(Color::White)][i] = (i + 8 < 64) ? (1ULL << (i + 8)) : 0;
		table[static_cast<int>(Color::Black)][i] = (i - 8 >= 0) ? (1ULL << (i - 8)) : 0;
	}
	return table;
}
constexpr std::array<std::array<Bitboard, 64>, 2> pawnPushTable = computePawnPushTable();

constexpr std::array<std::array<Bitboard, 64>, 2> computePawnCaptureTable() {
	std::array<std::array<Bitboard, 64>, 2> table{};

	for (int i = 0; i < 64; i++) {
		int file = i % 8;
		int rank = i / 8;
		Bitboard whiteCaptures = 0;
		if (rank < 7) {
			if (file > 0) whiteCaptures |= (1ULL << (i + 7));  // up-left
			if (file < 7) whiteCaptures |= (1ULL << (i + 9)); // up-right
		}
		table[static_cast<int>(Color::White)][i] = whiteCaptures;

		Bitboard blackCaptures = 0;
		if (rank > 0) {
			if (file > 0) blackCaptures |= (1ULL << (i - 9));
			if (file < 7) blackCaptures |= (1ULL << (i - 7));			
		}
		table[static_cast<int>(Color::Black)][i] = blackCaptures;
	}
	return table;
}
constexpr std::array<std::array<Bitboard, 64>, 2> pawnCaptureTable = computePawnCaptureTable();
std::vector<Move> generatePawnMoves(const Board& board, Color color);
