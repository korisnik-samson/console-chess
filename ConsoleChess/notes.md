# Piece Definitions 
`PieceDef.h` Notes:

**Ln 20** <br>
Check if potential move is pseudolegal, implemented by specific piece class.
The return value is true if the move is valid and false otherwise.

**Ln 24** <br>
Check if potential move is pseudolegal, implemented by specific piece class.
The return value is true if the move is valid and false otherwise.

# 8 x 8 Byte Matrix
`Byte88.h` Notes:

**Ln 36** <br>
OPERATORS
All operators are defined in a similar fashion. For each operation,
we implement the operator (as const) and its assignment variant (ie. +=).
Every operation is made element-wise with another Byte88 using a for loop.
assignment operators return the object.

# King Character
`King.h`
