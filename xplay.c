//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//лл                                                                       лл
//лл   XPLAY.C                                                             лл
//лл                                                                       лл
//лл   Standard XMIDI file performance utility                             лл
//лл                                                                       лл
//лл   V1.00 of 23-Oct-91                                                  лл
//лл   V1.01 of 12-Dec-91: New timbre request structure                    лл
//лл   V1.02 of 20-Dec-91: Register requested sequence only                лл
//лл   V1.03 of  4-Jul-92: Check GTL handle before closing                 лл
//лл                                                                       лл
//лл   Project: IBM Audio Interface Library                                лл
//лл    Author: John Miles                                                 лл
//лл                                                                       лл
//лл   C source compatible with Turbo C++ v1.0 or later                    лл
//лл                                                                       лл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//лл                                                                       лл
//лл   xplay.obj: xplay.c gen.h ail.h                                      лл
//лл      bcc -ml -c -v xplay.c                                            лл
//лл                                                                       лл
//лл   xplay.exe: xplay.obj gen.lib ail.obj                                лл
//лл      tlink @xplay.lls                                                 лл
//лл                                                                       лл
//лл   Contents of XPLAY.LLS:                                              лл
//лл     /c /v /x +                                                        лл
//лл     \bc\lib\c0l.obj +                                                 лл
//лл     xplay ail, +                                                      лл
//лл     xplay.exe, +                                                      лл
//лл     xplay.map, +                                                      лл
//лл     \bc\lib\cl.lib gen.lib                                            лл
//лл                                                                       лл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//лл                                                                       лл
//лл   Copyright (C) 1991, 1992 Miles Design, Inc.                         лл
//лл                                                                       лл
//лл   Miles Design, Inc.                                                  лл
//лл   10926 Jollyville #308                                               лл
//лл   Austin, TX 78759                                                    лл
//лл   (512) 345-2642 / FAX (512) 338-9630 / BBS (512) 454-9990            лл
//лл                                                                       лл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <alloc.h>
#include <string.h>

#include "ail.h"        // Audio Interface Library API function header
#include "gen.h"        // General DOS and system functions

const char VERSION[] = "1.04";

