/*
 * replay.c: Rog-O-Matic XIV (CMU) Wed Mar 20 00:13:45 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * Make a table of offsets to the beginning of each level of a
 * Rog-O-Matic log file.
 */
#include "replay.h"

#include <ctype.h>
#include <curses.h>
#include <string.h>

#include "globals.h"
#include "io.h"
#include "types.h"

#define MAXNUMLEV 50
#define FIRSTLEVSTR "\nR: "
#define NEWLEVSTR "\nR: {ff}"
#define POSITAT "{ff}"

struct levstruct levpos[MAXNUMLEV];

int numlev = 0;

/*
 * positionreplay: Called when user has typed the 'R' ocmmand, it fills
 * the level table by  calling findlevel if necessary, and then positions
 * the log file to the level requested by the user.
 */
void positionreplay()
{
    int curlev;
    long curpos;
    char cmd;

    /* Prompt user for a command character, read it, and lower case it */
    saynow("Which level (f = first, p = previous, c = current, l = last): ");
    
    cmd = getch();
    if(isupper(cmd)) {
        cmd = tolower(cmd);
    }

    /* Clear the prompt */
    saynow("");

    /* If command is not in the list, clear the prompt and exit. */
    switch(cmd) {
    case 'f':
    case 'p':
    case 'c':
    case 'n':
    case 'l':

        break;
    default:
        return;
    }

    /* Save the current position in the file */
    curpos = ftell(logfile);

    /* Read the log file, if we have not already done so */
    if(!logdigested) {
        saynow("Reading whole log file to find levels...");

        if(!findlevel(logfile, levpos, &numlev, MAXNUMLEV)) {
            saynow("Findlevel failed! Let's try to get back to where we were...");
            fseek(logfile, curpos, 0);

            return;
        }

        ++logdigested;
    }

    /* Now figure out the current level (so relative commands will work) */
    for(curlev = 0; curlev < (numlev - 1); ++curlev) {
        if(levpos[curlev + 1].pos > curpos) {
            break;
        }
    }

    /* Now clear the screen, position the log file, and return */
    switch(cmd) {
    case 'f':
        fseek(logfile, levpos[0].pos, 0);

        break;
    case 'p':
        if(curlev > 0) {
            fseek(logfile, levpos[curlev - 1].pos, 0);
        }
        else {
            fseek(logfile, levpos[0].pos, 0);
        }

        break;
    case 'c':
        fseek(logfile, levpos[curlev].pos, 0);

        break;
    case 'n':
        if(curlev < (numlev - 1)) {
            fseek(logfile, levpos[curlev + 1].pos, 0);
        }
        else {
            fseek(logfile, levpos[curlev].pos, 0);
        }

        break;
    case 'l':
        fseek(logfile, levpos[numlev - 1].pos, 0);

        break;
    default:
        fseek(logfile, 0L, 0);
    }

    /* Clear the screen */
    clearscreen();

    /* Force a newlevel() call */
    Level = -1;
}

/*
 * findlevel: Make a table of offsets to the verious levels of a
 *            Rog-O-Matic log file.
 */
int findlevel(FILE *f, struct levstruct *lvpos, int *nmlev, int maxnum)
{
    char ch;
    int l = 0;

    *nmlev = 0;

    /* Position file after first newline */
    rewind(f);
    
    ch = getc(f);
    while((ch != '\n') && ((int)ch != EOF)) {
        ch = getc(f);
    }

    /* This is the start of level one */
    lvpos[l].pos = ftell(f);

    if(!findmatch(f, FIRSTLEVSTR)) {
        rewind(f);
        
        return FAILURE;
    }

    fillstruct(f, &lvpos[l]);

    while((++l <= maxnum) && (findmatch(f, NEWLEVSTR))) {
        fseek(f, (long)-strlen(POSITAT), 1);
        lvpos[l].pos = ftell(f);
        fillstruct(f, &levpos[l]);
    }

    *nmlev = l;
    rewind(f);

    return SUCCESS;
}

/*
 * fillstruct: Scan the logfile from the current point, and fill in the
 *             fields of a levstruct.
 */
void fillstruct(FILE *f, struct levstruct *lev)
{
    lev->level = 0;
    lev->gold = 0;
    lev->hp = 0;
    lev->hpmax = 0;
    lev->str = 0;
    lev->strmax = 0;
    lev->ac = 0;
    lev->explev = 0;
    lev->exp = 0;

    if(!findmatch(f, "Level:")) {
        return;
    }

    fscanf(f, "%d", &lev->level);

    if(!findmatch(f, "Gold:")) {
        return;
    }

    fscanf(f, "%d", &lev->gold);

    if(!findmatch(f, "Hp:")) {
        return;
    }

    fscanf(f, "%d(%d)", &lev->hp, &lev->hpmax);

    if(!findmatch(f, "Str:")) {
        return;
    }

    fscanf(f, "%d(%d)", &lev->str, &lev->strmax);

    /* Armor class */
    if(!findmatch(f, ":")) {
        return;
    }

    fscanf(f, "%d", &lev->ac);

    if(!findmatch(f, "Exp:")) {
        return;
    }

    fscanf(f, "%d/%d", &lev->explev, &lev->exp);

    saynow("Found level %d, has %d gold...", lev->level, lev->gold);
}

/*
 * findmatch: Read from a stream until string 's' has been read. Returns 0
 * if EOF is read, and 1 if the match is found. The stream is left
 * immediately after the matched string.
 *
 * Restriction: 's' mush not contain prefix of itself as a substring
 */
int findmatch(FILE *f, char *s)
{
    char *m = s;
    char ch;

    while(*m) {
        ch = fgetc(f);

        if(ch == EOF) {
            break;
        }

        if(ch != *(m++)) {
            m = s;
        }
    }

    if(*m) {
        return 0;
    }
    else {
        return 1;
    }
}