/*
 * Modern port of MIDIFORM.C from DOS to Linux, made by Vitaly Novichkov
 *
 * Original header:
 *
 * ███████████████████████████████████████████████████████████████████████████
 * ██                                                                       ██
 * ██   MIDIFORM.C                                                          ██
 * ██                                                                       ██
 * ██   Extended MIDI (XMIDI) file conversion utility                       ██
 * ██                                                                       ██
 * ██   V1.00 of 15-Sep-91                                                  ██
 * ██   V1.01 of 27-Nov-91: MAX_EVLEN increased to 384                      ██
 * ██   V1.02 of  7-Feb-92: Input line comments allowed                     ██
 * ██                       Quant math precision increased                  ██
 * ██                                                                       ██
 * ██   Project: IBM Audio Interface Library                                ██
 * ██    Author: John Miles                                                 ██
 * ██                                                                       ██
 * ██   C source compatible with Turbo C++ v1.0 or later                    ██
 * ██                                                                       ██
 * ███████████████████████████████████████████████████████████████████████████
 * ██                                                                       ██
 * ██   midiform.obj: midiform.c gen.h                                      ██
 * ██      bcc -ml -c -v midiform.c                                         ██
 * ██                                                                       ██
 * ██   midiform.exe: midiform.obj gen.lib                                  ██
 * ██      tlink @midiform.lls                                              ██
 * ██                                                                       ██
 * ██   Contents of MIDIFORM.LLS:                                           ██
 * ██     /c /v /x +                                                        ██
 * ██     \bc\lib\c0l.obj +                                                 ██
 * ██     midiform, +                                                       ██
 * ██     midiform.exe, +                                                   ██
 * ██     midiform.map, +                                                   ██
 * ██     \bc\lib\cl.lib gen.lib                                            ██
 * ██                                                                       ██
 * ███████████████████████████████████████████████████████████████████████████
 * ██                                                                       ██
 * ██   Copyright (C) 1991, 1992 Miles Design, Inc.                         ██
 * ██                                                                       ██
 * ██   Miles Design, Inc.                                                  ██
 * ██   10926 Jollyville #308                                               ██
 * ██   Austin, TX 78759                                                    ██
 * ██   (512) 345-2642 / FAX (512) 338-9630 / BBS (512) 454-9990            ██
 * ██                                                                       ██
 * ███████████████████████████████████████████████████████████████████████████
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef __linux__
#include <dos.h>
#include <alloc.h>
#endif
#include <string.h>
#ifdef __linux__
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#define far /* workarounds */
#define near
#define farfree free
#define far_memmove memmove
#define farmalloc malloc
#define fileno(x) (x)
#else
#include <io.h>
#include <conio.h>
#endif
#include <ctype.h>

#ifdef __linux__

static FILE  *s_tracked_files[1000];
static size_t s_tracked_files_count = 0;

static FILE *my_fopen(const char *filename, const char *mode)
{
    FILE *f = fopen(filename, mode);
    if(!f)
        return NULL;
    if(s_tracked_files_count >= 1000)
        return f; /* don't track this! */
    s_tracked_files[s_tracked_files_count++] = f;
    return f;
}

static int my_fclose(FILE *f)
{
    size_t i;
    int ret;
    int found = 0;

    ret = fclose(f);

    for(i = 0; i < s_tracked_files_count; ++i)
    {
        if(f == s_tracked_files[i])
        {
            found = 1;
            s_tracked_files_count--;
        }

        if(found)
            s_tracked_files[i] = s_tracked_files[i + 1];
    }

    return ret;
}

static void my_fcloseall()
{
    size_t i;
    for(i = 0; i < s_tracked_files_count; ++i)
    {
        fclose(s_tracked_files[i]);
        s_tracked_files[i] = NULL;
    }

    s_tracked_files_count = 0;
}


extern int mkstemp(char *__template);

static char my_getche()
{
    char ret[100];
    ret[0] = '\0';

    fgets(ret, 100, stdin);

    if(ret[1] == '\n')
        ret[1] = '\0';

    if(strlen(ret) > 1)
        ret[0] = -1;

    return ret[0];
}

uint32_t wswap(uint32_t n)
{
    union
    {
        uint32_t i;
        uint8_t c[4];
    } src, dst;

    src.i = n;
    dst.c[0] = src.c[3];
    dst.c[1] = src.c[2];
    dst.c[2] = src.c[1];
    dst.c[3] = src.c[0];

    return dst.i;
}

uint16_t bswap(uint16_t n)
{
    union
    {
        uint16_t i;
        uint8_t c[2];
    } src, dst;

    src.i = n;
    dst.c[0] = src.c[1];
    dst.c[1] = src.c[0];

    return dst.i;
}

void *norm(void *farptr)
{
    return  farptr;
}

long ptr_dif(void far *sub2, void far *sub1)
{
    return (intptr_t)sub2 - (intptr_t)sub1;
}

void *add_ptr(void *farptr, long offset)
{
    char *p = (char *)farptr;
    p += offset;
    return p;
}

int get_disk_error(void)
{
    return 7;
}

int filelength(FILE *f)
{
    off_t prev;
    off_t siz;

    prev = ftell(f);
    fseek(f, 0, SEEK_END);
    siz = ftell(f);
    fseek(f, prev, SEEK_SET);

    return siz;
}


int strnicmp(const char *a, const char *b, size_t max)
{
    int ca, cb;
    size_t len = 0;
    do
    {
        ca = (unsigned char) * a++;
        cb = (unsigned char) * b++;
        ca = tolower(toupper(ca));
        cb = tolower(toupper(cb));
        len++;
    }
    while(ca == cb && ca != '\0' && len < max);
    return ca - cb;
}


