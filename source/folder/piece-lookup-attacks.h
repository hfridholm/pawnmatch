#ifndef PIECE_LOOKUP_ATTACKS_H
#define PIECE_LOOKUP_ATTACKS_H

extern U64 bishop_lookup_attacks(Square square, U64 occupancy);

extern U64 rook_lookup_attacks(Square square, U64 occupancy);

extern U64 queen_lookup_attacks(Square square, U64 occupancy);

extern U64 pawn_lookup_attacks(Side side, Square square);

extern U64 king_lookup_attacks(Square square);

extern U64 knight_lookup_attacks(Square square);

#endif // PIECE_LOOKUP_ATTACKS_H