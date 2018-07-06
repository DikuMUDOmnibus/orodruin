/****************************************************************************
 *   ___  ___  ___  __    __                                                *
 *  |   ||   ||   ||  |\/|  | 2-x    NiMUD is a software currently under    *
 *   |  \ | |  | |  | |\/| |         development.  It is based primarily on *
 *   | |\\| |  | |  | |  | |         the discontinued package, Merc 2.2.    *
 *   | | \  |  | |  | |  | |         NiMUD is being written and developed   *
 *  |___||___||___||__|  |__|        By Locke and Surreality as a new,      *
 *   NAMELESS INCARNATE *MUD*        frequently updated package similar to  *
 *        S O F T W A R E            the original Merc 2.x.                 *
 *                                                                          *
 *  Just look for the Iron Maiden skull wherever NiMUD products are found.  *
 *                                                                          *
 *  Much time and thought has gone into this software and you are           *
 *  benefitting.  We hope that you share your changes too.  What goes       *
 *  around, comes around.                                                   *
 ****************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/*
 * Return ascii name of an item type.
 */
char *item_type_name( int item_type )
{
    switch ( item_type )
    {
    case ITEM_LIGHT:         return "light";
    case ITEM_SCROLL:        return "scroll";
    case ITEM_WAND:          return "wand";
    case ITEM_STAFF:         return "staff";
    case ITEM_WEAPON:        return "weapon";
    case ITEM_RANGED_WEAPON: return "ranged";
    case ITEM_AMMO:          return "ammo";
    case ITEM_TREASURE:      return "treasure";
    case ITEM_ARMOR:         return "armor";
    case ITEM_POTION:        return "potion";
    case ITEM_BOOK:          return "book";
    case ITEM_FURNITURE:     return "furniture";
    case ITEM_TRASH:         return "trash";
    case ITEM_CONTAINER:     return "container";
    case ITEM_THROWN:        return "thrown";
    case ITEM_DRINK_CON:     return "drink container";
    case ITEM_KEY:           return "key";
    case ITEM_FOOD:          return "food";
    case ITEM_MONEY:         return "money";
    case ITEM_GEM:           return "gem";
    case ITEM_VEHICLE:       return "vehicle";
    case ITEM_CORPSE_NPC:    return "npc corpse";
    case ITEM_CORPSE_PC:     return "pc corpse";
    case ITEM_FOUNTAIN:      return "fountain";
    case ITEM_PILL:          return "pill";
    case ITEM_TOOL:          return "tool";
    case ITEM_LIST:          return "list";
    case ITEM_BOARD:         return "board";
    }

    bug( "Item_type_name: unknown type %d.", item_type );
    return "(unknown)";
}

int item_name_type( char *name )
{
    if ( !str_cmp( name, "light"     ) ) return ITEM_LIGHT;
    if ( !str_cmp( name, "scroll"    ) ) return ITEM_SCROLL;
    if ( !str_cmp( name, "wand"      ) ) return ITEM_WAND;
    if ( !str_cmp( name, "staff"     ) ) return ITEM_STAFF;
    if ( !str_cmp( name, "weapon"    ) ) return ITEM_WEAPON;
    if ( !str_cmp( name, "ranged"    ) ) return ITEM_RANGED_WEAPON;
    if ( !str_cmp( name, "ammo"      ) ) return ITEM_AMMO;
    if ( !str_cmp( name, "treasure"  ) ) return ITEM_TREASURE;
    if ( !str_cmp( name, "armor"     ) ) return ITEM_ARMOR;
    if ( !str_cmp( name, "potion"    ) ) return ITEM_POTION;
    if ( !str_cmp( name, "book"      ) ) return ITEM_BOOK;
    if ( !str_cmp( name, "furniture" ) ) return ITEM_FURNITURE;
    if ( !str_cmp( name, "trash"     ) ) return ITEM_TRASH;
    if ( !str_cmp( name, "container" ) ) return ITEM_CONTAINER;
    if ( !str_cmp( name, "thrown"    ) ) return ITEM_THROWN;
    if ( !str_cmp( name, "drink"     ) ) return ITEM_DRINK_CON;
    if ( !str_cmp( name, "key"       ) ) return ITEM_KEY;
    if ( !str_cmp( name, "food"      ) ) return ITEM_FOOD;
    if ( !str_cmp( name, "money"     ) ) return ITEM_MONEY;
    if ( !str_cmp( name, "gem"       ) ) return ITEM_GEM;
    if ( !str_cmp( name, "vehicle"   ) ) return ITEM_VEHICLE;
    if ( !str_cmp( name, "corpse"    ) ) return ITEM_CORPSE_NPC;
    if ( !str_cmp( name, "fountain"  ) ) return ITEM_FOUNTAIN;
    if ( !str_cmp( name, "pill"      ) ) return ITEM_PILL;
    if ( !str_cmp( name, "tool"      ) ) return ITEM_TOOL;
    if ( !str_cmp( name, "list"      ) ) return ITEM_LIST;
    if ( !str_cmp( name, "board"     ) ) return ITEM_BOARD;
    return 0;
}


