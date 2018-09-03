//Title: NonogramSolver.cpp
//Date: 12-30-2015
//Description: Reads in nonogram clues
//from a file, constructs the corresponding
//puzzle grid, solves the puzzle,
//and prints the resulting picture to a file
//or the screen.

#include "NonogramLogic.h"
#include "NonogramObjects.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string>

using namespace std;

int main()
{
   //first, find the file that holds the clues
   cout << "Enter the filename for your puzzle's clues: ";
   string filename;
   cin >> filename;
   ifstream infile;
   infile.open(filename.c_str());
   while(!infile)
   {
      cout << "File not found! Enter a valid filename. ";
      cin >> filename;
      infile.open(filename.c_str());
   }
   
   //read in the first line; set up the board
   int numOfRows, numOfColumns;
   infile >> numOfRows >> numOfColumns;
   while(infile.get() != '\n');
   GameGrid g(numOfRows,numOfColumns);
   RowManager allRows;
   allRows.numOfRows = numOfRows;
   allRows.numOfCols = numOfColumns;
   allRows.stripes.resize(numOfRows + numOfColumns);
   allRows.numUnsolved = numOfRows + numOfColumns;
   allRows.g = &g;
   //read in the clues for each row...
   int rowCellCount = 0, colCellCount= 0;
   for(int i = 0; i < numOfRows; i++)
   {
      NonogramRow *r = new NonogramRow(i, numOfColumns, &g);
      int nextSegmentLength;
      Segment *nextSegment;
      while(infile.peek() != '\n')
      {
         if(infile.peek()<58 && infile.peek()>47)
         {
            infile >> nextSegmentLength;
            rowCellCount += nextSegmentLength;
            nextSegment = new Segment;
            nextSegment->length = nextSegmentLength;
            r->add(nextSegment);
            
         }
         else
            infile.ignore();
      }
      infile.ignore();
      allRows.stripes[i] = r;
   }
   
   //...followed by the clues for each column
   infile.ignore();
   for(int j = 0; j < numOfColumns; j++)
   {
      NonogramColumn *c = new NonogramColumn(numOfRows, j, &g);
      int nextSegmentLength;
      Segment *nextSegment;
      while(infile.peek() != '\n'&& !infile.eof())
      {
         if(infile.peek()<58 && infile.peek()>47)
         {
            infile >> nextSegmentLength;
            colCellCount += nextSegmentLength;
            nextSegment = new Segment;
            nextSegment->length = nextSegmentLength;
            c->add(nextSegment);
            
         }
         else
            infile.ignore();
      }
      infile.ignore();
      allRows.stripes[numOfRows + j] = c;
   }
   if(rowCellCount != colCellCount)
   {
      cout << "Number of dark cells by row and by column" << endl;
      cout << "are unequal!! Please proofread clue file.";
   }
   //Next, solve the puzzle.
   else
   {
      cout << "Solving..." <<endl;
      int stepcount = 0;
      allRows.nextToCheck = 0;
      while(allRows.numUnsolved > 0)
      {
         //fetch a row to check, then check it
         NonogramStripe *r = allRows.stripes[allRows.nextToCheck];
         check(r);
         if(allRows.stripes[allRows.nextToCheck]->isFinal())
            allRows.numUnsolved--;
         //make sure solver is making progress
         if(stepcount==0 || r->modified)
         {
            r->modified = false;
            allRows.lastModified = allRows.nextToCheck; //i.e. index of r
         }
         else if(allRows.lastModified == allRows.nextToCheck)//program has checked every row without modifying anything
         {
            cout << "Can\'t solve puzzle!";
            _getch();
         }
         //then iterate to the next row to be checked
         if(allRows.numUnsolved > 0)
         {
            do{
               if(allRows.nextToCheck == numOfRows + numOfColumns - 1)
                  allRows.nextToCheck = 0;
               else
                  allRows.nextToCheck++;
            }while(allRows.stripes[allRows.nextToCheck]->isFinal());
         }
         stepcount++;
         cout << "Step " << stepcount << endl;
      }
      cout << "Solved in " << stepcount << " steps." <<endl;
      //Finally, print the result to a file.
      cout << "Enter a file name to save your picture: ";
      string picname;
      cin >> picname;
      g.printToFile(picname);
      cout << "All done!" <<endl;
   }
}