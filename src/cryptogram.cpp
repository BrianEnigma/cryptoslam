/*
 *cryptoslam: A tool for decoding cryptograms
 *
 *Copyright (C) 1997,2000,2003,2012 Brian Enigma
 *                             brian@netninja.com
 *                             https://github.com/BrianEnigma/cryptoslam
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or EXISTANCE OF KITTEN.  See the GNU General
 *   Public License for more details.
 *
 *   http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "config.h"
#include "cryptogram.h"
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>

Cryptogram::Cryptogram()
{
    cyphertext = "";
    plaintext = "";
    reset();
}

Cryptogram::~Cryptogram()
{
}

void Cryptogram::reset()
{
    cyphertext = SAMPLE_TEXT;
    plaintext = "";
    memset(alphabet, ' ', sizeof(alphabet));
    calcPlaintext();
}

void Cryptogram::loadFile(const char *filename)
{
    FILE *f;
    char block[4096];
    int size;
    
    cyphertext = SAMPLE_TEXT;
    plaintext = "";
    memset(alphabet, ' ', sizeof(alphabet));

    f = fopen(filename, "rb");
    if (!f)
    {
        reset();
        return;
    }
    size = fread(block, 1, 4096, f);
    block[4095] = 0;
    block[size] = 0;
    fclose(f);

    if (strncmp(block, FILE_HEADER, strlen(FILE_HEADER) + 1) == 0)
    {
        memcpy(alphabet, block + strlen(FILE_HEADER) + 1, sizeof(alphabet));
        cyphertext = strdup(block + strlen(FILE_HEADER) + 1 + sizeof(alphabet));
    } else {
        cyphertext = strdup(block);
    }
    calcPlaintext();
}

void Cryptogram::saveFileText(const char *filename)
{
    FILE *f;
    
    f = fopen(filename, "wb");
    if (!f)
        return;
    fwrite(plaintext.c_str(), 1, plaintext.length(), f);
    fwrite("\n\n", 1, 2, f);
    fwrite(cyphertext.c_str(), 1, cyphertext.length(), f);
    fclose(f);
}

void Cryptogram::saveFileBinary(const char *filename)
{
    FILE *f;
    
    f = fopen(filename, "wb");
    if (!f)
        return;
    // Write the header
    fwrite(FILE_HEADER, 1, strlen(FILE_HEADER) + 1, f);
    // Write the alphabet
    fwrite(alphabet, 1, sizeof(alphabet), f);
    // Write the cyphertext
    fwrite(cyphertext.c_str(), 1, cyphertext.length(), f);
    fclose(f);
}

void Cryptogram::saveKeys(const char *filename)
{
    FILE *f;
    f = fopen(filename, "wb");
    if (!f)
        return;
    fwrite(alphabet, 1, sizeof(alphabet), f);
    fclose(f);
}

void Cryptogram::loadKeys(const char *filename)
{
    FILE *f;
    unsigned int i;
    f = fopen(filename, "rb");
    if (!f)
        return;
    for (i=0; i<sizeof(alphabet); i++)
        alphabet[i] = ' ';
    fread(alphabet, 1, sizeof(alphabet), f);
    fclose(f);
    for (i=0; i<sizeof(alphabet); i++)
        if ((alphabet[i] < 'A') || (alphabet[i] > 'Z'))
            alphabet[i] = ' ';
    calcPlaintext();
}

void Cryptogram::set(char cypherLetter, char plainLetter)
{
    // Normalize to uppercase
    cypherLetter = normalize(cypherLetter);
    plainLetter = normalize(plainLetter);
    if (isKey(cypherLetter))
        alphabet[cypherLetter - 'A'] = plainLetter;
}

void Cryptogram::unset(char cypherLetter)
{
    set(cypherLetter, ' ');
}

void Cryptogram::randomizeKeys()
{
    // Probably a better way of doing this....
    int r1, r2;
    char temp;
    for (int i=0; i<26; i++)
        alphabet[i] = i + 'A';
    srand(time(NULL));
    for (int i=0; i<1000; i++)
    {
        r1 = rand() % 26;
        r2 = rand() % 26;
        temp = alphabet[r1];
        alphabet[r1] = alphabet[r2];
        alphabet[r2] = temp;
    }
}

void Cryptogram::randomizeMessage()
{
    string tempStr;
    bool matchKey;
    do{
        matchKey = false;
        randomizeKeys();
        for (int i=0; i<26; i++)
        {
            if (alphabet[i]==i+'A')
                matchKey = true;
        }
    }while(matchKey);
    calcPlaintext();
    cyphertext = plaintext;
    plaintext = string(" ", cyphertext.length());
    calcPlaintext();
    for (int i=0; i<26; i++)
        alphabet[i] = ' ';
}

void Cryptogram::newRandomMessage()
{
    int lines;
    do{
        system("fortune > tmp.tmp");
        loadFile("tmp.tmp");
        lines = 0;
        for (string::iterator c = cyphertext.begin(); 
             c != cyphertext.end(); 
             c++)
        {
            if (*c == '\n')
                lines++;
        }
    }while((lines < 3) || (lines > 8));
    randomizeMessage();
}

void Cryptogram::calcPlaintext()
{
    if (plaintext.length() != cyphertext.length())
        plaintext = string(" ", cyphertext.length());
    string::iterator p = plaintext.begin();
    string::iterator c = cyphertext.begin();
    char ch;
    while (c != cyphertext.end())
    {
        // Get the character
        ch = *c;
        // Normalize it to uppercase
        ch = normalize(ch);
        // Calc the plaintext
        if (isKey(ch))
        {
            *p = alphabet[ch - 'A'];
            if (*p == ' ')
                *p = '_';
        } else {
            *p = *c;
        }
        p++;
        c++;
    }
}

void Cryptogram::letterDistribution(char letters[], int counts[])
{
    char letter;
    for (char c = 'A'; c <= 'Z'; c++)
    {
        letters[c-'A'] = c;
        counts[c-'A'] = 0;
    }
    for (string::iterator t = cyphertext.begin(); t != cyphertext.end(); t++)
    {
        letter = normalize(*t);
        if (isKey(letter))
            counts[letter-'A']++;
    }
    // BUBBLESORT!  YEAY!
    int tempInt;
    char tempChar;
    for (int i=0; i < 26; i++)
        for (int j = i; j < 26; j++)
            if (counts[j] > counts[i])
            {
                tempChar = letters[i];
                letters[i] = letters[j];
                letters[j] = tempChar;
                tempInt = counts[i];
                counts[i] = counts[j];
                counts[j] = tempInt;
            }
}

wordDistribution_t Cryptogram::wordDistribution()
{
    wordDistribution_t result;
    result.insert(wordDistributionPair_t(5, string("five")));
    result.insert(wordDistributionPair_t(9, string("nine")));
    result.insert(wordDistributionPair_t(2, string("two")));
    result.insert(wordDistributionPair_t(12, string("twelve")));
    result.insert(wordDistributionPair_t(15, string("fifteen")));
    result.insert(wordDistributionPair_t(7, string("seven")));
    result.insert(wordDistributionPair_t(9, string("nine-b")));
    result.insert(wordDistributionPair_t(7, string("seven-b")));
    return result;
}

char Cryptogram::normalize(char letter)
{
    if ( (letter >= 'a') && (letter <= 'z') )
        return letter - ('a' - 'A');
    else
        return letter;
}

bool Cryptogram::isKey(char letter)
{
    bool result = false;
    if ( (letter >= 'A') && (letter <= 'Z') )
        result = true;
    return result;
}

void Cryptogram::paintAlphabet(WINDOW *w)
{
    // Print the alphabet...
    char used[26];
    bool duplicates = false;
    char letter;
    
    for (int i=0; i<26; i++)
        used[i] = 0;

    wcolor_set(w, 2, NULL);
    wmove(w, 0, 0);
    wprintw(w, "Cyphertext: ");
    for (char c='A'; c <= 'Z'; c++)
        wprintw(w, "%c", c);
    wprintw(w, "\nPlaintext:  ");
    for (int i=0; i<26; i++)
    {
        letter = alphabet[i];
        wprintw(w, "%c", letter);
        if (isKey(letter))
        {
            if (used[letter-'A'] == 1)
                duplicates = true;
            else
                used[letter-'A'] = 1;
        }
    }
    if (duplicates)
        wprintw(w, " <-- Warning: duplicates");
    else
        wprintw(w, "                        ");
    mvwprintw(w, 2, 0, "%s %s by %s", PROGNAME, PROGVER, BYLINE);
}

void Cryptogram::paintMessage(WINDOW *w)
{
    // 911:enigma THIS NEEDS WORK!
    int here=0, newHere=0, max;
    char *line;
    // Print the message, interlaced...
    wclear(w);
    wmove(w, 0, 0);
    // Find next \n
    wcolor_set(w, 3, NULL);
    max = cyphertext.length();
    // Traverse the string, one line at a time
    while (here != max)
    {
        // Print plaintext line
        line = strdup(plaintext.c_str() + here);
        for (char *c = line; *c; c++)
            if (*c == '\n') *c = 0;
        wattron(w, A_BOLD);
        wprintw(w, "%s\n", line);
        wattroff(w, A_BOLD);
        free(line);
        // Print cyphertext line
        line = strdup(cyphertext.c_str() + here);
        for (char *c = line; *c; c++)
            if (*c == '\n') *c = 0;
        wattron(w, A_DIM);
        wprintw(w, "%s\n", line);
        wattroff(w, A_DIM);
        free(line);
        // Move on to next line
        newHere = cyphertext.find('\n', here);
        // If no more newlines, ensure we are at the end
        if (newHere == -1)
            newHere = max;
        else
            here = newHere + 1;
        //else
        //    here = newHere;
    }
}

void Cryptogram::paintStats(WINDOW *w)
{
    // Print the statistics...
    char letters[26];
    int counts[26];
    int row, col;
    letterDistribution(letters, counts);
    wordDistribution_t words = wordDistribution();

    wclear(w);
    mvwprintw(w, 1, 0, 
              "Number of times each letter/word appears in cyphertext:");
    for (int i=0; i < 26; i++)
    {
        if (i < 13)
        {
            row = 3 + i;
            col = 1;
        } else {
            row = 3 + i - 13;
            col = 20;
        }
        wmove(w, row, col);
        wprintw(w, "%c %4d\n", letters[i], counts[i]);
    }
    int c = 0;
    for (wordDistribution_t::reverse_iterator t = words.rbegin();
         t != words.rend() && c < 13;
         t++, c++)
    {
        wmove(w, 3+c, 40);
        wprintw(w, "%3d %s", (*t).first, (*t).second.c_str());
    }
}
