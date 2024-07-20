#include "game.h"

static GDF_Game* GAME;

bool GDF_GAME_Init()
{
    GAME = GDF_Malloc(sizeof(*GAME), GDF_MEMTAG_GAME);

    // TODO! uncomment later, the game will be initilaized in world state for now.
    // GAME->current_screen = GDF_GAME_SCREEN_MAIN_MENU;
    // GAME->current_screen_type = GDF_GAME_SCREENTYPE_GUI_MENU;
    // GAME->main_player = NULL;
    // GAME->world = NULL;

    GAME->current_screen = GDF_GAME_SCREEN_IN_WORLD;
    GAME->current_screen_type = GDF_GAME_SCREENTYPE_WORLD;
    GAME->main_player = NULL;
    GAME->world = NULL;
}

GDF_Game* GDF_GAME_GetInstance() 
{
    return GAME;
}