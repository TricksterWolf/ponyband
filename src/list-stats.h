/**
 * \file src/list-stats.h
 * \brief player stats
 *
 * index: the stat number
 * power: base power rating for the stat as an object modifier
 * mult: weight of this stat as an object modifier relative to others
 * message: what is printed when the stat is IDd (but see also obj-identify.c 
 * and list-slays.h)
 */
/* index       	power	sustain			sust_p	mult		pos adj		neg adj		name */
STAT(STR,        9,	SUST_STR,		9,	13,		"honest",	"deceitful",	"honesty")
STAT(INT,        5,	SUST_INT,		4,	10,		"magical",	"powerless",	"magic")
STAT(WIS,        5,	SUST_WIS,		4,	10,		"joyous",	"bitter",	"laughter")
STAT(DEX,        8,	SUST_DEX,		7,	10,		"loyal",	"selfish",	"loyalty")
STAT(CON,        12,	SUST_CON,		8,	15,		"generous",	"greedy",	"generosity")
STAT(CHA,        5,	SUST_CHA,		4,      10,             "kind",		"mean",		"kindness")
