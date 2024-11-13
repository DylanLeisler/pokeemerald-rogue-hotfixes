import re
import random

NUMBER_TO_GENERATE = 100


abilities = """
    ABILITY_GOOD_AS_GOLD,
    ABILITY_WATER_ABSORB,
    ABILITY_VOLT_ABSORB,
    ABILITY_FLASH_FIRE,
    ABILITY_SPEED_BOOST,
    ABILITY_ADAPTABILITY,
    ABILITY_INTIMIDATE,
    ABILITY_DRIZZLE,
    ABILITY_DROUGHT,
    ABILITY_SNOW_WARNING,
    ABILITY_SAND_STREAM,
    ABILITY_UNBURDEN,
    ABILITY_NEUTRALIZING_GAS,
    ABILITY_ELECTRIC_SURGE,
    ABILITY_PSYCHIC_SURGE,
    ABILITY_MISTY_SURGE,
    ABILITY_GRASSY_SURGE,
    ABILITY_PROTEAN,
    ABILITY_MAGIC_BOUNCE,
    ABILITY_MOXIE,
    ABILITY_WANDERING_SPIRIT,
    ABILITY_REGENERATOR,
    ABILITY_SUPER_LUCK,
    ABILITY_CONTRARY,
    ABILITY_NO_GUARD,
    ABILITY_SKILL_LINK,
    ABILITY_MIRROR_ARMOR,
    ABILITY_GUTS,
    ABILITY_THICK_FAT,
    ABILITY_INTREPID_SWORD,
    ABILITY_SERENE_GRACE,
    ABILITY_DAUNTLESS_SHIELD,
    ABILITY_WEAK_ARMOR,
    ABILITY_TOXIC_DEBRIS,
    ABILITY_LONG_REACH,
    ABILITY_GORILLA_TACTICS,
    ABILITY_TOUGH_CLAWS,
    ABILITY_CLEAR_BODY,
    ABILITY_TECHNICIAN,
    ABILITY_EMERGENCY_EXIT,
    ABILITY_PRANKSTER,
    ABILITY_BEADS_OF_RUIN,
    ABILITY_SWORD_OF_RUIN,
    ABILITY_LEVITATE,
    ABILITY_MULTISCALE,
    ABILITY_VESSEL_OF_RUIN,
    ABILITY_POISON_HEAL,
    ABILITY_NEUROFORCE,
    ABILITY_MOODY,
    ABILITY_TABLETS_OF_RUIN,
    ABILITY_COMATOSE,
    ABILITY_ANGER_SHELL,
    ABILITY_SIMPLE,
    ABILITY_UNAWARE,
    ABILITY_HARVEST,
    ABILITY_MOLD_BREAKER,
    ABILITY_SHEER_FORCE,
    ABILITY_OVERCOAT,
    ABILITY_ROUGH_SKIN,
    ABILITY_DEFIANT,
    ABILITY_PERISH_BODY,
    ABILITY_SNIPER,
    ABILITY_COLOR_CHANGE,
    ABILITY_AERILATE,
    ABILITY_BAD_DREAMS,
    ABILITY_BEAST_BOOST,
    ABILITY_CLOUD_NINE,
    ABILITY_COMPETITIVE,
    ABILITY_COMPOUND_EYES,
    ABILITY_CORROSION,
    ABILITY_COTTON_DOWN,
    ABILITY_CUD_CHEW,
    ABILITY_DELTA_STREAM,
    ABILITY_DESOLATE_LAND,
    ABILITY_DOWNLOAD,
    ABILITY_DRAGONS_MAW,
    ABILITY_DRY_SKIN,
    ABILITY_EARTH_EATER,
    ABILITY_EFFECT_SPORE,
    ABILITY_FILTER,
    ABILITY_FLARE_BOOST,
    ABILITY_FUR_COAT,
    ABILITY_GALE_WINGS,
    ABILITY_GALVANIZE,
    ABILITY_HUSTLE,
    ABILITY_ICE_SCALES,
    ABILITY_LIGHTNING_ROD,
    ABILITY_MAGIC_GUARD,
    ABILITY_MEGA_LAUNCHER,
    ABILITY_MERCILESS,
    ABILITY_MINDS_EYE,
    ABILITY_MOTOR_DRIVE,
    ABILITY_NORMALIZE,
    ABILITY_OBLIVIOUS,
    ABILITY_OWN_TEMPO,
    ABILITY_PIXILATE,
    ABILITY_POISON_PUPPETEER,
    ABILITY_PRIMORDIAL_SEA,
    ABILITY_PROTOSYNTHESIS,
    ABILITY_PUNK_ROCK,
    ABILITY_PURIFYING_SALT,
    ABILITY_QUARK_DRIVE,
    ABILITY_QUEENLY_MAJESTY,
    ABILITY_RECKLESS,
    ABILITY_REFRIGERATE,
    ABILITY_RIVALRY,
    ABILITY_ROCKY_PAYLOAD,
    ABILITY_SAP_SIPPER,
    ABILITY_SHADOW_TAG,
    ABILITY_SHARPNESS,
    ABILITY_SHED_SKIN,
    ABILITY_SHIELD_DUST,
    ABILITY_SOUL_HEART,
    ABILITY_STAMINA,
    ABILITY_STEELY_SPIRIT,
    ABILITY_STENCH,
    ABILITY_STORM_DRAIN,
    ABILITY_STRONG_JAW,
    ABILITY_STURDY,
    ABILITY_TINTED_LENS,
    ABILITY_TOXIC_CHAIN,
    ABILITY_TRACE,
    ABILITY_TRANSISTOR,
    ABILITY_TRIAGE,
    ABILITY_WATER_BUBBLE,
    ABILITY_WELL_BAKED_BODY,
    ABILITY_WIND_RIDER,
"""

