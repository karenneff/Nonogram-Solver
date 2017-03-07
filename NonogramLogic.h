//Title: NonogramLogic.h
//Date: 12-29-2015
//Description: Includes each of the logical steps necessary 
//to solve a nonogram puzzle, assuming that each row and 
//column has already been filled with the appropriate clues.

#include "NonogramObjects.h"
#include <iostream>

void step1(NonogramStripe*);
void step2(NonogramStripe*);
void step3(NonogramStripe*);
void step4(NonogramStripe*);
void step5(NonogramStripe*);
void step6(NonogramStripe*);
void step7(NonogramStripe*);
void step8(NonogramStripe*);

void check(NonogramStripe* row)
{
   step1(row);
   step3(row);
   step2(row);
   step4(row);
   if(row->isFinal())
      step5(row);
      
   else
   {
      step6(row);
      step7(row);
      step8(row);
   }
   
};

//Step 1: The most basic check.
//Each segment must be to the right of the previous one,
//and there must be at least one square separating them.
//This step is reused so often that checks from the left
//and right are separated, for efficiency.

void step1A(NonogramStripe* row)
{
   //first, from left to right
   Segment *left, *right;
   left = row->getFirstSegment();
   if(left != NULL) //the row could, conceivably, be empty
   {
      right = left->next;
      while(right != NULL)
      {
         if(right->minpos < left->minpos + left->length + 1)
         {
            right->minpos = left->minpos + left->length + 1;
            if(right->maxpos < right->minpos)
               throw "Logic error!";
         }
         right = right->next;
         left = left->next;
      }
   }
};

void step1B(NonogramStripe* row)
{
   Segment *left, *right;
   right = row->getLastSegment();
   if(left != NULL) //the row could, conceivably, be empty
   {
      //now, from right to left
      left = right->previous;
      while(left != NULL)
      {
         if(left->maxpos > right->maxpos - left->length - 1)
         {
            left->maxpos = right->maxpos - left->length - 1;
            if(left->maxpos < left->minpos)
               throw "Logic error!";
         }
         right = right->previous;
         left = left->previous;
      }
   }
};

void step1(NonogramStripe* row)
{
   step1A(row);
   step1B(row);
};

//Step 2: Includes three separate, but related, checks.
//Check 1: The proposed area for any segment may not contain
//any empty spaces. If it does, move the segment past them.
//Check 2: The proposed area for any segment must be followed
//by an empty or undecided space. If it isn't, move the segment up.
//Check 3: If, after doing this, there is a full square just before
//a proposed segment, the segment that exists there is bigger than
//the one you're trying to place. Move the segment out and start over.
void step2(NonogramStripe* row)
{
   //first, from left to right
   Segment *s = row->getFirstSegment();
   while(s != NULL)
   {
      int before = s->minpos;
      for(int i = s->minpos; i < s->minpos + s->length; i++)
      {
         if(row->cellAt(i) == ' ')
         {
            s->minpos = i+1;
            if(s->minpos > s->maxpos)
               throw "Logic error!";
         }
      }
      while(row->cellAt(s->minpos + s->length)=='#')
         s->minpos += 1;
      if(row->cellAt(s->minpos - 1)=='#')
      {
         while(row->cellAt(s->minpos - 1)=='#')
            s->minpos += 1;
         step1A(row);
      }
      else
      {
         if(before != s->minpos)
            step1A(row);
         s = s->next;
      }
   }
   
   //now, from right to left
   s = row->getLastSegment();
   while(s != NULL)
   {
      int before = s->maxpos;
      for(int i = s->maxpos + s->length - 1; i >= s->maxpos; i--)
      {
         if(row->cellAt(i) == ' ')
         {
            s->maxpos = i - s->length;
            if(s->minpos > s->maxpos)
               throw "Logic error!";
         }
      }
      while(row->cellAt(s->maxpos - 1)=='#')
         s->maxpos -= 1;
         
      if(row->cellAt(s->maxpos + s->length)=='#')
      {
         while(row->cellAt(s->maxpos + s->length)=='#')
            s->maxpos -= 1;
         step1B(row);
      }
      else
      {
         if(before != s->maxpos)
            step1B(row);
         s = s->previous;
      }
   }
   
};