/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name( int location )
{
    switch ( location )
    {
    case APPLY_NONE:            return "none";
    case APPLY_STR:             return "strength";
    case APPLY_DEX:             return "dexterity";
    case APPLY_INT:             return "intelligence";
    case APPLY_WIS:             return "wisdom";
    case APPLY_CON:             return "constitution";
    case APPLY_SEX:             return "sex";
    case APPLY_CLASS:           return "class";
    case APPLY_LEVEL:           return "level";
    case APPLY_AGE:             return "age";
    case APPLY_HIT:             return "hp";
    case APPLY_MOVE:            return "moves";
    case APPLY_GOLD:            return "gold";
    case APPLY_EXP:             return "experience";
    case APPLY_AC:              return "armor class";
    case APPLY_HITROLL:         return "hit roll";
    case APPLY_DAMROLL:         return "damage roll";
    case APPLY_SAVING_THROW:    return "saving throw";
    }

    bug( "Affect_location_name: unknown location %d.", location );
    return "(unknown)";

}

int affect_name_loc( char* name )
{
    if ( !str_cmp( name, "none"          ) ) return APPLY_NONE;
    if ( !str_cmp( name, "strength"      ) ) return APPLY_STR;
    if ( !str_cmp( name, "dexterity"     ) ) return APPLY_DEX;
    if ( !str_cmp( name, "intelligence"  ) ) return APPLY_INT;
    if ( !str_cmp( name, "wisdom"        ) ) return APPLY_WIS;
    if ( !str_cmp( name, "constitution"  ) ) return APPLY_CON;
    if ( !str_cmp( name, "sex"           ) ) return APPLY_SEX;
    if ( !str_cmp( name, "class"         ) ) return APPLY_CLASS;
    if ( !str_cmp( name, "level"         ) ) return APPLY_LEVEL;
    if ( !str_cmp( name, "age"           ) ) return APPLY_AGE;
    if ( !str_cmp( name, "hp"            ) ) return APPLY_HIT;
    if ( !str_cmp( name, "move"          ) ) return APPLY_MOVE;
    if ( !str_cmp( name, "gold"          ) ) return APPLY_GOLD;
    if ( !str_cmp( name, "experience"    ) ) return APPLY_EXP;
    if ( !str_cmp( name, "ac"            ) ) return APPLY_AC;
    if ( !str_cmp( name, "hit"           ) ) return APPLY_HITROLL;
    if ( !str_cmp( name, "dam"           ) ) return APPLY_DAMROLL;
    if ( !str_cmp( name, "saving_throw"  ) ) return APPLY_SAVING_THROW;
    return APPLY_NONE;
}



