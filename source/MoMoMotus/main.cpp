/*-----------------------------------------------------------------------*
Project: Recruitment
File: MoMoMotus/main.cpp

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
#include <iostream>
#include <string>
#include <math.h>
#include <assert.h>
#include <random>
#include <chrono>

#ifdef HAVE_CHECK
    #include "check.h"
#endif

#ifndef CANDIDATE
#define CANDIDATE REC2659
#endif

/*
 * Command line arguments:
 */
int _tmain(int argc, TCHAR* argv[])
{
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);
    using namespace CANDIDATE;

    try
    {
        GlobalInit();

        const uint64_t N = 100000000;
        const uint64_t K = 1000000;

        // Perform runs
#ifdef DEBUG
        const size_t runs = 1;
#else
        const size_t runs = 100;
#endif
        printf("Performing %zu runs...\n", runs);

        Timer shortest = std::numeric_limits<Timer>::max();
        for (size_t r = 0; r < runs; ++r)
        {
            UniqueRandomGenerator generator{ N, K };

            Timer t1 = GetTimer();
            for (size_t i = 0; i < K; ++i)
            {
                generator.GenerateOneUniqueRand();
            }
            Timer t2 = GetTimer();
            shortest = std::min(t2 - t1, shortest);
        }

        printf("Shortest run: %3.6f ms\n", TimerToUs(shortest) / 1000.0);

#ifdef HAVE_CHECK
        Check();
        CheckCandidate<UniqueRandomGenerator>();
#else
        {
            // Store results to test
            std::vector<uint64_t> results_vector;
            results_vector.reserve(K);
            UniqueRandomGenerator generator{ N, K };
            for (size_t i = 0; i < K; ++i)
            {
                results_vector.emplace_back(generator.GenerateOneUniqueRand());
            }

            // OPERM5 check
            float chi2 = nerd_recruitment::OPERM5Test(results_vector.data(), results_vector.size());
            printf("chi2 value! %f\n", chi2);
            if (chi2 < 77 || chi2 > 173)
                printf("There is 99.9%% probability that the random properties are bad\n");
            else if (chi2 < 86 || chi2 > 160)
                printf("There is 99%% probability that the random properties are bad\n");
        }
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
