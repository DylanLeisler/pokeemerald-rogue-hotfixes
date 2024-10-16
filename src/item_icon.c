#include "global.h"
#include "decompress.h"
#include "graphics.h"
#include "item_icon.h"
#include "malloc.h"
#include "palette.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "window.h"
#include "constants/items.h"

// EWRAM vars
EWRAM_DATA u8 *gItemIconDecompressionBuffer = NULL;
EWRAM_DATA u8 *gItemIcon4x4Buffer = NULL;

// const rom data
#include "data/item_icon_table.h"

#include "rogue_baked.h"

static const struct OamData sOamData_ItemIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 2,
    .affineParam = 0
};

static const union AnimCmd sSpriteAnim_ItemIcon[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_ItemIcon[] =
{
    sSpriteAnim_ItemIcon
};

const struct SpriteTemplate gItemIconSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &sOamData_ItemIcon,
    .anims = sSpriteAnimTable_ItemIcon,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// code
bool8 AllocItemIconTemporaryBuffers(void)
{
    gItemIconDecompressionBuffer = Alloc(0x120);
    if (gItemIconDecompressionBuffer == NULL)
        return FALSE;

    gItemIcon4x4Buffer = AllocZeroed(0x200);
    if (gItemIcon4x4Buffer == NULL)
    {
        Free(gItemIconDecompressionBuffer);
        return FALSE;
    }

    return TRUE;
}

void FreeItemIconTemporaryBuffers(void)
{
    Free(gItemIconDecompressionBuffer);
    Free(gItemIcon4x4Buffer);
}

void CopyItemIconPicTo4x4Buffer(const void *src, void *dest)
{
    u8 i;

    for (i = 0; i < 3; i++)
        CpuCopy16(src + i * 96, dest + i * 128, 0x60);
}

u8 AddItemIconSprite(u16 tilesTag, u16 paletteTag, u16 itemId)
{
    const u32* image = (const u32*)GetItemIconPicOrPalette(itemId, 0);
    const u32* palette = (const u32*)GetItemIconPicOrPalette(itemId, 1);

    return AddIconSprite(tilesTag, paletteTag, image, palette);
}

u8 AddIconSprite(u16 tilesTag, u16 paletteTag, const u32* image, const u32* palette)
{
    if (!AllocItemIconTemporaryBuffers())
    {
        return MAX_SPRITES;
    }
    else
    {
        u8 spriteId;
        struct SpriteSheet spriteSheet;
        struct CompressedSpritePalette spritePalette;
        struct SpriteTemplate *spriteTemplate;

        LZDecompressWram(image, gItemIconDecompressionBuffer);
        CopyItemIconPicTo4x4Buffer(gItemIconDecompressionBuffer, gItemIcon4x4Buffer);
        spriteSheet.data = gItemIcon4x4Buffer;
        spriteSheet.size = 0x200;
        spriteSheet.tag = tilesTag;
        LoadSpriteSheet(&spriteSheet);

        spritePalette.data = palette;
        spritePalette.tag = paletteTag;
        LoadCompressedSpritePalette(&spritePalette);

        spriteTemplate = Alloc(sizeof(*spriteTemplate));
        CpuCopy16(&gItemIconSpriteTemplate, spriteTemplate, sizeof(*spriteTemplate));
        spriteTemplate->tileTag = tilesTag;
        spriteTemplate->paletteTag = paletteTag;
        spriteId = CreateSprite(spriteTemplate, 0, 0, 0);

        FreeItemIconTemporaryBuffers();
        Free(spriteTemplate);

        return spriteId;
    }
}

u8 BlitItemIconToWindow(u16 itemId, u8 windowId, u16 x, u16 y, void * paletteDest) 
{
    if (!AllocItemIconTemporaryBuffers())
        return 16;

    LZDecompressWram(GetItemIconPicOrPalette(itemId, 0), gItemIconDecompressionBuffer);
    CopyItemIconPicTo4x4Buffer(gItemIconDecompressionBuffer, gItemIcon4x4Buffer);
    BlitBitmapToWindow(windowId, gItemIcon4x4Buffer, x, y, 32, 32);

    // if paletteDest is nonzero, copies the decompressed palette directly into it
    // otherwise, loads the compressed palette into the windowId's BG palette ID
    if (paletteDest) 
    {
        LZDecompressWram(GetItemIconPicOrPalette(itemId, 1), gPaletteDecompressionBuffer);
        CpuFastCopy(gPaletteDecompressionBuffer, paletteDest, PLTT_SIZE_4BPP);
    } 
    else 
    {
        LoadCompressedPalette(GetItemIconPicOrPalette(itemId, 1), BG_PLTT_ID(gWindows[windowId].window.paletteNum), PLTT_SIZE_4BPP);
    }
    FreeItemIconTemporaryBuffers();
    return 0;
}

u8 BlitPokemonIconToWindow(u16 species, u8 windowId, u16 x, u16 y, void * paletteDest)
{
    u8 palId;

    if (!AllocItemIconTemporaryBuffers())
        return 16;

    palId = GetMonIconPaletteIndexFromSpecies(species);

    //LZDecompressWram(GetMonIconTiles(species, FALSE), gItemIconDecompressionBuffer);
    //CopyItemIconPicTo4x4Buffer(GetMonIconTiles(species, FALSE), gItemIcon4x4Buffer);
    
    BlitBitmapToWindow(windowId, GetMonIconTiles(species, FALSE), x, y, 32, 32);

    //gMonIconPaletteTable

    // if paletteDest is nonzero, copies the decompressed palette directly into it
    // otherwise, loads the compressed palette into the windowId's BG palette ID
    if (paletteDest) 
    {
        CpuFastCopy(gMonIconPaletteTable[palId].data, paletteDest, PLTT_SIZE_4BPP);
    } 
    else 
    {
        LoadPalette(gMonIconPaletteTable[palId].data, BG_PLTT_ID(gWindows[windowId].window.paletteNum), PLTT_SIZE_4BPP);
    }
    FreeItemIconTemporaryBuffers();
    return 0;
}

u8 BlitCustomItemIconToWindow(u8 windowId, u16 x, u16 y, void * paletteDest, u32 const* icon, u32 const* palette) 
{
    if (!AllocItemIconTemporaryBuffers())
        return 16;

    LZDecompressWram(icon, gItemIconDecompressionBuffer);
    CopyItemIconPicTo4x4Buffer(gItemIconDecompressionBuffer, gItemIcon4x4Buffer);
    BlitBitmapToWindow(windowId, gItemIcon4x4Buffer, x, y, 32, 32);

    // if paletteDest is nonzero, copies the decompressed palette directly into it
    // otherwise, loads the compressed palette into the windowId's BG palette ID
    if (paletteDest) 
    {
        LZDecompressWram(palette, gPaletteDecompressionBuffer);
        CpuFastCopy(gPaletteDecompressionBuffer, paletteDest, PLTT_SIZE_4BPP);
    } 
    else 
    {
        LoadCompressedPalette(palette, BG_PLTT_ID(gWindows[windowId].window.paletteNum), PLTT_SIZE_4BPP);
    }
    FreeItemIconTemporaryBuffers();
    return 0;
}

u8 AddCustomItemIconSprite(const struct SpriteTemplate *customSpriteTemplate, u16 tilesTag, u16 paletteTag, u16 itemId)
{
    if (!AllocItemIconTemporaryBuffers())
    {
        return MAX_SPRITES;
    }
    else
    {
        u8 spriteId;
        struct SpriteSheet spriteSheet;
        struct CompressedSpritePalette spritePalette;
        struct SpriteTemplate *spriteTemplate;

        LZDecompressWram(GetItemIconPicOrPalette(itemId, 0), gItemIconDecompressionBuffer);
        CopyItemIconPicTo4x4Buffer(gItemIconDecompressionBuffer, gItemIcon4x4Buffer);
        spriteSheet.data = gItemIcon4x4Buffer;
        spriteSheet.size = 0x200;
        spriteSheet.tag = tilesTag;
        LoadSpriteSheet(&spriteSheet);

        spritePalette.data = GetItemIconPicOrPalette(itemId, 1);
        spritePalette.tag = paletteTag;
        LoadCompressedSpritePalette(&spritePalette);

        spriteTemplate = Alloc(sizeof(*spriteTemplate));
        CpuCopy16(customSpriteTemplate, spriteTemplate, sizeof(*spriteTemplate));
        spriteTemplate->tileTag = tilesTag;
        spriteTemplate->paletteTag = paletteTag;
        spriteId = CreateSprite(spriteTemplate, 0, 0, 0);

        FreeItemIconTemporaryBuffers();
        Free(spriteTemplate);

        return spriteId;
    }
}

const void *GetItemIconPicOrPalette(u16 itemId, u8 which)
{
    if (itemId == 0xFFFF)
        itemId = ITEM_FIELD_ARROW;
    else if (itemId >= ITEMS_COUNT)
        itemId = 0;

    return Rogue_GetItemIconPicOrPalette(itemId, which);
}
