/**
 * \file player-calcs.c
 * \brief Player status calculation, signalling ui events based on 
 *	status changes.
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * Copyright (c) 2014 Nick McConnell
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "cave.h"
#include "game-event.h"
#include "game-input.h"
#include "game-world.h"
#include "init.h"
#include "mon-msg.h"
#include "mon-util.h"
#include "obj-gear.h"
#include "obj-identify.h"
#include "obj-ignore.h"
#include "obj-power.h"
#include "obj-tval.h"
#include "obj-util.h"
#include "player-calcs.h"
#include "player-spell.h"
#include "player-timed.h"
#include "player-util.h"

/**
 * Stat Table (INT) -- Magic devices
 */
const byte adj_int_dev[STAT_RANGE] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18 */,
	3	/* 19 */,
	3	/* 20 */,
	3	/* 21 */,
	3	/* 22 */,
	4	/* 23 */,
	4	/* 24 */,
	5	/* 25 */,
	5	/* 26 */,
	6	/* 27 */,
	6	/* 28 */,
	7	/* 29 */,
	7	/* 30 */,
	8	/* 31 */,
	8	/* 32 */,
	9	/* 33 */,
	9	/* 34 */,
	10	/* 35 */,
	10	/* 36 */,
	11	/* 37 */,
	11	/* 38 */,
	12	/* 39 */,
	13	/* 40+ */
};

/**
 * Stat Table (WIS) -- Saving throw
 */
const byte adj_wis_sav[STAT_RANGE] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18 */,
	3	/* 19 */,
	3	/* 20 */,
	3	/* 21 */,
	3	/* 22 */,
	4	/* 23 */,
	4	/* 24 */,
	5	/* 25 */,
	5	/* 26 */,
	6	/* 27 */,
	7	/* 28 */,
	8	/* 29 */,
	9	/* 30 */,
	10	/* 31 */,
	11	/* 32 */,
	12	/* 33 */,
	13	/* 34 */,
	14	/* 35 */,
	15	/* 36 */,
	16	/* 37 */,
	17	/* 38 */,
	18	/* 39 */,
	19	/* 40+ */
};


/**
 * Stat Table (DEX) -- disarming
 */
const byte adj_dex_dis[STAT_RANGE] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	4	/* 18 */,
	4	/* 19 */,
	4	/* 20 */,
	4	/* 21 */,
	5	/* 22 */,
	5	/* 23 */,
	5	/* 24 */,
	6	/* 25 */,
	6	/* 26 */,
	7	/* 27 */,
	8	/* 28 */,
	8	/* 29 */,
	8	/* 30 */,
	8	/* 31 */,
	8	/* 32 */,
	9	/* 33 */,
	9	/* 34 */,
	9	/* 35 */,
	9	/* 36 */,
	9	/* 37 */,
	10	/* 38 */,
	10	/* 39 */,
	10	/* 40+ */
};


/**
 * Stat Table (INT) -- disarming
 */
const byte adj_int_dis[STAT_RANGE] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18 */,
	3	/* 19 */,
	3	/* 20 */,
	4	/* 21 */,
	4	/* 22 */,
	5	/* 23 */,
	6	/* 24 */,
	7	/* 25 */,
	8	/* 26 */,
	9	/* 27 */,
	10	/* 28 */,
	10	/* 29 */,
	11	/* 30 */,
	12	/* 31 */,
	13	/* 32 */,
	14	/* 33 */,
	15	/* 34 */,
	16	/* 35 */,
	17	/* 36 */,
	18	/* 37 */,
	19	/* 38 */,
	19	/* 39 */,
	19	/* 40+ */
};

/**
 * Stat Table (DEX) -- bonus to ac (plus 128)
 */
const byte adj_dex_ta[STAT_RANGE] =
{
	128 + -4	/* 3 */,
	128 + -3	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 1	/* 15 */,
	128 + 1	/* 16 */,
	128 + 1	/* 17 */,
	128 + 2	/* 18 */,
	128 + 2	/* 19 */,
	128 + 2	/* 20 */,
	128 + 2	/* 21 */,
	128 + 2	/* 22 */,
	128 + 3	/* 23 */,
	128 + 3	/* 24 */,
	128 + 3	/* 25 */,
	128 + 4	/* 26 */,
	128 + 5	/* 27 */,
	128 + 6	/* 28 */,
	128 + 7	/* 29 */,
	128 + 8	/* 30 */,
	128 + 9	/* 31 */,
	128 + 9	/* 32 */,
	128 + 10	/* 33 */,
	128 + 11	/* 34 */,
	128 + 12	/* 35 */,
	128 + 13	/* 36 */,
	128 + 14	/* 37 */,
	128 + 15	/* 38 */,
	128 + 15	/* 39 */,
	128 + 15	/* 40+ */
};

/**
 * Stat Table (STR) -- bonus to dam (plus 128)
 */
const byte adj_str_td[STAT_RANGE] =
{
	128 + -2	/* 3 */,
	128 + -2	/* 4 */,
	128 + -1	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 2	/* 18 */,
	128 + 2	/* 19 */,
	128 + 3	/* 20 */,
	128 + 3	/* 21 */,
	128 + 3	/* 22 */,
	128 + 3	/* 23 */,
	128 + 3	/* 24 */,
	128 + 4	/* 25 */,
	128 + 5	/* 26 */,
	128 + 5	/* 27 */,
	128 + 6	/* 28 */,
	128 + 7	/* 29 */,
	128 + 8	/* 30 */,
	128 + 9	/* 31 */,
	128 + 10	/* 32 */,
	128 + 11	/* 33 */,
	128 + 12	/* 34 */,
	128 + 13	/* 35 */,
	128 + 14	/* 36 */,
	128 + 15	/* 37 */,
	128 + 16	/* 38 */,
	128 + 18	/* 39 */,
	128 + 20	/* 40+ */
};


/**
 * Stat Table (DEX) -- bonus to hit (plus 128)
 */
const byte adj_dex_th[STAT_RANGE] =
{
	128 + -3	/* 3 */,
	128 + -2	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + -1	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 3	/* 18 */,
	128 + 3	/* 19 */,
	128 + 3	/* 20 */,
	128 + 3	/* 21 */,
	128 + 3	/* 22 */,
	128 + 4	/* 23 */,
	128 + 4	/* 24 */,
	128 + 4	/* 25 */,
	128 + 4	/* 26 */,
	128 + 5	/* 27 */,
	128 + 6	/* 28 */,
	128 + 7	/* 29 */,
	128 + 8	/* 30 */,
	128 + 9	/* 31 */,
	128 + 9	/* 32 */,
	128 + 10	/* 33 */,
	128 + 11	/* 34 */,
	128 + 12	/* 35 */,
	128 + 13	/* 36 */,
	128 + 14	/* 37 */,
	128 + 15	/* 38 */,
	128 + 15	/* 39 */,
	128 + 15	/* 40+ */
};


/**
 * Stat Table (STR) -- bonus to hit (plus 128)
 */
const byte adj_str_th[STAT_RANGE] =
{
	128 + -3	/* 3 */,
	128 + -2	/* 4 */,
	128 + -1	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 0	/* 16 */,
	128 + 0	/* 17 */,
	128 + 1	/* 18 */,
	128 + 1	/* 19 */,
	128 + 1	/* 20 */,
	128 + 1	/* 21 */,
	128 + 1	/* 22 */,
	128 + 1	/* 23 */,
	128 + 1	/* 24 */,
	128 + 2	/* 25 */,
	128 + 3	/* 26 */,
	128 + 4	/* 27 */,
	128 + 5	/* 28 */,
	128 + 6	/* 29 */,
	128 + 7	/* 30 */,
	128 + 8	/* 31 */,
	128 + 9	/* 32 */,
	128 + 10	/* 33 */,
	128 + 11	/* 34 */,
	128 + 12	/* 35 */,
	128 + 13	/* 36 */,
	128 + 14	/* 37 */,
	128 + 15	/* 38 */,
	128 + 15	/* 39 */,
	128 + 15	/* 40+ */
};


/**
 * Stat Table (STR) -- weight limit in deca-pounds
 */
const byte adj_str_wgt[STAT_RANGE] =
{
	5	/* 3 */,
	6	/* 4 */,
	7	/* 5 */,
	8	/* 6 */,
	9	/* 7 */,
	10	/* 8 */,
	11	/* 9 */,
	12	/* 10 */,
	13	/* 11 */,
	14	/* 12 */,
	15	/* 13 */,
	16	/* 14 */,
	17	/* 15 */,
	18	/* 16 */,
	19	/* 17 */,
	20	/* 18 */,
	22	/* 19 */,
	24	/* 20 */,
	26	/* 21 */,
	28	/* 22 */,
	30	/* 23 */,
	30	/* 24 */,
	30	/* 25 */,
	30	/* 26 */,
	30	/* 27 */,
	30	/* 28 */,
	30	/* 29 */,
	30	/* 30 */,
	30	/* 31 */,
	30	/* 32 */,
	30	/* 33 */,
	30	/* 34 */,
	30	/* 35 */,
	30	/* 36 */,
	30	/* 37 */,
	30	/* 38 */,
	30	/* 39 */,
	30	/* 40+ */
};


/**
 * Stat Table (STR) -- weapon weight limit in pounds
 */
