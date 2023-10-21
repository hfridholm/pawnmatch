#include "../engine.h"

#include "bitboard-manipulation.h"

U64 create_index_occupancy(int index, U64 attackMask, int bitAmount)
{
    U64 occupancy = 0ULL;
    
    for(int amount = 0; amount < bitAmount; amount++)
    {
        int square = board_ls1b_index(attackMask);

        attackMask = BOARD_SQUARE_POP(attackMask, square);
        
        if(index & (1 << amount)) occupancy |= (1ULL << square);
    }
    return occupancy;
}

U64 calculate_bishop_attacks(Square square, U64 block)
{
    U64 attacks = 0ULL;
    
    int targetRank = square / BOARD_FILES;
    int targetFile = square % BOARD_FILES;
    
    for (int rank = targetRank + 1, file = targetFile + 1; rank < BOARD_RANKS && file < BOARD_FILES; rank++, file++)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + file));

        if ((1ULL << (rank * BOARD_FILES + file)) & block) break;
    }
    
    for (int rank = targetRank - 1, file = targetFile + 1; rank >= 0 && file < BOARD_FILES; rank--, file++)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + file));

        if ((1ULL << (rank * BOARD_FILES + file)) & block) break;
    }
    
    for (int rank = targetRank + 1, file = targetFile - 1; rank < BOARD_RANKS && file >= 0; rank++, file--)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + file));

        if ((1ULL << (rank * BOARD_FILES + file)) & block) break;
    }
    
    for (int rank = targetRank - 1, file = targetFile - 1; rank >= 0 && file >= 0; rank--, file--)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + file));

        if ((1ULL << (rank * BOARD_FILES + file)) & block) break;
    }
    
    return attacks;
}

U64 calculate_rook_attacks(Square square, U64 block)
{
    U64 attacks = 0ULL;
    
    int targetRank = square / BOARD_FILES;
    int targetFile = square % BOARD_FILES;
    
    for (int rank = targetRank + 1; rank < BOARD_RANKS; rank++)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + targetFile));

        if ((1ULL << (rank * BOARD_FILES + targetFile)) & block) break;
    }
    
    for (int rank = targetRank - 1; rank >= 0; rank--)
    {
        attacks |= (1ULL << (rank * BOARD_FILES + targetFile));

        if ((1ULL << (rank * BOARD_FILES + targetFile)) & block) break;
    }
    
    for (int file = targetFile + 1; file < BOARD_FILES; file++)
    {
        attacks |= (1ULL << (targetRank * BOARD_FILES + file));

        if ((1ULL << (targetRank * BOARD_FILES + file)) & block) break;
    }
    
    for (int file = targetFile - 1; file >= 0; file--)
    {
        attacks |= (1ULL << (targetRank * BOARD_FILES + file));

        if ((1ULL << (targetRank * BOARD_FILES + file)) & block) break;
    }
    
    return attacks;
}
