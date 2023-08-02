#pragma once
#include <Windows.h>
#include <vector>
#include "PieceDef.h"
#include <algorithm>

// Enum defining the types of symmetry that can be applied to unit moves. 
enum MoveSymmetry : int {
	None =		0b0000, 
	Rotate90 =	0b0001,
	Rot90_45 =  0b0010,
	Rotate45 =	0b0011,
	FlipX =		0b0100,
	FlipY =		0b1000
};
 
class UnitMovePiece : public PieceDef {
private:
	byte moveset[256];

public:
	bool canJump;

	UnitMovePiece(byte id, bool critical, bool canJump, Byte88 sprite) : moveset{ } {
		this->id = id;
		this->critical = critical;
		this->canJump = canJump;
		this->sprite = Byte88(sprite);
	}

	void generateMoveset(std::vector<IVec2> unitMoves, int sym, bool repeat) {
		if (sym & Rotate90) {
			size_t csz = unitMoves.size();

			for (int i = 0; i < csz; i++) {
				IVec2 mv = unitMoves[i];
				IVec2 dv = IVec2(mv.x - mv.y, mv.x + mv.y);

				unitMoves.push_back(IVec2(-mv.y, mv.x));
				unitMoves.push_back(IVec2(mv.y, -mv.x));
				unitMoves.push_back(IVec2(-mv.x, -mv.y));

				if (sym & Rot90_45) {
					unitMoves.push_back(dv);
					unitMoves.push_back(IVec2(-dv.y, dv.x));
					unitMoves.push_back(IVec2(dv.y, -dv.x));
					unitMoves.push_back(IVec2(-dv.x,-dv.y));
				}
			}
		}

		if (sym & (FlipX | FlipY)) {
			size_t csz = unitMoves.size();

			for (int i = 0; i < csz; i++) {
				IVec2 mv = unitMoves[i]; // Orig. move

				if (sym & FlipX) unitMoves.push_back(IVec2(-mv.x, mv.y));
				if (sym & FlipY) unitMoves.push_back(IVec2(mv.x, -mv.y));
			}
		}

		std::fill_n(moveset, sizeof(moveset), 0);

		for (int i = 0; i < unitMoves.size(); i++) {
			IVec2 mv = unitMoves[i];
			IVec2 delta = mv;
			int j = 0x88;

			do {
				int jc = (delta.y + 8) << 4 | (delta.x + 8);
				moveset[jc] = j;
				j = jc;
				delta += mv;

			} while (repeat && abs(delta.x) < 8 && abs(delta.y) < 8);
		}
	}

	// Check if potential move is pseudolegal
	bool isValidMove(IVec2 start, IVec2 end, const BoardState &board) override {
		if (board[end] != 0 && board.getPiece(end).team == board.getPiece(start).team) { return false; }
		IVec2 delta = end - start;

		int i = (delta.y + 8) << 4 | (delta.x + 8);

		if (moveset[i] == 0) return false; 

		else if (canJump) return true; 

		else {
			// Jump to previous positions on the move path until the 0 delta
			while (moveset[i] != 0x88) {
				i = moveset[i];
				IVec2 d = IVec2((i & 0xF) - 8, (i >> 4) - 8);
				if (board[start + d] != 0) { return false; }
			}

			return true;
		}
	}
};