const byte adj_str_hold[STAT_RANGE] =
{
	4	/* 3 */,
	5	/* 4 */,
	6	/* 5 */,
	7	/* 6 */,
	8	/* 7 */,
	10	/* 8 */,
	12	/* 9 */,
	14	/* 10 */,
	16	/* 11 */,
	18	/* 12 */,
	20	/* 13 */,
	22	/* 14 */,
	24	/* 15 */,
	26	/* 16 */,
	28	/* 17 */,
	30	/* 18 */,
	30	/* 19 */,
	35	/* 20 */,
	40	/* 21 */,
	45	/* 22 */,
	50	/* 23 */,
	55	/* 24 */,
	60	/* 25 */,
	65	/* 26 */,
	70	/* 27 */,
	80	/* 28 */,
	80	/* 29 */,
	80	/* 30 */,
	80	/* 31 */,
	80	/* 32 */,
	90	/* 33 */,
	90	/* 34 */,
	90	/* 35 */,
	90	/* 36 */,
	90	/* 37 */,
	100	/* 38 */,
	100	/* 39 */,
	100	/* 40+ */
};


/**
 * Stat Table (STR) -- digging value
 */
const byte adj_str_dig[STAT_RANGE] =
{
	0	/* 3 */,
	0	/* 4 */,
	1	/* 5 */,
	2	/* 6 */,
	3	/* 7 */,
	4	/* 8 */,
	4	/* 9 */,
	5	/* 10 */,
	5	/* 11 */,
	6	/* 12 */,
	6	/* 13 */,
	7	/* 14 */,
	7	/* 15 */,
	8	/* 16 */,
	8	/* 17 */,
	9	/* 18 */,
	10	/* 19 */,
	12	/* 20 */,
	15	/* 21 */,
	20	/* 22 */,
	25	/* 23 */,
	30	/* 24 */,
	35	/* 25 */,
	40	/* 26 */,
	45	/* 27 */,
	50	/* 28 */,
	55	/* 29 */,
	60	/* 30 */,
	65	/* 31 */,
	70	/* 32 */,
	75	/* 33 */,
	80	/* 34 */,
	85	/* 35 */,
	90	/* 36 */,
	95	/* 37 */,
	100	/* 38 */,
	100	/* 39 */,
	100	/* 40+ */
};


/**
 * Stat Table (STR) -- help index into the "blow" table
 */
const byte adj_str_blow[STAT_RANGE] =
{
	3	/* 3 */,
	4	/* 4 */,
	5	/* 5 */,
	6	/* 6 */,
	7	/* 7 */,
	8	/* 8 */,
	9	/* 9 */,
	10	/* 10 */,
	11	/* 11 */,
	12	/* 12 */,
	13	/* 13 */,
	14	/* 14 */,
	15	/* 15 */,
	16	/* 16 */,
	17	/* 17 */,
	20 /* 18 */,
	30 /* 19 */,
	40 /* 20 */,
	50 /* 21 */,
	60 /* 22 */,
	70 /* 23 */,
	80 /* 24 */,
	90 /* 25 */,
	100 /* 26 */,
	110 /* 27 */,
	120 /* 28 */,
	130 /* 29 */,
	140 /* 30 */,
	150 /* 31 */,
	160 /* 32 */,
	170 /* 33 */,
	180 /* 34 */,
	190 /* 35 */,
	200 /* 36 */,
	210 /* 37 */,
	220 /* 38 */,
	230 /* 39 */,
	240 /* 40+ */
};


/**
 * Stat Table (DEX) -- index into the "blow" table
 */
const byte adj_dex_blow[STAT_RANGE] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	1	/* 16 */,
	2	/* 17 */,
	2	/* 18 */,
	2	/* 19 */,
	3	/* 20 */,
	3	/* 21 */,
	4	/* 22 */,
	4	/* 23 */,
	5	/* 24 */,
	5	/* 25 */,
	6	/* 26 */,
	6	/* 27 */,
	7	/* 28 */,
	7	/* 29 */,
	8	/* 30 */,
	8	/* 31 */,
	8	/* 32 */,
	9	/* 33 */,
	9	/* 34 */,
	9	/* 35 */,
	10	/* 36 */,
	10	/* 37 */,
	11	/* 38 */,
	11	/* 39 */,
	11	/* 40+ */
};


/**
 * Stat Table (DEX) -- chance of avoiding "theft" and "falling"
 */
const byte adj_dex_safe[STAT_RANGE] =
{
	0	/* 3 */,
	1	/* 4 */,
	2	/* 5 */,
	3	/* 6 */,
	4	/* 7 */,
	5	/* 8 */,
	5	/* 9 */,
	6	/* 10 */,
	6	/* 11 */,
	7	/* 12 */,
	7	/* 13 */,
	8	/* 14 */,
	8	/* 15 */,
	9	/* 16 */,
	9	/* 17 */,
	10	/* 18 */,
	10	/* 19 */,
	15	/* 20 */,
	15	/* 21 */,
	20	/* 22 */,
	25	/* 23 */,
	30	/* 24 */,
	35	/* 25 */,
	40	/* 26 */,
	45	/* 27 */,
	50	/* 28 */,
	60	/* 29 */,
	70	/* 30 */,
	80	/* 31 */,
	90	/* 32 */,
	100	/* 33 */,
	100	/* 34 */,
	100	/* 35 */,
	100	/* 36 */,
	100	/* 37 */,
	100	/* 38 */,
	100	/* 39 */,
	100	/* 40+ */
};


/**
 * Stat Table (CON) -- base regeneration rate
 */
const byte adj_con_fix[STAT_RANGE] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	0	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	1	/* 16 */,
	1	/* 17 */,
	2	/* 18 */,
	2	/* 19 */,
	2	/* 20 */,
	2	/* 21 */,
	2	/* 22 */,
	3	/* 23 */,
	3	/* 24 */,
	3	/* 25 */,
	3	/* 26 */,
	3	/* 27 */,
	4	/* 28 */,
	4	/* 29 */,
	5	/* 30 */,
	6	/* 31 */,
	6	/* 32 */,
	7	/* 33 */,
	7	/* 34 */,
	8	/* 35 */,
	8	/* 36 */,
	8	/* 37 */,
	9	/* 38 */,
	9	/* 39 */,
	9	/* 40+ */
};


/**
 * Stat Table (CON) -- extra 1/100th hitpoints per level
 */
const int adj_con_mhp[STAT_RANGE] =
{
	-250	/* 3 */,
	-150	/* 4 */,
	-100	/* 5 */,
	 -75	/* 6 */,
	 -50	/* 7 */,
	 -25	/* 8 */,
	 -10	/* 9 */,
	  -5	/* 10 */,
	   0	/* 11 */,
	   5	/* 12 */,
	  10	/* 13 */,
	  25	/* 14 */,
	  50	/* 15 */,
	  75	/* 16 */,
	 100	/* 17 */,
	 150	/* 18 */,
	 175	/* 19 */,
	 200	/* 20 */,
	 225	/* 21 */,
	 250	/* 22 */,
	 275	/* 23 */,
	 300	/* 24 */,
	 350	/* 25 */,
	 400	/* 26 */,
	 450	/* 27 */,
	 500	/* 28 */,
	 550	/* 29 */,
	 600	/* 30 */,
	 650	/* 31 */,
	 700	/* 32 */,
	 750	/* 33 */,
	 800	/* 34 */,
	 900	/* 35 */,
	1000	/* 36 */,
	1100	/* 37 */,
	1250	/* 38 */,
	1250	/* 39 */,
	1250	/* 40+ */
};

const int adj_mag_study[STAT_RANGE] =
{
	  0	/* 3 */,
	  0	/* 4 */,
	 10	/* 5 */,
	 20	/* 6 */,
	 30	/* 7 */,
	 40	/* 8 */,
	 50	/* 9 */,
	 60	/* 10 */,
	 70	/* 11 */,
	 80	/* 12 */,
	 85	/* 13 */,
	 90	/* 14 */,
	 95	/* 15 */,
	100	/* 16 */,
	105	/* 17 */,
	110	/* 18 */,
	115	/* 19 */,
	120	/* 20 */,
	130	/* 21 */,
	140	/* 22 */,
	150	/* 23 */,
	160	/* 24 */,
	170	/* 25 */,
	180	/* 26 */,
	190	/* 27 */,
	200	/* 28 */,
	210	/* 29 */,
	220	/* 30 */,
	230	/* 31 */,
	240	/* 32 */,
	250	/* 33 */,
	250	/* 34 */,
	250	/* 35 */,
	250	/* 36 */,
	250	/* 37 */,
	250	/* 38 */,
	250	/* 39 */,
	250	/* 40+ */
};

/**
 * Stat Table (DEX/INT/WIS/CHA) -- extra 1/100 mana-points per level
 */
const int adj_mag_mana[STAT_RANGE] =
{
	  0	/* 3 */,
	 10	/* 4 */,
	 20	/* 5 */,
	 30	/* 6 */,
	 40	/* 7 */,
	 50	/* 8 */,
	 60	/* 9 */,
	 70	/* 10 */,
	 80	/* 11 */,
	 90	/* 12 */,
	100	/* 13 */,
	110	/* 14 */,
	120	/* 15 */,
	130	/* 16 */,
	140	/* 17 */,
	150	/* 18 */,
	160	/* 19 */,
	170	/* 20 */,
	180	/* 21 */,
	190	/* 22 */,
	200	/* 23 */,
	225	/* 24 */,
	250	/* 25 */,
	300	/* 26 */,
	350	/* 27 */,
	400	/* 28 */,
	450	/* 29 */,
	500	/* 30 */,
	550	/* 31 */,
	600	/* 32 */,
	650	/* 33 */,
	700	/* 34 */,
	750	/* 35 */,
	800	/* 36 */,
	800	/* 37 */,
	800	/* 38 */,
	800	/* 39 */,
	800	/* 40+ */
};

