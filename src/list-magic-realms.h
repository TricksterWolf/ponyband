/**
 * \file list-magic-realms.h
 * \brief Spell realms
 */
/*	index		spell stat	verb		spell noun	book/tval name	realm name*/
REALM(NONE,		STAT_STR,	"",		"",		"",		"")
REALM(ARCANE,		STAT_INT,	"cast",		"spell",	"spellbook",    "arcane")
REALM(DARK,		STAT_INT,	"invoke",	"malediction", 	"dark tome",    "dark")
REALM(ALCHEMY,		STAT_INT,	"brew", 	"recipe",	"formulary",	"alchemy")
REALM(RANDOM,		STAT_WIS,	"play",         "trick", 	"prop",         "random")
REALM(ANIMAL,		STAT_CHA,	"forge",	"bond",         "bestiary",	"animal")
REALM(EARTH,		STAT_STR,	"complete",	"ritual",	"almanac",	"earth")
REALM(WEATHER,		STAT_DEX,	"execute",	"stunt",	"swag item",	"weather")
REALM(MUSIC,            STAT_CHA,       "perform",      "song",         "songbook",     "musical")
REALM(HEALING,          STAT_CHA,       "produce",      "cure",         "medkit",       "healing")