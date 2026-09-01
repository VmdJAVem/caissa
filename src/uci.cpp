#include "uci.hpp"
#include "board.hpp"
#include "search.hpp"
#include <iostream>
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

void Uci::loop()
{
	std::string line;
	std::cout.setf(std::ios::unitbuf);
	Board board;
	std::jthread searchThread;

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
			std::string subcommand;
			iss >> subcommand;
			if (subcommand == "depth") {
				int depth;
				iss >> depth;

				searchThread = std::jthread([&board, depth](std::stop_token st) {
					auto bestMove = iterativeNegaMax(board, depth, st);
					if (bestMove) {
						std::cout << "bestmove " << squareName(bestMove->from) << squareName(bestMove->to);
						
						if (bestMove->promotionPiece != Piece::None) {
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
						}
						std::cout << "\n";
					} else {
						std::cout << "bestmove 0000\n";
					}
				});
			}
		} else if (command == "stop") {
			if (searchThread.joinable()) {
				searchThread.request_stop();
				searchThread.join();
			}
		}
	}
}
