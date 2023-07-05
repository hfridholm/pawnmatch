#ifndef ROOK_BISHOP_ATTACKS_H
#define ROOK_BISHOP_ATTACKS_H

extern U64 create_index_occupancy(int index, U64 attackMask, int bitAmount);

extern U64 calculate_bishop_attacks(Square square, U64 block);

extern U64 calculate_rook_attacks(Square square, U64 block);

#endif // ROOK_BISHOP_ATTACKS_H