#pragma once

#include "PieceDef.h"
#include "SpriteDefs.h"

class Pawn : public PieceDef
{
public:
	Pawn(byte id, Byte88 sprite) : PieceDef(id, false, sprite) { }

	bool isValidMove(IVec2 start, IVec2 end, const BoardState &board) override {
		IVec2 delta = end - start;
		Piece p = board.getPiece(start);
		int dir = p.team ? -1 : 1;

		if (start.y == (p.team ? 0 : 7)) return false;

		if (board[end] == 0) {
			if (delta == IVec2(0, dir)) return true;
			if (delta == IVec2(0, 2*dir) && !p.moved && board[start + IVec2(0,dir)] == 0) return true;
		}
		// Capture / en passant case
		for (int i = -1; i <= 1; i += 2) {
			if (delta == IVec2(i,dir)) {
				Piece cap = board.getPiece(start + IVec2(i,dir));

				if (cap.team != p.team && cap.id != 0) return true;

				Piece enp = board.getPiece(start + IVec2(i,0));

				if (cap.id == 0 && enp.team != p.team && enp.id == id && enp.spTemp == 1) return true;
			}
		}

		return false;
	}

	bool makeMove(IVec2 start, IVec2 end, BoardState& board) override {
		IVec2 delta = end - start;
		Piece p = board.getPiece(start);

		if (delta.x != 0 && board[end] == 0) // En passant capture
			board[start + IVec2(delta.x, 0)] = 0;

		board[end] = board[start] | PIECE_MOVED;
		board[start] = 0;

		if (abs(delta.y) > 1) board[end] |= PIECE_SPTEMP;

		return end.y == 0 || end.y == 7;
	}
};