#include <Windows.h>
#include <vector>
#include "ChessGame.h"
#include "UnitMovePiece.h"
#include "SpriteDefs.h"
#include "Pawn.h"
#include "King.h"

int main() {
	// Pawn definition
	Pawn pawn = Pawn(1, PawnSprite);
	UnitMovePiece bishop = UnitMovePiece(2, false, false, BishopSprite);
	bishop.generateMoveset(std::vector<IVec2> {IVec2(1, 1)}, Rotate90, true);

	UnitMovePiece knight = UnitMovePiece(3, false, true, KnightSprite);
	knight.generateMoveset(std::vector<IVec2> {IVec2(2, 1)}, Rotate90 | FlipY, false);

	UnitMovePiece rook = UnitMovePiece(4, false, false, RookSprite);
	rook.generateMoveset(std::vector<IVec2> {IVec2(1, 0)}, Rotate90, true);

	UnitMovePiece queen = UnitMovePiece(5, false, false, QueenSprite);
	queen.generateMoveset(std::vector<IVec2> {IVec2(1, 0)}, Rotate45, true);

	King king = King(6, 4, KingSprite);

	// Create vector of PieceDef pointers
	std::vector<PieceDef*> pieces {
		&pawn,
		&bishop,
		&knight,
		&rook,
		&queen,
		&king
	};
	// Initialize board state with initital chess position
	BoardState board = BoardState(new byte[64] {
		0x04, 0x03, 0x02, 0x05, 0x06, 0x02, 0x03, 0x04,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x14, 0x13, 0x12, 0x15, 0x16, 0x12, 0x13, 0x14
	});

	// Create ChessGame object based on pieces and board, and start its main loop
	ChessGame game = ChessGame(pieces, board);
	game.mainloop();
}