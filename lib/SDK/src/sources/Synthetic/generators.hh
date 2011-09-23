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
// $Id: generators.hh 10820 2007-09-17 09:20:48Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <vUtils/image.h>

#define NUMBUFFS 2

class syntheticSrc_t
{
protected:
    u8 *__buffer[NUMBUFFS];

    unsigned int __maxBufSize;
    struct
    {
        unsigned   width;
        unsigned   height;
        unsigned   side;

        u32        fcc;

        int        grab_number;
    } __grabState;

    unsigned posX;
    unsigned posY;
    int signoX;
    int signoY;

protected:
    virtual void mallocBuffers(void)= 0;
    virtual void reallocBuffers(void)= 0;
    virtual void freeBuffers(void)= 0;

public:
    static unsigned minimun(unsigned x, unsigned y)
    {
        return x < y ? x : y;
    }

    virtual u32      getGrabFormat(void)= 0;
    virtual image_t *genImage(u32 timestamp)=0;

    syntheticSrc_t(void);

    virtual ~syntheticSrc_t(void);

    virtual bool setImageSize(unsigned width, unsigned height);
    virtual void getImageSize(unsigned *width, unsigned *height);
};

class syntheticRAW24Src_t: public virtual syntheticSrc_t
{
private:
    bool  __doBGR;

    // pass 'false' for RGB24, 'true' for BGR24
    syntheticRAW24Src_t(bool doBGR= false);

    virtual ~syntheticRAW24Src_t(void);

protected:
    virtual void mallocBuffers(void);
    virtual void reallocBuffers(void);
    virtual void freeBuffers(void);

    virtual u32 getGrabFormat(void);
    virtual image_t *genImage(u32 timestamp);

public:
    virtual bool setImageSize(unsigned width, unsigned height);

    friend class syntheticGrabber_t;
};

class syntheticYUV422iSrc_t: public virtual syntheticSrc_t
{
private:
    syntheticYUV422iSrc_t(void);

    virtual ~syntheticYUV422iSrc_t(void);

protected:
    virtual void mallocBuffers(void);
    virtual void reallocBuffers(void);
    virtual void freeBuffers(void);

    virtual u32 getGrabFormat(void);
    virtual image_t *genImage(u32 timestamp);

public:
    virtual bool setImageSize(unsigned width, unsigned height);

    friend class syntheticGrabber_t;
};

class syntheticYUV422PSrc_t: public virtual syntheticSrc_t
{
private:
    syntheticYUV422PSrc_t(void);

    virtual ~syntheticYUV422PSrc_t(void);

protected:
    virtual void mallocBuffers(void);
    virtual void reallocBuffers(void);
    virtual void freeBuffers(void);

    virtual u32 getGrabFormat(void);
    virtual image_t *genImage(u32 timestamp);

public:
    virtual bool setImageSize(unsigned width, unsigned height);

    friend class syntheticGrabber_t;
};

class syntheticYUV420PSrc_t: public virtual syntheticSrc_t
{
private:
    syntheticYUV420PSrc_t(void);

    virtual ~syntheticYUV420PSrc_t(void);

protected:
    virtual void mallocBuffers(void);
    virtual void reallocBuffers(void);
    virtual void freeBuffers(void);

    virtual u32 getGrabFormat(void);
    virtual image_t *genImage(u32 timestamp);

public:
    virtual bool setImageSize(unsigned width, unsigned height);

    friend class syntheticGrabber_t;
};

class syntheticYUV411PSrc_t: public virtual syntheticSrc_t
{
private:
    syntheticYUV411PSrc_t(void);

    virtual ~syntheticYUV411PSrc_t(void);

protected:
    virtual void mallocBuffers(void);
    virtual void reallocBuffers(void);
    virtual void freeBuffers(void);

    virtual u32 getGrabFormat(void);
    virtual image_t *genImage(u32 timestamp);

public:
    virtual bool setImageSize(unsigned width, unsigned height);

    friend class syntheticGrabber_t;
};


