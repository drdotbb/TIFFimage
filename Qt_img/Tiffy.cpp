
#include <iostream>
using namespace std;



typedef struct Tiff {
    uint32_t width;
    uint32_t height;
    uint16_t compression;

    uint32_t rowsperstrip;
    uint32_t* stripoffsets;
    uint32_t* stripbytecounts;
    uint32_t stripcount;
    uint16_t samplesperpixel;
    uint32_t bitspersample;
    uint16_t planarconfiguration;
    uint16_t sampleformat;
    uint32_t imagelength;
    uint8_t orientation;
    uint8_t fillorder;
    uint32_t photometric_interpretation;
    uint8_t isTiled;

    char* description;
} Tiff;

static Tiff getEmpty() {
    Tiff d;
    d.width = 0;
    d.height = 0;
    d.stripcount = 0;
    d.compression = 1;
    d.rowsperstrip = 0;
    d.stripoffsets = 0;
    d.stripbytecounts = 0;
    d.samplesperpixel = 1;
    d.bitspersample = 0;
    d.planarconfiguration = 1;
    d.sampleformat = 1;
    d.imagelength = 0;
    d.description = 0;
    d.orientation = 1;
    d.fillorder = 1;
    d.photometric_interpretation = 2;
    d.isTiled = 0;
    return d;
}
