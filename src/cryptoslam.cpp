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

#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>
#include "cryptogram.h"

#define MENUBAR " S:SetChar U:UnSetChar R:ResetChars F:File T:Tools Q:Quit "
#define FILEMENU " L:Load T:saveText B:saveBinary S:SaveKeys K:LoadKeys N:New C:Cancel "
#define TOOLSMENU " R:RandomCypher 3:ROT13 T:Transform G:Generate C:Cancel "

void drawScreen(WINDOW *menu, 
                WINDOW *message, 
                WINDOW *alphabet, 
                Cryptogram *cr)
{
    // Print the menu...
    wclear(menu);
    wcolor_set(menu, 1, NULL);
    wattron(menu, WA_REVERSE);
    mvwprintw(menu, 0, 0, "%s", MENUBAR);
    wattroff(menu, WA_REVERSE);
    wmove(menu, 0, strlen(MENUBAR));

    // Print the alphabet...
    cr->paintAlphabet(alphabet);

    // Paint the message...
    cr->paintMessage(message);
    
    wrefresh(menu);
    wrefresh(message);
    wrefresh(alphabet);
}

void fileMenu(WINDOW *menu, Cryptogram *cr)
{
    char filename[255];
    char key;
    char *editor = NULL;
    char command[1024] = "";
    while (1)
    {
        wclear(menu);
        wcolor_set(menu, 1, NULL);
        wattron(menu, WA_REVERSE);
        mvwprintw(menu, 0, 0, "%s", FILEMENU);
        wattroff(menu, WA_REVERSE);
        wmove(menu, 0, strlen(FILEMENU));
        wrefresh(menu);

        noecho();
        key = getch();
        echo();
        switch(key)
        {
            case 'L':
            case 'l':
                wclear(menu);
                mvwprintw(menu, 0, 0, "File to open? ");
                wrefresh(menu);
                wgetnstr(menu, filename, 255);
                filename[254] = 0;
                cr->loadFile(filename);
                cr->calcPlaintext();
                return;
            case 'T':
            case 't':
                wclear(menu);
                mvwprintw(menu, 0, 0, "Text file to save? ");
                wrefresh(menu);
                wgetnstr(menu, filename, 255);
                filename[254] = 0;
                cr->saveFileText(filename);
                cr->calcPlaintext();
                return;
            case 'B':
            case 'b':
                wclear(menu);
                mvwprintw(menu, 0, 0, "Binary file to save? ");
                wrefresh(menu);
                wgetnstr(menu, filename, 255);
                filename[254] = 0;
                cr->saveFileBinary(filename);
                cr->calcPlaintext();
                return;
            case 'N':
            case 'n':
                wclear(menu);
                mvwprintw(menu, 0, 0, "File to create? ");
                wrefresh(menu);
                wgetnstr(menu, filename, 255);
                filename[254] = 0;
                editor = getenv("VISUAL");
                if (!editor)
                    editor = getenv("EDITOR");
                if (!editor)
                    return;
                strncpy(command, editor, 512);
                strcat(command, " ");
                strcat(command, filename);
                system(command);
                cr->loadFile(filename);
                cr->calcPlaintext();
                return;
            case 'S':
            case 's':
                wclear(menu);
                mvwprintw(menu, 0, 0, "File to save keys to? ");
                wrefresh(menu);
                wgetnstr(menu, filename, 255);
                filename[254] = 0;
                cr->saveKeys(filename);
                return;
            case 'K':
            case 'k':
                wclear(menu);
                mvwprintw(menu, 0, 0, "File to load keys from? ");
                wrefresh(menu);
                wgetnstr(menu, filename, 255);
                filename[254] = 0;
                cr->loadKeys(filename);
                return;
            case 'C':
            case 'c':
                return;
        }
    }
}

void toolsMenu(WINDOW *menu, WINDOW *message, WINDOW *alphabet, Cryptogram *cr)
{
    char key;
    
    wclear(message);
    cr->paintStats(message);
    wrefresh(message);
    while (1)
    {
        wclear(menu);
        wcolor_set(menu, 1, NULL);
        wattron(menu, WA_REVERSE);
        mvwprintw(menu, 0, 0, "%s", TOOLSMENU);
        wattroff(menu, WA_REVERSE);
        wmove(menu, 0, strlen(TOOLSMENU));
        wrefresh(menu);

        noecho();
        key = getch();
        echo();
        switch(key)
        {
            case 'R':
            case 'r':
                cr->randomizeKeys();
                cr->calcPlaintext();
                return;
            case 'T':
            case 't':
                wclear(menu);
                wcolor_set(menu, 1, NULL);
                mvwprintw(menu, 0, 0, "Please wait, randomizing...");
                wmove(menu, 0, 27);
                wrefresh(menu);
                cr->randomizeMessage();
                cr->calcPlaintext();
                return;
            case 'G':
            case 'g':
                wclear(menu);
                wcolor_set(menu, 1, NULL);
                mvwprintw(menu, 0, 0, "Please wait, generating...");
                wmove(menu, 0, 27);
                wrefresh(menu);
                cr->newRandomMessage();
                cr->calcPlaintext();
                return;
            case '3':
                for (int i=0; i < 26; i++)
                    cr->set(i + 'A', ((i+13)%26) + 'A');
                cr->calcPlaintext();
                return;
            case 'C':
            case 'c':
                return;
        }
    }
}

void handleKey(char key, 
               WINDOW *menu, 
               WINDOW *message, 
               WINDOW *alphabet, 
               Cryptogram *cr)
{
    char key1, key2;
    switch(key)
    {
        case 's':
        case 'S':
            wclear(menu);
            mvwprintw(menu, 0, 0, "Cyphertext letter to set? ");
            wrefresh(menu);
            key1 = getch();
            wclear(menu);
            mvwprintw(menu, 0, 0, "Set letter \"%c\" to which plaintext letter (spacebar to unset)? ", 
                      key1);
            wrefresh(menu);
            key2 = getch();
            cr->set(key1, key2);
            cr->calcPlaintext();
            break;
        case 'u':
        case 'U':
            wclear(menu);
            mvwprintw(menu, 0, 0, "Cyphertext letter to unset? ");
            wrefresh(menu);
            key1 = getch();
            cr->unset(key1);
            cr->calcPlaintext();
            break;
        case 'r':
        case 'R':
            for (char c = 'A'; c <= 'Z'; c++)
                cr->unset(c);
            cr->calcPlaintext();
            break;
        case 't':
        case 'T':
            toolsMenu(menu, message, alphabet, cr);
            break;
        case 'f':
        case 'F':
            fileMenu(menu, cr);
            break;
    }
}

void mainLoop(Cryptogram *cr)
{
    WINDOW *main = initscr();
    WINDOW *menu = subwin(main, 1, COLS, 0, 0);
    WINDOW *message = subwin(main, LINES-4, COLS, 1, 0);
    WINDOW *alphabet = subwin(main, 3, COLS, LINES-3, 0);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    char opt;
    while (1)
    {
        drawScreen(menu, message, alphabet, cr);
        wmove(main, 0, strlen(MENUBAR));
        wrefresh(main);
        noecho();
        opt = getch();
        echo();
        if ( (opt=='q') || (opt=='Q') )
            break;
        handleKey(opt, menu, message, alphabet, cr);
    }
    endwin();
}

int main(int argc, char *argv[])
{
    Cryptogram cr;
    if (argc==2)
        cr.loadFile(argv[1]);
    mainLoop(&cr);
}