static const char *const gm_names[] =
{
    "AcouGrandPiano",
    "BrightAcouGrand",
    "ElecGrandPiano",
    "Honky-tonkPiano",
    "Rhodes Piano",
    "Chorused Piano",
    "Harpsichord",
    "Clavinet",
    "Celesta",
    "Glockenspiel",
    "Music box",
    "Vibraphone",
    "Marimba",
    "Xylophone",
    "Tubular Bells",
    "Dulcimer",
    "Hammond Organ",
    "Percussive Organ",
    "Rock Organ",
    "Church Organ",
    "Reed Organ",
    "Accordion",
    "Harmonica",
    "Tango Accordion",
    "Acoustic Guitar1",
    "Acoustic Guitar2",
    "Electric Guitar1",
    "Electric Guitar2",
    "Electric Guitar3",
    "Overdrive Guitar",
    "Distorton Guitar",
    "Guitar Harmonics",
    "Acoustic Bass",
    "Electric Bass 1",
    "Electric Bass 2",
    "Fretless Bass",
    "Slap Bass 1",
    "Slap Bass 2",
    "Synth Bass 1",
    "Synth Bass 2",
    "Violin",
    "Viola",
    "Cello",
    "Contrabass",
    "Tremulo Strings",
    "Pizzicato String",
    "Orchestral Harp",
    "Timpany",
    "String Ensemble1",
    "String Ensemble2",
    "Synth Strings 1",
    "SynthStrings 2",
    "Choir Aahs",
    "Voice Oohs",
    "Synth Voice",
    "Orchestra Hit",
    "Trumpet",
    "Trombone",
    "Tuba",
    "Muted Trumpet",
    "French Horn",
    "Brass Section",
    "Synth Brass 1",
    "Synth Brass 2",
    "Soprano Sax",
    "Alto Sax",
    "Tenor Sax",
    "Baritone Sax",
    "Oboe",
    "English Horn",
    "Bassoon",
    "Clarinet",
    "Piccolo",
    "Flute",
    "Recorder",
    "Pan Flute",
    "Bottle Blow",
    "Shakuhachi",
    "Whistle",
    "Ocarina",
    "Lead 1 squareea",
    "Lead 2 sawtooth",
    "Lead 3 calliope",
    "Lead 4 chiff",
    "Lead 5 charang",
    "Lead 6 voice",
    "Lead 7 fifths",
    "Lead 8 brass",
    "Pad 1 new age",
    "Pad 2 warm",
    "Pad 3 polysynth",
    "Pad 4 choir",
    "Pad 5 bowedpad",
    "Pad 6 metallic",
    "Pad 7 halo",
    "Pad 8 sweep",
    "FX 1 rain",
    "FX 2 soundtrack",
    "FX 3 crystal",
    "FX 4 atmosphere",
    "FX 5 brightness",
    "FX 6 goblins",
    "FX 7 echoes",
    "FX 8 sci-fi",
    "Sitar",
    "Banjo",
    "Shamisen",
    "Koto",
    "Kalimba",
    "Bagpipe",
    "Fiddle",
    "Shanai",
    "Tinkle Bell",
    "Agogo Bells",
    "Steel Drums",
    "Woodblock",
    "Taiko Drum",
    "Melodic Tom",
    "Synth Drum",
    "Reverse Cymbal",
    "Guitar FretNoise",
    "Breath Noise",
    "Seashore",
    "Bird Tweet",
    "Telephone",
    "Helicopter",
    "Applause/Noise",
    "Gunshot",

    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>", // 27..34:  High Q; Slap; Scratch Push; Scratch Pull; Sticks;
    "<Reserved>", //          Square Click; Metronome Click; Metronome Bell
    "Ac Bass Drum",
    "Bass Drum 1",
    "Side Stick",
    "Acoustic Snare",
    "Hand Clap",
    "Electric Snare",
    "Low Floor Tom",
    "Closed High Hat",
    "High Floor Tom",
    "Pedal High Hat",
    "Low Tom",
    "Open High Hat",
    "Low-Mid Tom",
    "High-Mid Tom",
    "Crash Cymbal 1",
    "High Tom",
    "Ride Cymbal 1",
    "Chinese Cymbal",
    "Ride Bell",
    "Tambourine",
    "Splash Cymbal",
    "Cow Bell",
    "Crash Cymbal 2",
    "Vibraslap",
    "Ride Cymbal 2",
    "High Bongo",
    "Low Bongo",
    "Mute High Conga",
    "Open High Conga",
    "Low Conga",
    "High Timbale",
    "Low Timbale",
    "High Agogo",
    "Low Agogo",
    "Cabasa",
    "Maracas",
    "Short Whistle",
    "Long Whistle",
    "Short Guiro",
    "Long Guiro",
    "Claves",
    "High Wood Block",
    "Low Wood Block",
    "Mute Cuica",
    "Open Cuica",
    "Mute Triangle",
    "Open Triangle",
    "Shaker",
    "Jingle Bell",
    "Bell Tree",
    "Castanets",
    "Mute Surdu",
    "Open Surdu",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>",
    "<Reserved>"
};


const char *get_instrument_name(unsigned bank, unsigned patch)
{
    if(bank == 0)
        return gm_names[patch];
    if(bank == 127)
        return gm_names[patch + 128];
    return "<unknown>";
}

int endsWith(const char *str, const char *suffix)
{
    size_t lenstr;
    size_t lensuffix;

    if(!str || !suffix)
        return 0;

    lenstr = strlen(str);
    lensuffix = strlen(suffix);

    if(lensuffix >  lenstr)
        return 0;

    return stricmp(str + lenstr - lensuffix, suffix) == 0;
}


/***************************************************************/

//
// Standard C routine for Global Timbre Library access
//

void far *load_global_timbre(FILE *GTL, unsigned bank, unsigned patch)
{
    unsigned far *timb_ptr;
    static unsigned len;

    static struct                  // GTL file header entry structure
    {
        char patch;
        char bank;
        unsigned long offset;
    }
    GTL_hdr;

    if(GTL == NULL) return NULL;   // if no GTL, return failure

    rewind(GTL);                   // else rewind to GTL header

    do                             // search file for requested timbre
    {
        fread(&GTL_hdr, sizeof(GTL_hdr), 1, GTL);
        if(GTL_hdr.bank == -1)
            return NULL;             // timbre not found, return NULL
    }
    while((GTL_hdr.bank != bank) ||
          (GTL_hdr.patch != patch));

    fseek(GTL, GTL_hdr.offset, SEEK_SET);
    fread(&len, 2, 1, GTL);        // timbre found, read its length

    timb_ptr = farmalloc(len);     // allocate memory for timbre ..
    *timb_ptr = len;
    // and load it
    fread((timb_ptr + 1), len - 2, 1, GTL);

    if(ferror(GTL))                // return NULL if any errors
        return NULL;                // occurred
    else
        return timb_ptr;            // else return pointer to timbre
}

