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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#include "imageBuf.hh"

imageBuf_t::imageBuf_t(void)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&notEmpty, NULL);
    pthread_cond_init(&notFull, NULL);
}

imageBuf_t::~imageBuf_t(void)
{
    image_t *img;

    pthread_mutex_lock(&mutex);

    for (int i= 0; i < imgQ.size(); i++)
    {
        img= imgQ.elementAt(0);
        imgQ.remove(0);

        delete img;
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&notEmpty);
    pthread_cond_destroy(&notFull);
}

void
imageBuf_t::put(image_t *newImg)
{
fprintf(stderr, "imageBuf_t::put\n");
    pthread_mutex_lock(&mutex);

    while (imgQ.size() > MAX_ELEMENTS)
    {
        pthread_cond_wait(&notFull, &mutex);
    }
     
    imgQ.add(newImg);

    pthread_cond_signal(&notEmpty);

    pthread_mutex_unlock(&mutex);
}

image_t *
imageBuf_t::get(void)
{
fprintf(stderr, "imageBuf_t::get\n");
    pthread_mutex_lock(&mutex);

    while (imgQ.size() == 0)
    {
        pthread_cond_wait(&notEmpty, &mutex);
    }

    image_t *img= imgQ.elementAt(0);

    imgQ.remove(0);

    pthread_cond_signal(&notFull);

    pthread_mutex_unlock(&mutex);

    return img;
}

