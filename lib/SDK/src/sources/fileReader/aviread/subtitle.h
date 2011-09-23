/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVIFILE_SUBTITLE_H
#define AVIFILE_SUBTITLE_H

/** Maximum supported number of lines which may compose one subtitle */
#define SUBTITLE_MAX_LINES 5

/**
 * subtitle is plain C library - usable outside of avifile
 * once I had a dream we all could use such library to parse
 * various subtitle file - I've been naive...
 */

/*
 * All subtitle lines are internally stored in UTF-8 format
 * (iconv has to be present for this)
 */

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Types of recognized subtitles
 * with a short example of such subtitle line
 */
typedef struct __subtitles_s subtitles_t;

typedef enum
{
    /** \internal */
    SUBTITLE_UNSELECTED,
    /**
     * .SUB
     * {123}{345} text
     */
    SUBTITLE_MICRODVD,
    /**
     * .SRT
     * 123
     * 1:23:55.125 --> 1:24:00.275
     */
    SUBTITLE_SUBRIP,
    /**
     * 1:23:55
     * text
     */
    SUBTITLE_VPLAYER,
    SUBTITLE_AQT,
    SUBTITLE_SAMI,
    /**
     * 1:23:55.125,1:24:00.275
     * text
     */
    SUBTITLE_SUBVIEWER,
    SUBTITLE_MPSUB,		/* say hello to Gabuciono */
    SUBTITLE_LAST
} subtitle_t;

typedef struct
{
    /** how many lines are in this subtitle */
    int lines;
    /** starting time for display  (time in 1/1000s = 1ms) */
    unsigned int start;
    /** ending time for display  (time in 1/1000s = 1ms) */
    unsigned int end;
    /** pointers to individual subtitle lines */
    char* line[SUBTITLE_MAX_LINES];
} subtitle_line_t;

/**
 * tries to open subtitle for the given filename
 * \returns  file descriptor
 */
int subtitle_filename(const char* filename, char** opened_filename);

/**
 * Opens subtiles from a given filename
 *
 * \param fd  file descriptor with opened file with subtitles
 * \param fps frame per seconds (needed for some subtitles types
 * \param codepage  when NULL default system codepage is used
 */
subtitles_t* subtitle_open(int fd, double fps, const char* codepage);
/**
 * Closes and destroys array of subtitles
 * \param subset  array of subtitles for freeing
 *
 * \warning after calling this function the subset array could no longer
 * be accessed as all the memory allocated by this structure has been
 * released
 */
void subtitle_close(subtitles_t* subs);
void subtitle_set_fps(subtitles_t*, double fps);
/** Returns number of lines in the subtitle array */
int subtitle_get_lines(subtitles_t*);
/** Returns type of subtitles */
subtitle_t subtitle_get_type(subtitles_t*, const char**);
/**
 * Copies subtitle into given subtitle_line_t pointer
 * It will check first if the line isn't already copied
 * returns true if subtitle is found
 */
int subtitle_get(subtitle_line_t*, subtitles_t*, double timepos);
/**
 * Check if two subtitles are equal
 * checking linecount & start,end pos (text is not being compared!)
 */
int subtitle_line_equals(const subtitle_line_t*, const subtitle_line_t*);
/** creates new empty subtitle line */
subtitle_line_t* subtitle_line_new(void);
/** duplicates one subtitle line */
subtitle_line_t* subtitle_line_copy(const subtitle_line_t*);
/** free allocated subtitle line (obtained with subtitle_line_copy) */
void subtitle_line_free(subtitle_line_t*);

/**
 * write subtitles to file
 */
int subtitle_write(const subtitles_t* st, const char* filename, subtitle_t type);
/*
 will be implemented later

 write
 update
 insert
 delete
 */

/*
 * private
 */
struct __subtitles_s
{
    /*
     * never accessed directly
     * use suplied methods to manipulate with subtitle array
     */
    int fd;
    subtitle_t type;
    int allocated;
    int count;
    int frame_based; /* if subtitles use frame number */
    int time_diff;
    double fps;
    char* encoding;  /* original encoding */
    char* out_encoding; /* for write */
    subtitle_line_t* subtitle;
};

#if defined(__cplusplus)
}
#endif

#endif