//Step 3: Now we begin to deal with the needs
//of the row as a whole, not just the segments.
//If there is a full square at the beginning of
//a row, anchor the first segment to it.
//Oddly enough, it makes more sense to do this
//step before step 2.
void step3(NonogramStripe* row)
{
   //first, from left to right... sort of
   int i = 0;
   Segment* s = row->getFirstSegment();
   while(i < row->getLength() && s != NULL)
   {
      if(i >= s->maxpos + s->length)
         s = s->next;
      if(row->cellAt(i) == '#')
      {
         if(s==NULL)
            throw "Logic error!";
         if(i < s->maxpos)
         {
            s->maxpos = i;
            if(s->minpos > s->maxpos)
               throw "Logic error!";
         }
         i = s->maxpos + s->length;
      }
      else
         i++;
   }
   
   //now, from right to left... again, sort of
   i = row->getLength()-1;
   s = row->getLastSegment();
   while(i > 0 && s != NULL)
   {
      if(i < s->minpos)
         s = s->previous;
      if(row->cellAt(i) == '#')
      {
         if(s==NULL)
            throw "Logic error!";
         if(i > s->minpos + s->length - 1)
         {
            s->minpos = i - s->length + 1;
            if(s->minpos > s->maxpos)
               throw "Logic error!";
         }
         i = s->minpos - 1;
      }
      else
         i--;
   }
};

//Step 4: Having placed each segment as well as we
//can for the moment, it is time to start filling
//some squares.
//If the furthest left and the furthest right a segment
//can be are at the same spot, that segment is finalized.
//If they are closer together than the segment's length, 
//the overlapping squares, at least, can be filled.
void step4(NonogramStripe* row)
{
   //from left to right, but there is no need to do right to left as well
   Segment *s = row->getFirstSegment();
   while(s != NULL)
   {
      if(s->minpos == s->maxpos)
      {
         s->placed = true;
         row->empty(s->minpos - 1);
         row->empty(s->minpos + s->length);
      }
      int i = s->maxpos;
      while(i < s->minpos + s->length)//if minpos = maxpos, this will fill the entire segment
      {
         row->fill(i);
         i++;
      }
      s = s->next;
   }
};

//Step 5: To be used only on a row that is being "finalized",
//in other words, all segments are in their permanent places.
//Empty any squares that are not already taken up by the segments.
//If the row has not been finalized, the check function calls 
//steps 6 through 8 instead of this more simplistic one.
void step5(NonogramStripe* row)
{
   int i = 0;
   while(i < row->getLength())
   {
      if(row->cellAt(i) == '-')
         row->empty(i);
      i++;
   }
};

//Step 6: Now, we check to see if any squares can be emptied
//based on the entire row, not just a particular segment.
//This is the first of three steps: if a particular square is 
//known to be after one segment but before the next one, it is 
//empty for sure. Ends are checked as well.
//This step should not be reached if a row is finalized, and the 
//row should be finalized immediately if it contains no segments;
//therefore, the first segment is assumed to be non-null.
void step6(NonogramStripe* row)
{
   //checked from left to right; first, the far left end
   Segment* left = row->getFirstSegment();
   int i = 0;
   while(i < left->minpos)
   {
      row->empty(i);
      i++;
   }
   
   //next, the spaces between any consecutive segments
   Segment* right = left->next;
   while(right != NULL)
   {
      i = left->maxpos + left->length;
      while(i < right->minpos)
      {
         row->empty(i);
         i++;
      }
      left = left->next;
      right = right->next;
   }
   
   //finally, any space at the end of the row
   i = left->maxpos + left->length;
   while(i < row->getLength())
   {
      row->empty(i);
      i++;
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
   while(i < row->getLength())
   {
      if(row->cellAt(i) == ' ')
         i++;
      else
      {
         while(i > firstAvailable->maxpos)
            firstAvailable = firstAvailable->next;
         int minLength, start, end;
         start = i;
         while(row->cellAt(i) != ' ')
            i++;
         end = i;
         minLength = firstAvailable->length;
         Segment *s = firstAvailable->next;
         while(s != NULL)
         {
            if(s->minpos <= end && s->length < minLength)
               minLength = s->length;
            s = s->next;
         }
         if(end - start + 1 < minLength)//empty any intervening spaces
         {
            for(int j = start; j <= end; j++)
               row->empty(j);
         }
         else if(minLength > 1)//ensure minimum length will be fulfilled
         {
            //first, from left to right
            bool flag = false;
            minLength = firstAvailable->length;
            s = firstAvailable->next;
            while(s != NULL)
            {
               if(s->minpos <= start && s->length < minLength)
                  minLength = s->length;
               s = s->next;
            }
            for(int j = start; j < start + minLength; j++)
            {
               if(row->cellAt(j) == '#')
                  flag = true;
               else if(flag)
                  row->fill(j);
            }
            //then, from right to left
            flag = false;
            while(firstAvailable->maxpos < end - firstAvailable->length)
               firstAvailable = firstAvailable->next; //should not result in null pointer; if some segment can't go there, square should already be empty
            minLength = firstAvailable->length;
            s = firstAvailable->next;
            while(s != NULL)
            {
               if(s->minpos <= end && s->length < minLength)
                  minLength = s->length;
               s = s->next;
            }
            for(int j = end; j >= end - minLength; j--)
            {
               if(row->cellAt(j) == '#')
                  flag = true;
               else if(flag)
                  row->fill(j);
            }
            
         }
      }
   }
};

//Step 8: The "Maximim Length" step.
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