/***************************************************************/
void main(int argc, char *argv[])
{
    HDRIVER hdriver;
    HSEQUENCE hseq;
    drvr_desc far *desc;
    FILE *GTL;
    char GTL_filename[32];
    char far *state;
    char far *drvr;
    char far *timb;
    char far *tc_addr;
    unsigned char far *buffer;
    unsigned long state_size;
    unsigned bank, patch, tc_size, seqnum, treq;
    char key;
    const char *drPath = "sb16fm.adv";
    const char *gtlPath = NULL;
    const char *xmiPath = NULL;
    const char *xmiPathOrig = NULL;
    int keepWork = 0;
    int i;

    if(!strcmp((char far *) 0x000004f0, "XPLAY"))
    {
        printf("You must type 'EXIT' before re-starting XPLAY.\n");
        exit(1);
    }

    for(i = 1; i < argc;)
    {
        if(!strcmp(argv[i], "-d"))
        {
            if(i == argc - 1)
            {
                printf("Missing driver filename after -d argument!\n");
                exit(1);
            }
            drPath = argv[i + 1];
            i += 2;
            continue;
        }
        else if(!strcmp(argv[i], "-g"))
        {
            if(i == argc - 1)
            {
                printf("Missing global timbre library filename after -g argument!\n");
                exit(1);
            }
            gtlPath = argv[i + 1];
            i += 2;
            continue;
        }

        break;
    }

    if(i < argc)
        xmiPath = argv[i];
    i++;

    seqnum = 0;
    if(i < argc)
        seqnum = val(argv[i], 10);

    printf("\nXPLAY version %s  Copyright (C) 1991, 1992 Miles Design, Inc., 2020 Wohlstand\n", VERSION);
    printf("-------------------------------------------------------------------------------\n\n");

    if(argc < 2 || !xmiPath)
    {
        printf("This program plays an Extended MIDI (XMIDI) sequence through a \n");
        printf("specified Audio Interface Library V2.0 sound driver.\n\n");
        printf("Usage: XPLAY [-d driver_filename] [-g timbre_file_name] XMIDI_filename [sequence_number]\n");
        exit(1);
    }

    if(endsWith(xmiPath, ".mid"))
    {
        xmiPathOrig = xmiPath;
        spawnlp(P_WAIT, "midiform.exe", "midiform.exe", "tmp.xmi", xmiPath, NULL);
        xmiPath = "tmp.xmi";
    }

    //
    // Load driver file at seg:0000
    //

    drvr = load_driver((char *)drPath);
    if(drvr == NULL)
    {
        printf("Driver %s not found\n", drPath);
        exit(1);
    }

    //
    // Initialize API before calling any Library functions
    //

    AIL_startup();

    //
    // Register the driver with the API
    //

    hdriver = AIL_register_driver(drvr);
    if(hdriver == -1)
    {
        printf("Driver %s not compatible with linked API version.\n", drPath);
        AIL_shutdown(NULL);
        exit(1);
    }

    //
    // Get driver type and factory default I/O parameters; exit if
    // driver is not capable of interpreting MIDI files
    //

    desc = AIL_describe_driver(hdriver);

    if(desc->drvr_type != XMIDI_DRVR)
    {
        printf("Driver %s not an XMIDI driver.\n", drPath);
        AIL_shutdown(NULL);
        exit(1);
    }

    //
    // Verify presence of driver's sound hardware and prepare
    // driver/hardware for use
    //

    if(!AIL_detect_device(hdriver, desc->default_IO, desc->default_IRQ,
                          desc->default_DMA, desc->default_DRQ))
    {
        printf("Sound hardware not found.\n");
        AIL_shutdown(NULL);
        exit(1);
    }

    AIL_init_driver(hdriver, desc->default_IO, desc->default_IRQ,
                    desc->default_DMA, desc->default_DRQ);

    state_size = AIL_state_table_size(hdriver);

    //
    // Load XMIDI data file
    //

    buffer = read_file((char *)xmiPath, NULL);
    if(buffer == NULL)
    {
        printf("Can't load XMIDI file %s.\n", xmiPath);
        AIL_shutdown(NULL);
        exit(1);
    }

    //
    // Get name of Global Timbre Library file by appending suffix
    // supplied by XMIDI driver to GTL filename prefix "SAMPLE."
    //

    if(!gtlPath)
    {
        strcpy(GTL_filename, "fat.");
        strcat(GTL_filename, desc->data_suffix);
    }
    else
        strcpy(GTL_filename, gtlPath);

    //
    // Set up local timbre cache; open Global Timbre Library file
    //

    tc_size = AIL_default_timbre_cache_size(hdriver);
    if(tc_size)
    {
        tc_addr = farmalloc((unsigned long) tc_size);
        AIL_define_timbre_cache(hdriver, tc_addr, tc_size);
    }

    GTL = fopen(GTL_filename, "rb");

    //
    // Look up and register desired sequence in XMIDI file, loading
    // timbres if needed
    //

    state = farmalloc(state_size);
    if((hseq = AIL_register_sequence(hdriver, buffer, seqnum, state, NULL)) == -1)
    {
        printf("Sequence %u not present in XMIDI file \"%s\".\n", seqnum, xmiPath);
        AIL_shutdown(NULL);
        exit(1);
    }

    //    bank = 0;
    //    patch = 33;
    //    timb = load_global_timbre(GTL, bank, patch);
    //    AIL_install_timbre(hdriver, bank, patch, timb);
    //    printf("Installed timbre bank %u, patch %u (%s)\n", bank, patch, gm_names[patch]);
    //    farfree(timb);

    while((treq = AIL_timbre_request(hdriver, hseq)) != 0xffff)
    {
        bank = treq / 256;
        patch = treq % 256;

        timb = load_global_timbre(GTL, bank, patch);
        if(timb != NULL)
        {
            AIL_install_timbre(hdriver, bank, patch, timb);
            farfree(timb);
            printf("Installed timbre bank %u, patch %u (%s)\n", bank, patch, get_instrument_name(bank, patch));
        }
        else
        {
            printf("Timbre bank %u, patch %u not found ", bank, patch);
            printf("in Global Timbre Library %s\n", GTL_filename);
            AIL_shutdown(NULL);
            exit(1);
        }
    }

    if(GTL != NULL)
        fclose(GTL);

    //
    // Start music playback and set flag to prevent user from
    // launching multiple copies of XPLAY from the DOS shell
    //

    printf("Playing sequence %u from XMIDI file \"%s\" ...\n\n", seqnum, xmiPath);

    AIL_start_sequence(hdriver, hseq);

    //   strcpy((char far *) 0x000004f0, "XPLAY");
    printf("----------------------\n");
    if(xmiPathOrig)
        printf("Song: %s (sequence %d)\n", xmiPathOrig, seqnum);
    else
        printf("Song: %s (sequence %d)\n", xmiPath, seqnum);
    printf("Timbre bank: %s\n", GTL_filename);
    printf("Driver: %s\n", drPath);
    printf("----------------------\n");
    printf("Press ESC to stop the song.\n");
    printf("----------------------\n");
    printf("S - Pause/Stop song\n");
    printf("R - Resume paused song\n");
    printf("P - Play song at start\n");
    printf("----------------------\n");
    // spawnlp(P_WAIT, "pause", NULL);

    keepWork = 1;
    while(keepWork)
    {
        key = getch();
        switch(key)
        {
        case 'r':
        case 'R':
            AIL_resume_sequence(hdriver, hseq);
            break;
        case 's':
        case 'S':
            AIL_stop_sequence(hdriver, hseq);
            break;
        case 'p':
        case 'P':
            AIL_start_sequence(hdriver, hseq);
            break;
        case 27:
            keepWork = 0;
            break;
        }
    }

    //
    // Shut down API and all installed drivers; write XMIDI filename
    // to any front-panel displays
    //

    if(xmiPathOrig)
        remove(xmiPath);

    //   strcpy((char far *) 0x000004f0, "        ");
    printf("XPLAY stopped.\n");
    AIL_shutdown((char *)xmiPathOrig);
}

