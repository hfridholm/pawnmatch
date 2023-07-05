#include "../header.h"

#include "engine-values.h"
#include "bitboard-manipulation.h"

U64 BISHOP_LOOKUP_MASKS[BOARD_SQUARES];

U64 ROOK_LOOKUP_MASKS[BOARD_SQUARES];

U64 PAWN_LOOKUP_MASKS[2][BOARD_SQUARES];

U64 KNIGHT_LOOKUP_MASKS[BOARD_SQUARES];

U64 KING_LOOKUP_MASKS[BOARD_SQUARES];


int BISHOP_RELEVANT_BITS[BOARD_SQUARES];

int ROOK_RELEVANT_BITS[BOARD_SQUARES];


U64 mask_pawn_attacks(Side side, Square square)
{
    U64 attacks = 0ULL;

    U64 bitboard = BOARD_SQUARE_SET(0ULL, square);

    if(side == SIDE_WHITE)
    {
        if((bitboard >> 7) & NOT_A_FILE) attacks |= (bitboard >> 7);
        if((bitboard >> 9) & NOT_H_FILE) attacks |= (bitboard >> 9);
    }
    else
    {
        if((bitboard << 7) & NOT_H_FILE) attacks |= (bitboard << 7);
        if((bitboard << 9) & NOT_A_FILE) attacks |= (bitboard << 9);    
    }
    return attacks;
}

U64 mask_knight_attacks(Square square)
{
    U64 attacks = 0ULL;

    U64 bitboard = BOARD_SQUARE_SET(0ULL, square);

    if((bitboard >> 17) & NOT_H_FILE)  attacks |= (bitboard >> 17);
    if((bitboard >> 15) & NOT_A_FILE)  attacks |= (bitboard >> 15);
    if((bitboard >> 10) & NOT_HG_FILE) attacks |= (bitboard >> 10);
    if((bitboard >> 6)  & NOT_AB_FILE) attacks |= (bitboard >> 6);

    if((bitboard << 17) & NOT_A_FILE)  attacks |= (bitboard << 17);
    if((bitboard << 15) & NOT_H_FILE)  attacks |= (bitboard << 15);
    if((bitboard << 10) & NOT_AB_FILE) attacks |= (bitboard << 10);
    if((bitboard << 6)  & NOT_HG_FILE) attacks |= (bitboard << 6);

    return attacks;
}

U64 mask_king_attacks(Square square)
{
    U64 attacks = 0ULL;

    U64 bitboard = BOARD_SQUARE_SET(0ULL, square);
    
    if (bitboard >> 8)                attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & NOT_H_FILE) attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & NOT_A_FILE) attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & NOT_H_FILE) attacks |= (bitboard >> 1);

    if (bitboard << 8)                attacks |= (bitboard << 8);
    if ((bitboard << 9) & NOT_A_FILE) attacks |= (bitboard << 9);
    if ((bitboard << 7) & NOT_H_FILE) attacks |= (bitboard << 7);
    if ((bitboard << 1) & NOT_A_FILE) attacks |= (bitboard << 1);
    
    return attacks;
}

U64 mask_bishop_attacks(Square square)
{
    U64 attacks = 0ULL;
    
    int targetRank = square / BOARD_FILES;
    int targetFile = square % BOARD_FILES;
    
    for (int rank = targetRank + 1, file = targetFile + 1; rank <= 6 && file <= 6; rank++, file++)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + file));
    }
    for (int rank = targetRank - 1, file = targetFile + 1; rank >= 1 && file <= 6; rank--, file++) 
    {
        attacks |= (1ULL << (rank * BOARD_FILES + file));
    }
    for (int rank = targetRank + 1, file = targetFile - 1; rank <= 6 && file >= 1; rank++, file--) 
    {
        attacks |= (1ULL << (rank * BOARD_FILES + file));
    }
    for (int rank = targetRank - 1, file = targetFile - 1; rank >= 1 && file >= 1; rank--, file--) 
    {
        attacks |= (1ULL << (rank * BOARD_FILES + file));
    }
    return attacks;
}

U64 mask_rook_attacks(Square square)
{
    U64 attacks = 0ULL;
    
    int targetRank = square / BOARD_FILES;
    int targetFile = square % BOARD_FILES;
    
    for (int rank = targetRank + 1; rank <= 6; rank++)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + targetFile));
    }
    for (int rank = targetRank - 1; rank >= 1; rank--)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + targetFile));
    }
    for (int file = targetFile + 1; file <= 6; file++)
    {
        attacks |= (1ULL << (targetRank * BOARD_FILES + file));
    }
    for (int file = targetFile - 1; file >= 1; file--)
    {
        attacks |= (1ULL << (targetRank * BOARD_FILES + file));
    }
    return attacks;
}

void init_piece_lookup_masks()
{
    for (Square square = 0; square < BOARD_SQUARES; square++)
    {
        PAWN_LOOKUP_MASKS[SIDE_WHITE][square] = mask_pawn_attacks(SIDE_WHITE, square);

        PAWN_LOOKUP_MASKS[SIDE_BLACK][square] = mask_pawn_attacks(SIDE_BLACK, square);
        
        KNIGHT_LOOKUP_MASKS[square] = mask_knight_attacks(square);
        
        KING_LOOKUP_MASKS[square] = mask_king_attacks(square);

        ROOK_LOOKUP_MASKS[square] = mask_rook_attacks(square);

        BISHOP_LOOKUP_MASKS[square] = mask_bishop_attacks(square);
    }
}

void init_bishop_rook_relevant_bits()
{
    for (Square square = 0; square < BOARD_SQUARES; square++)
    {
        BISHOP_RELEVANT_BITS[square] = board_bit_amount(BISHOP_LOOKUP_MASKS[square]);

        ROOK_RELEVANT_BITS[square] = board_bit_amount(ROOK_LOOKUP_MASKS[square]);
    }
}