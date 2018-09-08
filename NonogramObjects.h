//Title: NonogramObjects.h
//Date: 12-28-2015
//Description: Row, column, and gamegrid objects to be used
//as part of a nonogram solving program. Could be used to
//create an interface to play them, as well.

#ifndef NONOGRAMOBJECTS_H
#define NONOGRAMOBJECTS_H

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class GameGrid //the grid of cells, each full, empty, or undecided as yet
{
   private: 
      vector<vector<char> > grid;
      int numOfRows,numOfCols;
   
   public: 
   GameGrid(int rows, int columns)
   {
      grid.resize(rows);
      for(int i = 0; i<rows; i++)
      {
         grid[i].resize(columns);
         for(int j = 0; j<columns; j++)
            grid[i][j] = 45; //minus sign for undecided; leaves spacebar free for empty, producing clearer picture in the end
      }
      numOfRows = rows;
      numOfCols = columns;
   }
   
   char at(int row, int column)
   {
      if(row<0 || column<0 || row >= numOfRows || column >= numOfCols)
         return ' ';//squares outside grid are considered empty
      else
         return grid[row][column];
   }
   
   void fill(int row, int column)
   {
      if(row<0 || column<0 || row >= numOfRows || column >= numOfCols)
         throw "Out of bounds error!";
      else if(grid[row][column]==32)//space
         throw "Logic error!";
      else
         grid[row][column]=35;//#
   }
   
   void empty(int row, int column)
   {
      if(row<0 || column<0 || row >= numOfRows || column >= numOfCols); // squares outside the grid are already considered empty; this can happen when finalizing segments
      else if(grid[row][column]==35)//#
         throw "Logic error!";
      else
         grid[row][column]=32;//space
   }
   void print()
   {
      for(int i = 0; i<numOfRows; i++)
      {
         for(int j = 0; j<numOfCols; j++)
            cout << grid[i][j];
         cout << endl;
      }
   }
   void printToFile(string filename)
   {
      ofstream outfile;
      outfile.open(filename.c_str());
      for(int i = 0; i<numOfRows; i++)
      {
         for(int j = 0; j<numOfCols; j++)
            outfile << grid[i][j];
         outfile << endl;
      }
      outfile.close();
   }
};

struct Segment //a "dark" segment, as given by one of the clues
{
  int length, minpos, maxpos;
  bool placed;
  Segment *previous, *next; 
};

class NonogramStripe //to include both rows and columns, as implemented below
{
   protected:
   GameGrid *board;
   int stripelength;
   Segment *first;
   bool finalized;
   
   public:
   bool modified;
   virtual char cellAt(int position) = 0;
   virtual void fill(int position) = 0;
   void fill(int start, int end) {
	   for (int ii = start; ii < end; ii++)
		   this->fill(ii);
   }
   virtual void empty(int position) = 0;
   void empty(int start, int end) {
	   for (int ii = start; ii < end; ii++)
		   this->empty(ii);
   }
   
   void add(Segment *nextSeg)
   {
      nextSeg->minpos = 0;
      nextSeg->maxpos = stripelength - nextSeg->length;
      nextSeg->placed = false;
      if(first==NULL)
      {
         nextSeg->previous = NULL;
         nextSeg->next = NULL;
         first = nextSeg;
      }
      else
      {
         Segment *i = first;
         while(i->next != NULL)
            i = i->next;
         i->next = nextSeg;
         nextSeg->previous = i;
         nextSeg->next = NULL;
      }
   }
   
   int getLength()
   {
      return stripelength;
   }
   
   Segment* getFirstSegment()
   {
      return first;
   }
   
   Segment* getLastSegment()
   {
      Segment* result = first;
      if(result==NULL)
         return result;
      while(result->next != NULL)
         result = result->next;
      return result;
   }
   
   bool isFinal()
   {
      if(finalized)
         return true;
      bool result = true;
      Segment *i = first;
      while(i != NULL && result)
      {
         if(!(i->placed))
            result = false;
         i = i->next;
      }
      if(result)
         finalized = true;
      return result;
   }
   
   //destructor; deletes all the segments this row contains, as segments are meaningless outside a row
   ~NonogramStripe()
   {
      cout << "Row deleted." <<endl;
      Segment *i = first;
      Segment *j;
      while(i != NULL)
      {
         j = i;
         i = i->next;
         delete j;
      }
   }
};

class NonogramRow: public NonogramStripe
{
   private:
   int rowNumber;
   
   public:
   NonogramRow(int row, int columns, GameGrid *g)
   {
      rowNumber = row;
      stripelength = columns;
      board = g;
      first = NULL;
      finalized = false;
      modified = false;
   }
   
   char cellAt(int position)
   {
      if(position < 0 || position >= stripelength)
         return ' ';
      else
         return board->at(rowNumber, position);
   }
   void fill(int position)
   {
      if(board->at(rowNumber, position) != '#')
         modified = true;
      board->fill(rowNumber, position);
   }
   void empty(int position)
   {
      if(board->at(rowNumber, position) != ' ')
         modified = true;
      board->empty(rowNumber, position);
   }
};

class NonogramColumn: public NonogramStripe
{
   private:
   int colNumber;
   
   public:
   NonogramColumn(int rows, int column, GameGrid *g)
   {
      colNumber = column;
      stripelength = rows;
      board = g;
      first = NULL;
      finalized = false;
      modified = false;
   }
   
   char cellAt(int position)
   {
      if(position <0 || position >= stripelength)
         return ' ';
      else
         return board->at(position, colNumber);
   }
   void fill(int position)
   {
      if(board->at(position, colNumber) != '#')
         modified = true;
      board->fill(position, colNumber);
   }
   void empty(int position)
   {
      if(board->at(position, colNumber) != ' ')
         modified = true;
      board->empty(position, colNumber);
   }
};

//This struct maintains the entire board,
//keeping track of where each row is and
//which has been last modified; this is necessary
//for Step 10, the "emergency check."

struct RowManager
{
   int numOfRows, numOfCols, nextToCheck, lastModified, numUnsolved;
   vector<NonogramStripe*> stripes;
   GameGrid *g;
};


#endif