/*
 * Stat Table (CHA) -- payment percentages
 */
const int adj_cha_gold[STAT_RANGE] =
{
	130     /* 3 */,
	125     /* 4 */,
	122     /* 5 */,
	120     /* 6 */,
	118     /* 7 */,
	116     /* 8 */,
	114     /* 9 */,
	112     /* 10 */,
	110     /* 11 */,
	108     /* 12 */,
	106     /* 13 */,
	104     /* 14 */,
	103     /* 15 */,
	102     /* 16 */,
	101     /* 17 */,
	100     /* 18 */,
	99      /* 19 */,
	98      /* 20 */,
	97      /* 21 */,
	96      /* 22 */,
	95      /* 23 */,
	94      /* 24 */,
	93      /* 25 */,
	92      /* 26 */,
	91      /* 27 */,
	90      /* 28 */,
	89      /* 29 */,
	88      /* 30 */,
	87      /* 31 */,
	86      /* 32 */,
	85      /* 33 */,
	84      /* 34 */,
	83      /* 35 */,
	82      /* 36 */,
	81      /* 37 */,
	80      /* 38 */,
	79      /* 39 */,
	78      /* 40+ */
};


/*
 * Stat Table (CHA) -- maximum pets
 */
const int adj_cha_maxpets[STAT_RANGE] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	2       /* 13 */,
	2       /* 14 */,
	2       /* 15 */,
	3       /* 16 */,
	3       /* 17 */,
	4       /* 18 */,
	4       /* 19 */,
	4       /* 20 */,
	5       /* 21 */,
	5       /* 22 */,
	5       /* 23 */,
	6       /* 24 */,
	6       /* 25 */,
	6       /* 26 */,
	7       /* 27 */,
	7       /* 28 */,
	7       /* 29 */,
	8       /* 30 */,
	8       /* 31 */,
	8       /* 32 */,
	9       /* 33 */,
	9       /* 34 */,
	10      /* 35 */,
	10      /* 36 */,
	11      /* 37 */,
	11      /* 38 */,
	12      /* 39 */,
	13      /* 40+ */
};

/*
 * Stat Table (CHA) -- animal empathy
 */
const int adj_cha_empathy[STAT_RANGE] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	5	/* 8 */,
	10	/* 9 */,
	15	/* 10 */,
	20	/* 11 */,
	24	/* 12 */,
	28	/* 13 */,
	32	/* 14 */,
	36	/* 15 */,
	40	/* 16 */,
	45	/* 17 */,
	50	/* 18 */,
	55	/* 19 */,
	60	/* 20 */,
	64	/* 21 */,
	68	/* 22 */,
	71	/* 23 */,
	74	/* 24 */,
	77	/* 25 */,
	80	/* 26 */,
	82	/* 27 */,
	84	/* 28 */,
	86	/* 29 */,
	88	/* 30 */,
	90	/* 31 */,
	91	/* 32 */,
	92	/* 33 */,
	93	/* 34 */,
	95	/* 35 */,
	96	/* 36 */,
	97	/* 37 */,
	98	/* 38 */,
	99	/* 39 */,
	100	/* 40+ */
};


/**
 * This table is used to help calculate the number of blows the player can
 * make in a single round of attacks (one player turn) with a normal weapon.
 *
 * This number ranges from a single blow/round for weak players to up to six
 * blows/round for powerful warriors.
 *
 * Note that certain artifacts and ego-items give "bonus" blows/round.
 * 
 *
 * First, from the player class, we extract some values:
 *
 *    Warrior --> num = 6; mul = 5; div = MAX(30, weapon_weight);
 *    Mage    --> num = 4; mul = 2; div = MAX(40, weapon_weight);
 *    Priest  --> num = 4; mul = 3; div = MAX(35, weapon_weight);
 *    Rogue   --> num = 5; mul = 4; div = MAX(30, weapon_weight);
 *    Ranger  --> num = 5; mul = 4; div = MAX(35, weapon_weight);
 *    Paladin --> num = 5; mul = 5; div = MAX(30, weapon_weight);
 * (all specified in class.txt now)
 *
 * To get "P", we look up the relevant "adj_str_blow[]" (see above),
 * multiply it by "mul", and then divide it by "div", rounding down.
 *
 * To get "D", we look up the relevant "adj_dex_blow[]" (see above).
 *
 * Then we look up the energy cost of each blow using "blows_table[P][D]".
 * The player gets blows/round equal to 100/this number, up to a maximum of
 * "num" blows/round, plus any "bonus" blows/round.
 */
const byte blows_table[12][12] =
{
	/* P */
   /* D:   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11+ */
   /* DEX: 3,   10,  17,  20,  22,  24,  26,  28,  30,  33,  36,  38 */

	/* 0  */
	{  100, 100, 95,  85,  75,  60,  50,  42,  35,  30,  25,  23 },

	/* 1  */
	{  100, 95,  85,  75,  60,  50,  42,  35,  30,  25,  23,  21 },

	/* 2  */
	{  95,  85,  75,  60,  50,  42,  35,  30,  26,  23,  21,  20 },

	/* 3  */
	{  85,  75,  60,  50,  42,  36,  32,  28,  25,  22,  20,  19 },

	/* 4  */
	{  75,  60,  50,  42,  36,  33,  28,  25,  23,  21,  19,  18 },

	/* 5  */
	{  60,  50,  42,  36,  33,  30,  27,  24,  22,  21,  19,  17 },

	/* 6  */
	{  50,  42,  36,  33,  30,  27,  25,  23,  21,  20,  18,  17 },

	/* 7  */
	{  42,  36,  33,  30,  28,  26,  24,  22,  20,  19,  18,  17 },

	/* 8  */
	{  36,  33,  30,  28,  26,  24,  22,  21,  20,  19,  17,  16 },

	/* 9  */
	{  35,  32,  29,  26,  24,  22,  21,  20,  19,  18,  17,  16 },

	/* 10 */
	{  34,  30,  27,  25,  23,  22,  21,  20,  19,  18,  17,  16 },

	/* 11+ */
	{  33,  29,  26,  24,  22,  21,  20,  19,  18,  17,  16,  15 },
   /* DEX: 3,   10,  17,  20,  22,  24,  26,  28,  30,  33,  36,  38 */
};

/**
 * Decide which object comes earlier in the standard inventory listing,
 * defaulting to the first if nothing separates them.
 *
 * \return whether to replace the original object with the new one
 */
bool earlier_object(struct object *orig, struct object *new, bool store)
{
	/* Check we have actual objects */
	if (!new) return FALSE;
	if (!orig) return TRUE;

	if (!store) {
		/* Readable books always come first */
		if (obj_can_browse(orig) && !obj_can_browse(new)) return FALSE;
		if (!obj_can_browse(orig) && obj_can_browse(new)) return TRUE;
	}

	/* Objects sort by decreasing type */
	if (orig->tval > new->tval) return FALSE;
	if (orig->tval < new->tval) return TRUE;

	if (!store) {
		/* Non-aware (flavored) items always come last (default to orig) */
		if (!object_flavor_is_aware(new)) return FALSE;
		if (!object_flavor_is_aware(orig)) return TRUE;
	}

	/* Objects sort by increasing sval */
	if (orig->sval < new->sval) return FALSE;
	if (orig->sval > new->sval) return TRUE;

	if (!store) {
		/* Unidentified objects always come last (default to orig) */
		if (!object_is_known(new)) return FALSE;
		if (!object_is_known(orig)) return TRUE;

		/* Lights sort by decreasing fuel */
		if (tval_is_light(orig)) {
			if (orig->pval > new->pval) return FALSE;
			if (orig->pval < new->pval) return TRUE;
		}
	}

	/* Objects sort by decreasing value */
	if (object_value_real(orig, 1, FALSE, FALSE) >
		object_value_real(new, 1, FALSE, FALSE))
		return FALSE;
	if (object_value_real(orig, 1, FALSE, FALSE) <
		object_value_real(new, 1, FALSE, FALSE))
		return TRUE;

	/* No preference */
	return FALSE;
}

int equipped_item_slot(struct player_body body, struct object *item)
{
	int i;

	if (item == NULL) return body.count;

	/* Look for an equipment slot with this item */
	for (i = 0; i < body.count; i++)
		if (item == body.slots[i].obj) break;

	/* Correct slot, or body.count if not equipped */
	return i;
}

/**
 * Put the player's inventory and quiver into easily accessible arrays.  The
 * pack may be overfull by one item
 */
void calc_inventory(struct player_upkeep *upkeep, struct object *gear,
					struct player_body body)
{
	int i;
	int old_inven_cnt = upkeep->inven_cnt;
	struct object **old_quiver = mem_zalloc(z_info->quiver_size *
												sizeof(struct object *));
	struct object **old_pack = mem_zalloc(z_info->pack_size *
											  sizeof(struct object *));

	/* Prepare to fill the quiver */
	upkeep->quiver_cnt = 0;

	/* Copy the current quiver */
	for (i = 0; i < z_info->quiver_size; i++)
		if (upkeep->quiver[i])
			old_quiver[i] = upkeep->quiver[i];
		else
			old_quiver[i] = NULL;

