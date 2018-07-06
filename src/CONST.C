/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"



/*
 * Class table.
 */
const	struct	class_type	class_table	[MAX_CLASS]	=
{
	 {
	"Mag",   APPLY_INT, OBJ_VNUM_SCHOOL_DAGGER,
	3018,  95,  18, 10,  6,  8, TRUE
	 },

	 {
	"Cle",   APPLY_WIS, OBJ_VNUM_SCHOOL_MACE,
	3003,  95,  18, 12,  7, 10, TRUE
	 },

	 {
	"Thi",   APPLY_DEX, OBJ_VNUM_SCHOOL_DAGGER,
	3028,  85,  18,  8,  8, 13, FALSE
	 },

	 {
	"War",   APPLY_STR, OBJ_VNUM_SCHOOL_SWORD,
	3022,  85,  18,  6,  11, 15, FALSE
	 },

	 {
	"Psi",   APPLY_INT, OBJ_VNUM_SCHOOL_DAGGER,
	3151,  95,  18,  9,   6,  9, TRUE
	 },

	 {
	"Ran",   APPLY_CON, OBJ_VNUM_SCHOOL_SWORD,
	3160,  90,  18,  6,   11, 15, TRUE
	  }
	  };

#define CLASS_MAGE       0
#define CLASS_CLERIC     1
#define CLASS_THIEF      2
#define CLASS_WARRIOR    3
#define CLASS_PSIONICIST 4
#define CLASS_RANGER     5
/*
 * Titles.
 */
