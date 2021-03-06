/**
 * \file object.h
 * \brief basic object structs and enums
 */
#ifndef INCLUDED_OBJECT_H
#define INCLUDED_OBJECT_H

#include "z-rand.h"
#include "z-quark.h"
#include "z-bitflag.h"
#include "z-dice.h"
#include "obj-properties.h"
#include "effects.h"


/*** Game constants ***/

/**
 * Elements
 */
enum
{
	#define ELEM(a, b, c, d, e, f, g, h, i, col) ELEM_##a,
	#include "list-elements.h"
	#undef ELEM

	ELEM_MAX
};

#define ELEM_BASE_MIN  ELEM_ACID
#define ELEM_HIGH_MIN  ELEM_POIS
#define ELEM_HIGH_MAX  ELEM_DISEN

/**
 * Identify flags
 */
enum
{
	ID_NONE,
	#define STAT(a, b, c, d, e, f, g, h) ID_##a,
	#include "list-stats.h"
	#undef STAT
	#define OBJ_MOD(a, b, c, d) ID_##a,
	#include "list-object-modifiers.h"
	#undef OBJ_MOD
	#define ID(a) ID_##a,
	#include "list-identify-flags.h"
	#undef ID
	ID_MAX
};

#define ID_MOD_MIN  ID_STR
#define ID_MISC_MIN ID_ARTIFACT

#define ID_SIZE                	FLAG_SIZE(ID_MAX)

#define id_has(f, flag)        	flag_has_dbg(f, ID_SIZE, flag, #f, #flag)
#define id_next(f, flag)       	flag_next(f, ID_SIZE, flag)
#define id_is_empty(f)         	flag_is_empty(f, ID_SIZE)
#define id_is_full(f)          	flag_is_full(f, ID_SIZE)
#define id_is_inter(f1, f2)    	flag_is_inter(f1, f2, ID_SIZE)
#define id_is_subset(f1, f2)   	flag_is_subset(f1, f2, ID_SIZE)
#define id_is_equal(f1, f2)    	flag_is_equal(f1, f2, ID_SIZE)
#define id_on(f, flag)         	flag_on_dbg(f, ID_SIZE, flag, #f, #flag)
#define id_off(f, flag)        	flag_off(f, ID_SIZE, flag)
#define id_wipe(f)             	flag_wipe(f, ID_SIZE)
#define id_setall(f)           	flag_setall(f, ID_SIZE)
#define id_negate(f)           	flag_negate(f, ID_SIZE)
#define id_copy(f1, f2)        	flag_copy(f1, f2, ID_SIZE)
#define id_union(f1, f2)       	flag_union(f1, f2, ID_SIZE)
#define id_comp_union(f1, f2)  	flag_comp_union(f1, f2, ID_SIZE)
#define id_inter(f1, f2)       	flag_inter(f1, f2, ID_SIZE)
#define id_diff(f1, f2)        	flag_diff(f1, f2, ID_SIZE)


/**
 * Object origin kinds
 */

enum {
	#define ORIGIN(a, b, c) ORIGIN_##a,
	#include "list-origins.h"
	#undef ORIGIN

	ORIGIN_MAX
};


/* Values for struct object->marked */
enum {
	MARK_UNAWARE = 0,
	MARK_AWARE = 1,
	MARK_SEEN = 2
};



/*** Structures ***/

/* Brand type */
struct brand {
	char *name;
	int element;
	int multiplier;
	int damage; /* Storage for damage during description */
	bool known;
	struct brand *next;
};

/* Slay type */
struct slay {
	char *name;
	int race_flag;
	int multiplier;
	int damage; /* Storage for damage during description */
	bool known;
	struct slay *next;
};

enum {
	EL_INFO_KNOWN = 0x01,
	EL_INFO_HATES = 0x02,
	EL_INFO_IGNORE = 0x04,
	EL_INFO_RANDOM = 0x08,
};

/* Element info type */
struct element_info {
	s16b res_level;
	bitflag flags;
};

/**
 * Activation structure
 */
struct activation {
	struct activation *next;
	char *name;
	int index;
	bool aim;
	int power;
	struct effect *effect;
	char *message;
	char *desc;
};

extern struct activation *activations;

/**
 * Player-and-book-independent information about a magic spell
 */
struct magic_spell {
        char *name;
        char *text;
        struct effect *effect;
        struct magic_spell *next;
};

extern struct magic_spell *m_info;

/**
 * Book data 
 */


/**
 * Information about object types, like rods, wands, etc.
 */
struct object_base {
	char *name;

	int tval;
	struct object_base *next;

	int attr;

