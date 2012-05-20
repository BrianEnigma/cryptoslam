#Cryptoslam v1.3

Cryptoslam is a curses-based tool for solving the simple pencil-and-paper
cryptograms found in newspapers and puzzle magazines.  It is a tool that aids
in analyzing, examining, and trying possible decodings of simple substitution
cyphers.

Start the Cryptoslam program with the command "./cryptoslam".  If you already
have a cyphertext file you would like to decode, you can start it with
"./cryptoslam {filename}".  Once running, you will be see a screen with three
main areas.  The top area always shows a menu or an input prompt.  The bottom
area always shows the mapping between a cyphertext alphabet and a plaintext
alphabet.  The central area displays what you are working on (usually, the
cyphertext and plaintext side-by-side, but sometimes it shows the output
of different commands, like the statistics of letter distribution).  The menu
gives you the following options:

    S:SetChar       Set a cyphertext letter's decoding to a plaintext letter
    U:UnsetChar     Set a cyphertext letter's decoding back to unknown
    R:ResetChars    Set ALL cyphertext letters back to unknown
    F:File          File Menu
        L:Load      Load a file.  If the file is a text file, the entire 
                    contents will be treated as a cyphertext to decode.  If
                    the file is a binary file (see "saveBin" below), the
                    saved state of the system--cyphertext, plaintext, and
                    current alphabet decoding--will be restored.
        T:saveText  Save the cyphertext and current plaintext representation
                    to a text file.
        B:saveBin   Save the current state of the system (the cyphertext and
                    the current alphabet decoding) to a binary data file that
                    can be loaded again later.
        N:New       Launch your text editor ($VISUAL or $EDITOR environmental
                    variable) to create a new cyphertext file.
        C:Cancle    Return to main menu
    T:Tools         Tools Menu.  This will show a letter distribution--how many
                    times each letter occurs in the cyphertext.  In a normal
                    English language distribution, the top letters on this
                    list are bound to be something like "ERSTLN."
        R:Random    Scramble the cyphertext decoding (useful for creating a
                    new cypher--feed it plaintext, and treat the output as
                    cyphertext)
        T:Transform If the file you loaded was plaintext, it will generate
                    a random substitution transform.  Use this to create
                    a new puzzle from a message of your own.  Write your
                    message in a simple text file, load it, then use 
                    "Transform" upon it.
        3:ROT13     Set the decoding to the standard Unix ROT13.  Specifically,
                    A decodes to N, B decodes O, C becomes P, D becomes Q, etc.
        G:Generate  If you have the "fortune" command installed on your
                    system, generate a random puzzle.  This could take a
                    little while because it has to keep running fortune
                    until a puzzle of sufficient size can be generated
                    (more than about 3 lines and less than about 9).  If you
                    really have to give up, the plaintext version is stored
                    in the local directory as "tmp.tmp".
        C:Cancel    Return to main menu

Have fun, and happy decoding!

    Brian Enigma
    <brian@netninja.com>

