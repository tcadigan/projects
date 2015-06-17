/*
 * globals.h: Rog-O-Matic XIV (CMU) Thu Jan 31 18:12:50 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * Global variables
 */
#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "types.h"

/* global files */
extern FILE *frogue; /* From Rogue */
extern FILE *trogue; /* To Rogue */
extern FILE *fecho; /* Rogomatic score file */
extern FILE *logfile; /* Rogomatic score file */
extern FILE *realstdout; /* Real stdout when in terse or emacs mode */
extern FILE *snapshot; /* file for snapshot command */
FILE *wopen(); /* Open a file for world access */

/* global characters and strings */
extern char afterid; /* Index of object after identify */
extern char *genocide; /* List of monsters to genocide */
extern char genocided[]; /* List of monsters genocided */
extern char lastcmd[]; /* Copy of last ocmmand sent to Rogue */
extern char lastname[]; /* Name of last postion/scroll/wand */
extern char nextid; /* Next object to identify */
extern char ourkiller[]; /* What was listed on the tombstone */
extern char *parmstr; /* Pointer to argument space */
extern char queue[]; /* Stuff to be sent to Rogue */
extern char *roguename; /* Name we are playing under */
extern char screen[24][80]; /* Characters drawn by Rogue */
extern char sumline[]; /* Summation line */
extern char *termination; /* Latin verb for how we died */
extern char versionstr[]; /* Version of Rogue we are playing */

/* character and string functions */
extern char getlogtoken();
extern char *getname();
extern char *itemstr();
extern char logchar();
extern char *monnam();
extern char *realname();

/* double precision floating point functions for stats.c */
double prob();
double mean();
double stdev();

/* global integers */
extern int aggravated; /* True if we aggravated this level */
extern int agoalr; /* Where we killed a monster */
extern int agoalc; /* Where we killed a monster */
extern int ammo; /* Number of missiles in pack */
extern int arglen; /* Length of argument space */
extern int arrowshot; /* True if trap fired at us this round */
extern int atrow; /* Where is the '@'? (us) */
extern int atcol; /* Where is the '@'? (us( */
extern int atrow0; /* Where was the '@' last time */
extern int atrow1; /* Where was the '@' last time */
extern int attempt; /* Number of times we have explored this level */
extern int badarrow; /* True if we missed with this arrow */
extern int beingheld; /* True if being held by a fungus */
extern int beingstalked; /* True if an Invisible Stalker is around */
extern int blinded; /* True if blinded */
extern int blinddir; /* Last direction we moved when blind */
extern int cancelled; /* Turns till use cancellation again */
extern int cecho; /* Last message type to logfile */
extern int cheat; /* True ==> cheat to win */
extern int checkrange; /* True ==> check range */
extern int chicken; /* True ==> test run away code */
extern int compression; /* True ==> move multiple squares */
extern int confused; /* True if confused */
extern int cosmic; /* True if hallucinating */
extern int currentarmor; /* Index of our armor */
extern int currentweapon; /* Index of our weapon */
extern int cursedarmor; /* True if armor is cursed */
extern int cursedweapon; /* True if weapon is cursed */
extern int darkdir; /* Direction of arror in dark room */
extern int darkturns; /* Number of arrows left to fire in dark room */
extern int debugging; /* Debugging options in effect */
extern int didreadmap; /* Last magically mapped lavel */
extern int doorlist[]; /* Holds r,c of new doors found */
extern int *newdoors;
extern int doublehasted; /* True if double hasted (3.6 only) */
extern int droppedscare; /* Number of scare monster on this level */
extern int emacs; /* True if in emacs mode */
extern int exploredlevel; /* We completely explored this level */
extern int floating; /* True if we are levitating */
extern int foughtmonster; /* True if we recently faught a monster */
extern int foundarrowtrap; /* Well, did we? */
extern int foundtrapdoor; /* Well, did we? */
extern int goalr; /* Where are we trying to go */
extern int goalc; /* Where are we trying to go */
extern int goodarrow; /* Number of times we killed in one blow */
extern int goodweapon; /* Used for two-handed sword */
extern int gplusdam; /* Global damage bonus */
extern int gplushit; /* Global hit bonus */
extern int hasted; /* True if hasted */
extern int head; /* Endpoints of circular queue */
extern int tail; /* Endpoints of circular queue */
extern int hitstokill; /* Number of hits to kill last monster */
extern int interrupted; /* True if at commandtop from onintr() */
extern int knowident; /* Found an identify scroll? */
extern int larder; /* Number of foods left */
extern int lastate; /* Time we last ate */
extern int lastdamage; /* Amount of last hit by a monster */
extern int lastdrop; /* What did we last try to drop */
extern int lastfoodlevel; /* Last food found */
extern int lastmonster; /* Last monster we tried to hit */
extern int lastobj; /* What did we last try to use */
extern int lastwand; /* Index of last wand */
extern int leftring; /* Index of our left ring */
extern int logdigested; /* True if gamelog has been read by replay */
extern int logging; /* True if logging game */
extern int lyinginwait; /* Did we lie in wait last turn? */
extern int maxobj; /* How much can we carry */
extern int missedstairs; /* True if we missed the stairs */
extern int morecount; /* Number of messages since last command */
extern int msgonscreen; /* There is a rogomatic msg on the screen */
extern int newarmor; /* True if our armor status has changed */
extern int newring; /* True if our ring status has changed */
extern int newweapon; /* True if our weapon status has changed */
extern int nohalf; /* True if no halftime show */
extern int noterm; /* True if no human watching */
extern int objcount; /* Number of objects */
extern int ourscore; /* Our score when we died/quit */
extern int playing; /* True if still playing the game */
extern int poorarrow; /* Number of times we failed to kill in one blow */
extern int protected; /* True if we protected our armor */
extern int putonseeinv; /* Time when last put on see invisible ring */
extern int quitat; /* Score we are trying to beat */
extern int redhands; /* True if our hands are red */
extern int replaying; /* True if replaying old game */
extern int revvideo; /* True if in rev. video mode */
extern int rightring; /* Index of our right ring */
extern int rogpid; /* Process id of Rogue process */
extern int room[]; /* Flags for each room */
extern int row; /* Where is the cursor? */
extern int col; /* Where is the cursor? */
extern int scrmap[24][80]; /* Attribute flags for squares */
extern int slowed; /* True if we recently slowed a monster */
extern int stairrow; /* Where is the staircase */
extern int staircol; /* Where is the staircase */
extern int teleported; /* Times teleported on this level */
extern int terse; /* True if in terse mode */
extern int transparent; /* True ==> user mode */
extern int trapc; /* Column of last trap */
extern int trapr; /* Row of last trap */
extern int urocnt; /* Un-identified Rogue Object count */
extern int usesynch; /* Have we finished using something? */
extern int usingarrow; /* True if wielding an arrow from a trap */
extern int version; /* From types.h, set by getrogueversion */
extern int wplusdam; /* Weapon damage bonus */
extern int wplushit; /* Weapon hit bonus */
extern int zone; /* Current zone (0 to 8) */
extern int zonemap[9][9]; /* Connectivity map */

