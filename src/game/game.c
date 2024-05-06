#include "game.h"

static GDF_Game* GAME_INSTANCE;

GDF_Game* GDF_GetGame() 
{
    return GAME_INSTANCE;
}