	/* First, allocate inscribed items */
	for (i = 0; i < z_info->quiver_size; i++) {
		struct object *current;

		/* Start with an empty slot */
		upkeep->quiver[i] = NULL;

		/* Find the first quiver object with the correct label */
		for (current = gear; current; current = current->next) {
			/* Ignore non-ammo */
			if (!tval_is_ammo(current)) continue;

			/* Allocate inscribed objects if it's the right slot */
			if (current->note) {
				const char *s = strchr(quark_str(current->note), '@');
				if (s && s[1] == 'f') {
					int choice = s[2] - '0';

					/* Correct slot, fill it straight away */
					if (choice == i) {
						upkeep->quiver[i] = current;
						upkeep->quiver_cnt += current->number;

						/* Notice stuff if it's first time in the quiver */
						if (!object_was_worn(current))
							object_notice_on_wield(current);

						/* Done with this slot */
						break;
					}
				}
			}
		}
	}

	/* Now fill the rest of the slots in order */
	for (i = 0; i < z_info->quiver_size; i++) {
		struct object *current, *first = NULL;
		int j;

		/* If the slot is full, move on */
		if (upkeep->quiver[i]) continue;

		/* Find the first quiver object not yet allocated */
		for (current = gear; current; current = current->next) {
			bool already = FALSE;

			/* Ignore non-ammo */
			if (!tval_is_ammo(current)) continue;

			/* Ignore stuff already quivered */
			for (j = 0; j < z_info->quiver_size; j++)
				if (upkeep->quiver[j] == current)
					already = TRUE;
			if (already) continue;

			/* Choose the first in order */
			if (earlier_object(first, current, FALSE)) {
				first = current;
			}
		}

		/* Stop looking if there's nothing left */
		if (!first) break;

		/* If we have an item, slot it */
		upkeep->quiver[i] = first;
		upkeep->quiver_cnt += first->number;

		/* Notice stuff if it's first time in the quiver */
		if (!object_was_worn(first))
			object_notice_on_wield(first);
	}

	/* Note reordering */
	if (character_dungeon)
		for (i = 0; i < z_info->quiver_size; i++)
			if (old_quiver[i] && (upkeep->quiver[i] != old_quiver[i])) {
				msg("You re-arrange your quiver.");
				break;
			}

	/* Copy the current pack */
	for (i = 0; i < z_info->pack_size; i++)
		old_pack[i] = upkeep->inven[i];

	/* Prepare to fill the inventory */
	upkeep->inven_cnt = 0;

	for (i = 0; i <= z_info->pack_size; i++) {
		struct object *current, *first = NULL;
		for (current = gear; current; current = current->next) {
			bool possible = TRUE;
			int j;

			/* Skip equipment */
			if (object_is_equipped(body, current))
				possible = FALSE;

			/* Skip quivered objects */
			for (j = 0; j < z_info->quiver_size; j++)
				if (upkeep->quiver[j] == current)
					possible = FALSE;

			/* Skip objects already allocated to the inventory */
			for (j = 0; j < upkeep->inven_cnt; j++)
				if (upkeep->inven[j] == current)
					possible = FALSE;

			/* If still possible, choose the first in order */
			if (!possible)
				continue;
			else if (earlier_object(first, current, FALSE)) {
				first = current;
			}
		}

		/* Allocate */
		upkeep->inven[i] = first;
		if (first)
			upkeep->inven_cnt++;
	}

	/* Note reordering */
	if (character_dungeon && (upkeep->inven_cnt == old_inven_cnt))
		for (i = 0; i < z_info->pack_size; i++)
			if (old_pack[i] && (upkeep->inven[i] != old_pack[i]) &&
				!object_is_equipped(body, old_pack[i])) {
				msg("You re-arrange your pack.");
				break;
			}

	mem_free(old_quiver);
	mem_free(old_pack);
}

static void update_inventory(struct player *p)
{
	calc_inventory(p->upkeep, p->gear, p->body);
}

/**
 * "Percentage" (actually some complicated hack that needs redoing) of
 * player's spells they can learn per level (-> realms - NRM)
 */
int level_spells(struct player *p)
{
	int stat = p->class->magic.spell_realm->stat;
	return adj_mag_study[p->state.stat_ind[stat]];
}

/**
 * Calculate number of spells player should have, and forget,
 * or remember, spells until that number is properly reflected.
 *
 * Note that this function induces various "status" messages,
 * which must be bypasses until the character is created.
 */
static void calc_spells(struct player *p)
{
	int i, j, k, levels;
	int num_allowed, num_known, num_total = p->class->magic.total_spells;
	int percent_spells;

	const struct class_spell *spell;

	s16b old_spells;

	const char *noun = p->class->magic.spell_realm->spell_noun;

	/* Hack -- must be literate */
	if (!p->class->magic.total_spells) return;

	/* Hack -- wait for creation */
	if (!character_generated) return;

	/* Hack -- handle partial mode */
	if (p->upkeep->only_partial) return;

	/* Save the new_spells value */
	old_spells = p->upkeep->new_spells;

	/* Determine the number of spells allowed */
	levels = p->lev - p->class->magic.spell_first + 1;

	/* Hack -- no negative spells */
	if (levels < 0) levels = 0;

	/* Number of 1/100 spells per level (or something - needs clarifying) */
	percent_spells = level_spells(p);

	/* Extract total allowed spells (rounded up) */
	num_allowed = (((percent_spells * levels) + 50) / 100);

	/* Assume none known */
	num_known = 0;

	/* Count the number of spells we know */
	for (j = 0; j < num_total; j++)
		if (p->spell_flags[j] & PY_SPELL_LEARNED)
			num_known++;

	/* See how many spells we must forget or may learn */
	p->upkeep->new_spells = num_allowed - num_known;

	/* Forget spells which are too hard */
	for (i = num_total - 1; i >= 0; i--) {
		/* Get the spell */
		j = p->spell_order[i];

		/* Skip non-spells */
		if (j == PY_SPELL_NEVER_LEARNED) continue;

		/* Get the spell */
		spell = spell_by_index(j);

		/* Skip spells we are allowed to know */
		if (spell->slevel <= p->lev) continue;

		/* Is it known? */
		if (p->spell_flags[j] & PY_SPELL_LEARNED) {
			/* Mark as forgotten */
			p->spell_flags[j] |= PY_SPELL_FORGOTTEN;

			/* No longer known */
			p->spell_flags[j] &= ~PY_SPELL_LEARNED;

			/* Message */
			msg("You have forgotten the %s of %s.", noun, spell->name);

			/* One more can be learned */
			p->upkeep->new_spells++;
		}
	}

	/* Forget spells if we know too many spells */
	for (i = num_total - 1; i >= 0; i--) {
		/* Stop when possible */
		if (p->upkeep->new_spells >= 0) break;

		/* Get the (i+1)th spell learned */
		j = p->spell_order[i];

		/* Skip unknown spells */
		if (j == PY_SPELL_NEVER_LEARNED) continue;

		/* Get the spell */
		spell = spell_by_index(j);

		/* Forget it (if learned) */
		if (p->spell_flags[j] & PY_SPELL_LEARNED) {
			/* Mark as forgotten */
			p->spell_flags[j] |= PY_SPELL_FORGOTTEN;

			/* No longer known */
			p->spell_flags[j] &= ~PY_SPELL_LEARNED;

			/* Message */
			msg("You have forgotten the %s of %s.", noun, spell->name);

			/* One more can be learned */
			p->upkeep->new_spells++;
		}
	}

	/* Check for spells to remember */
	for (i = 0; i < num_total; i++) {
		/* None left to remember */
		if (p->upkeep->new_spells <= 0) break;

		/* Get the next spell we learned */
		j = p->spell_order[i];

		/* Skip unknown spells */
		if (j == PY_SPELL_NEVER_LEARNED) break;

		/* Get the spell */
		spell = spell_by_index(j);

		/* Skip spells we cannot remember */
		if (spell->slevel > p->lev) continue;

		/* First set of spells */
		if (p->spell_flags[j] & PY_SPELL_FORGOTTEN) {
			/* No longer forgotten */
			p->spell_flags[j] &= ~PY_SPELL_FORGOTTEN;

			/* Known once more */
			p->spell_flags[j] |= PY_SPELL_LEARNED;

			/* Message */
			msg("You have remembered the %s of %s.", noun, spell->name);

			/* One less can be learned */
			p->upkeep->new_spells--;
		}
	}

	/* Assume no spells available */
	k = 0;

	/* Count spells that can be learned */
	for (j = 0; j < num_total; j++) {
		/* Get the spell */
		spell = spell_by_index(j);

		/* Skip spells we cannot remember or don't exist */
		if (!spell) continue;
		if (spell->slevel > p->lev || spell->slevel == 0) continue;

		/* Skip spells we already know */
		if (p->spell_flags[j] & PY_SPELL_LEARNED)
			continue;

		/* Count it */
		k++;
	}

	/* Cannot learn more spells than exist */
	if (p->upkeep->new_spells > k) p->upkeep->new_spells = k;

	/* Spell count changed */
	if (old_spells != p->upkeep->new_spells) {
		/* Message if needed */
		if (p->upkeep->new_spells)
			/* Message */
			msg("You can learn %d more %s%s.", p->upkeep->new_spells, noun,
				(p->upkeep->new_spells != 1) ? "s" : "");

		/* Redraw Study Status */
		p->upkeep->redraw |= (PR_STUDY | PR_OBJECT);
	}
}


/**
 * Get the player's max spell points per effective level
 */
int mana_per_level(struct player *p, struct player_state *state)
{
	int stat = p->class->magic.spell_realm->stat;
	return adj_mag_mana[state->stat_ind[stat]];
}

/**
 * Calculate maximum mana.  You do not need to know any spells.
 * Note that mana is lowered by heavy (or inappropriate) armor.
 *
 * This function induces status messages.
 */
