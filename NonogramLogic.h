//Title: NonogramLogic.h
//Date: 12-29-2015
//Description: Includes each of the logical steps necessary 
//to solve a nonogram puzzle, assuming that each row and 
//column has already been filled with the appropriate clues.

#include "NonogramObjects.h"
#include <iostream>

void putSegmentsInOrder(NonogramStripe*);
void accountForExistingFills(NonogramStripe*);
void validateSegmentPositions(NonogramStripe*);
void fillKnownSquares(NonogramStripe*);
void emptyKnownSquares(NonogramStripe*);
void step7(NonogramStripe*);
void step8(NonogramStripe*);

void check(NonogramStripe* row)
{
   putSegmentsInOrder(row);
   accountForExistingFills(row);
   validateSegmentPositions(row);
   fillKnownSquares(row);
   emptyKnownSquares(row);
   if (!(row->isFinal()))
   {
	   step7(row);
	   step8(row);
   }
};

//The first and most basic check.
//Segments must appear in the correct order,
//and there must be at least one square separating them.

void orderFromLeft(NonogramStripe* row)
{
   Segment *left, *right;
   left = row->getFirstSegment();
   if(left != NULL) //the row could be empty
   {
      right = left->next;
      while(right != NULL)
      {
		 int correctPos = left->minpos + left->length + 1;
         if(right->minpos < correctPos)
         {
            right->minpos = correctPos;
            if(right->maxpos < right->minpos)
               throw "Logic error!";
         }
         right = right->next;
         left = left->next;
      }
   }
};

void orderFromRight(NonogramStripe* row)
{
   Segment *left, *right;
   right = row->getLastSegment();
   if(right != NULL) //the row could be empty
   {
      left = right->previous;
      while(left != NULL)
      {
		 int correctPos = right->maxpos - left->length - 1;
         if(left->maxpos > correctPos)
         {
            left->maxpos = correctPos;
            if(left->maxpos < left->minpos)
               throw "Logic error!";
         }
         right = right->previous;
         left = left->previous;
      }
   }
};

void putSegmentsInOrder(NonogramStripe* row)
{
   orderFromLeft(row);
   orderFromRight(row);
};

//If the row already contains one or more filled squares,
//make sure that a segment can reach it.
//For example,
//if there is a full square at the beginning of
//a row, anchor the first segment to it.
void anchorLeft(NonogramStripe* row)
{
	int i = 0;
	Segment* s = row->getFirstSegment();
	while (i < row->getLength() && s != NULL)
	{
		if (i >= s->maxpos + s->length)
			s = s->next;
		if (row->cellAt(i) == '#')
		{
			if (s == NULL)
				throw "Logic error!";
			if (i < s->maxpos)
			{
				s->maxpos = i;
				if (s->minpos > s->maxpos)
					throw "Logic error!";
			}
			i = s->maxpos + s->length;
		}
		else
			i++;
	}
}

void anchorRight(NonogramStripe* row)
{
	int i = row->getLength() - 1;
	Segment* s = row->getLastSegment();
	while (i >= 0 && s != NULL)
	{
		if (i < s->minpos)
			s = s->previous;
		if (row->cellAt(i) == '#')
		{
			if (s == NULL)
				throw "Logic error!";
			if (i > s->minpos + s->length - 1)
			{
				s->minpos = i - s->length + 1;
				if (s->minpos > s->maxpos)
					throw "Logic error!";
			}
			i = s->minpos - 1;
		}
		else
			i--;
	}
}

void accountForExistingFills(NonogramStripe* row)
{
	anchorLeft(row);
	anchorRight(row);
};

//Validation of each segment's placement.
//No square within a segment may be empty,
//and the squares immediately before and after
//a segment may not be full.
void validLeft(NonogramStripe* row)
{
	Segment *s = row->getFirstSegment();
	while (s != NULL)
	{
		int before = s->minpos;
		int i = before;
		while (i < s->minpos + s->length)
		{
			if (row->cellAt(i) == ' ')
			{
				s->minpos = i + 1;
				if (s->minpos > s->maxpos)
					throw "Logic error!";
			}
			while(row->cellAt(s->minpos + s->length) == '#')
				s->minpos += 1;
			while (row->cellAt(s->minpos - 1) == '#')
				s->minpos += 1;
			i++;
			if (i < s->minpos)
				i = s->minpos;
		}
		if (before != s->minpos)
			orderFromLeft(row);
		s = s->next;
	}
}

void validRight(NonogramStripe* row)
{
	Segment* s = row->getLastSegment();
	while (s != NULL)
	{
		int before = s->maxpos;
		int i = s->maxpos + s->length - 1;
		while (i >= s->maxpos)
		{
			if (row->cellAt(i) == ' ')
			{
				s->maxpos = i - s->length;
				if (s->minpos > s->maxpos)
					throw "Logic error!";
			}
			while (row->cellAt(s->maxpos - 1) == '#')
				s->maxpos -= 1;
			while (row->cellAt(s->maxpos + s->length) == '#')
				s->maxpos -= 1;
			i--;
			if (i > s->maxpos + s->length - 1)
				i = s->maxpos + s->length - 1;
		}
		if (before != s->maxpos)
			orderFromRight(row);
		s = s->previous;
		
	}
}

void validateSegmentPositions(NonogramStripe* row)
{
	validLeft(row);
	validRight(row);
};