	bitflag flags[OF_SIZE];
	bitflag kind_flags[KF_SIZE];			/**< Kind flags */
	struct element_info el_info[ELEM_MAX];

	int break_perc;
	int num_svals;
};

extern struct object_base *kb_info;

/**
 * Information about object kinds, including player knowledge.
 *
 * TODO: split out the user-changeable bits into a separate struct so this
 * one can be read-only.
 */
struct object_kind {
	char *name;
	char *text;

	struct object_base *base;

	struct object_kind *next;
	u32b kidx;

	int tval;				/**< General object type (see TV_ macros) */
	int sval;				/**< Object sub-type  */
        
        int num_spells;                         /* Number of spells (below) */
        struct magic_spell *spell;              /* Spells in this book/tool */

	random_value pval;			/* Item extra-parameter */

	random_value to_h;			/**< Bonus to hit */
	random_value to_d;			/**< Bonus to damage */
	random_value to_a;			/**< Bonus to armor */
	int ac;					/**< Base armor */

	int dd;					/**< Damage dice */
	int ds;					/**< Damage sides */
	int weight;				/**< Weight, in 1/10lbs */

	int cost;					/**< Object base cost */

	bitflag flags[OF_SIZE];					/**< Flags */
	bitflag kind_flags[KF_SIZE];			/**< Kind flags */

	random_value modifiers[OBJ_MOD_MAX];
	struct element_info el_info[ELEM_MAX];

	struct brand *brands;
	struct slay *slays;

	byte d_attr;			/**< Default object attribute */
	wchar_t d_char;			/**< Default object character */

	int alloc_prob;			/**< Allocation: commonness */
	int alloc_min;			/**< Highest normal dungeon level */
	int alloc_max;			/**< Lowest normal dungeon level */
	int level;				/**< Level (difficulty of activation) */

	struct effect *effect;	/**< Effect this item produces (effects.c) */
	int power;				/**< Power of the item's effect */
	char *effect_msg;
	random_value time;		/**< Recharge time (rods/activation) */
	random_value charge;	/**< Number of charges (staves/wands) */

	int gen_mult_prob;		/**< Probability of generating more than one */
	random_value stack_size;/**< Number to generate */

	struct flavor *flavor;	/**< Special object flavor (or zero) */

	/** Also saved in savefile **/

	quark_t note; 	/**< Autoinscription quark number */

	bool aware;		/**< Set if player is aware of the kind's effects */
	bool tried;		/**< Set if kind has been tried */

	byte ignore;  	/**< Ignore settings */
	bool everseen; 	/**< Kind has been seen (to despoilify ignore menus) */
};

extern struct object_kind *k_info;

/**
 * Information about artifacts.
 *
 * Note that ::cur_num is written to the savefile.
 *
 * TODO: Fix this max_num/cur_num crap and just have a big boolean array of
 * which artifacts have been created and haven't, so this can become read-only.
 */
struct artifact {
	char *name;
	char *text;

	u32b aidx;

	struct artifact *next;

	int tval;		/**< General artifact type (see TV_ macros) */
	int sval;		/**< Artifact sub-type  */

	int to_h;		/**< Bonus to hit */
	int to_d;		/**< Bonus to damage */
	int to_a;		/**< Bonus to armor */
	int ac;		/**< Base armor */

	int dd;		/**< Base damage dice */
	int ds;		/**< Base damage sides */

	int weight;	/**< Weight in 1/10lbs */

	int cost;		/**< Artifact (pseudo-)worth */

	bitflag flags[OF_SIZE];			/**< Flags */

	int modifiers[OBJ_MOD_MAX];
	struct element_info el_info[ELEM_MAX];

	struct brand *brands;
	struct slay *slays;

	int level;			/** Difficulty level for activation */

	int alloc_prob;		/** Chance of being generated (i.e. rarity) */
	int alloc_min;		/** Minimum depth (can appear earlier) */
	int alloc_max;		/** Maximum depth (will NEVER appear deeper) */

	bool created;		/**< Whether this artifact has been created */
	bool seen;			/**< Whether this artifact has been seen this game */
	bool everseen;		/**< Whether this artifact has ever been seen  */

	struct activation *activation;	/**< Artifact activation */
	char *alt_msg;

	random_value time;	/**< Recharge time (if appropriate) */
};

/**
 * The artifact arrays
 */
extern struct artifact *a_info;


/**
 * Stricture for possible object kinds for an ego item
 */
struct ego_poss_item {
	u32b kidx;
	struct ego_poss_item *next;
};

/**
 * Information about ego-items.
 */
struct ego_item {
	struct ego_item *next;

	char *name;
	char *text;

	u32b eidx;

	int cost;						/* Ego-item "cost" */

