#pragma once

#include "core.h"
#include "game/entity/humanoid.h"

typedef enum GDF_ITEM_ID {
    GDF_ITEM_ID_DiamondSword,
    // no violence!! :nerd:
    GDF_ITEM_ID_BigStick,
} GDF_ITEM_ID;

typedef enum GDF_ITEM_Type {
    GDF_ITEM_TYPE_Weapon,
    GDF_ITEM_TYPE_Block,
    GDF_ITEM_TYPE_Consumable,
} GDF_ITEM_Type;

typedef struct GDF_Item {
    GDF_ITEM_Type item_type;
    GDF_ITEM_ID item_id;
    u16 stack_size;
    u16 max_stack_size;
    void* other_info;
    // press down left click
    void (*on_attack_start)(struct GDF_Item* item_used, HumanoidEntity* player);
    // release left click
    void (*on_attack_finish)(struct GDF_Item* item_used, HumanoidEntity* player);
    // press down right click
    void (*on_use_start)(struct GDF_Item* item_used, HumanoidEntity* player);
    // release right click
    void (*on_use_finish)(struct GDF_Item* item_used, HumanoidEntity* player);
    // TODO! texture later
} GDF_Item;

// big switch inc unlucky
GDF_Item* GDF_MakeItem(GDF_ITEM_ID item_id);
void GDF_DestroyItem(GDF_Item*);
GDF_ITEM_Type GDF_GetItemType(GDF_ITEM_ID item_id);

// some default implementaitons of useful callbacks
void GDF_ITEMFUNC_BLOCK_UseStart(GDF_Item* block, HumanoidEntity* player);
void GDF_ITEMFUNC_WEAPON_AttackStart(GDF_Item* weapon, HumanoidEntity* player);
void GDF_ITEMFUNC_CONSUMABLE_UseStart(GDF_Item* consumable, HumanoidEntity* player);
void GDF_ITEMFUNC_CONSUMABLE_UseEnd(GDF_Item* consumable, HumanoidEntity* player);