char *	const			title_table	[MAX_CLASS][MAX_LEVEL+1][2] =
{
	 {
	{ "Man",			"Woman"				},

	{ "Apprentice of Magic",	"Apprentice of Magic"		},
	{ "Spell Student",		"Spell Student"			},
	{ "Scholar of Magic",		"Scholar of Magic"		},
	{ "Delver in Spells",		"Delveress in Spells"		},
	{ "Medium of Magic",		"Medium of Magic"		},

	{ "Scribe of Magic",		"Scribess of Magic"		},
	{ "Seer",			"Seeress"			},
	{ "Sage",			"Sage"				},
	{ "Illusionist",		"Illusionist"			},
	{ "Abjurer",			"Abjuress"			},

	{ "Invoker",			"Invoker"			},
	{ "Enchanter",			"Enchantress"			},
	{ "Conjurer",			"Conjuress"			},
	{ "Magician",			"Witch"				},
	{ "Creator",			"Creator"			},

	{ "Savant",			"Savant"			},
	{ "Magus",			"Craftess"			},
	{ "Wizard",			"Wizard"			},
	{ "Warlock",			"War Witch"			},
	{ "Sorcerer",			"Sorceress"			},

	{ "Elder Sorcerer",		"Elder Sorceress"		},
	{ "Grand Sorcerer",		"Grand Sorceress"		},
	{ "Great Sorcerer",		"Great Sorceress"		},
	{ "Golem Maker",		"Golem Maker"			},
	{ "Greater Golem Maker",	"Greater Golem Maker"		},

	{ "Maker of Stones",		"Maker of Stones",		},
	{ "Maker of Potions",		"Maker of Potions",		},
	{ "Maker of Scrolls",		"Maker of Scrolls",		},
	{ "Maker of Wands",		"Maker of Wands",		},
	{ "Maker of Staves",		"Maker of Staves",		},

	{ "Demon Summoner",		"Demon Summoner"		},
	{ "Greater Demon Summoner",	"Greater Demon Summoner"	},
	{ "Dragon Charmer",		"Dragon Charmer"		},
	{ "Greater Dragon Charmer",	"Greater Dragon Charmer"	},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Hero",			"Heroine"			},

	{ "Angel",		"Angel"		},
	{ "Saint",		"Saint"		},
	{ "Deity",		"Deity"		},
	{ "God",		"Goddess" 		}
	 },

	 {
	{ "Man",			"Woman"				},

	{ "Believer",			"Believer"			},
	{ "Attendant",			"Attendant"			},
	{ "Acolyte",			"Acolyte"			},
	{ "Novice",			"Novice"			},
	{ "Missionary",			"Missionary"			},

	{ "Adept",			"Adept"				},
	{ "Deacon",			"Deaconess"			},
	{ "Vicar",			"Vicaress"			},
	{ "Priest",			"Priestess"			},
	{ "Minister",			"Lady Minister"			},

	{ "Canon",			"Canon"				},
	{ "Levite",			"Levitess"			},
	{ "Curate",			"Curess"			},
	{ "Monk",			"Nun"				},
	{ "Healer",			"Healess"			},

	{ "Chaplain",			"Chaplain"			},
	{ "Expositor",			"Expositress"			},
	{ "Bishop",			"Bishop"			},
	{ "Arch Bishop",		"Arch Lady of the Church"	},
	{ "Patriarch",			"Matriarch"			},

	{ "Elder Patriarch",		"Elder Matriarch"		},
	{ "Grand Patriarch",		"Grand Matriarch"		},
	{ "Great Patriarch",		"Great Matriarch"		},
	{ "Demon Killer",		"Demon Killer"			},
	{ "Greater Demon Killer",	"Greater Demon Killer"		},

	{ "Cardinal of the Sea",	"Cardinal of the Sea"		},
	{ "Cardinal of the Earth",	"Cardinal of the Earth"		},
	{ "Cardinal of the Air",	"Cardinal of the Air"		},
	{ "Cardinal of the Ether",	"Cardinal of the Ether"		},
	{ "Cardinal of the Heavens",	"Cardinal of the Heavens"	},

	{ "Avatar of an Immortal",	"Avatar of an Immortal"		},
	{ "Avatar of a Deity",		"Avatar of a Deity"		},
	{ "Avatar of a Supremity",	"Avatar of a Supremity"		},
	{ "Avatar of an Implementor",	"Avatar of an Implementor"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Hero",			"Heroine"			},

	{ "Angel",			"Angel"				},
	{ "Saint",			"Saint"				},
	{ "Deity",		"Deity"		},
	{ "God",		"Goddess"			}
	 },

	 {
	{ "Man",			"Woman"				},

	{ "Pilferer",			"Pilferess"			},
	{ "Footpad",			"Footpad"			},
	{ "Filcher",			"Filcheress"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Sneak",			"Sneak"				},

	{ "Pincher",			"Pincheress"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Rogue",			"Rogue"				},

	{ "Robber",			"Robber"			},
	{ "Magsman",			"Magswoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Burglar",			"Burglaress"			},
	{ "Thief",			"Thief"				},

	{ "Knifer",			"Knifer"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Killer",			"Murderess"			},
	{ "Brigand",			"Brigand"			},
	{ "Cut-Throat",			"Cut-Throat"			},

	{ "Spy",			"Spy"				},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Assassin",			"Assassin"			},
	{ "Greater Assassin",		"Greater Assassin"		},

	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Touch",		"Mistress of Touch"		},

	{ "Crime Lord",			"Crime Mistress"		},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Hero",		"Heroine"		},

	{ "Angel",		"Angel"		},
	{ "Saint",		"Saint"		},
	{ "Deity",		"Deity"		},
	{ "God",		"Goddess"			}
	 },

	 {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Hero",		"Heroine"		},

	{ "Angel",		"Angel"			},
	{ "Saint",		"Saint"			},
	{ "Deity",	"Deity"	},
	{ "God",		"Goddess"			}
	 },

	 {
		  { "Man",                        "Woman"                         },

		  { "Psychic",                    "Psychic",                      },
		  { "Medium",                     "Medium",                       },
		  { "Gypsy",                      "Gypsy",                        },
		  { "Meditator",                  "Meditator",                    },
		  { "Mind Prober",                "Mind Prober",                  },

		  { "Soul Searcher",              "Soul Searcher",                },
		  { "Astral Voyager",             "Astral Voyager",               },
		  { "Seeker",                     "Seeker",                       },
		  { "Empath",                     "Empath",                       },
		  { "Mind Reader",                "Mind Reader"                   },

		  { "Telepath",                   "Telepath",                     },
		  { "Mental Adept",               "Mental Adept",                 },
		  { "Spoonbender",                "Spoonbender",                  },
		  { "Perceptive",                 "Perceptive",                   },
		  { "Clever",                     "Clever",                       },

		  { "Wise",                       "Wise",                         },
		  { "Genius",                     "Genius",                       },
		  { "Oracle",                     "Oracle",                       },
		  { "Soothsayer",                 "Soothsayer",                   },
		  { "Truthteller",                "Truthteller",                  },

		  { "Sage",                       "Sage",                         },
		  { "Master Psychic",             "Mistress Psychic",             },
		  { "Master Meditator",           "Mistress Meditator",           },
		  { "Master Empath",              "Mistress Empath",              },
		  { "Master Clairvoyant",         "Mistress Clairvoyant",         },

		  { "Master Mind Reader",         "Mistress Mind Reader",         },
		  { "Master Telepath",            "Mistress Telepath",            },
		  { "Master Spoonbender",         "Mistress Spoonbender",         },
		  { "Grand Master Psychic",       "Grand Mistress Psychic",       },
		  { "Grand Master Meditator",     "Grand Mistress Meditator",     },

		  { "Grand Master Empath",        "Grand Mistress Empath",        },
		  { "Grand Master Clairvoyant",   "Grand Mistress Clairvoyant",   },
		  { "Grand Master Mind Reader",   "Grand Mistress Mind Reader",   },
		  { "Grand Master Telepath",      "Grand Mistress Telepath",      },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
		  { "Hero",            "Heroine",           },

		  { "Angel",           "Angel"              },
		  { "Saint",          "Saint"             },
		  { "Deity",          "Deity"            },
		  { "God",                "Goddess"                 }
	 },

	 {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Hero",		"Heroine"		},

	{ "Angel",		"Angel"			},
	{ "Saint",		"Saint"			},
	{ "Deity",	"Deity"	},
	{ "God",		"Goddess"			}
	 },
};

/*
 * Race table.
 */
const	struct	race_type	race_table	[MAX_RACE]	=
{
	 {
	 "Hum", "Human", 0, 0, 0, 0, 0, 3054, 3001,
		 0
	 },

	 {
	 "Dwa", "Dwarf", 3, -1, 0, -3, 5, 3054, 3001,
		  0
	 },

	 {
	 "Hor", "Half-Ogre", 5, -6, -3, -4, 6, 3054, 3001,
		  AFF_INFRARED
	 },

	 {
	 "Elf", "Grey Elf", -1, 4, 2, 3, 0, 3054, 3001,
		  AFF_INFRARED
	 },

	 {
	 "Orc", "Orc", 4, -5, -4, -1, 5, 3054, 3001,
		  AFF_INFRARED
	 },

	 {
	 "Dro", "Drow Elf", -1, 3, 2, 5, -2, 3054, 3001,
		  AFF_INFRARED
	 },

		 {
	 "Tro", "Troll", 6, -4, -4, 1, 5, 3054, 3001,
		  AFF_INFRARED
	 },

		 {
	 "Hel", "Half-Elf", -1, 2, 1, 2, -1, 3054, 3001,
		  AFF_INFRARED
	 },

		 {
	 "Pix", "Pixie", -3, 5, 5, 3, -3, 3054, 3001,
		  AFF_FLYING
	 },

	 {
	 "Haf", "Halflings", -2, 2, 2, 6, 0, 3054, 3001,
		  AFF_SNEAK
	 },

		 {
	 "Gno", "Gnome", -2, 3, 3, 1, 0, 3054, 3001,
		  0
	 },

		 {
	 "Gia", "Giant", 5, -2, -2, -3, 4, 3054, 3001,
		  AFF_PROTECT
	 }
};


/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
	 { -5, -4,   0,  0 },  /* 0  */
	 { -5, -4,   3,  1 },  /* 1  */
	 { -3, -2,   3,  2 },
	 { -3, -1,  10,  3 },  /* 3  */
	 { -2, -1,  25,  4 },
	 { -2, -1,  55,  5 },  /* 5  */
	 { -1,  0,  80,  6 },
	 { -1,  0,  90,  7 },
	 {  0,  0, 100,  8 },
	 {  0,  0, 100,  9 },
	 {  0,  0, 115, 10 }, /* 10  */
	 {  0,  0, 115, 11 },
	 {  0,  0, 140, 12 },
	 {  0,  0, 140, 13 }, /* 13  */
	 {  0,  1, 170, 14 },
	 {  1,  1, 170, 15 }, /* 15  */
	 {  1,  2, 195, 16 },
	 {  2,  3, 220, 22 },
	 {  2,  4, 250, 25 }, /* 18  */
	 {  3,  5, 400, 30 },
	 {  3,  6, 500, 35 }, /* 20  */
	 {  4,  7, 600, 40 },
	 {  5,  7, 700, 45 },
	 {  6,  8, 800, 50 },
	 {  8, 10, 900, 55 },
	 { 10, 12, 999, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
	 {  3 },	/*  0 */
	 {  5 },	/*  1 */
	 {  7 },
	 {  8 },	/*  3 */
	 {  9 },
	 { 10 },	/*  5 */
	 { 11 },
	 { 12 },
	 { 13 },
	 { 15 },
	 { 17 },	/* 10 */
	 { 19 },
	 { 22 },
	 { 25 },
	 { 28 },
	 { 31 },	/* 15 */
	 { 34 },
	 { 37 },
	 { 40 },	/* 18 */
	 { 44 },
	 { 49 },	/* 20 */
	 { 55 },
	 { 60 },
	 { 70 },
	 { 85 },
	 { 99 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
	 { 0 },	/*  0 */
	 { 0 },	/*  1 */
	 { 0 },
	 { 0 },	/*  3 */
	 { 0 },
	 { 1 },	/*  5 */
	 { 1 },
	 { 1 },
	 { 1 },
	 { 2 },
	 { 2 },	/* 10 */
	 { 2 },
	 { 2 },
	 { 2 },
	 { 2 },
	 { 3 },	/* 15 */
	 { 3 },
	 { 4 },
	 { 4 },	/* 18 */
	 { 5 },
	 { 5 },	/* 20 */
	 { 6 },
	 { 7 },
	 { 7 },
	 { 7 },
	 { 8 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
	 {   60 },   /* 0 */
	 {   50 },   /* 1 */
	 {   50 },
	 {   40 },
	 {   30 },
	 {   20 },   /* 5 */
	 {   10 },
	 {    0 },
	 {    0 },
	 {    0 },
	 {    0 },   /* 10 */
	 {    0 },
	 {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 65 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};



const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
	 { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
	 {  7, 99 },
    {  8, 99 }    /* 25 */
};

const   struct  color_data color_table[] =
{
	{ "\x1b[30m",     "$R$0",   "black",         0 },
	{ "\x1b[34m",     "$R$1",   "blue",          1 },
	{ "\x1b[32m",     "$R$2",   "green",         2 },
	{ "\x1b[36m",     "$R$3",   "cyan",          3 },
	{ "\x1b[31m",     "$R$4",   "red",           4 },
	{ "\x1b[35m",     "$R$5",   "purple",        5 },
	{ "\x1b[33m",     "$R$6",   "brown",         6 },
	{ "\x1b[37m",     "$R$7",   "grey",          7 },
	{ "\x1b[30;1m",   "$B$0",   "dark_grey",     8 },
	{ "\x1b[34;1m",   "$B$1",   "light_blue",    9 },
	{ "\x1b[32;1m",   "$B$2",   "light_green",  10 },
	{ "\x1b[36;1m",   "$B$3",   "light_cyan",   11 },
	{ "\x1b[31;1m",   "$B$4",   "light_red",    12 },
	{ "\x1b[35;1m",   "$B$5",   "magenta",      13 },
	{ "\x1b[33;1m",   "$B$6",   "yellow",       14 },
	{ "\x1b[37;1m",   "$B$7",   "white",        15 },
	{ "\x1b[1m",      "$B",     "bold",         16 },
	{ "\x1b[5m",      "$F",     "flashing",     17 },
	{ "\x1b[7m",      "$I",     "inverse",      18 },
	{ "\x1b[0m",      "$R",     "normal",       19 }
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
	 { "water",			"clear",	{  0, 1, 10 }	},  /*  0 */
	 { "beer",			"amber",	{  3, 2,  5 }	},
	 { "wine",			"rose",		{  5, 2,  5 }	},
	 { "ale",			"brown",	{  2, 2,  5 }	},
	 { "dark ale",		"dark",		{  1, 2,  5 }	},

	 { "whisky",			"golden",	{  6, 1,  4 }	},  /*  5 */
	 { "lemonade",		"pink",		{  0, 1,  8 }	},
	 { "firebreather",		"boiling",	{ 10, 0,  0 }	},
	 { "local specialty",	"everclear",	{  3, 3,  3 }	},
	 { "slime mold juice",	"green",	{  0, 4, -8 }	},

	 { "milk",			"white",	{  0, 3,  6 }	},  /* 10 */
	 { "tea",			"tan",		{  0, 1,  6 }	},
	 { "coffee",			"black",	{  0, 1,  6 }	},
	 { "blood",			"red",		{  0, 2, -1 }	},
	 { "salt water",		"clear",	{  0, 1, -2 }	},

	 { "cola",			"cherry",	{  0, 1,  5 }	}   /* 15 */
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const	struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

	 {
	"reserved",		{ 99, 99, 99, 99, 99, 99 },
	0,			TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT( 0),	 0,	 0,
	"",			""
	 },

	 {
	"acid blast",		{ 20, 51, 51, 51, 51, 51 },
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(70),	20,	12,
	"acid blast",		"!Acid Blast!"
	 },

	 {
	"armor",		{  5,  1, 51, 51, 51, 51  },
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 1),	 5,	12,
	"",			"You feel less protected."
	 },

	 {
	"bless",		{ 51,  5, 51, 51, 51, 51  },
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 3),	 5,	12,
	"",			"You feel less righteous."
	 },

	 {
	"blindness",		{  8,  5, 51, 51, 51, 51  },
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		SLOT( 4),	 5,	12,
	"",			"You can see again."
	 },

	 {
	"burning hands",	{  5, 51, 51, 51, 51, 51  },
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 5),	15,	12,
	"burning hands",	"!Burning Hands!"
	 },

	 {
	"call lightning",	{ 51, 12, 51, 51, 51, 51  },
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT( 6),	15,	12,
	"lightning bolt",	"!Call Lightning!"
	 },

	 {
	"cause critical",	{ 51,  9, 51, 51, 51, 51  },
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(63),	20,	12,
	"spell",		"!Cause Critical!"
	 },

	 {
	"cause light",		{ 51,  1, 51, 51, 51, 51  },
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(62),	15,	12,
	"spell",		"!Cause Light!"
	 },

	 {
	"cause serious",	{ 51,  5, 51, 51, 51, 51  },
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(64),	17,	12,
	"spell",		"!Cause Serious!"
	 },

	 {
	"change sex",		{ 51, 51, 51, 51, 51, 51  },
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(82),	15,	12,
	"",			"Your body feels familiar again."
	 },

	 {
	"charm person",		{ 14, 51, 51, 51, 51, 51  },
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	SLOT( 7),	 5,	12,
	"",			"You feel more self-confident."
	 },

	 {
	"chill touch",		{  3, 51, 51, 51, 51, 51  },
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 8),	15,	12,
	"chilling touch",	"You feel less cold."
	 },

	 {
	"colour spray",		{ 11, 51, 51, 51, 51, 51  },
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(10),	15,	12,
	"colour spray",		"!Colour Spray!"
	 },

	 {
	"continual light",	{  4,  2, 51, 51, 51, 35  },
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(57),	 7,	12,
	"",			"!Continual Light!"
	 },

	 {
	"control weather",	{ 10, 13, 51, 51, 51, 15  },
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(11),	25,	12,
	"",			"!Control Weather!"
	 },

	 {
	"create food",		{ 51,  3, 51, 51, 51, 15  },
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(12),	 5,	12,
	"",			"!Create Food!"
	 },

	 {
	"create spring",	{ 10, 51, 51, 51, 51, 51  },
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(80),	20,	12,
	"",			"!Create Spring!"
	 },

	 {
	"create water",		{ 51,  2, 51, 51, 51, 5  },
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(13),	 5,	12,
	"",			"!Create Water!"
	 },

	 {
	"cure blindness",	{ 51,  4, 51, 51, 51, 51  },
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(14),	 5,	12,
	"",			"!Cure Blindness!"
	 },

	 {
	"cure critical",	{ 51,  9, 51, 51, 51, 51  },
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(15),	20,	12,
	"",			"!Cure Critical!"
	 },

	 {
	"cure light",		{ 51,  1, 51, 51, 51, 51  },
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(16),	10,	12,
	"",			"!Cure Light!"
	 },

	 {
	"cure poison",		{ 51,  9, 51, 51, 51, 45  },
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(43),	 5,	12,
	"",			"!Cure Poison!"
	 },

	 {
	"cure serious",		{ 51,  5, 51, 51, 51, 51  },
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(61),	15,	12,
	"",			"!Cure Serious!"
	 },

	 {
	"curse",		{ 12, 12, 51, 51, 51, 51  },
	spell_curse,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_curse,		SLOT(17),	20,	12,
	"curse",		"The curse wears off."
	 },

	 {
	"deafness",		{ 51, 51, 51, 51, 51, 51  },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(52),	15,	12,
	"",		"!Deafness!"
	 },

	 {
	"detect evil",		{ 51,  4, 51, 51, 51, 51  },
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(18),	 5,	12,
	"",			"The red in your vision disappears."
	 },

	 {
	"detect hidden",	{ 51,  7, 51, 51, 51, 15  },
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(44),	 5,	12,
	"",			"You feel less aware of your suroundings."
	 },

	 {
	"detect invis",		{  2,  5, 51, 51, 51, 51  },
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(19),	 5,	12,
	"",			"You no longer see invisible objects."
	 },

	 {
	"detect magic",		{  2,  3, 51, 51, 51, 51  },
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(20),	 5,	12,
	"",			"The detect magic wears off."
	 },

	 {
	"detect poison",	{ 51,  5, 51, 51, 51, 51  },
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(21),	 5,	12,
	"",			"!Detect Poison!"
	 },

	 {
	"dispel evil",		{ 51, 10, 51, 51, 51, 51  },
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(22),	15,	12,
	"dispel evil",		"!Dispel Evil!"
	 },

	 {
	"dispel magic",		{ 26, 31, 51, 51, 51, 51  },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(59),	15,	12,
	"",			"!Dispel Magic!"
	 },

	 {
	"earthquake",		{ 51,  7, 51, 51, 51, 51  },
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(23),	15,	12,
	"earthquake",		"!Earthquake!"
	 },

	 {
	"enchant weapon",	{ 12, 51, 51, 51, 51, 51  },
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(24),	100,	24,
	"",			"!Enchant Weapon!"
	 },

	 {
	"energy drain",		{ 13, 51, 51, 51, 51, 51  },
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(25),	35,	12,
	"energy drain",		"!Energy Drain!"
	 },

	 {
	"faerie fire",		{  4,  2, 51, 51, 51, 51  },
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(72),	 5,	12,
	"faerie fire",		"The pink aura around you fades away."
	 },

	 {
	"faerie fog",		{ 10, 14, 51, 51, 51, 51  },
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(73),	12,	12,
	"faerie fog",		"!Faerie Fog!"
	 },

	 {
	"fireball",		{ 15, 51, 51, 51, 51, 51  },
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(26),	15,	12,
	"fireball",		"!Fireball!"
	 },

	 {
	"flamestrike",		{ 51, 13, 51, 51, 51, 51  },
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(65),	20,	12,
	"flamestrike",		"!Flamestrike!"
	 },

	 {
	"fly",			{  7, 12, 51, 51, 51, 51  },
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(56),	10,	18,
	"",			"You slowly float to the ground."
	 },

	 {
	"gate",			{ 51, 51, 51, 51, 51, 51  },
	spell_gate,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(83),	50,	12,
	"",			"!Gate!"
	 },

	 {
	"giant strength",	{  7, 51, 51, 51, 51, 51  },
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(39),	20,	12,
	"",			"You feel weaker."
	 },

	 {
	"harm",			{ 51, 15, 51, 51, 51, 51  },
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(27),	35,	12,
	"harm spell",		"!Harm!"
	 },

	 {
	"heal",			{ 51, 14, 51, 51, 51, 30  },
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(28),	50,	12,
	"",			"!Heal!"
	 },

	 {
	"identify",		{ 10, 10, 51, 51, 51, 51  },
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(53),	12,	24,
	"",			"!Identify!"
	 },

	 {
	"infravision",		{  6,  9, 51, 51, 51, 51  },
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(77),	 5,	18,
	"",			"You no longer see in the dark."
	 },

	 {
	"invis",		{  4, 51, 51, 51, 51, 51  },
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		SLOT(29),	 5,	12,
	"",			"You are no longer invisible."
	 },

	 {
	"know alignment",	{  8,  5, 51, 51, 51, 51  },
	spell_know_alignment,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(58),	 9,	12,
	"",			"!Know Alignment!"
	 },

	 {
	"lightning bolt",	{  9, 51, 51, 51, 51, 51  },
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(30),	15,	12,
	"lightning bolt",	"!Lightning Bolt!"
	 },

	 {
	"locate object",	{  6, 10, 51, 51, 51, 51  },
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(31),	20,	18,
	"",			"!Locate Object!"
	 },

	 {
	"magic missile",	{  1, 51, 51, 51, 51, 51  },
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(32),	15,	12,
	"magic missile",	"!Magic Missile!"
	 },

	 // @@@
	 {
	"make purse",	{ 51, 51, 51, 51, 51, 51  },
	spell_make_purse,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(50),	100,	24,
	"",			"!Make Purse!"
	 },

	 {
	"mass invis",		{ 15, 17, 51, 51, 51, 51  },
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	SLOT(69),	20,	24,
	"",			"!Mass Invis!"
	 },

	 {
	"pass door",		{ 18, 51, 51, 51, 51, 51  },
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(74),	20,	12,
	"",			"You feel solid again."
	 },

	 {
	"poison",		{ 51,  8, 51, 51, 51, 51  },
	spell_poison,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_poison,		SLOT(33),	10,	12,
	"poison",		"You feel less sick."
	 },

	 {
	"protection",		{ 51,  6, 51, 51, 51, 51  },
	spell_protection,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(34),	 5,	12,
	"",			"You feel less protected."
	 },

	 {
	"refresh",		{  5,  3, 51, 51, 51, 51  },
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(81),	12,	18,
	"refresh",		"!Refresh!"
	 },

	 {
	"remove curse",		{ 51, 12, 51, 51, 51, 51  },
	spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(35),	 5,	12,
	"",			"!Remove Curse!"
	 },

	 {
	"sanctuary",		{ 51, 13, 51, 51, 51, 51  },
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(36),	75,	12,
	"",			"The white aura around your body fades."
	 },

	 {
	"shield",		{ 13, 51, 51, 51, 51, 51  },
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(67),	12,	18,
	"",			"Your force shield shimmers then fades away."
	 },

	 {
	"shocking grasp",	{  7, 51, 51, 51, 51, 51  },
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(51),	15,	12,
	"shocking grasp",	"!Shocking Grasp!"
	 },

	 {
	"silence",		{ 51, 51, 51, 51, 51, 51  },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(112),	15,	12,
	"",		"!Silence!"
	 },

	 {
	"sleep",		{ 14, 51, 51, 51, 51, 51  },
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		SLOT(38),	15,	12,
	"",			"You feel less tired."
	 },

	 {
	"stone skin",		{ 17, 51, 51, 51, 51, 51  },
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(66),	12,	18,
	"",			"Your skin feels soft again."
	 },

	 {
	"summon",		{ 51,  8, 51, 51, 51, 51  },
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(40),	50,	12,
	"",			"!Summon!"
	 },

	 {
	"teleport",		{  8, 51, 51, 51, 51, 51  },
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		SLOT( 2),	35,	12,
	"",			"!Teleport!"
	 },

	 {
	"vampiric touch",		{ 51, 51, 51, 51, 51, 51  },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(85),	15,	12,
	"",		"!Vampiric Touch!"
	 },

	 {
	"ventriloquate",	{  1, 51, 51, 51, 51, 45  },
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(41),	 5,	12,
	"",			"!Ventriloquate!"
	 },

	 {
	"weaken",		{  7, 51, 51, 51, 51, 51  },
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(68),	20,	12,
	"spell",		"You feel stronger."
	 },

	 {
	"word of recall",	{ 51, 51, 51, 51, 51, 51  },
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			SLOT(42),	 5,	12,
	"",			"!Word of Recall!"
	 },

/*
 * Dragon breath
 */
	 {
	"acid breath",		{ 33, 51, 51, 51, 51, 51  },
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(200),	 0,	 4,
	"blast of acid",	"!Acid Breath!"
	 },

	 {
	"fire breath",		{ 34, 51, 51, 51, 51, 51  },
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(201),	 0,	 4,
	"blast of flame",	"!Fire Breath!"
	 },

	 {
	"frost breath",		{ 31, 51, 51, 51, 51, 51  },
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(202),	 0,	 4,
	"blast of frost",	"!Frost Breath!"
	 },

	 {
	"gas breath",		{ 35, 51, 51, 51, 51, 51  },
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(203),	 0,	 4,
	"blast of gas",		"!Gas Breath!"
	 },

	 {
	"lightning breath",	{ 32, 51, 51, 51, 51, 51  },
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(204),	 0,	 4,
	"blast of lightning",	"!Lightning Breath!"
	 },

/*
 * Fighter and thief skills.
 */
	 {
	"backstab",		{ 51, 51,  1, 51, 51, 51  },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		SLOT( 0),	 0,	24,
	"backstab",		"!Backstab!"
	 },

	 {
	"bash door", 	{ 51, 51, 51, 8, 51, 51 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_bash,		   SLOT( 0),    0,	24,
	"bash",			"!Bash Door!"
	 },

	 {
	"berserk",		{ 51, 51, 51, 18, 51, 51 },
	spell_null,    TAR_IGNORE,    POS_FIGHTING,
	&gsn_berserk,    	SLOT( 0),    0,   24,
	"",            "You feel your pulse slow down."
	 },

	 {
	"disarm",		{ 51, 51, 10, 15, 51, 15  },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		SLOT( 0),	 0,	24,
	"",			"!Disarm!"
	 },

	 {
	"dodge",		{ 51, 51,  1, 10, 51, 12  },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dodge,		SLOT( 0),	 0,	 0,
	"",			"!Dodge!"
	 },

	 {
	"enhanced damage",	{ 51, 51, 51,  1, 51, 1  },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_damage,	SLOT( 0),	 0,	 0,
	"",			"!Enhanced Damage!"
	 },

	 {
	"hide",			{ 51, 51,  1, 51, 51, 51  },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		SLOT( 0),	 0,	12,
	"",			"!Hide!"
	 },

	 {
	 "hunt",                 { 51, 51, 51, 20, 51, 1 },
	 spell_null,             TAR_IGNORE,             POS_RESTING,
	 &gsn_hunt,              SLOT( 0),        0,     12,
	 "",                     "!Hunt!"
	 },

	 {
	"kick",			{ 51, 51, 51,  1, 51, 5  },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		SLOT( 0),	 0,	 8,
	"kick",			"!Kick!"
	 },

	 {
	"parry",		{ 51, 51, 51,  1, 51, 3  },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_parry,		SLOT( 0),	 0,	 0,
	"",			"!Parry!"
	 },

	 {
	"peek",			{ 51, 51,  1, 51, 51, 51  },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		SLOT( 0),	 0,	 0,
	"",			"!Peek!"
	 },

	 {
	"pick lock",		{ 51, 51,  1, 51, 51, 51  },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		SLOT( 0),	 0,	12,
	"",			"!Pick!"
	 },

	 {
	"poison weapon",	{ 51, 51, 13, 51, 51, 51 },
	spell_null,		TAR_OBJ_INV,	POS_STANDING,
	&gsn_poison_weapon,	SLOT( 0),    0,	12,
	"poisonous concoction",	"!Poison Weapon!"
	 },

	 {
	"rescue",		{ 51, 51, 51,  1, 51, 1  },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		SLOT( 0),	 0,	12,
	"",			"!Rescue!"
	 },

	 {
	"second attack",	{ 51, 51,  1,  1, 51, 1  },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_second_attack,	SLOT( 0),	 0,	 0,
	"",			"!Second Attack!"
	 },

	 {
	"sneak",		{ 51, 51,  1, 51, 51, 35  },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		SLOT( 0),	 0,	12,
	"",			NULL
	 },

	 {
	"steal",		{ 51, 51,  1, 51, 51, 51  },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		SLOT( 0),	 0,	24,
	"",			"!Steal!"
	 },

	 {
	"third attack",		{ 51, 51, 51,  1, 51, 10  },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_third_attack,	SLOT( 0),	 0,	 0,
	"",			"!Third Attack!"
	 },

/*
 *  Spells for mega1.are from Glop/Erkenbrand.
*/
	 {
		  "general purpose",      { 51, 51, 51, 51, 51, 51  },
	spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SLOT(205),      0,      12,
	"general purpose ammo", "!General Purpose Ammo!"
	 },

	 {
		  "high explosive",       { 51, 51, 51, 51, 51, 51  },
	spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SLOT(206),      0,      12,
	"high explosive ammo",  "!High Explosive Ammo!"
	 },

	 /*
	  * Psionicist spell/skill definitions
	  * Psi class conceived, created, and coded by Thelonius (EnvyMud)
	  */

	 {
		  "adrenaline control",           { 51, 51, 51, 51,    10, 51 },
		  spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
		  NULL,                           SLOT(500), 6,      12,
		  "",                             "The adrenaline rush wears off."
	 },

	 {
		  "agitation",            { 51, 51, 51, 51,     6, 51 },
		  spell_agitation,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(501), 10,     12,
		  "agitation",            "!Agitation!"
	 },

	 {
		  "aura sight",           { 51, 51, 51, 51,    14, 51 },
		  spell_aura_sight,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
		  NULL,                   SLOT(502), 9,     12,
		  "",                     "!Aura Sight!"
	 },

	 {
		  "awe",                  { 51, 51, 51, 51,    21, 51 },
		  spell_awe,              TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(503), 35,     12,
		  "",                     "!Awe!"
	 },

	 {
		  "ballistic attack",     { 51, 51, 51 , 51,   1, 51 },
		  spell_ballistic_attack, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(504), 5,     12,
		  "ballistic attack",     "!Ballistic Attack!"
	 },

	 {
		  "biofeedback",          { 51, 51, 51, 51,    18, 51 },
		  spell_biofeedback,      TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(505), 75,     12,
		  "",                     "Your biofeedback is no longer effective."
	 },

	 {
		  "cell adjustment",      { 51, 51, 51, 51,    11, 51 },
		  spell_cell_adjustment,  TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(506), 8,     12,
		  "",                     "!Cell Adjustment!"
	 },

	 {
		  "chameleon power",      { 51, 51, 51, 51,     6, 51 },
		  spell_null,             TAR_IGNORE,             POS_STANDING,
		  &gsn_chameleon,         SLOT(507), 0,     12,
		  "",                     "!Chameleon Power!"
	 },

	 {
		  "combat mind",          { 51, 51, 51, 51,   15, 51 },
		  spell_combat_mind,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
		  NULL,                   SLOT(508), 15,     12,
		  "",                     "Your battle sense has faded."
	 },

	 {
		  "complete healing",     { 51, 51, 51, 51,    28, 51 },
		  spell_complete_healing, TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(509), 100,    12,
		  "",                     "!Complete Healing!"
	 },

	 {
		  "control flames",       { 51, 51, 51, 51,     8, 51 },
		  spell_control_flames,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(510), 15,     12,
		  "tongue of flame",      "!Control Flames!"
	 },

	 {
		  "create sound",         { 51, 51, 51, 51,     2, 51 },
		  spell_create_sound,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
		  NULL,                   SLOT(511), 5,     12,
		  "",                     "!Create Sound!"
	 },

	 {
		  "death field",          { 51, 51, 51, 51,    30, 51 },
		  spell_death_field,      TAR_IGNORE,             POS_FIGHTING,
		  NULL,                   SLOT(512), 200,    18,
		  "field of death",       "!Death Field!"
	 },

	 {
		  "detonate",             { 51, 51, 51, 51,    20, 51 },
		  spell_detonate,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(513), 35,     24,
		  "detonation",           "!Detonate!"
	 },

	 {
		  "disintegrate",         { 51, 51, 51, 51,    27, 51 },
		  spell_disintegrate,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(514), 150,    18,
		  "disintegration",       "!Disintegrate!"
	 },

	 {
		  "displacement",         { 51, 51, 51, 51,     9, 51 },
		  spell_displacement,     TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(515), 10,     12,
		  "",                     "You are no longer displaced."
	 },

	 {
		  "domination",           { 51, 51, 51, 51,    16, 51 },
		  spell_domination,       TAR_CHAR_OFFENSIVE,     POS_STANDING,
		  &gsn_domination,        SLOT(516), 5,     12,
		  "",                     "You regain control of your body."
	 },

	 {
		  "ectoplasmic form",     { 51, 51, 51, 51,    19, 51 },
		  spell_ectoplasmic_form, TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(517), 20,     12,
		  "",                     "You feel solid again."
	 },

	 {
		  "ego whip",             { 51, 51, 51, 51,    13, 51 },
		  spell_ego_whip,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(518), 20,     12,
		  "",                     "You feel more confident."
	 },

	 {
		  "energy containment",           { 51, 51, 51, 51,    10, 51 },
		  spell_energy_containment,       TAR_CHAR_SELF,  POS_STANDING,
		  NULL,                           SLOT(519), 10,     12,
		  "",                             "You no longer absorb energy."
	 },

	 {
		  "enhance armor",        { 51, 51, 51, 51,    15, 51 },
		  spell_enhance_armor,    TAR_OBJ_INV,    POS_STANDING,
		  NULL,                   SLOT(520), 100,    24,
		  "",                     "!Enhance Armor!"
	 },

	 {
		  "enhanced strength",            { 51, 51, 51, 51,     7, 51 },
		  spell_enhanced_strength,        TAR_CHAR_SELF,  POS_STANDING,
		  NULL,                           SLOT(521), 20,     12,
		  "",                             "You no longer feel so HUGE."
	 },

	 {
		  "flesh armor",          { 51, 51, 51, 51,    11, 51 },
		  spell_flesh_armor,      TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(522), 15,     12,
		  "",                     "Your skin returns to normal."
	 },

	 {
		  "heighten senses",      { 51, 51, 51, 51,     3, 51 },
		  spell_null,             TAR_CHAR_SELF,          POS_STANDING,
		  &gsn_heighten,          SLOT(523), 0,      0,
		  "",                     "Your senses return to normal."
	 },

	 {
		  "inertial barrier",     { 51, 51, 51, 51,    19, 51 },
		  spell_inertial_barrier, TAR_IGNORE,             POS_STANDING,
		  NULL,                   SLOT(524), 40,     24,
		  "",                     "Your inertial barrier dissipates."
	 },

	 {
		  "inflict pain",         { 51, 51, 51, 51,     5, 51 },
		  spell_inflict_pain,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(525), 10,     12,
		  "mindpower",            "!Inflict Pain!"
	 },

	 {
		  "intellect fortress",           { 51, 51, 51, 51,    12, 51 },
		  spell_intellect_fortress,       TAR_IGNORE,     POS_STANDING,
		  NULL,                           SLOT(526), 25,     24,
		  "",                     "Your intellectual fortress crumbles."
	 },

	 {
		  "lend health",          { 51, 51, 51, 51,    10, 51 },
		  spell_lend_health,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
		  NULL,                   SLOT(527), 10,     12,
		  "",                     "!Lend Health!"
	 },

	 {
		  "levitation",           { 51, 51, 51, 51,     3, 51 },
		  spell_levitation,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
		  NULL,                   SLOT(528), 10,     18,
		  "",                     "You slowly float to the ground."
	 },

	 {
		  "mental barrier",       { 51, 51, 51, 51,     6, 51 },
		  spell_mental_barrier,   TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(529), 8,     12,
		  "",                     "Your mental barrier breaks down."
	 },

	 {
		  "mind thrust",          { 51, 51, 51, 51,     2, 51 },
		  spell_mind_thrust,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(530), 8,     12,
		  "mind thrust",          "!Mind Thrust!"
	},

	 {
		  "project force",        { 51, 51, 51, 51,     9, 51 },
		  spell_project_force,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(531), 18,     12,
		  "projected force",      "!Project Force!"
	 },

	 {
		  "psionic blast",        { 51, 51, 51, 51,    17, 51 },
		  spell_psionic_blast,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(532), 25,     12,
		  "psionic blast",        "!Psionic Blast!"
	 },

	 {
		  "psychic crush",        { 51, 51, 51, 51,     8, 51 },
		  spell_psychic_crush,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(533), 15,     18,
		  "psychic crush",        "!Psychic Crush!"
	 },

	 {
		  "psychic drain",        { 51, 51, 51, 51,     4, 51 },
		  spell_psychic_drain,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
		  NULL,                   SLOT(534), 20,     12,
		  "",                     "You no longer feel drained."
	 },

	 {
		  "psychic healing",      { 51, 51, 51, 51,     5, 51 },
		  spell_psychic_healing,  TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(535), 20,      12,
		  "",                     "!Psychic Healing!"
	 },

	 {
		  "shadow form",          { 51, 51, 51, 51,     2, 51 },
		  spell_null,             TAR_IGNORE,             POS_STANDING,
		  &gsn_shadow,            SLOT(536), 0,     12,
		  "",                     "You no longer move in the shadows."
	 },

	 {
		  "share strength",       { 51, 51, 51, 51,    11, 51 },
		  spell_share_strength,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
		  NULL,                   SLOT(551), 8,     12,
		  "",                     "You no longer share strength with another."
	 },

	 {
		  "thought shield",       { 51, 51, 51, 51,     1, 51 },
		  spell_thought_shield,   TAR_CHAR_SELF,          POS_STANDING,
		  NULL,                   SLOT(538), 5,     12,
		  "",                     "You no longer feel so protected."
	 },

	 {
		  "ultrablast",           { 51, 51, 51, 51,    25, 51 },
		  spell_ultrablast,       TAR_IGNORE,             POS_FIGHTING,
		  NULL,                   SLOT(539), 75,     24,
		  "ultrablast",           "!Ultrablast!"
	 }


	  ,

/*
 *  These spells are to allow TFC zones to load for testing.  They all call
 *  spell_dispel_magic, which says 'unimplemented' or something...
*/
	 {
	"cause disease",		{ 54, 54, 54, 54, 54, 54 },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	NULL,			SLOT(105),	15,	12,
	"",			"!Cause Disease!"
	 },

	 {
	"cure disease",		{ 54, 54, 54, 54, 54, 54 },
	spell_dispel_magic,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(104),	15,	12,
	"",			"!Cause Disease!"
	 },

	 {
	"detect good",		{ 54, 54, 54, 54, 54, 54 },
	spell_dispel_magic,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,			SLOT(37),	15,	12,
	"",			"The blue in your vision disappears"
	 },

	 {
	"dispel good",		{ 54, 54, 54, 54, 54, 54 },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(51),	15,	12,
	"dispel good",		"!Dispel Good!"
	 },

	 {
	"jump",		{ 54, 54, 54, 54, 54, 54 },
	spell_dispel_magic,	TAR_CHAR_SELF,	POS_FIGHTING,
	NULL,			SLOT(47),	15,	12,
	"",		"!Jump!"
	 },

	 {
	"protection good",		{ 54, 54, 54, 54, 54, 54 },
	spell_dispel_magic,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,			SLOT(45),	15,	12,
	"",			"!Protection Good!"
	 },

	 {
	"tongues",		{ 54, 54, 54, 54, 54, 54 },
	spell_dispel_magic,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,			SLOT(98),	15,	12,
	"",			"!Tongues!"
	 },

	 {
	"true seeing",		{ 54, 54, 54, 54, 54, 54 },
	spell_dispel_magic,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,			SLOT(103),	15,	12,
	"",			"You see less true <cackle>"
	 },

	 {
	"animate dead",		{ 51, 51, 51, 51, 51, 51  },
	spell_animate_dead,	TAR_IGNORE,	POS_STANDING,
	NULL,	SLOT(97),	 5,	12,
	"",			"!Animate Dead!"
	 },


};
