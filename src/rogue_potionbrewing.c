#include "global.h"
#include "event_data.h"
#include "item.h"
#include "item_menu.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "rogue_potionbrewing.h"
#include "rogue_popup.h"
#include "script.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "window.h" 
#include "constants/songs.h"

// I don't think this is needed since I don't use the traditional windows method but ehh
enum
{
    WIN_QUANTITY_IN_BAG,
    WIN_QUANTITY_COST,
};

// Copied off shop.h
// Minus the baseblock. I had to manually find it via trial and error. God.
// But basically, what you want to do is to look for a free blank space
// previousBaseBlock + (prevWindowWidth * prevWindowHeight)
// Protip: MGBA has a tile viewer

static const struct WindowTemplate sBrewInputWindowTemplates[] =
{
    [WIN_QUANTITY_IN_BAG] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 11,
        .width = 10,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x10,
    },
    [WIN_QUANTITY_COST] = {
        .bg = 0,
        .tilemapLeft = 18,
        .tilemapTop = 11,
        .width = 10,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x30,
    },
    DUMMY_WIN_TEMPLATE
};

void StartBrewInput(void);
static void Task_InitBrewInput(u8 taskId); 
static void Task_ShowWindows(u8 taskId);
static void Task_DestroyBrewWindows(u8 taskId);
static void Task_HowManyHandleInput(u8 taskId); //Don't load memory extensive stuff here
static void PrintBrewCost(u8 taskId); // This one will change according to input

/*
* So, here's the gist
* - Init the UI
* - Handle the input
* - Destroy the UI
* I don't think I need to init the windows just for this, this is not a menu after all 
*/

//This is more readable than gTasks[taskId].data[x] 
//The only cost is to add "s16 *data = gTasks[taskId].data;" but that's fair I think
//Why not struct? because I don't understand how it works lol. this one seems to persist per task
#define winInBag                data[0]
#define winCost                 data[1]
#define berryCount              data[2]
#define tItemCount              data[3]
#define berryCost               data[4]
#define potionType              data[5]

void StartBrewInput(void)
{
    CreateTask(Task_InitBrewInput, 1); //I don't understand the task system that much yet, but this works when I use the dialogue so ig it's ok for now.
}

static void Task_InitBrewInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    LockPlayerFieldControls(); // It only works if I put it here? Probably has something to do with how task handled

    // Load the windows here. You only want to do it once, otherwise it just stops spawning the window lol
    winInBag = AddWindow(&sBrewInputWindowTemplates[0]); // "In bag" window
    winCost = AddWindow(&sBrewInputWindowTemplates[1]); // "Cost" window
    berryCount = GetItemCountInBag(ITEM_ORAN_BERRY);

    // Numbers
    tItemCount = 1;
    berryCost = gSpecialVar_0x8000; // From the poryscript
    potionType = gSpecialVar_0x8001;

    LoadUserWindowBorderGfx(0, 1, BG_PLTT_ID(13)); //Loads the player's frame
    DisplayItemMessageOnField(taskId, gText_HowManyToBrew, Task_ShowWindows);
}

static void Task_ShowWindows(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 quantityInBag = GetItemCountInBag(potionType);

    // Draw the windows. Set them to FALSE so we can copy the window later AFTER the texts are loaded
    // We only need to initialize them once anyway. This prevents weird artifact from overlapping on initialization, too
    DrawStdFrameWithCustomTileAndPalette(winInBag, FALSE, 1, 13);
    DrawStdFrameWithCustomTileAndPalette(winCost, FALSE, 1, 13);

    // Then process the magic numbers
    ConvertIntToDecimalStringN(gStringVar1, quantityInBag, STR_CONV_MODE_RIGHT_ALIGN, MAX_ITEM_DIGITS + 1);
    StringExpandPlaceholders(gStringVar4, gText_InBagVar1);
    AddTextPrinterParameterized(winInBag, FONT_NORMAL, gStringVar4, 0, 1, TEXT_SKIP_DRAW, NULL);

    PrintBrewCost(taskId);

    CopyWindowToVram(winInBag, COPYWIN_FULL);
    ScheduleBgCopyTilemapToVram(0); // Use this once

    gTasks[taskId].func = Task_HowManyHandleInput;
}

static void Task_HowManyHandleInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data; // to access the tItemCount

    if (AdjustQuantityAccordingToDPadInput(&tItemCount, berryCount / berryCost) == TRUE)
    {
        PrintBrewCost(taskId);
    } 
    else
    {
        if (JOY_NEW(A_BUTTON))  // Confirm selection
        {
            PlaySE(SE_SELECT);
            // Since this task is handled per frame, I think it's wise to just put the variables here.
            u8 totalCost = tItemCount * berryCost;
            RemoveBagItem(ITEM_ORAN_BERRY, totalCost);
            AddBagItem(potionType, tItemCount);
            Rogue_PushPopup_LostItem(ITEM_ORAN_BERRY, totalCost);
            Rogue_PushPopup_AddItem(potionType, tItemCount);
            Task_DestroyBrewWindows(taskId);
        }
        else if (JOY_NEW(B_BUTTON))  // Cancel selection
        {
            Task_DestroyBrewWindows(taskId);
        }
    }
}

//I don't plan on using this anywhere else
static u8 const sText_BerryReduction[] = _("{STR_VAR_1}-{COLOR LIGHT_RED}{SHADOW RED}{STR_VAR_2}");

static void PrintBrewCost(u8 taskId)
{
    s16 *data = gTasks[taskId].data; // To access the tItemCount
    FillWindowPixelBuffer(winCost, PIXEL_FILL(1)); //Clears the previous text. This window updates often so it's needed.

    //xAMOUNT
    ConvertIntToDecimalStringN(gStringVar1, tItemCount, STR_CONV_MODE_LEADING_ZEROS, SHOP_ITEM_CAPACITY_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    AddTextPrinterParameterized(winCost, FONT_NORMAL, gStringVar4, 8, 1, TEXT_SKIP_DRAW, NULL);

    //Berry in bag - berry needed
    ConvertIntToDecimalStringN(gStringVar1, berryCount, STR_CONV_MODE_RIGHT_ALIGN, MAX_ITEM_DIGITS + 1);
    ConvertIntToDecimalStringN(gStringVar2, tItemCount * berryCost, STR_CONV_MODE_LEFT_ALIGN, MAX_ITEM_DIGITS + 1);
    StringExpandPlaceholders(gStringVar4, sText_BerryReduction);
    AddTextPrinterParameterized(winCost, FONT_NORMAL, gStringVar4, 30, 1, TEXT_SKIP_DRAW, NULL);

    CopyWindowToVram(winCost, COPYWIN_FULL);
}

static void Task_DestroyBrewWindows(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    //Clean the windows etc
    ClearStdWindowAndFrameToTransparent(winInBag, TRUE);
    ClearStdWindowAndFrameToTransparent(winCost, TRUE);
    RemoveWindow(winInBag);
    RemoveWindow(winCost);
    ClearDialogWindowAndFrame(0, TRUE); // Clear the dialogue box. Took me a while to find ngl

    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

#undef winInBag
#undef winCost
#undef berryCount
#undef tItemCount
#undef berryCost
#undef potionType