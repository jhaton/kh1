/**
 * Provider for handling file names and extensions for world and room data files
 */

#include "common.h"
#include "worldfile.h"

#include "stdio.h"
#include "string.h"

// Loaded world/room index?
extern s32 D_002B8678;

extern s32 D_002B8680;

/**
 * World abbreviations used for file names
 * al - Agrabah
 * aw - Wonderland
 * dc - Disney Castle
 * dh - Dive to the Heart
 * di - Destiny Islands
 * ew - End of the World
 * he - Olympus Coliseum
 * lm - Atlantica
 * nm - Halloween Town
 * pc - Hollow Bastion
 * pi - Monstro
 * po - 100 Acre Wood
 * pp - Neverland
 * tw - Traverse Town
 * tz - Deep Jungle
 * xx - Unknown/Unused
 * zz - Miscellanous
 */
const char* worldAbbrs[21] = {
    "dh", "di", "dc", "tw", "aw", "tz", "po", "xx", "al", "lm", "nm",
    "he", "pi", "pp", "xx", "pc", "ew", "xx", "xx", "xx", "zz",
};

char worldDataExt[] = ".wdt";
char roomArchiveExt[] = ".ard";

s32 D_002B91E0 = 0;
s32 D_002B91E4 = 0;
s32 D_002B91E8 = 0;
s32 D_002B91EC = 0;

char worldDataFile[0x40] = {};
char worldBinImgFile[0x40] = {};
char roomArchiveRaw[0x40] = {};
char roomArchiveFile[0x40] = {};

const char* worldfile_getAbbr(int worldIndex, char* dest) {
    return strcpy(dest, worldAbbrs[worldIndex]);
}

char* worldfile_getNameNoSet(s32 worldIndex, s32 roomIndex, UNK_TYPE unused, char* dest) {
    char fileNumber[0x10];
    char worldAbbr[0x40];

    worldfile_getAbbr(worldIndex, worldAbbr);
    sprintf(fileNumber, "%2d", roomIndex + 1);
    if (fileNumber[0] == ' ') {
        fileNumber[0] = '0';
    }
    strcpy(dest, worldAbbr);
    return strcat(dest, fileNumber);
}

char* worldfile_getNames(void) {
    char fileNumber[0x10];
    char worldAbbr[0x40];

    worldfile_getAbbr(D_002B8678, worldAbbr);
    sprintf(fileNumber, "%2d", D_002B8680 + 1);
    if (fileNumber[0] == ' ') {
        fileNumber[0] = '0';
    }
    strcpy(worldDataFile, worldAbbr);
    strcat(worldDataFile, worldDataExt);
    strcpy(roomArchiveFile, worldAbbr);
    strcat(roomArchiveFile, fileNumber);
    strcat(roomArchiveFile, roomArchiveExt);
    strcpy(roomArchiveRaw, worldAbbr);
    return strcat(roomArchiveRaw, fileNumber);
}

char* worldfile_getBinImgName(s32 roomIndex) {
    char fileNumber[0x10];
    char worldAbbr[0x40];

    worldfile_getAbbr(D_002B8678, worldAbbr);
    sprintf(fileNumber, "%2d", roomIndex + 1);
    if (fileNumber[0] == ' ') {
        fileNumber[0] = '0';
    }
    strcpy(worldBinImgFile, worldAbbr);
    strcat(worldBinImgFile, "00_");
    return strcat(worldBinImgFile, fileNumber);
}
