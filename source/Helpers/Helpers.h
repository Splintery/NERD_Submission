/*-----------------------------------------------------------------------*
Project: Recruitment
File: Helpers/Helpers.h

Copyright (C) 2018 Nintendo, All rights reserved.

These coded instructions, statements, and computer programs contain proprietary
information of Nintendo and/or its licensed developers and are protected by
national and international copyright laws. They may not be disclosed to third
parties or copied or duplicated in any form, in whole or in part, without the
prior written consent of Nintendo.

The content herein is highly confidential and should be handled accordingly.
*-----------------------------------------------------------------------*/

#ifndef NERD_RECRUITEMENT_HELPERS_H
#define NERD_RECRUITEMENT_HELPERS_H


#include <algorithm>
#include <cstdint>
#include <string>


#ifndef NERD_FORCEINLINE
    #ifdef _MSC_VER
        #define NERD_FORCEINLINE  __forceinline
        #define NERD_NOINLINE     __declspec(noinline)
    #elif defined(__GNUC__)
        #define NERD_FORCEINLINE  inline __attribute__((always_inline))
        #define NERD_NOINLINE     __attribute__((noinline))
    #else
        #define NERD_FORCEINLINE  inline
        #define NERD_NOINLINE
    #endif
#endif


#define UNUSED_PARAM(x)  (void)x
#define STRINGIFY(x)     DO_STRINGIFY(x)
#define DO_STRINGIFY(x)  #x


#ifdef _WIN32
    #define NOMINMAX
    #include <Windows.h>
    #include <tchar.h>

    typedef int64_t Timer;

    Timer inline GetTimer()
    {
        LARGE_INTEGER ctr;
        ::QueryPerformanceCounter(&ctr);
        return ctr.QuadPart;
    }

    int64_t inline TimerToMs(Timer t)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (t * 1000) / freq.QuadPart;
    }

    int64_t inline TimerToUs(Timer t)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (t * 1000000) / freq.QuadPart;
    }

#else

    #include <time.h>

    typedef char TCHAR;
    #define _T(x)      x
    #define _tmain     main
    #define _ttof      atof
    #define _ttoi      atoi
    #define _tfopen    fopen
    #define _tprintf   printf
    #define _ftprintf  fprintf
    #define _sntprintf snprintf
    #define _tcsrchr   strrchr
    #define _tcslen    strlen

    typedef int64_t Timer;

    inline Timer GetTimer()
    {
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (Timer(ts.tv_sec) * 1000000000) + ts.tv_nsec;
    }


    int64_t inline TimerToMs(Timer t)
    {
        return t / 1000000;
    }

    int64_t inline TimerToUs(Timer t)
    {
        return t / 1000;
    }

#endif


namespace nerd_recruitment
{

class Bitmap
{
public:
    Bitmap(): m_buffer(NULL), m_width(0), m_height(0), m_stride(0), m_channels(0) {}
    ~Bitmap() {Free();}

    int  Create(unsigned width, unsigned height, unsigned channels, unsigned stride=0);
    void Free();
    int  Load(const TCHAR* path, unsigned channels=0);
    int  SaveAsBmp(const TCHAR* path);
    int  SaveAsPng(const TCHAR* path, unsigned compression=0);

    const uint8_t* GetData()     const {return m_buffer;}
    uint8_t*       GetData()           {return m_buffer;}
    unsigned       GetWidth()    const {return m_width;}
    unsigned       GetHeight()   const {return m_height;}
    unsigned       GetStride()   const {return m_stride;}
    unsigned       GetChannels() const {return m_channels;}

    bool HasSameDimensionsAndChannelsAs(const Bitmap& bmp) const
    {
        return (m_width==bmp.m_width && m_height==bmp.m_height && m_channels==bmp.m_channels);
    }

    void Swap(Bitmap& other)
    {
        std::swap(m_buffer,   other.m_buffer);
        std::swap(m_width,    other.m_width);
        std::swap(m_height,   other.m_height);
        std::swap(m_stride,   other.m_stride);
        std::swap(m_channels, other.m_channels);
    }

private:
    uint8_t* m_buffer;   ///< Buffer holding the bitmap data
    unsigned m_width;    ///< Bitmap's width in pixels
    unsigned m_height;   ///< Bitmap's height in pixels
    unsigned m_stride;   ///< Bitmap's stride in bytes
    unsigned m_channels; ///< Number of channels per pixel
};

inline void swap(Bitmap& bmp1, Bitmap& bmp2)
{
    bmp1.Swap(bmp2);
}


uint64_t MathsParser(const std::string &expression);

float OPERM5Test(uint64_t * rnd, uint64_t n);

} // namespace nerd_recruitment


#endif // NERD_RECRUITEMENT_HELPERS_H
