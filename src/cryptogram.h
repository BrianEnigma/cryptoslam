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

#ifndef _CRYPTOGRAM_H_
#define _CRYPTOGRAM_H_

#include <curses.h>
#include <string>
#include <map>
using namespace std;

typedef multimap<int, string> wordDistribution_t;
typedef pair<int, string> wordDistributionPair_t;

#define SAMPLE_TEXT \
"Welcome to Cryptoslam!  This program has been designed to help you solve\n" \
"those pencil-and-paper cryptograms found in newspapers and puzzle \n"\
"magazines.  Hit a key from the menu bar above.  S,U,R let you change the\n"\
"substitution values.  F lets you create, load, or save a file.  S gives you\n"\
"good statistics.  Load the file \"sample.txt\" to try it out.  (You could\n"\
"have also given the filename on the command line).\n"

#define FILE_HEADER "CGFILE1"

class Cryptogram
{
public:
    Cryptogram();
    ~Cryptogram();

    void reset();
    void calcPlaintext();
    void set(char cypherLetter, char plainLetter);
    void unset(char cypherLetter);
    void randomizeKeys();
    void randomizeMessage();
    void newRandomMessage();
    void loadFile(const char *file);
    void saveFileText(const char *file);
    void saveFileBinary(const char *file);
    void loadKeys(const char *file);
    void saveKeys(const char *file);

    void paintAlphabet(WINDOW *w);
    void paintMessage(WINDOW *w);
    void paintStats(WINDOW *w);
    
protected:
    string cyphertext;
    string plaintext;
    char alphabet[26];
    char textcolor[26];

    void letterDistribution(char letters[], int counts[]);
    wordDistribution_t wordDistribution();

    static char normalize(char letter);
    static bool isKey(char letter);
};

#endif

