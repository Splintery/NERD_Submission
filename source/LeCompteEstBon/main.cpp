/*-----------------------------------------------------------------------*
Project: Recruitment
File: LeCompteEstBon/main.cpp

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
#include <limits>

#ifdef HAVE_CHECK
    #include "check.h"
#endif

#ifndef CANDIDATE
#define CANDIDATE REC2659
#endif

/*
 * Command line arguments:
 * [target] [operands]
 */
int _tmain(int argc, TCHAR* argv[])
{
    unsigned target = 0;
    std::vector<unsigned> operands;

    if (argc >= 3)
    {
        target = _ttoi(argv[1]);
        for (int i = 2; i < argc; ++i)
            operands.push_back(_ttoi(argv[i]));
    }
    try
    {
        CANDIDATE::GlobalInit();

        // Perform runs
        if (!operands.empty())
        {
            const unsigned runs = 3;
            printf("Performing %u runs...\n", runs);
            Timer shortest = std::numeric_limits<Timer>::max();
            bool correctResult = true;

            for (unsigned i = 0; i < runs; ++i)
            {
                Timer t1 = GetTimer();
                const std::string solution = CANDIDATE::LeCompteEstBon(target, operands);
                Timer t2 = GetTimer();
                shortest = std::min(t2 - t1, shortest);

                if (!solution.empty())
                {
                    const unsigned result = static_cast<int>(nerd_recruitment::MathsParser(solution));
                    if (result != target)
                    {
                        correctResult = false;
                        printf("Your solution does not equal the given target. Please review your mathematical expression, or your code\n");
                        break;
                    }
                    else
                        printf("You found the following correct solution : %s\n", solution.c_str());
                }
                else
                    printf("You did not find any solution for the given target. There might not be..\n");
            }

            if (correctResult)
                printf("Shortest run: %3.1f ms\n", TimerToUs(shortest) / 1000.0);
        }

#ifdef HAVE_CHECK
        Check(CANDIDATE::LeCompteEstBon);
        std::cout << "info: ran solution from namespace " << STRINGIFY(CANDIDATE) << std::endl;
#endif

        CANDIDATE::GlobalTeardown();
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
