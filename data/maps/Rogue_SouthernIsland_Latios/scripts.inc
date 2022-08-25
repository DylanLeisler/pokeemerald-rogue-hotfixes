Rogue_SouthernIsland_Latios_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, SouthernIsland_Interior_OnResume
	.byte 0


Rogue_SouthernIsland_Interior_Latios_TryLatiEncounter::
	setvar VAR_ROAMER_POKEMON, 0
	setflag FLAG_ENABLE_SHIP_SOUTHERN_ISLAND
	clearflag FLAG_DEFEATED_LATIAS_OR_LATIOS
	clearflag FLAG_CAUGHT_LATIAS_OR_LATIOS
	call SouthernIsland_Interior_EventScript_SetUpLatios
	call SouthernIsland_Interior_EventScript_TryLatiEncounter
	call Common_EventScript_RemoveStaticPokemon
	return