	bitflag flags[OF_SIZE];			/**< Flags */
	bitflag kind_flags[KF_SIZE];	/**< Kind flags */

	random_value modifiers[OBJ_MOD_MAX];
	int min_modifiers[OBJ_MOD_MAX];
	struct element_info el_info[ELEM_MAX];

	struct brand *brands;
	struct slay *slays;

	int level;				/* Minimum level */
	int rarity;			/* Object rarity */
	int rating;			/* Level rating boost */
	int alloc_prob; 		/** Chance of being generated (i.e. rarity) */
	int alloc_min;			/** Minimum depth (can appear earlier) */
	int alloc_max;			/** Maximum depth (will NEVER appear deeper) */

	struct ego_poss_item *poss_items;

	random_value to_h;		/* Extra to-hit bonus */
	random_value to_d;		/* Extra to-dam bonus */
	random_value to_a;		/* Extra to-ac bonus */

	int min_to_h;			/* Minimum to-hit value */
	int min_to_d;			/* Minimum to-dam value */
	int min_to_a;			/* Minimum to-ac value */

	struct effect *effect;	/**< Effect this item produces (effects.c) */
	char *effect_msg;
	random_value time;		/**< Recharge time (rods/activation) */

	bool everseen;			/* Do not spoil ignore menus */
};

/*
 * The ego-item arrays
 */
extern struct ego_item *e_info;


/*
 * Object information, for a specific object.
 *
 * Note that inscriptions are now handled via the "quark_str()" function
 * applied to the "note" field, which will return NULL if "note" is zero.
 *
 * Each cave grid points to one (or zero) objects via the "obj" field in
 * its "squares" struct.  Each object then points to one (or zero) objects
 * via the "next" field, and (aside from the first) back via its "prev"
 * field, forming a doubly linked list, which in game terms represents a
 * stack of objects in the same grid.
 *
 * Each monster points to one (or zero) objects via the "held_obj"
 * field (see monster.h).  Each object then points to one (or zero) objects
 * and back to previous objects by its own "next" and "prev" fields,
 * forming a doubly linked list, which in game terms represents the
 * monster's inventory.
 *
 * The "held_m_idx" field is used to indicate which monster, if any,
 * is holding the object.  Objects being held have "ix = 0" and "iy = 0".
 *
 * Note that object records are not now copied, but allocated on object
 * creation and freed on object destruction.  These records are handed
 * around between player and monster inventories and the floor on a fairly
 * regular basis, and care must be taken when handling such objects.
 */
struct object {
	struct object_kind *kind;
	struct ego_item *ego;
	struct artifact *artifact;

	struct object *prev;	/* Previous object in a pile */
	struct object *next;	/* Next object in a pile */

	byte iy;			/* Y-position on map, or zero */
	byte ix;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	s16b pval;			/* Item extra-parameter */

	s16b weight;		/* Item weight */

	bitflag flags[OF_SIZE];			/**< Flags */
	bitflag known_flags[OF_SIZE];	/**< Player-known flags */
	bitflag id_flags[ID_SIZE];		/**< Object property ID flags */

	s16b modifiers[OBJ_MOD_MAX];
	struct element_info el_info[ELEM_MAX];

	struct brand *brands;
	struct slay *slays;

	s16b ac;			/* Normal AC */
	s16b to_a;			/* Plusses to AC */
	s16b to_h;			/* Plusses to hit */
	s16b to_d;			/* Plusses to damage */

	byte dd, ds;		/* Damage dice/sides */

	struct effect *effect;	/**< Effect this item produces (effects.c) */
	char *effect_msg;
	struct activation *activation;	/**< Artifact activation, if applicable */
	random_value time;	/**< Recharge time (rods/activation) */
	s16b timeout;		/* Timeout Counter */

	byte number;		/* Number of items */
	byte marked;		/* Object is marked */
	byte ignore;		/* Object is ignored */

	s16b held_m_idx;	/* Monster holding us (if any) */
	s16b mimicking_m_idx; /* Monster mimicking us (if any) */

	byte origin;		/* How this item was found */
	byte origin_depth;  /* What depth the item was found at */
	u16b origin_xtra;   /* Extra information about origin */

	quark_t note; 		/* Inscription index */
};

struct flavor
{
	char *text;
	struct flavor *next;
	unsigned int fidx;

	byte tval;	  /* Associated object type */
	byte sval;	  /* Associated object sub-type */

	byte d_attr;	/* Default flavor attribute */
	wchar_t d_char;	/* Default flavor character */
};

extern struct flavor *flavors;


typedef bool (*item_tester)(const struct object *);


#endif /* !INCLUDED_OBJECT_H */
