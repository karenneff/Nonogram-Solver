# Nonogram-Solver

This program is a nonogram solver that I wrote over Christmas break when I had nothing better to do.

For those who don't know, a nonogram puzzle (also known as a griddler puzzle) consists of a grid of squares with a set of numeric clues for each row and column. Each clue represents one segment of black squares, and the segments must appear in the order listed; all other squares are white, and each segment represented by a clue is separated from adjacent segments by at least one white square. Some nonogram puzzles contain clues for several colors; in this case, adjacent segments that are different colors do not need to be separated by white squares.

This program currently supports black-and-white nonograms only. First, it reads in the clues from a text file provided by the user. Next, it solves the puzzle using a set of logical steps. Finally, it prints the solution to a new text file, using ' ' for a white square and '#' for a black square. You can view the solved puzzle by opening the text file.

Included in this repository is a sample text file for a trivial nonogram puzzle. The solved puzzle should say "HELLO". An internet search will yield plenty of other nonogram puzzles from various sources. You can test the program further by converting any nonogram puzzle into the correct text file format:

Line 1: "r c"  where r is the number of rows and c is the number of columns

Lines 2 to r+1: clues for the rows, separated by spaces

Line r+2: empty (program ignores this line)

Lines r+3 to r+c+2: clues for the columns, separated by spaces

The number of rows and columns are specified at the beginning to provide support for empty lines (lines of all white squares).

This program has solved every nonogram puzzle I have thrown at it; however, I have not proven that its logic will solve an arbitrary nonogram puzzle. Please let me know if you find a puzzle that it cannot solve; I will try to update the program accordingly.

This program and its documentation are available as examples of my work and are intended to be used for informational and recreational purposes only. Please do not copy or use this code for any other purpose without my permission.
