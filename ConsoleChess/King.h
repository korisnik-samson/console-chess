#pragma once
#include "PieceDef.h"
#include "SpriteDefs.h"

class King : public PieceDef {
public:
	byte rookId;

	King(byte id, byte rookId, Byte88 sprite) : PieceDef(id, true, sprite) { 
		this->rookId = rookId;
	}

	bool isValidMove(IVec2 start, IVec2 end, const BoardState &board) override {
		IVec2 delta = end - start;
		Piece p = board.getPiece(start);

		if (start == end) return false; 

		if (!p.moved && abs(delta.x) == 2 && delta.y == 0) {
			IVec2 dir = IVec2((end.x > start.x) ? 1 : -1, 0);
			IVec2 rookPos = IVec2((end.x > start.x) ? 7 : 0, start.y);
			Piece rook = board.getPiece(rookPos);

			if (rook.id != rookId || rook.team != p.team) { return false; }

			for (IVec2 sqr = start + dir; sqr != rookPos; sqr += dir) {
				if (board[sqr] != 0) { return false; }
			}

			return true;
		}

		if (board[end] == 0 || board.getPiece(end).team != p.team) 
			return abs(delta.x) <= 1 && abs(delta.y) <= 1;

		return false;
	}

	// Perform a king move. Return value represents promotion and is thus false.
	bool makeMove(IVec2 start, IVec2 end, BoardState& board) override {
		// Get king piece and move delta
		Piece p = board.getPiece(start);
		IVec2 delta = end - start;

		// Normal move
		if (abs(delta.x) < 2) {
			board[end] = board[start] | PIECE_MOVED;
			board[start] = 0;
		}
		// Castle
		else {
			IVec2 dir = IVec2((end.x > start.x) ? 1 : -1, 0);
			IVec2 rookPos = IVec2((end.x > start.x) ? 7 : 0, start.y);
			
			board[start + 2 * dir] = board[start] | PIECE_MOVED;
			board[start] = 0;

			board[start + dir] = board[rookPos] | PIECE_MOVED;
			board[rookPos] = 0;
		}

		return false;
	}
};