/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name( int vector )
{
    static char buf[512];

    buf[0] = '\0';
    if ( vector & AFF_BLIND         ) strcat( buf, " blind"         );
    if ( vector & AFF_INVISIBLE     ) strcat( buf, " invisible"     );
    if ( vector & AFF_DETECT_EVIL   ) strcat( buf, " detect-evil"   );
    if ( vector & AFF_DETECT_INVIS  ) strcat( buf, " detect-invis"  );
    if ( vector & AFF_DETECT_MAGIC  ) strcat( buf, " detect-magic"  );
    if ( vector & AFF_DETECT_HIDDEN ) strcat( buf, " detect-hidden" );
    if ( vector & AFF_HOLD          ) strcat( buf, " hold"          );
    if ( vector & AFF_SANCTUARY     ) strcat( buf, " sanctuary"     );
    if ( vector & AFF_FAERIE_FIRE   ) strcat( buf, " faerie-fire"   );
    if ( vector & AFF_INFRARED      ) strcat( buf, " infrared"      );
    if ( vector & AFF_CURSE         ) strcat( buf, " curse"         );
    if ( vector & AFF_FLAMING       ) strcat( buf, " flaming"       );
    if ( vector & AFF_POISON        ) strcat( buf, " poison"        );
    if ( vector & AFF_PROTECT       ) strcat( buf, " protect"       );
    if ( vector & AFF_PARALYSIS     ) strcat( buf, " paralysis"     );
    if ( vector & AFF_SLEEP         ) strcat( buf, " sleep"         );
    if ( vector & AFF_SNEAK         ) strcat( buf, " sneak"         );
    if ( vector & AFF_HIDE          ) strcat( buf, " hide"          );
    if ( vector & AFF_CHARM         ) strcat( buf, " charm"         );
    if ( vector & AFF_FLYING        ) strcat( buf, " flying"        );
    if ( vector & AFF_PASS_DOOR     ) strcat( buf, " pass-door"     );
    if ( vector & AFF_FREEACTION    ) strcat( buf, " free-action"   );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}



/*
 * Return bit vector
 */
int affect_name_bit( char* buf )
{
    if (!str_cmp( buf, "blind"         )) return AFF_BLIND;
    if (!str_cmp( buf, "invisible"     )) return AFF_INVISIBLE;
    if (!str_cmp( buf, "detect-evil"   )) return AFF_DETECT_EVIL;
    if (!str_cmp( buf, "detect-invis"  )) return AFF_DETECT_INVIS;
    if (!str_cmp( buf, "detect-magic"  )) return AFF_DETECT_MAGIC;
    if (!str_cmp( buf, "detect-hidden" )) return AFF_DETECT_HIDDEN;
    if (!str_cmp( buf, "hold"          )) return AFF_HOLD;
    if (!str_cmp( buf, "sanctuary"     )) return AFF_SANCTUARY;
    if (!str_cmp( buf, "faerie-fire"   )) return AFF_FAERIE_FIRE;
    if (!str_cmp( buf, "infrared"      )) return AFF_INFRARED;
    if (!str_cmp( buf, "curse"         )) return AFF_CURSE;
    if (!str_cmp( buf, "flaming"       )) return AFF_FLAMING;
    if (!str_cmp( buf, "poisoned"      )) return AFF_POISON;
    if (!str_cmp( buf, "protect"       )) return AFF_PROTECT;
    if (!str_cmp( buf, "paralysis"     )) return AFF_PARALYSIS;
    if (!str_cmp( buf, "sleep"         )) return AFF_SLEEP;
    if (!str_cmp( buf, "sneak"         )) return AFF_SNEAK;
    if (!str_cmp( buf, "hide"          )) return AFF_HIDE;
    if (!str_cmp( buf, "charm"         )) return AFF_CHARM;
    if (!str_cmp( buf, "flying"        )) return AFF_FLYING;
    if (!str_cmp( buf, "pass-door"     )) return AFF_PASS_DOOR;
    if (!str_cmp( buf, "free-action"   )) return AFF_FREEACTION;
    return 0;
}



