#include <stdlib.h>

string wepNamesBM[] = { "GS", "Sns", "Hammer",
                        "LS", "SA", "GL",
                        "DB", "HH", "Lance" };

string dmgFilesBM[] = { "Lsword", "Sword", "Hammer",
                        "Lsword2", "Axe", "Gunlance",
                        "WSword", "Pipe", "Lance" };

int wepOffsetsBM[] = { 0x1BA9B4, 0x1BB894, 0x1BC800,
                       0x1BD6C4, 0x1BE358, 0x1BEFD0,
                       0x1BFC64, 0x1C0A2C, 0x1C1554 };

typedef struct {
    unsigned char model[2];
    int8_t rarity;
    int8_t wepVar;
    unsigned char price[4];
    int8_t sharp1;
    int8_t sharp2;
    unsigned char raw[2];
    int8_t defense;
    int8_t affinity;
    int8_t eleType;
    int8_t eleDmg;
    int8_t eleType2;
    int8_t eleDmg2;
    int8_t statusType;
    int8_t statusDmg;
    int8_t slots;
    int8_t wepVar2;
    int8_t unk;
    int8_t sort;
    int8_t color;
    char padding[3];
} WeaponDataBM;

class WeaponBM {
    string name;
    string desc;
    WeaponDataBM data;
};
