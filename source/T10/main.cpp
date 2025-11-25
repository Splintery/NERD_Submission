/*-----------------------------------------------------------------------*
Project: Recruitment
File: T10/main.cpp

Copyright (C) 2018 Nintendo, All rights reserved.

These coded instructions, statements, and computer programs contain proprietary
information of Nintendo and/or its licensed developers and are protected by
national and international copyright laws. They may not be disclosed to third
parties or copied or duplicated in any form, in whole or in part, without the
prior written consent of Nintendo.

The content herein is highly confidential and should be handled accordingly.
*-----------------------------------------------------------------------*/

#include "all.h"
#include <Helpers.h>
#include <algorithm>
#include <sstream>
#ifdef _MSC_VER
#define MSC_VER_LIMIT_FOR_EXPERIMENTAL_FS 1900
#define USE_EXPERIMENTAL_FS  (_MSC_VER < MSC_VER_LIMIT_FOR_EXPERIMENTAL_FS)
#else
#define USE_EXPERIMENTAL_FS 1
#endif
#if !USE_EXPERIMENTAL_FS
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <iostream>
#include <fstream>
#include <iomanip>

#ifdef HAVE_CHECK
    #include "check.h"
#endif

#ifndef CANDIDATE
#define CANDIDATE REC2659
#endif

static const TCHAR DEFAULT_DATA_DIR[] = { _T(NERD_RECRUITMENT_DIR) _T("/source/T10/Data/masc_500k_texts") };


using namespace CANDIDATE;
namespace
{
    static const char cp1252Normalization[256] =
    {
        //0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', // [00;0f]
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', // [10;1f]
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', // [20;2f]
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', // [30;3f]
        ' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o', // [40;4f]
        'p','q','r','s','t','u','v','w','x','y','z',' ',' ',' ',' ',' ', // [50;5f]
        ' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o', // [40;4f]
        'p','q','r','s','t','u','v','w','x','y','z',' ',' ',' ',' ',' ', // [50;5f]
        ' ',' ',' ','f',' ',' ',' ',' ',' ',' ','s',' ',' ',' ','z',' ', // [80;8f]
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','s',' ',' ',' ','z','y', // [90;9f]
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', // [a0;af]
        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', // [b0;bf]
        'a','a','a','a','a','a',' ','c','e','e','e','e','i','i','i','i', // [c0;cf]
        ' ','n','o','o','o','o','o',' ','o','u','u','u','u','y',' ',' ', // [d0;df]
        'a','a','a','a','a','a',' ','c','e','e','e','e','i','i','i','i', // [e0;ef]
        ' ','n','o','o','o','o','o',' ','o','u','u','u','u','y',' ','y'  // [f0;ff]
    };
    void Print(Layout const& layout);
}

/*
 * Command line arguments:
 * [target] [operands]
 */
int _tmain(int argc, TCHAR* argv[])
{
    std::ostringstream oss;
#if !USE_EXPERIMENTAL_FS
    namespace fs = std::filesystem;
#else
#if defined _MSC_VER && (_MSC_VER < 1900)
    namespace fs = std::tr2::sys;
#else
    namespace fs = std::experimental::filesystem;
#endif
#endif
    fs::path p(DEFAULT_DATA_DIR);
    if (argc > 2)
    {
        std::cout << "Accept only one argument !" << std::endl;
        getchar();
        return -1;
    }
    if (argc == 2)
    {
        p = argv[1];
    }
    std::string const ext = ".txt";
    for (auto f = fs::recursive_directory_iterator(p); f != fs::recursive_directory_iterator(); ++f)
    {
        const fs::path& e = *f;
        if (e.extension() == ext)
        {
            std::ifstream in(e.string());
            while (!in.eof())
            {
                char c = char(in.get());
                c = cp1252Normalization[static_cast<unsigned char>(c)];
                oss << c;
            }
        }
    }
    std::string corpus = oss.str();
    if (corpus.empty())
    {
        std::cout << "corpus is empty!" << std::endl;
        getchar();
        return -1;
    }
    try
    {
        printf("Size of the corpus %llu characters\n", static_cast<unsigned long long>(corpus.size()));
        GlobalInit();

        // Perform runs

        const unsigned runs = 1;
        printf("Performing %u runs...\n", runs);
        Timer shortest = std::numeric_limits<Timer>::max();

        for (unsigned i = 0; i < runs; ++i)
        {
            Timer t1 = GetTimer();

            Layout candidateLayout;
            ComputeT10Layout(candidateLayout, corpus);

            Timer t2 = GetTimer();
            shortest = std::min(t2 - t1, shortest);
        }
        printf("Shortest run: %3.1f ms\n", TimerToUs(shortest) / 1000.0);

        {
            Layout candidateLayout;
            ComputeT10Layout(candidateLayout, corpus);
#ifdef HAVE_CHECK
            Check(candidateLayout, corpus);
            std::cout << "solution from namespace " << STRINGIFY(CANDIDATE) << std::endl;
#else
            Print(candidateLayout);
#endif
        }

        GlobalTeardown();
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "An exception was thrown: %s\n", e.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        fprintf(stderr, "An exception was thrown\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

namespace
{
#ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif

    void Print(Layout const& layout)
    {
        std::cout << "+------+------+------+" << std::endl;
        std::cout << "|  1   |  2   |  3   |" << std::endl;
        std::cout << "|      | " << std::setw(5) << std::left
            << layout[0] << "| " << std::setw(5)
            << layout[1] << "|" << std::endl;
        std::cout << "+------+------+------+" << std::endl;
        std::cout << "|  4   |  5   |  6   |" << std::endl;
        std::cout << "| " << std::setw(5) << std::left
            << layout[2] << "| " << std::setw(5)
            << layout[3] << "| " << std::setw(5)
            << layout[4] << "|" << std::endl;
        std::cout << "+------+------+------+" << std::endl;
        std::cout << "|  7   |  8   |  9   |" << std::endl;
        std::cout << "| " << std::setw(5) << std::left
            << layout[5] << "| " << std::setw(5)
            << layout[6] << "| " << std::setw(5)
            << layout[7] << "|" << std::endl;
        std::cout << "+------+------+------+" << std::endl;
        std::cout << "       |  0   |" << std::endl;
        std::cout << "       |      |" << std::endl;
        std::cout << "       +------+" << std::endl;
    }
}