static uint32_t file_size(const char *filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

static uint8_t *read_file(const char *szName, void *dest)
{
    uint8_t *pData;
    int  wSize;
    int  hFile;

    (void)dest;

    if((hFile = open(szName, O_RDONLY)) == -1)
        return(NULL);

    wSize =  lseek(hFile, 0, SEEK_END);
    lseek(hFile, 0, SEEK_SET);

    if((pData = (uint8_t *)malloc(wSize)) == NULL)
    {
        close(hFile);
        return(NULL);
    }

    if(read(hFile, pData, wSize) != wSize)
    {
        close(hFile);
        free(pData);
        return(NULL);
    }

    close(hFile);
    return(pData);
}

int get_pos(int *y, int *x)
{
    char buf[30] = {0};
    int ret, i, pow;
    char ch;

    *y = 0;
    *x = 0;

    struct termios term, restore;

    tcgetattr(0, &term);
    tcgetattr(0, &restore);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &term);

    write(1, "\033[6n", 4);

    for(i = 0, ch = 0; ch != 'R'; i++)
    {
        ret = read(0, &ch, 1);
        if(!ret)
        {
            tcsetattr(0, TCSANOW, &restore);
            fprintf(stderr, "getpos: error reading response!\n");
            return 1;
        }
        buf[i] = ch;
        /* printf("buf[%d]: \t%c \t%d\n", i, ch, ch); */
    }

    if(i < 2)
    {
        tcsetattr(0, TCSANOW, &restore);
        printf("i < 2\n");
        return(1);
    }

    for(i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
        * x = *x + (buf[i] - '0') * pow;

    for(i--, pow = 1; buf[i] != '['; i--, pow *= 10)
        * y = *y + (buf[i] - '0') * pow;

    tcsetattr(0, TCSANOW, &restore);
    return 0;
}

#endif

static char *my_gets(char *out, int max)
{
#ifdef __linux__
    size_t l;
    char *ret = fgets(out, max, stdin);

    if(ret)
    {
        l = strlen(out) - 1;
        if(out[l] == '\n')
            out[l] = '\0';
    }

    return ret;
#else
    (void)max;
    return gets(out);
#endif
}

const char VERSION[] = "1.03";

#define DEFAULT_QUAN 120   /* Default quantization rate in hertz                */
#define MAX_TRKS 64        /* Max. # of tracks in MIDI input file               */
#define MAX_EVLEN 384      /* Max. length of MIDI event in bytes                */
#define MAX_TIMB 16384     /* Max. # of timbre requests in MIDI file            */
#define MAX_RBRN 128       /* Max. # of unique branch targets in MIDI file      */
#define MAX_NOTES 32       /* Max. # of notes simultaneously "on" in MIDI file  */
#define CHAN_CNT 16        /* # of MIDI channels                                */

#define AIL_BRANCH_PT 120  /* AIL MIDI controller for branch points */
#define AIL_TIMB_BNK 114   /* AIL MIDI controller: Timbre Bank Select */

#define EV_NOTE_OFF 0x80   /* Standard MIDI file event types */
#define EV_NOTE_ON 0x90
#define EV_POLY_PRESS 0xa0
#define EV_CONTROL 0xb0
#define EV_PROGRAM 0xc0
#define EV_CHAN_PRESS 0xd0
#define EV_PITCH 0xe0
#define EV_SYSEX 0xf0
#define EV_ESC 0xf7
#define EV_META 0xff
#define EV_INVALID 0x00

#define META_EOT 0x2f      /* Standard MIDI meta-event types */
#define META_TRK_NAME 0x03
#define META_INS_NAME 0x04
#define META_TEMPO 0x51

#define MAX_ULONG 4294967295L

typedef struct
{
    uint16_t bnum;
    uint32_t offset;
}
RBRN_entry;

typedef struct
{
    uint8_t t_num;
    uint8_t t_bank;
}
TIMB_entry;

typedef struct
{
    uint16_t quantization;
}
AILH_block;

typedef struct
{
    uint16_t cnt;
    RBRN_entry brn[MAX_RBRN];
}
RBRN_block;

typedef struct
{
    uint16_t cnt;
    TIMB_entry tbr[MAX_TIMB];
}
TIMB_block;

typedef struct
{
    uint32_t len;
    uint32_t avail;
    const uint8_t far *name;
    uint8_t far *base;
    uint8_t far *ptr;
}
IFF_block;

typedef struct
{
    char far *seq_fn;
    uint8_t far *base;
    uint32_t seq_len;

    uint16_t format;
    uint16_t ntrks;
    uint16_t division;
    uint32_t tick_time;
    uint8_t event[MAX_EVLEN];
    uint32_t event_len;
    uint32_t event_time;
    uint16_t event_chan;

    uint16_t event_trk;
    uint8_t far *trk_ptr[MAX_TRKS];
    uint8_t status[MAX_TRKS];
    uint8_t trk_active[MAX_TRKS];
    uint32_t pending_delta[MAX_TRKS];
}
MIDI;

typedef struct
{
    uint32_t q_int;
    uint32_t DDA_sum;
    uint32_t interval;
    uint32_t delta;
    uint8_t note_chan[MAX_NOTES];
    uint8_t note_num[MAX_NOTES];
    uint32_t note_intvl[MAX_NOTES];
    uint8_t far *note_next[MAX_NOTES];
    uint8_t timb_BNK[CHAN_CNT];
    uint16_t rbs_BNK[CHAN_CNT];
    AILH_block AILH;
    RBRN_block RBRN;
    TIMB_block TIMB;
    IFF_block EVNT;
}
XMIDI;

char tmp_fn[9] = "MFXXXXXX";
char tmp_fn2[9] = "MFXXXXXX";
char out_fn[128];

static void tmpReset(char *buf)
{
    strncpy(buf, "MFXXXXXX", 9);
}

#ifndef __linux__
union REGS inregs, outregs;
#endif

/************************************************************/
void abend(int err, intptr_t info_1, intptr_t info_2)
{
    if(!err) return;

    fprintf(stderr, "Error MF%03u: ", err);

    switch(err)
    {
    case 1:
        switch(info_1)
        {
        case 1:
            fprintf(stderr, "I/O fault");
            break;
        case 2:
            fprintf(stderr, "Insufficient free memory");
            break;
        case 3:
            fprintf(stderr, "File \"%s\" not found", (const char *)info_2);
            break;
        case 4:
            fprintf(stderr, "Can't write to file \"%s\"", (const char *)info_2);
            break;
        case 5:
            fprintf(stderr, "Can't read from file \"%s\"", (const char *)info_2);
            break;
        case 6:
            fprintf(stderr, "Disk full");
            break;
        default:
            fprintf(stderr, "Undefined disk error");
            break;
        }
        break;
    case 2:
        fprintf(stderr, "File \"%s\" not a standard MIDI file", (const char *)info_1);
        break;
    case 3:
        fprintf(stderr, "MIDI event > %u bytes long in file \"%s\"",
                MAX_EVLEN, (const char *)info_1);
        break;
    case 4:
        fprintf(stderr, "Illegal MIDI status byte in file \"%s\"", (const char *)info_1);
        break;
    case 5:
        fprintf(stderr, "Illegal MIDI event in file \"%s\"", (const char *)info_1);
        break;
    case 6:
        fprintf(stderr, "> %u tracks in MIDI file \"%s\"", MAX_TRKS, (const char *)info_1);
        break;
    case 7:
        fprintf(stderr, "Insufficient memory to convert \"%s\"", (const char *)info_1);
        break;
    case 8:
        fprintf(stderr, "Unpaired MIDI note-on event in file \"%s\"",
                (const char *)info_1);
        break;
    case 9:
        fprintf(stderr, "> %u simultaneous notes in file \"%s\"",
                MAX_NOTES, (const char *)info_1);
        break;
    case 10:
        fprintf(stderr, "> %u branch point controllers in file \"%s\"",
                MAX_RBRN, (const char *)info_1);
        break;
    case 11:
        fprintf(stderr, "> %u timbre request controller pairs in file \"%s\"",
                MAX_TIMB, (const char *)info_1);
        break;
    case 12:
        fprintf(stderr, "Duplicate branch point controller in ");
        fprintf(stderr, "file \"%s\"", (const char *)info_1);
        break;
    }

    fprintf(stderr, "\n");
    my_fcloseall();
    if(strlen(tmp_fn)) unlink(tmp_fn);
    if(strlen(out_fn)) unlink(out_fn);
    exit(err);
}

/***************************************************************/
void locate(int x, int y)
{
#ifdef __linux__
    printf("\033[%d;%dH", y, x);
#else
    inregs.h.ah = 0x0f;
    int86(0x10, &inregs, &outregs);

    inregs.h.ah = 0x02;
    inregs.h.dh = y;
    inregs.h.dl = x;
    inregs.h.bh = outregs.h.bh;
    int86(0x10, &inregs, &outregs);
#endif
}

int curpos_x(void)
{
#ifdef __linux__
    int x = 0, y = 0;
    get_pos(&y, &x);
    return x;
#else
    inregs.h.ah = 0x0f;
    int86(0x10, &inregs, &outregs);

    inregs.h.bh = outregs.h.bh;
    inregs.h.ah = 0x03;
    int86(0x10, &inregs, &outregs);

    return outregs.h.dl;
#endif
}

int curpos_y(void)
{
#ifdef __linux__
    int x = 0, y = 0;
    get_pos(&y, &x);
    return y;
#else
    inregs.h.ah = 0x0f;
    int86(0x10, &inregs, &outregs);

    inregs.h.bh = outregs.h.bh;
    inregs.h.ah = 0x03;
    int86(0x10, &inregs, &outregs);

    return outregs.h.dh;
#endif
}

/***************************************************************/
FILE *IFF_create_file(char *filename)
{
    static FILE *out;

    out = my_fopen(filename, "w+b");

    return out;
}

/***************************************************************/
int IFF_append_CAT(FILE *out, char *CAT_type)
{
    static uint32_t len = 0L;

    fputs("CAT ", out);
    fwrite(&len, 4, 1, out);
    fputs(CAT_type, out);
    fflush(out);

    return (!ferror(out));
}

/***************************************************************/
void IFF_close_CAT(FILE *out, uint32_t len_off)
{
    static uint32_t len;

    fflush(out);
    len = wswap(ftell(out) - len_off + 4L);
    fseek(out, len_off - 8L, SEEK_SET);
    fwrite(&len, 4, 1, out);
}

/***************************************************************/
void IFF_close_file(FILE *out)
{
    my_fclose(out);
}

/***************************************************************/
uint16_t IFF_construct(IFF_block *BLK)
{
    BLK->ptr = norm(BLK->base);
    BLK->len = 0L;

    return 0L;
}

/***************************************************************/
int IFF_write_block(FILE *out, IFF_block *BLK)
{
    uint32_t len, blen;
    uint8_t far *ptr;

    ptr = BLK->base;
    len = BLK->len;

    blen = wswap(len + (len & 1L));
    fputs((const char *)BLK->name, out);
    fwrite(&blen, 4, 1, out);

    blen = len;
    while(len--)
    {
        fputc(*ptr, out);
        ptr = add_ptr(ptr, 1L);
    }

    if(blen & 1L) fputc(0, out);

    return (!ferror(out));
}

/***************************************************************/
int IFF_append_FORM(FILE *out, char *FORM_type, FILE *in)
{
    static char buff[512];
    uint32_t len, blen;

    fseek(in, 0L, SEEK_SET);
    fflush(in);

    len = filelength(fileno(in));

    blen = wswap(len + (len & 1L) + 4L);

    fputs("FORM", out);
    fwrite(&blen, 4, 1, out);

    fputs(FORM_type, out);

    blen = len;
    while(len > 512L)
    {
        fread(buff, 512, 1, in);
        fwrite(buff, 512, 1, out);
        len -= 512L;
    }
    fread(buff, len, 1, in);
    fwrite(buff, len, 1, out);

    if(blen & 1L) fputc(0, out);

    return (!ferror(out));
}

/***************************************************************/
void IFF_put_byte(uint16_t val, IFF_block *BLK)
{
    *BLK->ptr = (uint8_t) val;
    BLK->ptr = add_ptr(BLK->ptr, 1L);
    BLK->len++;
}

/***************************************************************/
void IFF_put_vln(uint32_t val, IFF_block *BLK)
{
    uint16_t i, n/*, cnt*/;
    uint8_t bytefield[4];

    bytefield[3] = (val & 0x7fL);
    bytefield[2] = ((val >> 7) & 0x7fL) | 0x80;
    bytefield[1] = ((val >> 14) & 0x7fL) | 0x80;
    bytefield[0] = ((val >> 21) & 0x7fL) | 0x80;

    n = 3;
    for(i = 0; i <= 3; i++)
        if(bytefield[i] & 0x7f)
        {
            n = i;
            break;
        }

    for(i = n; i <= 3; i++)
    {
        *BLK->ptr = bytefield[i];
        BLK->ptr = add_ptr(BLK->ptr, 1L);
        BLK->len++;
    }
}

/***************************************************************/
uint16_t MIDI_get_chr(MIDI *MIDI, uint16_t trk)
{
    uint16_t val;
    uint8_t far *ptr = MIDI->trk_ptr[trk];

    val = (uint16_t) * ptr;
    ptr = add_ptr(ptr, 1L);

    MIDI->trk_ptr[trk] = ptr;

    return val;
}

uint16_t MIDI_next_chr(MIDI *MIDI, uint16_t trk)
{
    uint16_t val;

    val = (uint16_t) * MIDI->trk_ptr[trk];

    return val;
}

uint32_t MIDI_get_vln(MIDI *MIDI, uint16_t trk)
{
    uint32_t val = 0L;
    uint16_t i, cnt = 4;

    do
    {
        i = MIDI_get_chr(MIDI, trk);
        val = (val << 7) | (uint32_t)(i & 0x7f);
        if(!(i & 0x80))
            cnt = 0;
        else
            --cnt;
    }
    while(cnt);

    return val;
}

uint16_t MIDI_vln_size(uint32_t val)
{
    uint16_t cnt = 0;

    do
    {
        cnt++;
        val >>= 7;
    }
    while(val);

    return cnt;
}

uint16_t MIDI_put_vln(uint32_t val, uint8_t far *ptr)
{
    uint16_t i, n, cnt;
    uint8_t bytefield[4];

    bytefield[3] = (val & 0x7fL);
    bytefield[2] = ((val >> 7) & 0x7fL) | 0x80;
    bytefield[1] = ((val >> 14) & 0x7fL) | 0x80;
    bytefield[0] = ((val >> 21) & 0x7fL) | 0x80;

    n = 3;
    for(i = 0; i <= 3; i++)
        if(bytefield[i] & 0x7f)
        {
            n = i;
            break;
        }

    cnt = 0;
    for(i = n; i <= 3; i++)
    {
        *ptr = bytefield[i];
        ptr = add_ptr(ptr, 1L);
        ++cnt;
    }

    return cnt;
}

uint32_t MIDI_get_32(MIDI *MIDI, uint16_t trk)
{
    uint32_t val;

    val = (uint32_t) MIDI_get_chr(MIDI, trk);
    val = (val << 8) | (uint32_t) MIDI_get_chr(MIDI, trk);
    val = (val << 8) | (uint32_t) MIDI_get_chr(MIDI, trk);
    val = (val << 8) | (uint32_t) MIDI_get_chr(MIDI, trk);

    return val;
}

uint32_t MIDI_get_24(MIDI *MIDI, uint16_t trk)
{
    uint32_t val;

    val = (uint32_t) MIDI_get_chr(MIDI, trk);
    val = (val << 8) | (uint32_t) MIDI_get_chr(MIDI, trk);
    val = (val << 8) | (uint32_t) MIDI_get_chr(MIDI, trk);

    return val;
}

uint16_t MIDI_get_16(MIDI *MIDI, uint16_t trk)
{
    uint16_t val;

    val = MIDI_get_chr(MIDI, trk);
    val = (val << 8) | MIDI_get_chr(MIDI, trk);

    return val;
}

/***************************************************************/
uint16_t MIDI_construct(MIDI far *MIDI)
{
    uint32_t chunk_len;
    uint16_t trk, bad;
    uint8_t far *src;
    uint32_t len;

    src = norm(MIDI->base);
    len = MIDI->seq_len;
    bad = 1;
    while(len-- >= 4L)
    {
        if(!strnicmp((char *)src, "MThd", 4))
        {
            bad = 0;
            break;
        }
        src = add_ptr(src, 1L);
    };
    if(bad) return 2;

    chunk_len = wswap(*(uint32_t far *)(src + 4));

    MIDI->ntrks = bswap(*(uint16_t far *)(src + 10));
    if(MIDI->ntrks > MAX_TRKS) return 6;

    MIDI->format = bswap(*(uint16_t far *)(src + 8));
    MIDI->division = bswap(*(uint16_t far *)(src + 12));

    MIDI->tick_time = (50000000L) / (uint32_t) MIDI->division;

    MIDI->event_time = 0L;
    MIDI->event_trk = MIDI->ntrks - 1;

    src = add_ptr(src, chunk_len + 8L);

    trk = 0;
    do
    {
        chunk_len = wswap(*(uint32_t far *)(src + 4));
        if(!strnicmp((char *)src, "MTrk", 4))
        {
            MIDI->trk_ptr[trk] = add_ptr(src, 8L);
            MIDI->status[trk] = 0;
            MIDI->trk_active[trk] = 1;
            MIDI->pending_delta[trk] = MIDI_get_vln(MIDI, trk);
            trk++;
        }
        src = add_ptr(src, chunk_len + 8L);
    }
    while(trk < MIDI->ntrks);

    return 0;
}

void MIDI_destroy(MIDI far *MIDI)
{
    farfree(MIDI->base);
}

/***************************************************************/
uint16_t MIDI_event_type(MIDI far *MIDI)
{
    switch(MIDI->event[0] & 0xf0)
    {
    case EV_NOTE_OFF:
        return EV_NOTE_OFF;
    case EV_NOTE_ON:
        return (MIDI->event[2]) ? EV_NOTE_ON : EV_NOTE_OFF;
    case EV_POLY_PRESS:
        return EV_POLY_PRESS;
    case EV_CONTROL:
        return EV_CONTROL;
    case EV_PROGRAM:
        return EV_PROGRAM;
    case EV_CHAN_PRESS:
        return EV_CHAN_PRESS;
    case EV_PITCH:
        return EV_PITCH;
    case EV_SYSEX:
        switch(MIDI->event[0])
        {
        case EV_SYSEX:
            return EV_SYSEX;
        case EV_ESC:
            return EV_SYSEX;
        case EV_META:
            return EV_META;
        }
    default:
        return EV_INVALID;
    }
}

/***************************************************************/
uint16_t MIDI_get_event(MIDI far *MIDI, int trk)
{
    int type;
    uint32_t cnt, len;
    uint8_t far *temp;

    if(MIDI_next_chr(MIDI, trk) >= 0x80)
        MIDI->status[trk] = MIDI_get_chr(MIDI, trk);

    if(MIDI->status[trk] < 0x80)
        return 5;

    MIDI->event_len = 0;
    MIDI->event[MIDI->event_len++] = MIDI->status[trk];

    switch(MIDI->status[trk] & 0xf0)
    {
    case EV_NOTE_OFF:
    case EV_NOTE_ON:
    case EV_POLY_PRESS:
    case EV_CONTROL:
    case EV_PITCH:
        MIDI->event[MIDI->event_len++] = MIDI_get_chr(MIDI, trk); /*fallthrough*/
    case EV_PROGRAM:
    case EV_CHAN_PRESS:
        MIDI->event[MIDI->event_len++] = MIDI_get_chr(MIDI, trk);
        break;

    case EV_SYSEX:
        switch(MIDI->status[trk])
        {
        case EV_META:
            MIDI->event[MIDI->event_len++] = type = MIDI_get_chr(MIDI, trk);
            switch(type)
            {
            case META_EOT:
                MIDI->trk_active[trk] = 0;
                break;
            case META_TEMPO:
                temp = MIDI->trk_ptr[trk];
                MIDI_get_vln(MIDI, trk);
                MIDI->tick_time = (100L * MIDI_get_24(MIDI, trk)) /
                                  (uint32_t) MIDI->division;
                MIDI->trk_ptr[trk] = temp;
                break;
            }
        case EV_SYSEX:
        case EV_ESC:
            len = MIDI_get_vln(MIDI, trk);
            MIDI->event_len += MIDI_put_vln(len,
                                            &(MIDI->event[MIDI->event_len]));

            for(cnt = 0L; cnt < len; cnt++)
            {
                if(MIDI->event_len >= MAX_EVLEN)
                    return 3;
                MIDI->event[MIDI->event_len++] = MIDI_get_chr(MIDI, trk);
            }
            break;

        default:
            return 4;
        }
    }

    MIDI->pending_delta[trk] = MIDI_get_vln(MIDI, trk);
    MIDI->event_chan = MIDI->event[0] & 0x0f;
    return 0;
}

/***************************************************************/
uint16_t MIDI_get_next_event(MIDI far *MIDI)
{
    uint32_t event_delta, min_delta;
    int16_t trk, new_trk, trk_cnt;

    new_trk = -1;
    trk = MIDI->event_trk;
    trk_cnt = MIDI->ntrks;
    min_delta = MAX_ULONG;

    do
    {
        if(MIDI->trk_active[trk])
        {
            event_delta = MIDI->pending_delta[trk];
            if(event_delta <= min_delta)
            {
                min_delta = event_delta;
                new_trk = trk;
            }
        }
        if(trk-- == 0)
            trk = MIDI->ntrks - 1;
    }
    while(--trk_cnt);

    if(new_trk == -1) return 0;

    MIDI->event_trk = new_trk;
    MIDI->event_time = min_delta;

    for(trk = 0; trk < MIDI->ntrks; trk++)
        if(MIDI->trk_active[trk])
            MIDI->pending_delta[trk] -= min_delta;

    abend(MIDI_get_event(MIDI, new_trk), (intptr_t) MIDI->seq_fn, 0L);

    return 1;
}

/***************************************************************/
uint16_t XMIDI_construct(XMIDI *XMIDI)
{
    uint16_t i;

    XMIDI->RBRN.cnt = 0;
    XMIDI->TIMB.cnt = 0;

    XMIDI->q_int = (100000000L / (uint32_t) XMIDI->AILH.quantization);
    XMIDI->DDA_sum = 0L;
    XMIDI->interval = 0L;
    XMIDI->delta = 0L;

    for(i = 0; i < MAX_NOTES; i++)
        XMIDI->note_chan[i] = 255;

    for(i = 0; i < CHAN_CNT; i++)
    {
        XMIDI->rbs_BNK[i] = 0;
        XMIDI->timb_BNK[i] = 0;
    }

    XMIDI->rbs_BNK[9] = 127;

    XMIDI->EVNT.base = farmalloc(XMIDI->EVNT.avail);
    IFF_construct(&XMIDI->EVNT);

    return 0;
}

void XMIDI_destroy(XMIDI *XMIDI)
{
    farfree(XMIDI->EVNT.base);
}

/***************************************************************/
void XMIDI_accum_interval(XMIDI *XMIDI, MIDI *MIDI)
{
    XMIDI->DDA_sum += (MIDI->event_time * MIDI->tick_time);
    while(XMIDI->DDA_sum >= XMIDI->q_int)
    {
        XMIDI->DDA_sum -= XMIDI->q_int;
        XMIDI->interval++;
        XMIDI->delta++;
    }
}

/***************************************************************/
void XMIDI_write_interval(XMIDI *XMIDI)
{
    while(XMIDI->delta > 127L)
    {
        IFF_put_byte(127, &XMIDI->EVNT);
        XMIDI->delta -= 127L;
    }

    if(XMIDI->delta)
        IFF_put_byte((uint16_t) XMIDI->delta, &XMIDI->EVNT);

    XMIDI->delta = 0L;
}

/***************************************************************/
void XMIDI_put_MIDI_event(XMIDI *XMIDI, MIDI *MIDI)
{
    uint16_t i;

    if(XMIDI->delta) XMIDI_write_interval(XMIDI);

    for(i = 0; i < MIDI->event_len; i++)
        IFF_put_byte(MIDI->event[i], &XMIDI->EVNT);
}

/***************************************************************/
uint16_t XMIDI_log_branch(XMIDI *XMIDI, MIDI *MIDI)
{
    uint16_t i, b = XMIDI->RBRN.cnt;

    if(b >= MAX_RBRN) return 10;

    for(i = 0; i < b; i++)
        if(XMIDI->RBRN.brn[i].bnum == MIDI->event[2]) return 12;

    XMIDI->RBRN.brn[b].offset = ptr_dif(XMIDI->EVNT.ptr, XMIDI->EVNT.base);
    XMIDI->RBRN.brn[b].bnum = MIDI->event[2];

    XMIDI->RBRN.cnt++;
    return 0;
}

/***************************************************************/
uint16_t XMIDI_log_timbre_request(XMIDI *XMIDI, MIDI *MIDI)
{
    uint16_t i, ch, val, t = XMIDI->TIMB.cnt;

    ch = MIDI->event_chan;

    if(t >= MAX_TIMB) return 11;

    switch(MIDI_event_type(MIDI))
    {
    case EV_NOTE_ON:
        val = XMIDI->rbs_BNK[ch];
        for(i = 0; i < t; i++)
            if((XMIDI->TIMB.tbr[i].t_bank == val) &&
               (XMIDI->TIMB.tbr[i].t_num == MIDI->event[1])) break;
        if(i == t)
        {
            XMIDI->TIMB.tbr[t].t_bank = val;
            XMIDI->TIMB.tbr[t].t_num = MIDI->event[1];
            XMIDI->TIMB.cnt++;
        }
        break;
    case EV_CONTROL:
        switch(MIDI->event[1])
        {
        case AIL_TIMB_BNK:
            XMIDI->timb_BNK[ch] = MIDI->event[2];
            break;
        }
        break;
    case EV_PROGRAM:
        for(i = 0; i < t; i++)
            if((XMIDI->TIMB.tbr[i].t_bank == XMIDI->timb_BNK[ch]) &&
               (XMIDI->TIMB.tbr[i].t_num == MIDI->event[1])) break;
        if(i == t)
        {
            XMIDI->TIMB.tbr[t].t_bank = XMIDI->timb_BNK[ch];
            XMIDI->TIMB.tbr[t].t_num = MIDI->event[1];
            XMIDI->TIMB.cnt++;
        }
        break;

    }

    return 0;
}

/***************************************************************/
void XMIDI_note_off(XMIDI *XMIDI, MIDI *MIDI)
{
    uint16_t i, j;
    uint32_t duration, offset, len;
    uint8_t far *src, far *dest;
    uint16_t channel, note;

    channel = MIDI->event_chan;
    note = MIDI->event[1];

    for(i = 0; i < MAX_NOTES; i++)
    {
        if((XMIDI->note_chan[i] == channel) && (XMIDI->note_num[i] == note))
        {
            XMIDI->note_chan[i] = 255;

            duration = XMIDI->interval - XMIDI->note_intvl[i];
            offset = (uint32_t) MIDI_vln_size(duration) - 1L;

            if(offset)
            {
                dest = add_ptr(XMIDI->note_next[i], offset);
                src = XMIDI->note_next[i];
                len = ptr_dif(XMIDI->EVNT.ptr, XMIDI->note_next[i]);
                far_memmove(dest, src, len);
                XMIDI->EVNT.ptr = add_ptr(XMIDI->EVNT.ptr, offset);
                XMIDI->EVNT.len += offset;

                for(j = 0; j < MAX_NOTES; j++)
                    if(XMIDI->note_chan[j] != 255)
                        if(ptr_dif(XMIDI->note_next[j], src) >= 0L)
                            XMIDI->note_next[j] =
                                add_ptr(XMIDI->note_next[j], offset);

                for(j = 0; j < XMIDI->RBRN.cnt; j++)
                    if(ptr_dif(add_ptr(XMIDI->EVNT.base,
                                       XMIDI->RBRN.brn[j].offset), src) >= 0L)
                        XMIDI->RBRN.brn[j].offset += offset;
            }

            MIDI_put_vln(duration, add_ptr(XMIDI->note_next[i], -1L));
        }
    }
}

/***************************************************************/
uint16_t XMIDI_note_on(XMIDI *XMIDI, MIDI *MIDI)
{
    uint16_t i;

    XMIDI_put_MIDI_event(XMIDI, MIDI);
    IFF_put_byte(0x00, &XMIDI->EVNT);

    for(i = 0; i < MAX_NOTES; i++)
    {
        if(XMIDI->note_chan[i] == 255)
        {
            XMIDI->note_chan[i] = MIDI->event_chan;
            XMIDI->note_num[i] = MIDI->event[1];
            XMIDI->note_intvl[i] = XMIDI->interval;
            XMIDI->note_next[i] = XMIDI->EVNT.ptr;
            break;
        }
    }

    return (i == MAX_NOTES);
}

/***************************************************************/
uint16_t XMIDI_verify_notes(XMIDI *XMIDI)
{
    uint16_t i;

    for(i = 0; i < MAX_NOTES; i++)
        if(XMIDI->note_chan[i] != 255)
            return 8;

    return 0;
}

/***************************************************************/
uint16_t XMIDI_IFF_write_blocks(XMIDI *XMIDI, FILE *out)
{
    static IFF_block far IFF_TIMB;
    static IFF_block far IFF_RBRN;

    IFF_TIMB.name = (const uint8_t *)"TIMB";
    IFF_TIMB.base = (uint8_t far *) &XMIDI->TIMB;
    IFF_TIMB.len = sizeof(TIMB_block) -
                   ((MAX_TIMB - XMIDI->TIMB.cnt) * sizeof(TIMB_entry));

    IFF_RBRN.name = (const uint8_t *)"RBRN";
    IFF_RBRN.base = (uint8_t far *) &XMIDI->RBRN;
    IFF_RBRN.len = sizeof(RBRN_block) -
                   ((MAX_RBRN - XMIDI->RBRN.cnt) * sizeof(RBRN_entry));

    if(XMIDI->TIMB.cnt)
        if(!IFF_write_block(out, &IFF_TIMB)) return 1;

    if(XMIDI->RBRN.cnt)
        if(!IFF_write_block(out, &IFF_RBRN)) return 1;

    if(!IFF_write_block(out, &XMIDI->EVNT)) return 1;

    return 0;
}

/***************************************************************/
void XMIDI_compile(char *src_fn, FILE *out, uint16_t quant)
{
    static MIDI far MIDI;
    static XMIDI far XMIDI;

    MIDI.seq_fn = src_fn;
    MIDI.seq_len = file_size(MIDI.seq_fn);
    MIDI.base = read_file(MIDI.seq_fn, NULL);
    if(MIDI.base == NULL)
        abend(1, get_disk_error(), (intptr_t) MIDI.seq_fn);
    abend(MIDI_construct(&MIDI), (intptr_t) MIDI.seq_fn, 0L);

#ifdef __linux__
    XMIDI.EVNT.avail = 999999;
#else
    XMIDI.EVNT.avail = farcoreleft() - 16384L;
#endif
    XMIDI.EVNT.name = (const uint8_t *)"EVNT";
    XMIDI.AILH.quantization = quant;
    XMIDI_construct(&XMIDI);

    while(MIDI_get_next_event(&MIDI))
    {
        if((XMIDI.EVNT.avail - XMIDI.EVNT.len) < (MAX_EVLEN + 16))
            abend(7, (intptr_t) MIDI.seq_fn, 0L);

        XMIDI_accum_interval(&XMIDI, &MIDI);

        switch(MIDI_event_type(&MIDI))
        {
        case EV_NOTE_ON:
            if(XMIDI.rbs_BNK[MIDI.event_chan])
                abend(XMIDI_log_timbre_request(&XMIDI, &MIDI),
                      (intptr_t)MIDI.seq_fn, 0L);
            if(XMIDI_note_on(&XMIDI, &MIDI))
                abend(9, (intptr_t) MIDI.seq_fn, 0L);
            break;

        case EV_NOTE_OFF:
            XMIDI_note_off(&XMIDI, &MIDI);
            break;

        case EV_CONTROL:
            switch(MIDI.event[1])
            {
            case AIL_BRANCH_PT:
                abend(XMIDI_log_branch(&XMIDI, &MIDI),
                      (intptr_t) MIDI.seq_fn, 0L);
                break;
            case AIL_TIMB_BNK:
                abend(XMIDI_log_timbre_request(&XMIDI, &MIDI),
                      (intptr_t) MIDI.seq_fn, 0L);
                break;
            }
            XMIDI_put_MIDI_event(&XMIDI, &MIDI);
            break;

        case EV_PROGRAM:
            abend(XMIDI_log_timbre_request(&XMIDI, &MIDI),
                  (intptr_t) MIDI.seq_fn, 0L);
            XMIDI_put_MIDI_event(&XMIDI, &MIDI);
            break;

        case EV_META:
            switch(MIDI.event[1])
            {
            case META_EOT:
            case META_TRK_NAME:
            case META_INS_NAME:
                break;
            default:
                XMIDI_put_MIDI_event(&XMIDI, &MIDI);
            }
            break;

        case EV_INVALID:
            break;

        default:
            XMIDI_put_MIDI_event(&XMIDI, &MIDI);
        }
    }

    XMIDI_write_interval(&XMIDI);
    IFF_put_byte(EV_META, &XMIDI.EVNT);
    IFF_put_byte(META_EOT, &XMIDI.EVNT);
    IFF_put_byte(0x00, &XMIDI.EVNT);

    abend(XMIDI_verify_notes(&XMIDI), (intptr_t) MIDI.seq_fn, 0L);

    abend(XMIDI_IFF_write_blocks(&XMIDI, out), 6L, 0L);

    XMIDI_destroy(&XMIDI);
    MIDI_destroy(&MIDI);
}

/***************************************************************/
int main(int argc, char *argv[])
{
    int bad, strcnt;
    int infile;
    int i, done, seq_cnt;
    int x /*, y*/;
    uint16_t quant;
    static char seq_fn[128];
    static char buff[5];
    FILE *tmp, *XMID;
    uint32_t info_len;
    uint32_t catlen_off, seqcnt_off;

    printf("\nMIDIFORM version %s               Copyright (C) 1991, 1992 Miles Design, Inc.\n", VERSION);
    printf("-------------------------------------------------------------------------------\n\n");

    strcpy(out_fn, "");
    bad = strcnt = infile = 0;
    quant = DEFAULT_QUAN;

    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] != '/')
        {
            ++strcnt;
            if(strcnt == 1)
                strcpy(out_fn, argv[i]);
            if(strcnt == 2)
                infile = i;
        }
        else if(!strnicmp(argv[i], "/Q:", 3))
        {
            if(!infile)
                quant = (int16_t) strtol((char far *)&argv[i][3], NULL, 10);
        }
        else
        {
            bad = 1;
            break;
        }
    }

    if(bad || (!strcnt))
    {
        printf("This program converts Standard MIDI Format 0 or Format 1 sequence files to\n");
        printf("the XMIDI format used by the Audio Interface Library Version 2.X drivers.\n\n");

        printf("Usage: MIDIFORM [/Q:nn] output_filename [[input_filename...] | [< rspfile]]\n\n");

        printf("where /Q:nn = quantization rate in hertz (default=%u Hz.)\n", DEFAULT_QUAN);
        printf("    rspfile = list of newline-separated MIDI sequence files for input\n");
        return 1;
    }

    if(file_size(out_fn) > 12L)
    {
        tmp = my_fopen(out_fn, "rb");
        fread(buff, 4, 1, tmp);
        buff[4] = 0;
        my_fclose(tmp);
        if(strcmp(buff, "FORM") && (strcmp(buff, "CAT ")))
        {
            printf("WARNING: Non-IFF file \"%s\" will be overwritten.\n",
                   out_fn);
            printf("Continue? (Y/N) \a");
            if(toupper(my_getche()) == 'Y')
                printf("\n\n");
            else
            {
                printf("\n\nAborted\n");
                return 1;
            }
        }
    }

    if(infile)
        printf("Converting, please wait ....\n\n");

    XMID = IFF_create_file(out_fn);
    if(XMID == NULL) abend(1, 4L, (intptr_t) out_fn);

    seq_cnt = -1;

    tmpReset(tmp_fn);
