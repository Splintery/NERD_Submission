/*-----------------------------------------------------------------------*
Project: Recruitment
File: StarSystem/main.cpp

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
#include <cassert>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#ifdef HAVE_CHECK
    #include "check.h"
#endif

#ifndef CANDIDATE
#define CANDIDATE REC2659
#endif

static const size_t DEFAULT_NUMBER_STARS   = 50000;

/*
 * Command line arguments:
 * None
 */
int _tmain(int argc, TCHAR* argv[])
{
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);
    using namespace CANDIDATE;

    std::vector<float2> starSystem;
    
    starSystem.reserve(DEFAULT_NUMBER_STARS);
    float const radius = 10.f;
    //std::random_device rd;
    std::mt19937 gen(0);
    std::uniform_real_distribution<float> dis(-radius, radius);
    for (size_t i = 0; i < DEFAULT_NUMBER_STARS; ++i)
    {
        float2 const pos(dis(gen), dis(gen));
        starSystem.emplace_back(pos);
    }

    try
    {
        GlobalInit();

        // Perform runs
#ifdef DEBUG
        const unsigned runs = 1;
        // std::cout << "Testing QuadTree building correctly" << std::endl;
        // starSystem.clear();
        // starSystem.push_back({2, 2});
        // starSystem.push_back({4, -2});
        // starSystem.push_back({-3, -1});
        // starSystem.push_back({-2, 4});
        // starSystem.push_back({3, 6});
        // starSystem.push_back({5, 3});
        // starSystem.push_back({-7, -4});
        // starSystem.push_back({-5, 3});
        // starSystem.push_back({-3, 7});
        // starSystem.push_back({-1, 3});
        // starSystem.push_back({-1, -1});
        // starSystem.push_back({1, 1});
        // starSystem.push_back({1, 6});
        // starSystem.push_back({-3, 1});
        // starSystem.push_back({3, -4});
        // starSystem.push_back({8, -1});
        // starSystem.push_back({8, 2});
        // starSystem.push_back({8, 6});
        // starSystem.push_back({8, 9});
        // starSystem.push_back({5, 9});
        // starSystem.push_back({12, 6});
        // starSystem.push_back({16, 10});
        // starSystem.push_back({-5, -6});
        // starSystem.push_back({-6, 2});
        // starSystem.push_back({1, 2});

#else
        const unsigned runs = 5;
        printf("Performing %u runs...\n", runs);
#endif
        Timer shortest = std::numeric_limits<Timer>::max();
        float result = -1;
        for (unsigned i=0; i<runs; ++i)
        {
            Timer t1 = GetTimer();
            result = FindTheLowestDInTheStarSystem(starSystem);
            Timer t2 = GetTimer();
            std::cout << "finished calculating" << std::endl;

            shortest = std::min(t2-t1, shortest);
        }
        printf("Shortest run: %3.1f ms\n", TimerToUs(shortest) / 1000.0);

#ifdef HAVE_CHECK
        (void)result;
        Check(FindTheLowestDInTheStarSystem);
        std::cout << "info: ran solution from namespace " << STRINGIFY(CANDIDATE) << std::endl;
#else
        // Print results
        printf("Found the lowest distance: %f\n", result);

#endif
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