static void calc_mana(struct player *p, struct player_state *state)
{
	int i, msp, levels, cur_wgt, max_wgt; 
	struct object *obj;

	/* Hack -- Must be literate */
	if (!p->class->magic.total_spells) {
		p->msp = 0;
		p->csp = 0;
		p->csp_frac = 0;
		return;
	}

	/* Extract "effective" player level */
	levels = (p->lev - p->class->magic.spell_first) + 1;
	if (levels > 0) {
		msp = 1;
		msp += mana_per_level(p, state) * levels / 100;
	} else {
		levels = 0;
		msp = 0;
	}

	/* Process gloves for those disturbed by them */
	if (player_has(p, PF_CUMBER_GLOVE)) {
		/* Assume player is not encumbered by gloves */
		state->cumber_glove = FALSE;

		/* Get the gloves */
		obj = equipped_item_by_slot_name(p, "hands");

		/* Normal gloves hurt mage-type spells */
		if (obj && !of_has(obj->flags, OF_FREE_ACT) && 
			!kf_has(obj->kind->kind_flags, KF_SPELLS_OK) &&
			(obj->modifiers[OBJ_MOD_DEX] <= 0)) {
			/* Encumbered */
			state->cumber_glove = TRUE;

			/* Reduce mana */
			msp = (3 * msp) / 4;
		}
	}

	/* Assume player not encumbered by armor */
	state->cumber_armor = FALSE;

	/* Weigh the armor */
	cur_wgt = 0;
	for (i = 0; i < p->body.count; i++) {
		struct object *obj = slot_object(p, i);

		/* Ignore non-armor */
		if (slot_type_is(i, EQUIP_WEAPON)) continue;
		if (slot_type_is(i, EQUIP_BOW)) continue;
		if (slot_type_is(i, EQUIP_RING)) continue;
		if (slot_type_is(i, EQUIP_AMULET)) continue;
		if (slot_type_is(i, EQUIP_LIGHT)) continue;

		/* Add weight */
		if (obj)
			cur_wgt += obj->weight;
	}

	/* Determine the weight allowance */
	max_wgt = p->class->magic.spell_weight;

	/* Heavy armor penalizes mana */
	if (((cur_wgt - max_wgt) / 10) > 0) {
		/* Encumbered */
		state->cumber_armor = TRUE;

		/* Reduce mana */
		msp -= ((cur_wgt - max_wgt) / 10);
	}

	/* Mana can never be negative */
	if (msp < 0) msp = 0;

	/* Maximum mana has changed */
	if (p->msp != msp) {
		/* Save new limit */
		p->msp = msp;

		/* Enforce new limit */
		if (p->csp >= msp) {
			p->csp = msp;
			p->csp_frac = 0;
		}

		/* Display mana later */
		p->upkeep->redraw |= (PR_MANA);
	}
}


/**
 * Calculate the players (maximal) hit points
 *
 * Adjust current hitpoints if necessary
 */
static void calc_hitpoints(struct player *p)
{
	long bonus;
	int mhp;

	/* Get "1/100th hitpoint bonus per level" value */
	bonus = adj_con_mhp[p->state.stat_ind[STAT_CON]];

	/* Calculate hitpoints */
	mhp = p->player_hp[p->lev-1] + (bonus * p->lev / 100);

	/* Always have at least one hitpoint per level */
	if (mhp < p->lev + 1) mhp = p->lev + 1;

	/* New maximum hitpoints */
	if (p->mhp != mhp) {
		/* Save new limit */
		p->mhp = mhp;

		/* Enforce new limit */
		if (p->chp >= mhp) {
			p->chp = mhp;
			p->chp_frac = 0;
		}

		/* Display hitpoints (later) */
		p->upkeep->redraw |= (PR_HP);
	}
}


/**
 * Calculate and set the current light radius.
 *
 * The brightest wielded object counts as the light source; radii do not add
 * up anymore.
 *
 * Note that a cursed light source no longer emits light.
 */
static void calc_torch(struct player *p, struct player_state *state)
{
	int i;

	/* Assume no light */
	state->cur_light = 0;

	/* Ascertain lightness if in the town */
	if (!p->depth && is_daytime()) {
		/* Update the visuals if necessary*/
		if (p->state.cur_light != state->cur_light)
			p->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

		return;
	}

	/* Examine all wielded objects, use the brightest */
	for (i = 0; i < p->body.count; i++) {
		int amt = 0;
		struct object *obj = slot_object(p, i);

		/* Skip empty slots */
		if (!obj) continue;

		/* Light radius is now a modifier */
		amt = obj->modifiers[OBJ_MOD_LIGHT];

		/* Cursed objects emit no light */
		if (of_has(obj->flags, OF_LIGHT_CURSE))
			amt = 0;

		/* Examine actual lights */
		if (tval_is_light(obj) && !of_has(obj->flags, OF_NO_FUEL) &&
				obj->timeout == 0)
			/* Lights without fuel provide no light */
			amt = 0;

		/* Alter p->state.cur_light if reasonable */
	    state->cur_light += amt;
	}

	/* Limit light */
	state->cur_light = MIN(state->cur_light, 5);
	state->cur_light = MAX(state->cur_light, 0);
}

/**
 * Populates `chances` with the player's chance of digging through
 * the diggable terrain types in one turn out of 1600.
 */
void calc_digging_chances(struct player_state *state, int chances[DIGGING_MAX])
{
	int i;

	chances[DIGGING_RUBBLE] = state->skills[SKILL_DIGGING] * 8;
	chances[DIGGING_MAGMA] = (state->skills[SKILL_DIGGING] - 10) * 4;
	chances[DIGGING_QUARTZ] = (state->skills[SKILL_DIGGING] - 20) * 2;
	chances[DIGGING_GRANITE] = (state->skills[SKILL_DIGGING] - 40) * 1;
	/* Approximate a 1/1200 chance per skill point over 30 */
	chances[DIGGING_DOORS] = (state->skills[SKILL_DIGGING] * 4 - 119) / 3;

	/* Don't let any negative chances through */
	for (i = 0; i < DIGGING_MAX; i++)
		chances[i] = MAX(0, chances[i]);
}

/**
 * Calculate the blows a player would get.
 *
 * \param obj is the object for which we are calculating blows
 * \param state is the player state for which we are calculating blows
 * \param extra_blows is the number of +blows available from this object and
 * this state
 *
 * N.B. state->num_blows is now 100x the number of blows.
 */
int calc_blows(struct player *p, const struct object *obj,
			   struct player_state *state, int extra_blows)
{
	int blows;
	int str_index, dex_index;
	int div;
	int blow_energy;

	int weight = (obj == NULL) ? 0 : obj->weight;
	int min_weight = p->class->min_weight;

	/* Enforce a minimum "weight" (tenth pounds) */
	div = (weight < min_weight) ? min_weight : weight;

	/* Get the strength vs weight */
	str_index = adj_str_blow[state->stat_ind[STAT_STR]] *
			p->class->att_multiply / div;

	/* Maximal value */
	if (str_index > 11) str_index = 11;

	/* Index by dexterity */
	dex_index = MIN(adj_dex_blow[state->stat_ind[STAT_DEX]], 11);

	/* Use the blows table to get energy per blow */
	blow_energy = blows_table[str_index][dex_index];

	blows = MIN((10000 / blow_energy), (100 * p->class->max_attacks));

	/* Require at least one blow */
	return MAX(blows + (100 * extra_blows), 100);
}


/**
 * Computes current weight limit.
 */
static int weight_limit(struct player_state *state)
{
	int i;

	/* Weight limit based only on strength */
	i = adj_str_wgt[state->stat_ind[STAT_STR]] * 100;

	/* Return the result */
	return (i);
}


/**
 * Computes weight remaining before burdened.
 */
int weight_remaining(struct player *p)
{
	int i;

	/* Weight limit based only on strength */
	i = 60 * adj_str_wgt[p->state.stat_ind[STAT_STR]]
		- p->upkeep->total_weight - 1;

	/* Return the result */
	return (i);
}


/**
 * Calculate the players current "state", taking into account
 * not only race/class intrinsics, but also objects being worn
 * and temporary spell effects.
 *
 * See also calc_mana() and calc_hitpoints().
 *
 * Take note of the new "speed code", in particular, a very strong
 * player will start slowing down as soon as he reaches 150 pounds,
 * but not until he reaches 450 pounds will he be half as fast as
 * a normal kobold.  This both hurts and helps the player, hurts
 * because in the old days a player could just avoid 300 pounds,
 * and helps because now carrying 300 pounds is not very painful.
 *
 * The "weapon" and "bow" do *not* add to the bonuses to hit or to
 * damage, since that would affect non-combat things.  These values
 * are actually added in later, at the appropriate place.
 *
 * If known_only is true, calc_bonuses() will only use the known
 * information of objects; thus it returns what the player _knows_
 * the character state to be.
 */