moves = """
    MOVE_HYDRO_STEAM,
    MOVE_BOUNCY_BUBBLE,
    MOVE_TORCH_SONG,
    MOVE_AQUA_STEP,
    MOVE_STONE_AXE,
    MOVE_TRIPLE_ARROWS,
    MOVE_SANDSEAR_STORM,
    MOVE_SURGING_STRIKES,
    MOVE_PARTING_SHOT,
    MOVE_GRAV_APPLE,
    MOVE_CORE_ENFORCER,
    MOVE_THOUSAND_ARROWS,
    MOVE_BURNING_BULWARK,
    MOVE_DIAMOND_STORM,
    MOVE_MAGICAL_TORQUE,
    MOVE_FREEZE_DRY,
    MOVE_SHADOW_FORCE,
    MOVE_V_CREATE,
    MOVE_SECRET_SWORD,
    MOVE_TRICK_ROOM,
    MOVE_SPORE,
    MOVE_NOXIOUS_TORQUE,
    MOVE_TAIL_GLOW,
    MOVE_CLOSE_COMBAT,
    MOVE_U_TURN,
    MOVE_SHIFT_GEAR,
    MOVE_TAKE_HEART,
    MOVE_FLOWER_TRICK,
    MOVE_SUNSTEEL_STRIKE,
    MOVE_CEASELESS_EDGE,
    MOVE_COSMIC_POWER,
    MOVE_FAKE_OUT,
    MOVE_EXTREME_SPEED,
    MOVE_POWER_GEM,
    MOVE_RAPID_SPIN,
    MOVE_LEECH_LIFE,
    MOVE_SYNTHESIS,
    MOVE_ELECTRO_DRIFT,
    MOVE_SWORDS_DANCE,
    MOVE_THUNDERCLAP,
    MOVE_GIGATON_HAMMER,
    MOVE_SALT_CURE,
    MOVE_WICKED_BLOW,
    MOVE_SPECTRAL_THIEF,
    MOVE_METEOR_BEAM,
    MOVE_SPIRIT_BREAK,
    MOVE_SHORE_UP,
    MOVE_NO_RETREAT,
    MOVE_SHED_TAIL,
    MOVE_BELLY_DRUM,
    MOVE_TOPSY_TURVY,
    MOVE_QUIVER_DANCE,
    MOVE_BATON_PASS,
    MOVE_ASTRAL_BARRAGE,
    MOVE_ARMOR_CANNON,
    MOVE_POWER_TRIP,
    MOVE_BOOMBURST,
    MOVE_SUCKER_PUNCH,
    MOVE_COLLISION_COURSE,
    MOVE_MIGHTY_CLEAVE,
    MOVE_FIRST_IMPRESSION,
    MOVE_RAGE_FIST,
    MOVE_ACROBATICS,
    MOVE_AEROBLAST,
    MOVE_APPLE_ACID,
    MOVE_AURORA_VEIL,
    MOVE_BADDY_BAD,
    MOVE_BEAK_BLAST,
    MOVE_BITTER_BLADE,
    MOVE_BOLT_BEAK,
    MOVE_BUZZY_BUZZ,
    MOVE_CHILLY_RECEPTION,
    MOVE_CLANGOROUS_SOUL,
    MOVE_COIL,
    MOVE_COURT_CHANGE,
    MOVE_DIRE_CLAW,
    MOVE_DRACO_METEOR,
    MOVE_DRAGON_ASCENT,
    MOVE_DRAGON_DARTS,
    MOVE_DRAGON_ENERGY,
    MOVE_EARTH_POWER,
    MOVE_ELECTRO_SHOT,
    MOVE_ERUPTION,
    MOVE_ESPER_WING,
    MOVE_FIERY_WRATH,
    MOVE_FISHIOUS_REND,
    MOVE_FLEUR_CANNON,
    MOVE_FLOATY_FALL,
    MOVE_FLYING_PRESS,
    MOVE_FREEZY_FROST,
    MOVE_GEOMANCY,
    MOVE_GLACIAL_LANCE,
    MOVE_GLAIVE_RUSH,
    MOVE_GLITZY_GLOW,
    MOVE_HEADLONG_RUSH,
    MOVE_HYPER_DRILL,
    MOVE_INFERNAL_PARADE,
    MOVE_JUNGLE_HEALING,
    MOVE_LEAF_STORM,
    MOVE_MAKE_IT_RAIN,
    MOVE_MALIGNANT_CHAIN,
    MOVE_MATCHA_GOTCHA,
    MOVE_MOONBLAST,
    MOVE_MORTAL_SPIN,
    MOVE_OBLIVION_WING,
    MOVE_PHOTON_GEYSER,
    MOVE_POLLEN_PUFF,
    MOVE_POPULATION_BOMB,
    MOVE_PSYBLADE,
    MOVE_PSYCHIC_FANGS,
    MOVE_PSYCHO_BOOST,
    MOVE_SAPPY_SEED,
    MOVE_SEARING_SHOT,
    MOVE_SHEER_COLD,
    MOVE_SHELL_SMASH,
    MOVE_SIZZLY_SLIDE,
    MOVE_SPARKLY_SWIRL,
    MOVE_SPLISHY_SPLASH,
    MOVE_STICKY_WEB,
    MOVE_STRENGTH_SAP,
    MOVE_STUFF_CHEEKS,
    MOVE_TACHYON_CUTTER,
    MOVE_TERRAIN_PULSE,
    MOVE_TOXIC_SPIKES,
    MOVE_VICTORY_DANCE,
    MOVE_WATER_SPOUT,
    MOVE_ZIPPY_ZAP,
"""