/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name( int extra_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
    if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
    if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
    if ( extra_flags & ITEM_LOCK         ) strcat( buf, " lock"         );
    if ( extra_flags & ITEM_EVIL         ) strcat( buf, " evil"         );
    if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
    if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
    if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
    if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
    if ( extra_flags & ITEM_ANTI_GOOD    ) strcat( buf, " anti-good"    );
    if ( extra_flags & ITEM_ANTI_EVIL    ) strcat( buf, " anti-evil"    );
    if ( extra_flags & ITEM_ANTI_NEUTRAL ) strcat( buf, " anti-neutral" );
    if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
    if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
    if ( extra_flags & ITEM_NOSAVE       ) strcat( buf, " nosave"       );
    if ( extra_flags & ITEM_BURNING      ) strcat( buf, " burning"      );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}


int extra_name_bit( char* buf )
{
    if (!str_cmp( buf, "glow"        ) ) return ITEM_GLOW        ;
    if (!str_cmp( buf, "hum"         ) ) return ITEM_HUM         ;
    if (!str_cmp( buf, "dark"        ) ) return ITEM_DARK        ;
    if (!str_cmp( buf, "lock"        ) ) return ITEM_LOCK        ;
    if (!str_cmp( buf, "evil"        ) ) return ITEM_EVIL        ;
    if (!str_cmp( buf, "invis"       ) ) return ITEM_INVIS       ;
    if (!str_cmp( buf, "magic"       ) ) return ITEM_MAGIC       ;
    if (!str_cmp( buf, "nodrop"      ) ) return ITEM_NODROP      ;
    if (!str_cmp( buf, "bless"       ) ) return ITEM_BLESS       ;
    if (!str_cmp( buf, "anti-good"   ) ) return ITEM_ANTI_GOOD   ;
    if (!str_cmp( buf, "anti-evil"   ) ) return ITEM_ANTI_EVIL   ;
    if (!str_cmp( buf, "anti-neutral") ) return ITEM_ANTI_NEUTRAL;
    if (!str_cmp( buf, "noremove"    ) ) return ITEM_NOREMOVE    ;
    if (!str_cmp( buf, "inventory"   ) ) return ITEM_INVENTORY   ;
    if (!str_cmp( buf, "nosave"      ) ) return ITEM_NOSAVE      ;
    if (!str_cmp( buf, "burning"     ) ) return ITEM_BURNING     ;
    return 0;
}



/*
 * Return ascii name of room flags vector.
 */
