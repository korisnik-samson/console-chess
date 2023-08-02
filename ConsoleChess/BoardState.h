#pragma once
#include <algorithm>
#include <Windows.h>
#include "IVec2.h"
#include "Byte88.h"

#define PIECE_ID      0b00001111	// Bitmask for a Piece's ID
#define PIECE_TEAM    0b00010000	// Bitmask for a Piece's team
#define PIECE_MOVED   0b00100000	// Bitmask for a Piece's moved flag
#define PIECE_SPTEMP  0b01000000	// Bitmask for a Piece's temporary (one-move) special flag
#define PIECE_SPPERM  0b10000000	// Bitmask for a Piece's permanent special flag
#define PIECE_SPECIAL 0b11000000	// Bitmask for both special bits

struct Piece
{
	byte id : 4;		// Piece ID (1-15)
	byte team : 1;		// Piece team (black/white)
	byte moved : 1;		// if piece moved since start
	byte spTemp : 1;	// special bits reserved for the piece, last one move
	byte spPerm : 1;	// special bits reserved for the piece, last forever

	// Create a Piece from its byte representation, parsing the bits into the correct fields.
	Piece(byte b)
	{
		id = b & 0xf; b >>= 4;
		team = b & 1; b >>= 1;
		moved = b & 1; b >>= 1;
		spTemp = b & 1; b >>= 1;
		spPerm = b;
	}
	// Get the byte that represents this piece.
	byte getByte() {
		return spPerm << 7 | spTemp << 6 | moved << 5 | team << 4 | id;
	}
};

/// Byte88 subclass to represent chess board
struct BoardState : Byte88
{
	// Create empty byte8x8.
	BoardState() : Byte88() {};

	// Create copy of byte8x8.
	BoardState(const BoardState& b)
	{
		memcpy_s(data, 64, b.data, 64);
	}

	// Create a BoardState filled with the same value.
	BoardState(byte b)
	{
		std::fill_n(data, 64, b);
	}

	// Create a BoardState from a bit board with custom LOW and HIGH bytes. 
	BoardState(UINT64 bboard, byte low, byte high) : Byte88()
	{
		for (int i = 0; i < 64; i++)
		{	// Get bit at position i (LSB) and assign to low/high
			data[i] = (bboard & 1) ? high : low;
			// Shift board to the right, making space for next bit
			bboard >>= 1;
		}
	}

	// Create a BoardState from a pointer. Unsafe.
	BoardState(byte* ptr)
	{
		memcpy_s(data, 64, ptr, 64);
	}

	// Get a Piece structure at a given index.
	Piece getPiece(int pos) const
	{
		return Piece(data[pos]);
	}

	// Get a Piece structure at a given position
	Piece getPiece(IVec2 pos) const
	{
		// Shift y by 3 bits and OR with x to get a unique index from 0-63
		return Piece(data[pos.y << 3 | pos.x]);
	}

	// Set the Piece structure at a given index.
	void setPiece(int pos, Piece p)
	{
		data[pos] = p.getByte();
	}

	// Set the Piece structure at a given position.
	void setPiece(IVec2 pos, Piece p)
	{
		// Shift y by 3 bits and OR with x to get a unique index from 0-63
		data[pos.y << 3 | pos.x] = p.getByte();
	}
};