void calc_bonuses(struct player *p, struct player_state *state, bool known_only)
{
	int i, j, hold;

	int extra_blows = 0;
	int extra_shots = 0;
	int extra_might = 0;

	struct object *obj;

	bitflag f[OF_SIZE];
	bitflag collect_f[OF_SIZE];
	bool vuln[ELEM_MAX];

	/* Reset */
	memset(state, 0, sizeof *state);

	/* Set various defaults */
	state->speed = 110;
	state->num_blows = 100;


	/* ------------------------------------
	 * Extract race/class info
	 * ------------------------------------ */

	/* Base infravision (purely racial) */
	state->see_infra = p->race->infra;

	/* Base skills */
	for (i = 0; i < SKILL_MAX; i++)
		state->skills[i] = p->race->r_skills[i]
			+ p->class->c_skills[i];

	/* Base resists */
	for (i = 0; i < ELEM_MAX; i++) {
		vuln[i] = FALSE;
		if (p->race->el_info[i].res_level == -1)
			vuln[i] = TRUE;
		else
			state->el_info[i].res_level = p->race->el_info[i].res_level;
	}

	/* Base pflags */
	pf_wipe(state->pflags);
	pf_copy(state->pflags, p->race->pflags);
	pf_union(state->pflags, p->class->pflags);

	/* ------------------------------------
	 * Analyze player
	 * ------------------------------------ */

	/* Extract the player flags */
	player_flags(p, collect_f);

	/* Add player specific pflags */
	if (!p->csp)
		pf_on(state->pflags, PF_NO_MANA);

	/* ------------------------------------
	 * Analyze equipment
	 * ------------------------------------ */

	/* Scan the equipment */
	for (i = 0; i < p->body.count; i++) {
		obj = slot_object(p, i);

		/* Skip non-objects */
		if (!obj) continue;

		/* Extract the item flags */
		if (known_only)
			object_flags_known(obj, f);
		else
			object_flags(obj, f);

		of_union(collect_f, f);

		/* Affect stats */
		state->stat_add[STAT_STR] += obj->modifiers[OBJ_MOD_STR];
		state->stat_add[STAT_INT] += obj->modifiers[OBJ_MOD_INT];
		state->stat_add[STAT_WIS] += obj->modifiers[OBJ_MOD_WIS];
		state->stat_add[STAT_DEX] += obj->modifiers[OBJ_MOD_DEX];
		state->stat_add[STAT_CON] += obj->modifiers[OBJ_MOD_CON];
		state->stat_add[STAT_CHA] += obj->modifiers[OBJ_MOD_CHA];

		/* Affect stealth */
		state->skills[SKILL_STEALTH] += obj->modifiers[OBJ_MOD_STEALTH];

		/* Affect searching ability (factor of five) */
		state->skills[SKILL_SEARCH] += (obj->modifiers[OBJ_MOD_SEARCH] * 5);

		/* Affect searching frequency (factor of five) */
		state->skills[SKILL_SEARCH_FREQUENCY] += 
			(obj->modifiers[OBJ_MOD_SEARCH] * 5);

		/* Affect infravision */
		state->see_infra += obj->modifiers[OBJ_MOD_INFRA];

		/* Affect digging (factor of 20) */
		state->skills[SKILL_DIGGING] += (obj->modifiers[OBJ_MOD_TUNNEL] * 20);

		/* Affect speed */
		state->speed += obj->modifiers[OBJ_MOD_SPEED];

		/* Affect blows */
		extra_blows += obj->modifiers[OBJ_MOD_BLOWS];

		/* Affect shots */
		extra_shots += obj->modifiers[OBJ_MOD_SHOTS];

		/* Affect Might */
		extra_might += obj->modifiers[OBJ_MOD_MIGHT];

		/* Affect resists */
		for (j = 0; j < ELEM_MAX; j++)
			if (!known_only || object_is_known(obj) ||
				object_element_is_known(obj, j)) {

				/* Note vulnerability for later processing */
				if (obj->el_info[j].res_level == -1)
					vuln[i] = TRUE;

				/* OK because res_level has not included vulnerability yet */
				if (obj->el_info[j].res_level > state->el_info[j].res_level)
					state->el_info[j].res_level = obj->el_info[j].res_level;
			}

		/* Modify the base armor class */
		state->ac += obj->ac;

		/* Apply the bonuses to armor class */
		if (!known_only || object_is_known(obj) ||
			object_defence_plusses_are_visible(obj))
			state->to_a += obj->to_a;

		/* Do not apply weapon and bow bonuses until combat calculations */
		if (slot_type_is(i, EQUIP_WEAPON)) continue;
		if (slot_type_is(i, EQUIP_BOW)) continue;

		/* Apply the bonuses to hit/damage */
		if (!known_only || object_is_known(obj) ||
			object_attack_plusses_are_visible(obj))
		{
			state->to_h += obj->to_h;
			state->to_d += obj->to_d;
		}
	}


	/* ------------------------------------
	 * Update all flags
	 * ------------------------------------ */

	of_union(state->flags, collect_f);

	/* ------------------------------------
	 * Handle stats
	 * ------------------------------------ */

	/* Calculate stats */
	for (i = 0; i < STAT_MAX; i++) {
		int add, top, use, ind;

		/* Extract modifier */
		add = state->stat_add[i];

		/* Modify the stats for race/class */
		add += (p->race->r_adj[i] + p->class->c_adj[i]);

		/* Extract the new "stat_top" value for the stat */
		top = modify_stat_value(p->stat_max[i], add);

		/* Save the new value */
		state->stat_top[i] = top;

		/* Extract the new "stat_use" value for the stat */
		use = modify_stat_value(p->stat_cur[i], add);

		/* Save the new value */
		state->stat_use[i] = use;

		/* Values: n/a */
		if (use <= 3)
			ind = 0;
		else if (use <= 39) /* Values: 4 to 39 */
			ind = (use - 3);
//		else if (use <= 18+219) /* Ranges: 18, ..., 39 */
//			ind = (15 + (use - 18) / 10);
		else /* Range: 40+ */
			ind = (37);

		assert((0 <= ind) && (ind < STAT_RANGE));

		/* Save the new index */
		state->stat_ind[i] = ind;
	}

	/* Now deal with vulnerabilities */
	for (i = 0; i < ELEM_MAX; i++) {
		if (vuln[i] && (state->el_info[i].res_level < 3))
			state->el_info[i].res_level--;
	}

	/* ------------------------------------
	 * Temporary flags
	 * ------------------------------------ */

	/* Apply temporary "stun" */
	if (p->timed[TMD_STUN] > 50) {
		state->to_h -= 20;
		state->to_d -= 20;
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE]
			* 8 / 10;
	} else if (p->timed[TMD_STUN]) {
		state->to_h -= 5;
		state->to_d -= 5;
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE]
			* 9 / 10;
	}

	/* Invulnerability */
	if (p->timed[TMD_INVULN])
		state->to_a += 100;

	/* Temporary blessing */
	if (p->timed[TMD_BLESSED]) {
		state->to_a += 5;
		state->to_h += 10;
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE]
			* 105 / 100;
	}

	/* Temporary shield */
	if (p->timed[TMD_SHIELD])
		state->to_a += 50;

	/* Temporary stoneskin */
	if (p->timed[TMD_STONESKIN]) {
		state->to_a += 40;
		state->speed -= 5;
	}

	/* Temporary resistance to fear */
	if (p->timed[TMD_BOLD])
		of_on(state->flags, OF_PROT_FEAR);

	/* Temporary "Hero" */
	if (p->timed[TMD_HERO]) {
		of_on(state->flags, OF_PROT_FEAR);
		state->to_h += 12;
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE] * 105 / 100;
	}

	/* Temporary "Berserk" */
	if (p->timed[TMD_SHERO]) {
		of_on(state->flags, OF_PROT_FEAR);
		state->to_h += 24;
		state->to_a -= 10;
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE] * 9 / 10;
	}

	/* Temporary "fast" */
	if (p->timed[TMD_FAST] || p->timed[TMD_SPRINT])
		state->speed += 10;

	/* Temporary "slow" */
	if (p->timed[TMD_SLOW])
		state->speed -= 10;

	/* Temporary infravision boost */
	if (p->timed[TMD_SINFRA])
		state->see_infra += 5;

	/* Temporary ESP */
	if (p->timed[TMD_TELEPATHY])
		of_on(state->flags, OF_TELEPATHY);

	/* Temporary see invisible */
	if (p->timed[TMD_SINVIS])
		of_on(state->flags, OF_SEE_INVIS);

	/* Fear / terror flags */
	if (p->timed[TMD_AFRAID] || p->timed[TMD_TERROR])
		of_on(state->flags, OF_AFRAID);
	if (p->timed[TMD_TERROR])
		state->speed += 10;

	/* Resist acid */
	if (p->timed[TMD_OPP_ACID])
		if (state->el_info[ELEM_ACID].res_level < 2)
			state->el_info[ELEM_ACID].res_level++;

	/* Resist electricity */
	if (p->timed[TMD_OPP_ELEC])
		if (state->el_info[ELEM_ELEC].res_level < 2)
			state->el_info[ELEM_ELEC].res_level++;

	/* Resist fire */
	if (p->timed[TMD_OPP_FIRE])
		if (state->el_info[ELEM_FIRE].res_level < 2)
			state->el_info[ELEM_FIRE].res_level++;

	/* Resist cold */
	if (p->timed[TMD_OPP_COLD])
		if (state->el_info[ELEM_COLD].res_level < 2)
			state->el_info[ELEM_COLD].res_level++;

	/* Resist poison */
	if (p->timed[TMD_OPP_POIS])
		if (state->el_info[ELEM_POIS].res_level < 2)
			state->el_info[ELEM_POIS].res_level++;

	/* Resist confusion */
	if (p->timed[TMD_OPP_CONF])
		of_on(state->flags, OF_PROT_CONF);

	/* Confusion */
	if (p->timed[TMD_CONFUSED])
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE] * 75 / 100;

	/* Amnesia */
	if (p->timed[TMD_AMNESIA])
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE] * 8 / 10;

	/* Poison */
	if (p->timed[TMD_POISONED])
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE] * 95 / 100;

	/* Hallucination */
	if (p->timed[TMD_IMAGE])
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE] * 8 / 10;

	/* ------------------------------------
	 * Analyze flags
	 * ------------------------------------ */

	/* Check for fear */
	if (of_has(state->flags, OF_AFRAID)) {
		state->to_h -= 20;
		state->to_a += 8;
		state->skills[SKILL_DEVICE] = state->skills[SKILL_DEVICE] * 95 / 100;
	}

	/* ------------------------------------
	 * Analyze weight
	 * ------------------------------------ */

	/* Extract the current weight (in tenth pounds) */
	j = p->upkeep->total_weight;

	/* Extract the "weight limit" (in tenth pounds) */
	i = weight_limit(state);

	/* Apply "encumbrance" from weight */
	if (j > i / 2)
		state->speed -= ((j - (i / 2)) / (i / 10));

	/* Searching slows the player down */
	if (p->searching)
		state->speed -= 10;

	/* Sanity check on extreme speeds */
	if (state->speed < 0)
		state->speed = 0;
	if (state->speed > 199)
		state->speed = 199;

	/* ------------------------------------
	 * Apply modifier bonuses
	 * ------------------------------------ */

	/* Modifier Bonuses (Un-inflate stat bonuses) */
	state->to_a += ((int)(adj_dex_ta[state->stat_ind[STAT_DEX]]) - 128);
	state->to_d += ((int)(adj_str_td[state->stat_ind[STAT_STR]]) - 128);
	state->to_h += ((int)(adj_dex_th[state->stat_ind[STAT_DEX]]) - 128);
	state->to_h += ((int)(adj_str_th[state->stat_ind[STAT_STR]]) - 128);


	/* ------------------------------------
	 * Modify skills
	 * ------------------------------------ */

	/* Affect Skill -- disarming (DEX and INT) */
	state->skills[SKILL_DISARM] += adj_dex_dis[state->stat_ind[STAT_DEX]];
	state->skills[SKILL_DISARM] += adj_int_dis[state->stat_ind[STAT_INT]];

	/* Affect Skill -- magic devices (INT) */
	state->skills[SKILL_DEVICE] += adj_int_dev[state->stat_ind[STAT_INT]];

	/* Affect Skill -- saving throw (WIS) */
	state->skills[SKILL_SAVE] += adj_wis_sav[state->stat_ind[STAT_WIS]];

	/* Affect Skill -- digging (STR) */
	state->skills[SKILL_DIGGING] += adj_str_dig[state->stat_ind[STAT_STR]];

	/* Affect Skills (Level, by Class) */
	for (i = 0; i < SKILL_MAX; i++)
		state->skills[i] += (p->class->x_skills[i] * p->lev / 10);

	/* Limit Skill -- digging from 1 up */
	if (state->skills[SKILL_DIGGING] < 1) state->skills[SKILL_DIGGING] = 1;

	/* Limit Skill -- stealth from 0 to 30 */
	if (state->skills[SKILL_STEALTH] > 30) state->skills[SKILL_STEALTH] = 30;
	if (state->skills[SKILL_STEALTH] < 0) state->skills[SKILL_STEALTH] = 0;

	/* Apply Skill -- Extract noise from stealth */
	state->noise = (1L << (30 - state->skills[SKILL_STEALTH]));

	/* Affect Skill -- gold (CHA) */
	state->skills[SKILL_GOLD] += adj_cha_gold[state->stat_ind[STAT_CHA]];
        
	/* Affect Skill -- empathy (CHA) */
	state->skills[SKILL_EMPATHY] += adj_cha_empathy[state->stat_ind[STAT_CHA]];

	/* Affect Skill -- maxpets (CHA) */
	state->skills[SKILL_MAXPETS] += adj_cha_maxpets[state->stat_ind[STAT_CHA]];        

	/* Obtain the "hold" value */
	hold = adj_str_hold[state->stat_ind[STAT_STR]];


	/* ------------------------------------
	 * Analyze current bow
	 * ------------------------------------ */

	/* Examine the "current bow" */
	obj = equipped_item_by_slot_name(p, "shooting");

	/* Assume not heavy */
	state->heavy_shoot = FALSE;

	/* Analyze launcher */
	if (obj) {
		if (hold < obj->weight / 10) {
			/* Hard to wield a heavy bow */
			state->to_h += 2 * (hold - obj->weight / 10);
			
			/* Heavy Bow */
			state->heavy_shoot = TRUE;
		}

		/* Get to shoot */
		state->num_shots = 1;

		/* Type of ammo */
		if (kf_has(obj->kind->kind_flags, KF_SHOOTS_SHOTS))
			state->ammo_tval = TV_SHOT;
		else if (kf_has(obj->kind->kind_flags, KF_SHOOTS_ARROWS))
			state->ammo_tval = TV_ARROW;
		else if (kf_has(obj->kind->kind_flags, KF_SHOOTS_BOLTS))
			state->ammo_tval = TV_BOLT;

		/* Multiplier */
		state->ammo_mult = obj->pval;

		/* Apply special flags */
		if (!state->heavy_shoot) {
			/* Extra shots */
			state->num_shots += extra_shots;

			/* Extra might */
			state->ammo_mult += extra_might;

			/* Hack -- Rangers love Bows */
			if (player_has(p, PF_EXTRA_SHOT) &&
				(state->ammo_tval == TV_ARROW)) {
				/* Extra shot at level 20 */
				if (p->lev >= 20) state->num_shots++;

				/* Extra shot at level 40 */
				if (p->lev >= 40) state->num_shots++;
			}
		}

		/* Require at least one shot */
		if (state->num_shots < 1) state->num_shots = 1;
	}


	/* ------------------------------------
	 * Analyze weapon
	 * ------------------------------------ */

	/* Examine the "current weapon" */
	obj = equipped_item_by_slot_name(p, "weapon");

	/* Assume not heavy */
	state->heavy_wield = FALSE;

	/* Assume no pointy problem */
	state->icky_wield = FALSE;

	if (obj) {
		/* It is hard to hold a heavy weapon */
		if (hold < obj->weight / 10) {
			/* Hard to wield a heavy weapon */
			state->to_h += 2 * (hold - obj->weight / 10);
			
			/* Heavy weapon */
			state->heavy_wield = TRUE;
		}

		/* Normal weapons */
		if (!state->heavy_wield) {
			/* Calculate number of blows */
			state->num_blows = calc_blows(p, obj, state, extra_blows);

			/* Boost digging skill by weapon weight */
			state->skills[SKILL_DIGGING] += (obj->weight / 10);
		}

		/* Priest weapon penalty for non-blessed edged weapons */
		if (player_has(p, PF_BLESS_WEAPON) &&
			!of_has(state->flags, OF_BLESSED) && tval_is_pointy(obj)) {
			/* Reduce the real bonuses */
			state->to_h -= 2;
			state->to_d -= 2;

			/* Icky weapon */
			state->icky_wield = TRUE;
		}
	} else {
		state->num_blows = calc_blows(p, NULL, state, extra_blows);
	}

	/* Call individual functions for other state fields */
	calc_torch(p, state);
	calc_mana(p, state);

	return;
}