char *room_bit_name( int room_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( room_flags & ROOM_DARK         ) strcat( buf, " dark"         );
    if ( room_flags & ROOM_NO_MOB       ) strcat( buf, " no_mobs"      );
    if ( room_flags & ROOM_INDOORS      ) strcat( buf, " indoors"      );
    if ( room_flags & ROOM_PRIVATE      ) strcat( buf, " private"      );
    if ( room_flags & ROOM_SAFE         ) strcat( buf, " safe"         );
    if ( room_flags & ROOM_SOLITARY     ) strcat( buf, " solitary"     );
    if ( room_flags & ROOM_PET_SHOP     ) strcat( buf, " pet_shop"     );
    if ( room_flags & ROOM_NO_RECALL    ) strcat( buf, " no_recall"    );
    if ( room_flags & ROOM_BANK         ) strcat( buf, " bank"         );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

int room_name_bit( char* buf )
{
    if ( !str_cmp( buf, "dark"         ) ) return ROOM_DARK;
    if ( !str_cmp( buf, "no_mobs"      ) ) return ROOM_NO_MOB;
    if ( !str_cmp( buf, "indoors"      ) ) return ROOM_INDOORS;
    if ( !str_cmp( buf, "private"      ) ) return ROOM_PRIVATE;
    if ( !str_cmp( buf, "safe"         ) ) return ROOM_SAFE;
    if ( !str_cmp( buf, "solitary"     ) ) return ROOM_SOLITARY;
    if ( !str_cmp( buf, "pet_shop"     ) ) return ROOM_PET_SHOP;
    if ( !str_cmp( buf, "no_recall"    ) ) return ROOM_NO_RECALL;
    if ( !str_cmp( buf, "bank"         ) ) return ROOM_BANK;
    return 0;
}


char *act_bit_name( int act )
{
    static char buf[512];

    buf[0] = '\0';
    if ( act & ACT_IS_NPC     ) strcat( buf, " npc"           );
    if ( act & ACT_SENTINEL   ) strcat( buf, " sentinel"      );
    if ( act & ACT_SCAVENGER  ) strcat( buf, " scavenger"     );
    if ( act & ACT_AGGRESSIVE ) strcat( buf, " aggressive"    );
    if ( act & ACT_STAY_AREA  ) strcat( buf, " stay_area"     );
    if ( act & ACT_WIMPY      ) strcat( buf, " wimpy"         );
    if ( act & ACT_PET        ) strcat( buf, " pet"           );
    if ( act & ACT_TRAIN      ) strcat( buf, " trainer"       );
    if ( act & ACT_PRACTICE   ) strcat( buf, " practitioner"  );
    if ( act & ACT_FIGHTSCR   ) strcat( buf, " fight_scr"     );
    if ( act & ACT_TRACKSCR   ) strcat( buf, " track_scr"     );
    if ( act & ACT_HALT       ) strcat( buf, " halted"        );
    if ( act & ACT_SCRAMBLE   ) strcat( buf, " scramble"      );
    if ( act & ACT_MOUNTABLE  ) strcat( buf, " mountable"     );
    if ( act & ACT_TRACKING   ) strcat( buf, " tracking"      );
    if ( act & ACT_ANGRY      ) strcat( buf, " angry"         );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *plr_bit_name( int act )
{
    static char buf[512];

    buf[0] = '\0';
    if ( act & PLR_IS_NPC     ) strcat( buf, " npc"          );
    if ( act & PLR_BOUGHT_PET ) strcat( buf, " bought_pet"   );
    if ( act & PLR_HOLYLIGHT  ) strcat( buf, " holylight"    );
    if ( act & PLR_WIZBIT     ) strcat( buf, " wizbit"       );
    if ( act & PLR_LOG        ) strcat( buf, " log"          );
    if ( act & PLR_DENY       ) strcat( buf, " deny"         );
    if ( act & PLR_FREEZE     ) strcat( buf, " freeze"       );
    if ( act & WIZ_NOTIFY     ) strcat( buf, " notify"        );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

int act_name_bit( char* buf )
{
    if ( !str_cmp( buf, "npc"          ) ) return ACT_IS_NPC;
    if ( !str_cmp( buf, "sentinel"     ) ) return ACT_SENTINEL;
    if ( !str_cmp( buf, "scavenger"    ) ) return ACT_SCAVENGER;
    if ( !str_cmp( buf, "aggressive"   ) ) return ACT_AGGRESSIVE;
    if ( !str_cmp( buf, "stay_area"    ) ) return ACT_STAY_AREA;
    if ( !str_cmp( buf, "wimpy"        ) ) return ACT_WIMPY;
    if ( !str_cmp( buf, "pet"          ) ) return ACT_PET;
    if ( !str_cmp( buf, "trainer"      ) ) return ACT_TRAIN;
    if ( !str_cmp( buf, "practitioner" ) ) return ACT_PRACTICE;
    if ( !str_cmp( buf, "fight_scr"    ) ) return ACT_FIGHTSCR;
    if ( !str_cmp( buf, "track_scr"    ) ) return ACT_TRACKSCR;
    if ( !str_cmp( buf, "halted"       ) ) return ACT_HALT;
    if ( !str_cmp( buf, "scramble"     ) ) return ACT_SCRAMBLE;
    if ( !str_cmp( buf, "mountable"    ) ) return ACT_MOUNTABLE;
    if ( !str_cmp( buf, "tracking"     ) ) return ACT_TRACKING;
    if ( !str_cmp( buf, "angry"        ) ) return ACT_ANGRY;
    return 0;
}

/*
 * Returns the name of a wear bit.
 */
char *wear_bit_name( int wear )
{
    static char buf[512];

    buf[0] = '\0';
    if ( wear & ITEM_TAKE        ) strcat( buf, " take"       );
    if ( wear & ITEM_WEAR_FINGER ) strcat( buf, " finger"     );
    if ( wear & ITEM_WEAR_NECK   ) strcat( buf, " neck"       );
    if ( wear & ITEM_WEAR_BODY   ) strcat( buf, " body"       );
    if ( wear & ITEM_WEAR_HEAD   ) strcat( buf, " head"       );
    if ( wear & ITEM_WEAR_LEGS   ) strcat( buf, " legs"       );
    if ( wear & ITEM_WEAR_FEET   ) strcat( buf, " feet"       );
    if ( wear & ITEM_WEAR_HANDS  ) strcat( buf, " hands"      );
    if ( wear & ITEM_WEAR_ARMS   ) strcat( buf, " arms"       );
    if ( wear & ITEM_WEAR_SHIELD ) strcat( buf, " shield"     );
    if ( wear & ITEM_WEAR_ABOUT  ) strcat( buf, " about"      );
    if ( wear & ITEM_WEAR_WAIST  ) strcat( buf, " waist"      );
    if ( wear & ITEM_WEAR_WRIST  ) strcat( buf, " wrist"      );
    if ( wear & ITEM_WEAR_BELT   ) strcat( buf, " belt"       );
    if ( wear & ITEM_WIELD       ) strcat( buf, " wield"      );
    if ( wear & ITEM_HOLD        ) strcat( buf, " hold"       );
    if ( wear & ITEM_WEAR_SHOULDER )strcat(buf, " shoulder"   );
    if ( wear & ITEM_TWO_HANDED )   strcat(buf, " 2-handed"   );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}


/*
 * Returns the bit, given a certain name.
 */
int wear_name_bit( char* buf )
{
    if (!str_cmp( buf, "take"       ) ) return ITEM_TAKE;
    if (!str_cmp( buf, "finger"     ) ) return ITEM_WEAR_FINGER;
    if (!str_cmp( buf, "neck"       ) ) return ITEM_WEAR_NECK;
    if (!str_cmp( buf, "body"       ) ) return ITEM_WEAR_BODY;
    if (!str_cmp( buf, "head"       ) ) return ITEM_WEAR_HEAD;
    if (!str_cmp( buf, "legs"       ) ) return ITEM_WEAR_LEGS;
    if (!str_cmp( buf, "feet"       ) ) return ITEM_WEAR_FEET;
    if (!str_cmp( buf, "hands"      ) ) return ITEM_WEAR_HANDS;
    if (!str_cmp( buf, "arms"       ) ) return ITEM_WEAR_ARMS;
    if (!str_cmp( buf, "shield"     ) ) return ITEM_WEAR_SHIELD;
    if (!str_cmp( buf, "about"      ) ) return ITEM_WEAR_ABOUT;
    if (!str_cmp( buf, "waist"      ) ) return ITEM_WEAR_WAIST;
    if (!str_cmp( buf, "wrist"      ) ) return ITEM_WEAR_WRIST;
    if (!str_cmp( buf, "belt"       ) ) return ITEM_WEAR_BELT;
    if (!str_cmp( buf, "wield"      ) ) return ITEM_WIELD;
    if (!str_cmp( buf, "hold"       ) ) return ITEM_HOLD;
    if (!str_cmp( buf, "shoulder"   ) ) return ITEM_WEAR_SHOULDER;
    if (!str_cmp( buf, "2-handed"   ) ) return ITEM_TWO_HANDED;
    return 0;
}


/*
 * Return ascii name of wear location.
 */
char *wear_loc_name( int wearloc )
{
    switch( wearloc )
    {
               default: return "unknown";
        case WEAR_NONE: return "none";
    case WEAR_FINGER_L: return "left finger";
    case WEAR_FINGER_R: return "right finger";
      case WEAR_NECK_1: return "neck1";
      case WEAR_NECK_2: return "neck2";
        case WEAR_BODY: return "body";
        case WEAR_HEAD: return "head";
        case WEAR_LEGS: return "legs";
        case WEAR_FEET: return "feet";
       case WEAR_HANDS: return "hands";
        case WEAR_ARMS: return "arms";
      case WEAR_SHIELD: return "shield";
       case WEAR_ABOUT: return "about";
       case WEAR_WAIST: return "waist";
     case WEAR_WRIST_L: return "lwrist";
     case WEAR_WRIST_R: return "rwrist";
    case WEAR_FLOATING: return "floating";
      case WEAR_BELT_1: return "belt1";
      case WEAR_BELT_2: return "belt2";
      case WEAR_BELT_3: return "belt3";
      case WEAR_BELT_4: return "belt4";
      case WEAR_BELT_5: return "belt5";
      case WEAR_HOLD_1: return "hold1";
      case WEAR_HOLD_2: return "hold2";
  case WEAR_SHOULDER_L: return "lshoulder";
  case WEAR_SHOULDER_R: return "rshoulder";
         case MAX_WEAR: return "max_wear";
    }
}


int wear_name_loc( char *buf )
{
    if ( !str_cmp( buf, "lfinger" ) )   return WEAR_FINGER_L;
    if ( !str_cmp( buf, "rfinger" ) )   return WEAR_FINGER_R;
    if ( !str_cmp( buf, "neck1" ) )     return WEAR_NECK_1;
    if ( !str_cmp( buf, "neck2" ) )     return WEAR_NECK_2;
    if ( !str_cmp( buf, "body" ) )      return WEAR_BODY;
    if ( !str_cmp( buf, "head" ) )      return WEAR_HEAD;
    if ( !str_cmp( buf, "legs" ) )      return WEAR_LEGS;
    if ( !str_cmp( buf, "feet" ) )      return WEAR_FEET;
    if ( !str_cmp( buf, "hands" ) )     return WEAR_HANDS;
    if ( !str_cmp( buf, "arms" ) )      return WEAR_ARMS;
    if ( !str_cmp( buf, "shield" ) )    return WEAR_SHIELD;
    if ( !str_cmp( buf, "about" ) )     return WEAR_ABOUT;
    if ( !str_cmp( buf, "waist" ) )     return WEAR_WAIST;
    if ( !str_cmp( buf, "lwrist" ) )    return WEAR_WRIST_L;
    if ( !str_cmp( buf, "rwrist" ) )    return WEAR_WRIST_R;
    if ( !str_cmp( buf, "floating" ) )  return WEAR_FLOATING;
    if ( !str_cmp( buf, "belt1" ) )     return WEAR_BELT_1;
    if ( !str_cmp( buf, "belt2" ) )     return WEAR_BELT_2;
    if ( !str_cmp( buf, "belt3" ) )     return WEAR_BELT_3;
    if ( !str_cmp( buf, "belt4" ) )     return WEAR_BELT_4;
    if ( !str_cmp( buf, "belt5" ) )     return WEAR_BELT_5;
    if ( !str_cmp( buf, "hold1" ) )     return WEAR_HOLD_1;
    if ( !str_cmp( buf, "hold2" ) )     return WEAR_HOLD_2;
    if ( !str_cmp( buf, "lshoulder" ) ) return WEAR_SHOULDER_L;
    if ( !str_cmp( buf, "rshoulder" ) ) return WEAR_SHOULDER_R;
    if ( !str_cmp( buf, "max_wear" ) )  return MAX_WEAR;
    return WEAR_NONE;
}

char *sector_name( int sect )
{
    if ( sect == SECT_INSIDE )       return "inside";
    if ( sect == SECT_CITY )         return "city";
    if ( sect == SECT_FIELD )        return "field";
    if ( sect == SECT_FOREST )       return "forest";
    if ( sect == SECT_HILLS )        return "hills";
    if ( sect == SECT_MOUNTAIN )     return "mountain";
    if ( sect == SECT_WATER_SWIM )   return "swim";
    if ( sect == SECT_WATER_NOSWIM ) return "noswim";
    if ( sect == SECT_UNDERWATER )   return "underwater";
    if ( sect == SECT_AIR )          return "air";
    if ( sect == SECT_DESERT )       return "desert";
    return "unknown";
}