# Enclose each ability and move in double quotes and add the list brackets
result = "[" + re.sub(r'\b([A-Z_]+)\b(?=,)', r'"\1"', abilities).strip() + "]"

# print(result)

# Convert result to an actual Python list object
abilities_list = eval(result)  # Or use json.loads(result) after importing json
# print(abilities_list)

result = "[" + re.sub(r'\b([A-Z_]+)\b(?=,)', r'"\1"', moves).strip() + "]"

# print(result)

# Convert result to an actual Python list object
moves_list = eval(result)  # Or use json.loads(result) after importing json
# print(moves_list)



def rarity_generator():

    while True:
    # Generate a random number from 0 to 6
        rand_value = random.randint(0, 6)

        # Determine the rarity based on the generated value
        if rand_value == 0:
            rarity = {
                "value": "EXOTIC",
                "moves": 0,
                "abilities": 0,
            }
        elif rand_value in (1, 2):
            rarity = {
                "value": "EPIC",
                "moves": 3,
                "abilities": 1,
            }
        elif rand_value in (3, 4):
            rarity = {
                "value": "RARE",
                "moves": 1,
                "abilities": 1,
            }
        elif rand_value in (5, 6):
            rarity = {
                "value": "COMMON",
                "moves": 2,
                "abilities": 0,
            }

        # Apply the 75% chance to convert "EXOTIC" to "EPIC"
        if rarity["value"] == "EXOTIC":
            if random.random() < 0.75:  # 75% chance
                rarity["value"] = "EPIC"
                rarity["moves"] = 3
                rarity["abilities"] = 1
        
        yield rarity

rarities_dict = {
    "EXOTIC": [],
    "EPIC": [],
    "RARE": [],
    "COMMON": [],
}

rarity_gen = rarity_generator()

for _ in range(NUMBER_TO_GENERATE):
    generation = next(rarity_gen)
    rarities_dict[generation["value"]].append(generation)


rarities_count = {
    "EXOTIC": len(rarities_dict["EXOTIC"]),
    "EPIC": len(rarities_dict["EPIC"]),
    "RARE": len(rarities_dict["RARE"]),
    "COMMON": len(rarities_dict["COMMON"]),
}


for rarity in rarities_dict:
    if rarity == "EXOTIC":
        continue

    for mon in rarities_dict[rarity]:
        print()
        print(f"{mon["value"]}:")
        for ability in range(mon["abilities"]):
            print(f"\tABILITY: {random.choice(abilities_list)[8:]}")

        mon_move_list = []
        for move in range(mon["moves"]):
            mon_move_list.append(random.choice(moves_list)[5:])
        
        print(f"\tMOVES:\t{" | ".join(mon_move_list)}")

print()        
print(rarities_count)