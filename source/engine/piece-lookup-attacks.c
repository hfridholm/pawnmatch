#include "../engine.h"

#include "magic-lookup-numbers.h"
#include "rook-bishop-attacks.h"
#include "piece-lookup-masks.h"

U64 BISHOP_LOOKUP_ATTACKS[BOARD_SQUARES][512];

U64 ROOK_LOOKUP_ATTACKS[BOARD_SQUARES][4096];

void init_rook_lookup_attacks()
{
    for (Square square = 0; square < BOARD_SQUARES; square++)
    {
        int relevantBits = ROOK_RELEVANT_BITS[square];

        int occupancyIndicies = (1 << relevantBits);
        
        for (int index = 0; index < occupancyIndicies; index++)
        {
            U64 occupancy = create_index_occupancy(index, ROOK_LOOKUP_MASKS[square], relevantBits);
            
            int magicIndex = (occupancy * ROOK_MAGIC_NUMBERS[square]) >> (BOARD_SQUARES - relevantBits);
            
            ROOK_LOOKUP_ATTACKS[square][magicIndex] = calculate_rook_attacks(square, occupancy);
        }
    }
}

void init_bishop_lookup_attacks()
{
    for (Square square = 0; square < BOARD_SQUARES; square++)
    {
        int relevantBits = BISHOP_RELEVANT_BITS[square];

        int occupancyIndicies = (1 << relevantBits);
        
        for (int index = 0; index < occupancyIndicies; index++)
        {
            U64 occupancy = create_index_occupancy(index, BISHOP_LOOKUP_MASKS[square], relevantBits);
            
            int magicIndex = (occupancy * BISHOP_MAGIC_NUMBERS[square]) >> (BOARD_SQUARES - relevantBits);
            
            BISHOP_LOOKUP_ATTACKS[square][magicIndex] = calculate_bishop_attacks(square, occupancy);
        }
    }
}

void init_piece_lookup_attacks(void)
{
    init_rook_lookup_attacks();

    init_bishop_lookup_attacks();
}

int bishop_occupancy_index(Square square, U64 occupancy)
{
    U64 occupancyIndex = occupancy;

    occupancyIndex &= BISHOP_LOOKUP_MASKS[square];

    occupancyIndex *= BISHOP_MAGIC_NUMBERS[square];

    occupancyIndex >>= BOARD_SQUARES - BISHOP_RELEVANT_BITS[square];

    return (int) occupancyIndex;
}

int rook_occupancy_index(Square square, U64 occupancy)
{
    U64 occupancyIndex = occupancy;

    occupancyIndex &= ROOK_LOOKUP_MASKS[square];

    occupancyIndex *= ROOK_MAGIC_NUMBERS[square];

    occupancyIndex >>= BOARD_SQUARES - ROOK_RELEVANT_BITS[square];

    return (int) occupancyIndex;
}

U64 bishop_lookup_attacks(Square square, U64 occupancy)
{
    int occupancyIndex = bishop_occupancy_index(square, occupancy);
    
    return BISHOP_LOOKUP_ATTACKS[square][occupancyIndex];
}

U64 rook_lookup_attacks(Square square, U64 occupancy)
{
    int occupancyIndex = rook_occupancy_index(square, occupancy);

    return ROOK_LOOKUP_ATTACKS[square][occupancyIndex];
}

U64 queen_lookup_attacks(Square square, U64 occupancy)
{
    U64 queenAttacks = 0ULL;

    queenAttacks |= bishop_lookup_attacks(square, occupancy);

    queenAttacks |= rook_lookup_attacks(square, occupancy);

    return queenAttacks;
}

U64 pawn_lookup_attacks(Side side, Square square)
{
    return PAWN_LOOKUP_MASKS[side][square];
}

U64 knight_lookup_attacks(Square square)
{
    return KNIGHT_LOOKUP_MASKS[square];
}

U64 king_lookup_attacks(Square square)
{
    return KING_LOOKUP_MASKS[square];
}

Piece boards_square_piece(U64 boards[12], Square square)
{
  for(Piece piece = PIECE_WHITE_PAWN; piece <= PIECE_BLACK_KING; piece++)
  {
    if(BOARD_SQUARE_GET(boards[piece], square)) return piece;
  }
  return PIECE_NONE;
}

U64 piece_lookup_attacks(Position position, Square square)
{
    Piece piece = boards_square_piece(position.boards, square);

    if((piece == PIECE_WHITE_KING) || (piece == PIECE_BLACK_KING))
    {
        return king_lookup_attacks(square);
    }
    else if((piece == PIECE_WHITE_KNIGHT) || (piece == PIECE_BLACK_KNIGHT))
    {
        return knight_lookup_attacks(square);
    }
    else if((piece == PIECE_WHITE_BISHOP) || (piece == PIECE_BLACK_BISHOP))
    {
        return bishop_lookup_attacks(square, position.covers[SIDE_BOTH]);
    }
    else if((piece == PIECE_WHITE_ROOK) || (piece == PIECE_BLACK_ROOK))
    {
        return rook_lookup_attacks(square, position.covers[SIDE_BOTH]);
    }
    else if((piece == PIECE_WHITE_QUEEN) || (piece == PIECE_BLACK_QUEEN))
    {
        return queen_lookup_attacks(square, position.covers[SIDE_BOTH]);
    }
    else if(piece == PIECE_WHITE_PAWN)
    {
        return pawn_lookup_attacks(SIDE_WHITE, square);
    }
    else if(piece == PIECE_BLACK_PAWN)
    {
        return pawn_lookup_attacks(SIDE_BLACK, square);
    }
    else return 0ULL;
}