/**
 * Calculate bonuses, and print various things on changes.
 */
static void update_bonuses(struct player *p)
{
	int i;

	struct player_state state = p->state;
	struct player_state known_state = p->known_state;


	/* ------------------------------------
	 * Calculate bonuses
	 * ------------------------------------ */

	calc_bonuses(p, &state, FALSE);
	calc_bonuses(p, &known_state, TRUE);


	/* ------------------------------------
	 * Notice changes
	 * ------------------------------------ */

	/* Analyze stats */
	for (i = 0; i < STAT_MAX; i++) {
		/* Notice changes */
		if (state.stat_top[i] != p->state.stat_top[i])
			/* Redisplay the stats later */
			p->upkeep->redraw |= (PR_STATS);

		/* Notice changes */
		if (state.stat_use[i] != p->state.stat_use[i])
			/* Redisplay the stats later */
			p->upkeep->redraw |= (PR_STATS);

		/* Notice changes */
		if (state.stat_ind[i] != p->state.stat_ind[i]) {
			/* Change in CON affects Hitpoints */
			if (i == STAT_CON)
				p->upkeep->update |= (PU_HP);

			/* Change in stats may affect Mana/Spells */
			p->upkeep->update |= (PU_MANA | PU_SPELLS);
		}
	}


	/* Hack -- Telepathy Change */
	if (of_has(state.flags, OF_TELEPATHY) !=
		of_has(p->state.flags, OF_TELEPATHY))
		/* Update monster visibility */
		p->upkeep->update |= (PU_MONSTERS);
	/* Hack -- See Invis Change */
	if (of_has(state.flags, OF_SEE_INVIS) !=
		of_has(p->state.flags, OF_SEE_INVIS))
		/* Update monster visibility */
		p->upkeep->update |= (PU_MONSTERS);

	/* Redraw speed (if needed) */
	if (state.speed != p->state.speed)
		p->upkeep->redraw |= (PR_SPEED);

	/* Redraw armor (if needed) */
	if ((known_state.ac != p->known_state.ac) || 
		(known_state.to_a != p->known_state.to_a))
		p->upkeep->redraw |= (PR_ARMOR);

	/* Notice changes in the "light radius" */
	if (p->state.cur_light != state.cur_light) {
		/* Update the visuals */
		p->upkeep->update |= (PU_UPDATE_VIEW | PU_MONSTERS);
	}

	/* Hack -- handle partial mode */
	if (!p->upkeep->only_partial) {
		/* Take note when "heavy bow" changes */
		if (p->state.heavy_shoot != state.heavy_shoot) {
			/* Message */
			if (state.heavy_shoot)
				msg("You have trouble wielding such a heavy bow.");
			else if (equipped_item_by_slot_name(p, "shooting"))
				msg("You have no trouble wielding your bow.");
			else
				msg("You feel relieved to put down your heavy bow.");
		}

		/* Take note when "heavy weapon" changes */
		if (p->state.heavy_wield != state.heavy_wield) {
			/* Message */
			if (state.heavy_wield)
				msg("You have trouble wielding such a heavy weapon.");
			else if (equipped_item_by_slot_name(p, "weapon"))
				msg("You have no trouble wielding your weapon.");
			else
				msg("You feel relieved to put down your heavy weapon.");	
		}

		/* Take note when "illegal weapon" changes */
		if (p->state.icky_wield != state.icky_wield) {
			/* Message */
			if (state.icky_wield)
				msg("You do not feel comfortable with your weapon.");
			else if (equipped_item_by_slot_name(p, "weapon"))
				msg("You feel comfortable with your weapon.");
			else
				msg("You feel more comfortable after removing your weapon.");
		}

		/* Take note when "glove state" changes */
		if (p->state.cumber_glove != state.cumber_glove) {
			/* Message */
			if (state.cumber_glove)
				msg("Your covered hands feel unsuitable for spellcasting.");
			else
				msg("Your hands feel more suitable for spellcasting.");
		}

		/* Take note when "armor state" changes */
		if (p->state.cumber_armor != state.cumber_armor) {
			/* Message */
			if (state.cumber_armor)
				msg("The weight of your armor encumbers your movement.");
			else
				msg("You feel able to move more freely.");
		}
	}

	memcpy(&p->state, &state, sizeof(state));
	memcpy(&p->known_state, &known_state, sizeof(known_state));
}




