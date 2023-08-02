#pragma once

#include <vector>
#include <Windows.h>
#include "IVec2.h"
#include "BoardState.h"
#include "Byte88.h"

// Absract class for piece definitions. Will be inherited by the pieces to be added in the game.
class PieceDef {
public:
	byte id;
	bool critical;
	Byte88 sprite;

	// Constructor
	PieceDef() : id(0), critical(0), sprite() {};
	PieceDef(byte id, bool critical, Byte88 sprite) : id(id), critical(critical), sprite(sprite) {};

	virtual bool isValidMove(IVec2 start, IVec2 end, const BoardState &board) {
		return false;
	}

	virtual bool makeMove(IVec2 start, IVec2 end, BoardState& board) {
		board[end] = board[start] | PIECE_MOVED; // Set piece moved flag
		board[start] = 0;

		return false;
	}
};