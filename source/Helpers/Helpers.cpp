/*-----------------------------------------------------------------------*
Project: Recruitment
File: Helpers/Helpers.cpp

Copyright (C) 2018 Nintendo, All rights reserved.

These coded instructions, statements, and computer programs contain proprietary
information of Nintendo and/or its licensed developers and are protected by
national and international copyright laws. They may not be disclosed to third
parties or copied or duplicated in any form, in whole or in part, without the
prior written consent of Nintendo.

The content herein is highly confidential and should be handled accordingly.
*-----------------------------------------------------------------------*/

#include "Helpers.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include <cstring>

namespace nerd_recruitment
{

void Bitmap::Free()
{
    delete[] m_buffer;
    m_buffer   = NULL;
    m_width    = 0;
    m_height   = 0;
    m_stride   = 0;
    m_channels = 0;
}


/**
 * @brief Allocates a bitmap
 *
 * @param width    The bitmap's width in pixels
 * @param height   The bitmap's height in pixels
 * @param channels The number of channels per pixel [1-4]
 * @param stride   The bitmap's stride in bytes. If 0, a default stride will be used.
 *
 * @return O in case of success, non-zero in case of failure
 */
int Bitmap::Create(unsigned width, unsigned height, unsigned channels, unsigned stride)
{
    if (stride == 0)
        stride = width * channels;
    
    assert(m_buffer == NULL);
    assert(stride >= width*channels);

    m_buffer   = new uint8_t[height * stride];
    m_width    = width;
    m_height   = height,
    m_stride   = stride;
    m_channels = channels;

    return 0;
}


static int StbiRead(void* user, char* data, int size)
{
    return static_cast<int>(fread(data, 1, size, static_cast<FILE*>(user)));
}

static void StbiSkip(void* user, int n)
{
   fseek(static_cast<FILE*>(user), n, SEEK_CUR);
}

static int StbiEof(void* user)
{
   return feof(static_cast<FILE*>(user));
}

static const stbi_io_callbacks StbiCallbacks =
{
    StbiRead,
    StbiSkip,
    StbiEof
};


/**
 * @brief Loads a bitmap from a file
 *
 * @param path     The path to the bitmap file
 * @param channels The number of desired channels in the bitmap. If 0, the file is loaded as is.
 */
int Bitmap::Load(const TCHAR* path, unsigned channels)
{
    assert(channels <= 4);

    FILE* file = _tfopen(path, _T("rb"));
    if (file == NULL)
        return errno;

    int width, height, fileChannels;
    stbi_uc* buffer = stbi_load_from_callbacks(&StbiCallbacks, file, &width, &height, &fileChannels, channels);
    fclose(file);
    if (buffer == NULL)
        return -1;

    m_buffer   = buffer;
    m_width    = width;
    m_height   = height;
    m_channels = (channels==0) ? fileChannels : channels;
    m_stride   = width * m_channels;

    return 0;
}


static void StbiWrite(void* context, void* data, int size)
{
    fwrite(data, 1, size, static_cast<FILE*>(context));
}


/**
 * @brief Saves a bitmap to a file in the BMP format
 */
int Bitmap::SaveAsBmp(const TCHAR* path)
{
    assert(m_buffer != NULL);

    if (m_stride != m_width*m_channels)
    {
        fprintf(stderr, "Stride not supported for saving as BMP, try saving as PNG\n");
        return EOPNOTSUPP;
    }

    FILE* file = _tfopen(path, _T("wb"));
    if (file == NULL)
        return errno;

    int success = stbi_write_bmp_to_func(StbiWrite, file, m_width, m_height, m_channels, m_buffer);
    fclose(file);

    return (success) ? 0 : -1;
}


/**
 * @brief Saves a bitmap to a file in the PNG format
 */
int Bitmap::SaveAsPng(const TCHAR* path, unsigned compression)
{
    assert(m_buffer != NULL);
    assert(compression <= 9u);

    FILE* file = _tfopen(path, _T("wb"));
    if (file == NULL)
        return errno;

    int oldCompression = stbi_write_png_compression_level;
    int oldFilter      = stbi_write_force_png_filter;

    stbi_write_png_compression_level = compression;
    if (compression == 0u)
        stbi_write_force_png_filter = 0; // If no compression, that's because we want to be fast

    int success = stbi_write_png_to_func(StbiWrite, file, m_width, m_height, m_channels, m_buffer, m_stride);
    fclose(file);

    stbi_write_png_compression_level = oldCompression;
    stbi_write_force_png_filter      = oldFilter;

    return (success) ? 0 : -1;
}

/**
 * @brief Parse a mathematical expression and returns the result of it
 * @brief it handles parenthesis, spaces, operators "+", "-", "*", "/", "(" , ")", and their precedence
 * @brief example : "4+(2*3)", "4-3-2-1", "4* 2*3", "4*2+3", "4+2*3", "4+2+ 3", "4+2+3", "4*(2* 3)", "(4*2*3)", "(4+(2*(3+7-1*(2+3))))", "(9 * 25) + 7 + 6 - 2", "4 * (2-3)"
 */

static uint64_t Evaluate(const std::string &expression, size_t start = 0, size_t* end = nullptr)
{
    bool                   lastCharacterWasDigit = false;
    char                   digitsBuffer[45]; // N = 15, 15 * 3
    size_t                 digitsIndex = 0;
    std::vector<uint64_t>  operands;
    operands.reserve(20);
    std::vector<char>      operators;
    operators.reserve(20);
    size_t                 i = start;

    for (; i < expression.size(); ++i)
    {
        const char currentCharacter = expression[i];

        // Space, just ignore it
        if (currentCharacter == ' ')
        {
            if (i < expression.size() - 1)
                continue;
        }

        // We are parsing a number, just accumulate the digit and continue
        if (isdigit(currentCharacter))
        {
            lastCharacterWasDigit = true;
            digitsBuffer[digitsIndex++] = currentCharacter;

            if (i < expression.size() - 1)
                continue;
        }

        // We have finished parsing a number
        if (lastCharacterWasDigit)
        {
            digitsBuffer[digitsIndex] = '\0';
            operands.push_back(atoi(digitsBuffer));
            digitsIndex = 0;

            // Handle multiplication or division immediately
            if (!operators.empty())
            {
                const char op = operators.back();
                if (op == '*' || op == '/')
                {
                    operators.pop_back();
                    const uint64_t lastOperand = operands.back();
                    operands.pop_back();
                    if (op == '*')
                        operands.back() *= lastOperand;
                    else
                    {
                        assert(lastOperand != 0);
                        operands.back() /= lastOperand;
                    }
                }
            }
        }

        if (currentCharacter == '(')
        {
            if (i == expression.size() - 1)
                assert(false);

            // e.g "... 3 (..."  (aka we consider that there is an hidden * operator)
            if (lastCharacterWasDigit)
                operators.push_back('*');

            size_t         newPos = 0;
            const uint64_t evaluation = Evaluate(expression, i + 1, &newPos);
            assert(newPos > 0);
            i = newPos;

            operands.push_back(evaluation);

            if (!operators.empty())
            {
                const char op = operators.back();
                if (op == '*' || op == '/')
                {
                    operators.pop_back();
                    const uint64_t lastOperand = operands.back();
                    operands.pop_back();
                    if (op == '*')
                        operands.back() *= lastOperand;
                    else
                    {
                        assert(lastOperand != 0);
                        operands.back() /= lastOperand;
                    }
                }
            }

            lastCharacterWasDigit = false;
            continue;
        }

        else if (currentCharacter == '+' || currentCharacter == '-' || currentCharacter == '*' || currentCharacter == '/')
        {
            if (i == expression.size() - 1)
                assert(false);

            if (i > 0)
            {
                const char lastCharacter = expression[i - 1];
                if (lastCharacter == '+' || lastCharacter == '-' || lastCharacter == '*' || lastCharacter == '/' || lastCharacter == '(')
                    assert(false);
            }

            operators.push_back(currentCharacter);
        }

        else if (currentCharacter == ')')
        {
            const char lastCharacter = expression[i - 1];
            if (lastCharacter == '+' || lastCharacter == '-' || lastCharacter == '*' || lastCharacter == '/' || lastCharacter == '(')
                assert(false);

            // Means that there is too many right parentheses
            if (start == 0)
                assert(false);

            lastCharacterWasDigit = false;
            break;
        }

        else if (!lastCharacterWasDigit)
            assert(false);

        lastCharacterWasDigit = false;
    }

    // Means that there is too many left parentheses
    if (start > 0 && i == expression.size())
        assert(false);

    if (end)
        *end = i;

    assert(operands.size() == operators.size() + 1);
    uint64_t value = operands[0];

    for (size_t j = 0; j < operators.size(); ++j)
    {
        assert(operators[j] == '-' || operators[j] == '+');
        if (operators[j] == '-')
            value -= operands[j + 1];
        else
            value += operands[j + 1];
    }

    return value;
}

uint64_t MathsParser(const std::string &expression)
{
    if (expression.empty())
        assert(false);

    return Evaluate(expression);
}


//if (chi2 < 77 || chi2 > 173)
//printf("There is 99.9% probability that the random properties are bad\n");
//else if (chi2 < 86 || chi2 > 160)
//printf("There is 99% probability that the random properties are bad\n");
float OPERM5Test(uint64_t * rnd, uint64_t n)
{
    int permCount[120] = {};

    //-------------------------------------

    for (uint64_t i = 0; i < n - 5; i++)
    {
        uint64_t t[5];
        std::memcpy(t, rnd + i, 5 * sizeof(uint64_t));

        //identify in which kind of permutation order the 5 values are (out of 120 possible permutations)
        int pIndex = 0;
        int pMul = 1;
        for (uint32_t k = 5; k > 1; k--)
        {
            uint64_t min = t[0];
            uint32_t index = 0;
            for (uint32_t j = 1; j < k; j++)//find minimum's position
            {
                uint64_t r = t[j];
                if (r < min)
                {
                    min = r;
                    index = j;
                }
            }

            pIndex += pMul * index;//take it into account for permutation number
            pMul *= k;

            for (uint32_t j = 1; j < k; j++)//remove minimum found
            {
                if (j > index)
                    t[j - 1] = t[j];
            }
        }
        permCount[pIndex]++;
    }

    //-------------------------------------

    float chi2 = 0.f;
    for (uint32_t i = 0; i < 120; i++)
    {
        float d = permCount[i] - (n - 5) / 120.f;
        chi2 += (d*d) / ((n - 5) / 120.f);
    }

    //-------------------------------------

    return chi2;
}

} // namespace nerd_recruitment
