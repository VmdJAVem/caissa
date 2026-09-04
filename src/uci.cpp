#include "uci.hpp"
#include "board.hpp"
#include "search.hpp"
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stop_token>
#include <string>
#include <thread>

std::optional<Move> parseUciMove(const std::string &uci, const Board &board)
{
	if (uci.size() < 4 || uci.size() > 5)
		return std::nullopt;

	int fromFile = uci[0] - 'a';
	int fromRank = uci[1] - '1';
	auto fromSquare = static_cast<Square>(fromRank * 8 + fromFile);

	int toFile = uci[2] - 'a';
	int toRank = uci[3] - '1';
	auto toSquare = static_cast<Square>(toRank * 8 + toFile);

	auto piece = board.pieceAt(fromSquare);

	if (!piece)
		return std::nullopt;

	auto promotionPiece = Piece::None;

	if (uci.size() == 5) {
		switch (uci[4]) {
		case 'q':
			promotionPiece = Piece::Queen;
			break;
		case 'r':
			promotionPiece = Piece::Rook;
			break;
		case 'b':
			promotionPiece = Piece::Bishop;
			break;
		case 'n':
			promotionPiece = Piece::Knight;
			break;
		default:
			return std::nullopt;
		}
	}

	auto capturedPiece = Piece::None;
	auto temp = board.pieceAt(toSquare);

	if (temp && temp->color != board.getSideToMove())
		capturedPiece = temp->piece;

	bool isCastling = piece->piece == Piece::King && ((fromSquare == Square::E1 && (toSquare == Square::G1 || toSquare == Square::C1)) ||
							  (fromSquare == Square::E8 && (toSquare == Square::G8 || toSquare == Square::C8)));

	bool isEnPassant = false;

	if (piece->piece == Piece::Pawn && !temp && board.getEnPassantTarget() == toSquare) {
		int fileDelta = toFile - fromFile;
		int rankDelta = toRank - fromRank;
		if (std::abs(fileDelta) == 1 && std::abs(rankDelta) == 1) {
			isEnPassant = true;
			capturedPiece = Piece::Pawn;
		}
	}

	return Move{
	    .from = fromSquare,
	    .to = toSquare,
	    .piece = piece->piece,
	    .capturedPiece = capturedPiece,
	    .promotionPiece = promotionPiece,
	    .isEnPassant = isEnPassant,
	    .isCastling = isCastling,
	};
}

struct GoParams {
	int wtime = 0;
	int btime = 0;
	int winc = 0;
	int binc = 0;
	int movestogo = 0;
	int depth = 0;
	int movetime = 0;
	uint64_t nodes = 0;

	bool infinite = false;
};
void printBestMove(const std::optional<Move> &bestMove)
{
	if (!bestMove) {
		std::cout << "bestmove 0000\n";
		return;
	}
	std::cout << "bestmove " << squareName(bestMove->from) << squareName(bestMove->to);
	switch (bestMove->promotionPiece) {
	case Piece::Queen:
		std::cout << 'q';
		break;
	case Piece::Rook:
		std::cout << 'r';
		break;
	case Piece::Bishop:
		std::cout << 'b';
		break;
	case Piece::Knight:
		std::cout << 'n';
		break;
	default:
		break;
	}
	std::cout << "\n";
}

