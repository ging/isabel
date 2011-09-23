/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: pngDec.cc 10574 2007-07-12 16:08:15Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/errno.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <png.h>

#include "pngImpl.hh"

/*** local variables ***/

static double DISPLAY_GAMMA= 2.20; /* Default display gamme */

/*** local functions ***/

static void
png_my_error(png_struct *png_ptr, const char *message)
{
    NOTIFY("libpng error: %s\n", message);

    longjmp(png_ptr->jmpbuf, 1);
}

static void
png_my_warning(png_struct *png_ptr, const char *message)
{
    if ( ! png_ptr)
        return;

    NOTIFY("libpng warning: %s\n", message);
    longjmp(png_ptr->jmpbuf, 1);
}

image_t*
pngLoadFile(FILE *f)
{
    png_struct *png_ptr;
    png_info   *info_ptr;
    png_color_16 my_background;
    int i;
    int linesize;
    int pass;
    size_t commentsize;
    char fullInfo[1024];   // image info
    char shrtInfo[1024];   // image info

    u8 header[8];

    u8 *imageData = (u8*) NULL;
    char *comment = (char *) NULL;

    fseek(f, 0L, SEEK_SET); // paranoid, not needed

    if (fread(header, 1, 8, f) != 8)
    {
        NOTIFY("pngLoadFile: could not read header\n");
        return NULL;
    }

    if (png_sig_cmp(header, 0, 8))
    {
        NOTIFY("pngLoadFile: not a PNG file\n");
        return NULL;
    }


    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                     png_my_error, png_my_warning);
    if ( ! png_ptr)
    {
        NOTIFY("pngLoadFile: malloc failure");
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);

    if ( ! info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        NOTIFY("pngLoadFile: malloc failure");
        return NULL;
    }

    if (setjmp(png_ptr->jmpbuf))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        if (imageData)
        {
            free(imageData);
        }
        if (comment)
        {
            free(comment);
        }
        return NULL;
    }

    png_init_io(png_ptr, f);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    sprintf(fullInfo, "PNG, %d bit ", info_ptr->bit_depth * info_ptr->channels);

    switch (info_ptr->color_type)
    {
    case PNG_COLOR_TYPE_PALETTE:
        strcat(fullInfo, "palette color");
        break;

    case PNG_COLOR_TYPE_GRAY:
        strcat(fullInfo, "grayscale");
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
        strcat(fullInfo, "grayscale+alpha");
        break;

    case PNG_COLOR_TYPE_RGB:
        strcat(fullInfo, "truecolor");
        break;

    case PNG_COLOR_TYPE_RGB_ALPHA:
        strcat(fullInfo, "truecolor+alpha");
        break;
    }

    sprintf(fullInfo + strlen(fullInfo),
            ", %sinterlaced.",
            info_ptr->interlace_type ? "" : "non-"
           );

    sprintf(shrtInfo, "%lux%lu PNG", info_ptr->width, info_ptr->height);

    if (info_ptr->bit_depth < 8)
        png_set_packing(png_ptr);

    if (info_ptr->valid & PNG_INFO_gAMA)
        png_set_gamma(png_ptr, DISPLAY_GAMMA, info_ptr->gamma);
    else
        png_set_gamma(png_ptr, DISPLAY_GAMMA, 0.45);

    if (info_ptr->valid & PNG_INFO_bKGD)
    {
        png_set_background(png_ptr, &info_ptr->background,
                           PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    }
    else
    {
        my_background.red = my_background.green = my_background.blue =
          my_background.gray = 0;
        png_set_background(png_ptr, &my_background, PNG_BACKGROUND_GAMMA_SCREEN,
                           0, DISPLAY_GAMMA);
    }

    if (info_ptr->bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY ||
        info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA
       )
    {
        png_set_expand(png_ptr);
    }

    pass=png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    if (info_ptr->color_type == PNG_COLOR_TYPE_RGB ||
        info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA
       )
    {
        linesize = info_ptr->width * 3;
    }
    else
    {
        linesize = info_ptr->width;
    }
    imageData= (u8*)malloc(linesize*info_ptr->height);

    if ( ! imageData)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        NOTIFY("pngLoadFile: can't allocate space for PNG image");
        return NULL;
    }

    png_start_read_image(png_ptr);

    for (i = 0; i < pass; i++)
    {
        u8 *p= imageData;
        for (unsigned j= 0; j < info_ptr->height; j++)
        {
            png_read_row(png_ptr, p, NULL);
            p += linesize;
        }
    }

    png_read_end(png_ptr, info_ptr);

    if (info_ptr->num_text > 0)
    {
        commentsize = 1;

        for(i = 0; i < info_ptr->num_text; i++)
            commentsize += strlen(info_ptr->text[i].key) + 1 +
                           info_ptr->text[i].text_length + 2;

        if ((comment = (char*)malloc(commentsize)) == NULL)
        {
            png_warning(png_ptr,"can't allocate comment string");
        }
        else
        {
            comment[0] = '\0';
            for (i = 0; i < info_ptr->num_text; i++)
            {
                strcat(comment, info_ptr->text[i].key);
                strcat(comment, "::");
                strcat(comment, info_ptr->text[i].text);
                strcat(comment, "\n");
            }
        }
    }

    int npixels= info_ptr->width*info_ptr->height;
    int pixel;
    u8  *retVal= (u8*)malloc(npixels*3);

    if ( ! retVal)
    {
        NOTIFY("pngDec_t::decodeToRGB24: can't allocate space for PNG image");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return NULL;
    }

    if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY ||
        info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA
       )
    {
        for (int i= 0; i < npixels; i++)
        {
            retVal[3*i+0]= imageData[i];
            retVal[3*i+1]= imageData[i];
            retVal[3*i+2]= imageData[i];
        }
    }
    else if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
    {
        for (int i=0; i < npixels; i++)
        {
            pixel= imageData[i];
            retVal[3*i+0]= info_ptr->palette[pixel].red;
            retVal[3*i+1]= info_ptr->palette[pixel].green;
            retVal[3*i+2]= info_ptr->palette[pixel].blue;
        }
    }
    else if (info_ptr->color_type == PNG_COLOR_TYPE_RGB ||
             info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA
            )
    {
        memcpy (retVal, imageData, 3*npixels);
    }

    image_t *img= new image_t(retVal,
                              npixels * 3,
                              RGB24_FORMAT,
                              info_ptr->width,
                              info_ptr->height,
                              0
                             );

    free(retVal);

    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

    return img;
}

