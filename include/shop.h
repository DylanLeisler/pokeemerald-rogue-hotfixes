#ifndef GUARD_SHOP_H
#define GUARD_SHOP_H

extern struct ItemSlot gMartPurchaseHistory[3];

void CreatePokemartMenu(const u16 *);
void CreateDecorationShop1Menu(const u16 *);
void CreateDecorationShop2Menu(const u16 *);
void CreatePokemartMenuWithMinPrice(const u16 *, u16 minPrice);
void CreateDynamicPokemartMenu(const u16 category);
void CB2_ExitSellMenu(void);
void CB2_ExitAutoSellMenu(void);
void Task_HandleAutoSellYes(u8 taskId);
void Task_HandleAutoSellNo(u8 taskId);
void CreateYesNoMenuWithCallbacks(u8 taskId, const struct WindowTemplate* template, u8 unused1, u8 unused2, u8 unused3, u16 tileStart, u8 palette, const struct YesNoFuncTable* yesNo);
void ReturnToShopMenu(void);

#endif // GUARD_SHOP_H
