#pragma once

#include "board.hpp"
#include "moves.hpp"
#include <vector>

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
			if (file > 0)
				whiteCaptures |= (1ULL << (i + 7)); // up-left
			if (file < 7)
				whiteCaptures |= (1ULL << (i + 9)); // up-right
		}
		table[static_cast<int>(Color::White)][i] = whiteCaptures;

		Bitboard blackCaptures = 0;
		if (rank > 0) {
			if (file > 0)
				blackCaptures |= (1ULL << (i - 9));
			if (file < 7)
				blackCaptures |= (1ULL << (i - 7));
		}
		table[static_cast<int>(Color::Black)][i] = blackCaptures;
	}
	return table;
}
constexpr std::array<std::array<Bitboard, 64>, 2> pawnCaptureTable = computePawnCaptureTable();

constexpr std::array<Bitboard, 64> computeKnightAttackTable() {
	std::array<Bitboard, 64> table{};
	constexpr std::array<std::pair<int, int>, 8> offsets = {{{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}}};
	for (int i = 0; i < 64; i++) {
		int file = i % 8;
		int rank = i / 8;
		for (auto &[fileOffset, rankOffset] : offsets) {
			int newFile = file + fileOffset;
			int newRank = rank + rankOffset;

			if (newFile < 8 && newFile >= 0 && newRank < 8 && newRank >= 0) {
				Square sq = static_cast<Square>(newRank * 8 + newFile);
				table[i] |= squareToBitboard(sq);
			}
		}
	}
	// For each square 0-63:
	//   compute file, rank
	//   for each of the 8 (df, dr) offsets:
	//     compute newFile, newRank
	//     if both in bounds: OR in the bit for that resulting square
	return table;
}
inline constexpr auto knightAttackTable = computeKnightAttackTable();

std::vector<Move> generatePawnMoves(const Board &board, Color color);
std::vector<Move> generateKnightMoves(const Board &board, Color color);