//If the furthest left and the furthest right a segment
//can be are at the same spot, that segment is finalized,
//and all squares within it are filled.
//If they are closer together than the segment's length, 
//the overlapping squares, at least, can be filled.
void fillKnownSquares(NonogramStripe* row)
{
   //from left to right, but there is no need to do right to left as well
   Segment *s = row->getFirstSegment();
   while(s != NULL)
   {
      if(s->minpos == s->maxpos)
         s->placed = true;
	  row->fill(s->maxpos, s->minpos + s->length);//if minpos = maxpos, this will fill the entire segment
      s = s->next;
   }
};

//Empty any squares before the first segment,
//known to be between a particular segment and the next,
//or after the last segment.
void emptyKnownSquares(NonogramStripe* row)
{
	//from left to right, but there is no need to do right to left as well
   Segment* s = row->getFirstSegment();
   int i = 0;
   while(i < row->getLength())
   {
	   int last;
	   if (s == NULL)
		   last = row->getLength();
	   else
		   last = s->minpos;
	   row->empty(i,last);
	   if (s == NULL)
		   break;
	   i = s->maxpos + s->length;
	   s = s->next;
	   
   }
};

//Step 7: The "minimum length" step.
//If ALL the available segments for a particular section
//are of a minimum size, empty any undecided
//spaces that are less than that length; also, if one
//or more full spaces exist, fill in enough spaces to ensure
//that the minimum length will be met.
void step7(NonogramStripe* row)
{
	int i = 0;
	Segment *firstAvailable = row->getFirstSegment();
	while (i < row->getLength())
	{
		if (row->cellAt(i) == ' ')
			i++;
		else
		{
			while (i > firstAvailable->maxpos)
				firstAvailable = firstAvailable->next;
			int minLength, start, end;
			start = i;
			while (row->cellAt(i) != ' ')
				i++;
			end = i;
			minLength = firstAvailable->length;
			Segment *s = firstAvailable->next;
			while (s != NULL)
			{
				if (s->minpos <= end && s->length < minLength)
					minLength = s->length;
				s = s->next;
			}
			if (end - start + 1 < minLength)//empty any intervening spaces
				row->empty(start, end + 1);
			else if (minLength > 1)//ensure minimum length will be fulfilled
			{
				//first, from left to right
				minLength = firstAvailable->length;
				s = firstAvailable->next;
				while (s != NULL)
				{
					if (s->minpos <= start && s->length < minLength)
						minLength = s->length;
					s = s->next;
				}
				int j = start;
				while (row->cellAt(j) != '#' && j < start + minLength)
					j++;
				row->fill(j, start + minLength);
				//then, from right to left
				while (firstAvailable->maxpos < end - firstAvailable->length)
					firstAvailable = firstAvailable->next; //should not result in null pointer; if some segment can't go there, square should already be empty
				minLength = firstAvailable->length;
				s = firstAvailable->next;
				while (s != NULL)
				{
					if (s->minpos <= end && s->length < minLength)
						minLength = s->length;
					s = s->next;
				}
				j = end;
				while (row->cellAt(j) != '#' && j >= end - minLength)
					j--;
				row->fill(end - minLength, j);
			}
		}
	}
};

//Step 8: The "Maximum Length" step.
//If a section of all filled squares is as long as the longest
//of the available segments, empty the squares to the left and
//right of it. Also, if filling one square will create a section
//that is too long to fit any of the available segments, empty it.
//This step includes another advanced step, "step 9": 
//if the last available segment is in fact longer than the 
//filled segment in question, BUT it can only fit in that spot
//if the filled segment is at its left end, then the square just
//before the filled segment (but not the one after it) can be emptied.
//Same goes from the right.
void step8(NonogramStripe* row)
{
   int i = 0;
   Segment *firstAvailable = row->getFirstSegment();
   while(i < row->getLength())
   {
      if(row->cellAt(i) != '#')
         i++;
      else //filled square is encountered
      {
         while(i > firstAvailable->maxpos + firstAvailable->length - 1)
            firstAvailable = firstAvailable->next;
         int maxLength, start, end;
         start = i;
         //first, find out how many filled squares in a row there are
         while(row->cellAt(i) == '#')
            i++;
         end = i;
         //next, find out the maximum length of the available segments
         maxLength = firstAvailable->length;
         Segment *s = firstAvailable->next;
         while(s != NULL)
         {
            //Step 9 as performed from left to right
            if(s->minpos == start && end - start == maxLength)
               row->empty(start - 1);
            if(s->minpos < end && s->length > maxLength)
               maxLength = s->length;
            s = s->next;
         }
         if(end - start == maxLength)//empty spaces before and after
         {
            row->empty(end);
            row->empty(start-1);
         }
         else if(row->cellAt(end) != ' ')//see if filling next square will join two segments, exceeding length
         {
            int newEnd = end + 1;
            while(row->cellAt(newEnd) == '#')
               newEnd++;
            if(newEnd-start > maxLength)
               row->empty(end);
         }
         //Step 9, as performed from right to left
         if(firstAvailable->maxpos + firstAvailable->length == end)
         {
            Segment *s = firstAvailable->next;
            if(s != NULL)
            {
               maxLength = s->length;
               while(s != NULL)
               {
                  if(s->minpos < end && s->length > maxLength)
                     maxLength = s->length;
                  s = s->next;
               }
               if(end - start == maxLength)
                  row->empty(end);
               
            }
         }
      }
   }
};
