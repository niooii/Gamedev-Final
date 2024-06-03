#include "block.h"

#define BLOCK_DECL(blockid, name, strength, interactable)\
{blockid, name, strength, interactable},\

static BlockInfo block_infos[] = {
    BLOCK_DECL(BLOCK_ID_AIR, "Air", 0, false)
    BLOCK_DECL(BLOCK_ID_DIRT, "Dirt", 2, true)
    BLOCK_DECL(BLOCK_ID_GRASS, "Grass", 2, true)
};