void Uci::loop()
{
	std::string line;
	std::cout.setf(std::ios::unitbuf);
	Board board;
	std::jthread searchThread;
	std::jthread timerThread;

	while (std::getline(std::cin, line)) {
		std::istringstream iss(line);
		std::string command;
		iss >> command;

		if (command == "uci") {
			std::cout << "id name caissa\n";
			std::cout << "id author Luis Justo\n";
			std::cout << "uciok\n";

		} else if (command == "isready") {
			std::cout << "readyok\n";
		} else if (command == "quit") {
			break;
		} else if (command == "position") {
			std::string positionType;
			iss >> positionType;

			if (positionType == "fen") {
				std::string fen;
				std::string field;

				// FEN consists of exactly 6 fields.
				for (int i = 0; i < 6 && iss >> field; ++i) {
					if (i > 0)
						fen += ' ';
					fen += field;
				}

				auto result = Board::fromFen(fen);

				if (result) {
					board = result.value();
				} else {
					std::cout << "info string bad fen " << result.error() << "\n";
					continue;
				}
			} else if (positionType == "startpos") {
				board = Board();
			} else {
				continue;
			}

			std::string token;

			if (iss >> token && token == "moves") {
				std::string moveString;

				while (iss >> moveString) {
					auto move = parseUciMove(moveString, board);

					if (!move) {
						std::cout << "info string "
							     "invalid move "
							  << moveString << "\n";
						break;
					}

					board.makeMove(*move);
				}
			}
		} else if (command == "go") {
			std::string token;
			GoParams params;
			while (iss >> token) {
				if (token == "wtime") {
					iss >> params.wtime;
				} else if (token == "btime") {
					iss >> params.btime;
				} else if (token == "winc") {
					iss >> params.winc;
				} else if (token == "binc") {
					iss >> params.binc;
				} else if (token == "movestogo") {
					iss >> params.movestogo;
				} else if (token == "depth") {
					iss >> params.depth;
				} else if (token == "movetime") {
					iss >> params.movetime;
				} else if (token == "nodes") {
					iss >> params.nodes;
				} else if (token == "infinite") {
					params.infinite = true;
				}
			}

			if (params.depth > 0) {
				int depth = params.depth;

				searchThread = std::jthread([&board, depth](std::stop_token st) {
					auto bestMove = iterativeNegaMax(board, depth, st);
					printBestMove(bestMove);
				});
			} else if (params.movetime > 0) {
				searchThread = std::jthread([&board](std::stop_token st) {
					auto bestMove =
					    iterativeNegaMax(board, 999999999, st); // just use very large "depth" since whe will stop it from timer thread
					printBestMove(bestMove);
				});
				timerThread = std::jthread([params, &searchThread](std::stop_token st) {
					std::mutex m;
					std::unique_lock<std::mutex> lock(m);
					std::condition_variable_any cv;

					cv.wait_for(lock, st, std::chrono::milliseconds(params.movetime), [] { return false; });

					if (searchThread.joinable())
						searchThread.request_stop();
				});
			} else if (params.wtime > 0 && params.btime > 0) {
				auto color = board.getSideToMove();
				int totalTimeMs = color == Color::White ? params.wtime : params.btime;
				int incrementMs = color == Color::White ? params.winc : params.binc;
				int movesToGo = params.movestogo > 0 ? params.movestogo : 30;

				int budgetMs = std::max(((totalTimeMs / movesToGo) + incrementMs) - 50, 10);

				searchThread = std::jthread([&board](std::stop_token st) {
					auto bestMove =
					    iterativeNegaMax(board, 999999999, st); // just use very large "depth" since whe will stop it from timer thread
					printBestMove(bestMove);
				});
				timerThread = std::jthread([&searchThread, budgetMs](std::stop_token st) {
					std::mutex m;
					std::unique_lock<std::mutex> lock(m);
					std::condition_variable_any cv;

					cv.wait_for(lock, st, std::chrono::milliseconds(budgetMs), [] { return false; });

					if (searchThread.joinable())
						searchThread.request_stop();
				});
			} else {
				searchThread = std::jthread([&board](std::stop_token st) {
					auto bestMove = iterativeNegaMax(board, 999999999, st);
					printBestMove(bestMove);
				});
			}

		} else if (command == "stop") {
			if (searchThread.joinable()) {
				searchThread.request_stop();
				searchThread.join();
			}
		} else if (command == "ucinewgame") {
			// nothing to reset yet — no persistent state (transposition table, etc.) exists
		}
	}
}