/**
 * ------------------------------------------------------------------------
 * Monster and object tracking functions
 * ------------------------------------------------------------------------ */

/**
 * Track the given monster
 */
void health_track(struct player_upkeep *upkeep, struct monster *mon)
{
	upkeep->health_who = mon;
	upkeep->redraw |= PR_HEALTH;
}

/**
 * Track the given monster race
 */
void monster_race_track(struct player_upkeep *upkeep, struct monster_race *race)
{
	/* Save this monster ID */
	upkeep->monster_race = race;

	/* Window stuff */
	upkeep->redraw |= (PR_MONSTER);
}

/**
 * Track the given object
 */
void track_object(struct player_upkeep *upkeep, struct object *obj)
{
	upkeep->object = obj;
	upkeep->object_kind = NULL;
	upkeep->redraw |= (PR_OBJECT);
}

/**
 * Track the given object kind
 */
void track_object_kind(struct player_upkeep *upkeep, struct object_kind *kind)
{
	upkeep->object = NULL;
	upkeep->object_kind = kind;
	upkeep->redraw |= (PR_OBJECT);
}

/**
 * Is the given item tracked?
 */
bool tracked_object_is(struct player_upkeep *upkeep, struct object *obj)
{
	return (upkeep->object == obj);
}



/**
 * ------------------------------------------------------------------------
 * Generic "deal with" functions
 * ------------------------------------------------------------------------ */

/**
 * Handle "player->upkeep->notice"
 */
void notice_stuff(struct player *p)
{
	/* Notice stuff */
	if (!p->upkeep->notice) return;

	/* Deal with ignore stuff */
	if (p->upkeep->notice & PN_IGNORE) {
		p->upkeep->notice &= ~(PN_IGNORE);
		ignore_drop();
	}

	/* Combine the pack */
	if (p->upkeep->notice & PN_COMBINE) {
		p->upkeep->notice &= ~(PN_COMBINE);
		combine_pack();
	}

	/* Dump the monster messages */
	if (p->upkeep->notice & PN_MON_MESSAGE) {
		p->upkeep->notice &= ~(PN_MON_MESSAGE);

		/* Make sure this comes after all of the monster messages */
		flush_all_monster_messages();
	}
}

/**
 * Handle "player->upkeep->update"
 */
void update_stuff(struct player *p)
{
	/* Update stuff */
	if (!p->upkeep->update) return;


	if (p->upkeep->update & (PU_INVEN)) {
		p->upkeep->update &= ~(PU_INVEN);
		update_inventory(p);
	}

	if (p->upkeep->update & (PU_BONUS)) {
		p->upkeep->update &= ~(PU_BONUS);
		update_bonuses(p);
	}

	if (p->upkeep->update & (PU_TORCH)) {
		p->upkeep->update &= ~(PU_TORCH);
		calc_torch(p, &p->state);
	}

	if (p->upkeep->update & (PU_HP)) {
		p->upkeep->update &= ~(PU_HP);
		calc_hitpoints(p);
	}

	if (p->upkeep->update & (PU_MANA)) {
		p->upkeep->update &= ~(PU_MANA);
		calc_mana(p, &p->state);
	}

	if (p->upkeep->update & (PU_SPELLS)) {
		p->upkeep->update &= ~(PU_SPELLS);
		calc_spells(p);
	}

	/* Character is not ready yet, no map updates */
	if (!character_generated) return;

	/* Map is not shown, no map updates */
	if (!map_is_visible()) return;

	if (p->upkeep->update & (PU_FORGET_VIEW)) {
		p->upkeep->update &= ~(PU_FORGET_VIEW);
		forget_view(cave);
	}

	if (p->upkeep->update & (PU_UPDATE_VIEW)) {
		p->upkeep->update &= ~(PU_UPDATE_VIEW);
		update_view(cave, p);
	}


	if (p->upkeep->update & (PU_FORGET_FLOW)) {
		p->upkeep->update &= ~(PU_FORGET_FLOW);
		cave_forget_flow(cave);
	}

	if (p->upkeep->update & (PU_UPDATE_FLOW)) {
		p->upkeep->update &= ~(PU_UPDATE_FLOW);
		cave_update_flow(cave);
	}


	if (p->upkeep->update & (PU_DISTANCE)) {
		p->upkeep->update &= ~(PU_DISTANCE);
		p->upkeep->update &= ~(PU_MONSTERS);
		update_monsters(TRUE);
	}

	if (p->upkeep->update & (PU_MONSTERS)) {
		p->upkeep->update &= ~(PU_MONSTERS);
		update_monsters(FALSE);
	}


	if (p->upkeep->update & (PU_PANEL)) {
		p->upkeep->update &= ~(PU_PANEL);
		event_signal(EVENT_PLAYERMOVED);
	}
}



struct flag_event_trigger
{
	u32b flag;
	game_event_type event;
};



/**
 * Events triggered by the various flags.
 */
static const struct flag_event_trigger redraw_events[] =
{
	{ PR_MISC,    EVENT_RACE_CLASS },
	{ PR_TITLE,   EVENT_PLAYERTITLE },
	{ PR_LEV,     EVENT_PLAYERLEVEL },
	{ PR_EXP,     EVENT_EXPERIENCE },
	{ PR_STATS,   EVENT_STATS },
	{ PR_ARMOR,   EVENT_AC },
	{ PR_HP,      EVENT_HP },
	{ PR_MANA,    EVENT_MANA },
	{ PR_GOLD,    EVENT_GOLD },
	{ PR_HEALTH,  EVENT_MONSTERHEALTH },
	{ PR_DEPTH,   EVENT_DUNGEONLEVEL },
	{ PR_SPEED,   EVENT_PLAYERSPEED },
	{ PR_STATE,   EVENT_STATE },
	{ PR_STATUS,  EVENT_STATUS },
	{ PR_STUDY,   EVENT_STUDYSTATUS },
	{ PR_DTRAP,   EVENT_DETECTIONSTATUS },
	{ PR_FEELING, EVENT_FEELING },

	{ PR_INVEN,   EVENT_INVENTORY },
	{ PR_EQUIP,   EVENT_EQUIPMENT },
	{ PR_MONLIST, EVENT_MONSTERLIST },
	{ PR_ITEMLIST, EVENT_ITEMLIST },
	{ PR_MONSTER, EVENT_MONSTERTARGET },
	{ PR_OBJECT, EVENT_OBJECTTARGET },
	{ PR_MESSAGE, EVENT_MESSAGE },
};

/**
 * Handle "player->upkeep->redraw"
 */
void redraw_stuff(struct player *p)
{
	size_t i;

	/* Redraw stuff */
	if (!p->upkeep->redraw) return;

	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;

	/* Map is not shown, no screen updates */
	if (!map_is_visible()) return;

	/* For each listed flag, send the appropriate signal to the UI */
	for (i = 0; i < N_ELEMENTS(redraw_events); i++) {
		const struct flag_event_trigger *hnd = &redraw_events[i];

		if (p->upkeep->redraw & hnd->flag)
			event_signal(hnd->event);
	}

	/* Then the ones that require parameters to be supplied. */
	if (p->upkeep->redraw & PR_MAP) {
		/* Mark the whole map to be redrawn */
		event_signal_point(EVENT_MAP, -1, -1);
	}

	p->upkeep->redraw = 0;

	/* Hack - don't update while resting or running, makes it over quicker */
	if (player_is_resting(p) || p->upkeep->running) return;

	/*
	 * Do any plotting, etc. delayed from earlier - this set of updates
	 * is over.
	 */
	event_signal(EVENT_END);
}


/**
 * Handle "player->upkeep->update" and "player->upkeep->redraw"
 */
void handle_stuff(struct player *p)
{
	if (p->upkeep->update) update_stuff(p);
	if (p->upkeep->redraw) redraw_stuff(p);
}