#ifdef __linux__
    mkstemp(tmp_fn);
#else
    mktemp(tmp_fn);
#endif
    tmp = my_fopen(tmp_fn, "w+b");
    if(tmp == NULL) abend(1, 4L, (intptr_t) tmp_fn);
    fputs("INFO", tmp);
    info_len = wswap(2L);
    fwrite(&info_len, 4L, 1, tmp);
    fwrite(&seq_cnt, 2L, 1, tmp);
    fseek(tmp, 0L, SEEK_SET);
    IFF_append_FORM(XMID, "XDIR", tmp);
    fflush(XMID);
    seqcnt_off = ftell(XMID);
    my_fclose(tmp);
    unlink(tmp_fn);

    IFF_append_CAT(XMID, "XMID");
    catlen_off = ftell(XMID);

    done = seq_cnt = 0;
    while(!done)
    {
        strcpy(seq_fn, "");
        if(infile)
            if(infile >= argc)
                done = 1;
            else
                strcpy(seq_fn, argv[infile++]);
        else
        {
            locate(0, curpos_y());
            printf("Sequence filename (Enter to end): ");
            x = curpos_x();
            if(my_gets(seq_fn, 128) == NULL) done = 1;

            for(i = 0; i < (int)strlen(seq_fn); i++)
                if(seq_fn[i] == ';') seq_fn[i] = 0;

            if(!strlen(seq_fn)) done = 1;

            if(curpos_x() == x) printf("%s\n", seq_fn);
            if(done)
            {
                locate(x, curpos_y() - 1);
                printf("Done\n");
            }
        }

        if(done) continue;

        if(!strnicmp(seq_fn, "/Q:", 3))
        {
            quant = (int) strtol((char far *)&seq_fn[3], NULL, 10);
            continue;
        }

        tmpReset(tmp_fn);
#ifdef __linux__
        mkstemp(tmp_fn);
#else
        mktemp(tmp_fn);
#endif
        tmp = my_fopen(tmp_fn, "w+b");
        if(tmp == NULL) abend(1, 4L, (intptr_t)tmp_fn);

        XMIDI_compile(seq_fn, tmp, quant);
        fseek(tmp, 0L, SEEK_SET);
        IFF_append_FORM(XMID, "XMID", tmp);

        my_fclose(tmp);
        unlink(tmp_fn);
        ++seq_cnt;
    }

    IFF_close_CAT(XMID, catlen_off);

    fseek(XMID, seqcnt_off - 2L, SEEK_SET);
    fwrite(&seq_cnt, 2L, 1, XMID);

    IFF_close_file(XMID);
    if(!seq_cnt) unlink(out_fn);

    printf("%u sequence(s) converted.\n", seq_cnt);
    return 0;
}