/* Status line variables */
extern int Level;
extern int MaxLevel;
extern int Gold;
extern int Hp;
extern int Hpmax;
extern int Str;
extern int Strmax;
extern int Ac;
extern int Exp;
extern int Explev;
extern char Ms[]; /* Msg 'X', 'Hungry', 'Weak', 'Fainting' */
extern int turns; /* Estimated time in Rogue turns since start */

/* Geometry data */
extern int deltc[]; /* Displacements for directions */
extern int deltr[]; /* Displacements for directions */
extern int deltrc[]; /* Ditto */
extern char keydir[]; /* Directions for motion keys */
extern int movedir; /* Which direction did we last move */
extern stuff translate[]; /* What Rogue characters represent */

/* Time history */
extern timerec timespent[];

/* Objects in pack */
extern invrec inven[];
extern int invcount;

/* Stuff on this level */
extern stuffrec slist[];
extern int slistlen;

/* Monster on this level */
extern monrec mlist[];
extern int mlistlen;

extern char killedmonster;
extern char targetmonster;

/* Door search variables */
extern int new_mark;
extern int new_findroom;
extern int new_search;
extern int new_stairs;
extern int new_arch;
extern int timessearched[24][80];
extern int timetosearch;
extern int searchstartr;
extern int searchstartc;
extern int reusepsd;

/* Results of last makemove */
extern int ontarget;
extern int targetrow;
extern int targetcol;

/* Monster attribute and Long term memory arrays */
extern attrec monatt[]; /* Monster attributes */
extern lrnrec ltm; /* Long term memory -- general */
extern ltmrec monhist[]; /* Long term memory -- creatures */
extern ltmrec delhist[]; /* Long term memory -- changes this game */
extern int nextmon; /* Length of LTM */
extern int monindex[]; /* Index into monhist array */

/* Genetic learning arrays */
extern int knob[]; /* Knobs */
extern int k_srch; /* Propensity for searching squares */
extern int k_door; /* Propensity for searching doors */
extern int k_rest; /* Propensity for resting */
extern int k_arch; /* Propensity for firing arros */
extern int k_exper; /* Level on which to experiment with items */
extern int k_run; /* Propenstiy for retreating */
extern int k_wake; /* Propensity for waking things up */
extern int k_food; /* Propensity for hoarding food (rings) */

#endif
