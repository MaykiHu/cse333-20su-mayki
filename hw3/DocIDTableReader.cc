/*
 * Copyright ©2020 Travis McGaha.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Summer Quarter 2020 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./DocIDTableReader.h"

extern "C" {
  #include "libhw1/CSE333.h"
}

using std::list;

namespace hw3 {

// The constructor for DocIDTableReader calls the constructor
// of HashTableReader(), its superclass. The superclass takes
// care of taking ownership of f and using it to extract and
// cache the number of buckets within the table.
DocIDTableReader::DocIDTableReader(FILE *f, IndexFileOffset_t offset)
  : HashTableReader(f, offset) { }

bool DocIDTableReader::LookupDocID(const DocID_t &docID,
                                   list<DocPositionOffset_t> *retval) {
  // Use the superclass's "LookupElementPositions" function to
  // walk through the docIDtable and get back a list of offsets
  // to elements in the bucket for this docID.
  auto elements = LookupElementPositions(docID);

  // If the list of elements is empty, we're done.
  if (elements.empty())
    return false;

  // Iterate through the elements, looking for our docID.
  for (auto it = elements.begin(); it != elements.end(); it++) {
    IndexFileOffset_t curr = *it;

    // STEP 1.
    // Slurp the next docid out of the element.
    DocIDElementHeader currHeader;
    Verify333(fseek(file_, curr, SEEK_SET) == 0);  // get pos header for elem.
    Verify333(fread(&currHeader, sizeof(DocIDElementHeader), 1, file_) == 1);
    currHeader.toHostFormat();  // convert to host order

    // Is it a match?
    if (currHeader.docID == docID) {
      // STEP 2.
      // Yes!  Extract the positions themselves, appending to
      // std::list<DocPositionOffset_t>.  Be sure to push in the right
      // order, adding to the end of the list as you extract
      // successive positions.
      list<DocPositionOffset_t> positions;
      for (int i = 0; i < currHeader.numPositions; i++) {  // iterate positions
        DocIDElementPosition elementPos;
        Verify333(fread(&elementPos, sizeof(DocIDElementPosition), 1, file_)
                  == 1);  // Verify can read element data
        elementPos.toHostFormat();  // convert to host order
        positions.push_back(elementPos.position);  // extract positions and add
      }

      // STEP 3.
      // Return the positions list through the output parameter,
      // and return true.
      *retval = positions;
      return true;
    }
  }

  // We failed to find a matching docID, so return false.
  return false;
}

list<DocIDElementHeader> DocIDTableReader::GetDocIDList() {
  // This will be our returned list of docIDs within this table.
  list<DocIDElementHeader> docidlist;

  // Go through *all* of the buckets of this hashtable, extracting
  // out the docids in each element and the number of word positions
  // for the each docid.
  for (int i = 0; i < header_.numBuckets; i++) {
    // STEP 4.
    // Seek to the next BucketRecord.  The "offset_" member
    // variable stores the offset of this docid table within
    // the index file.

    // Next BucketRecord occurs after the table's BucketListHeader
    // and after all BucketRecords
    Verify333(fseek(file_, offset_ + sizeof(BucketListHeader) +
              sizeof(BucketRecord) * i, SEEK_SET) == 0);

    // STEP 5.
    // Read in the chain length and bucket position fields from
    // the bucket_rec.
    BucketRecord bucketRec;
    Verify333(fread(&bucketRec, sizeof(BucketRecord), 1, file_) == 1);
    bucketRec.toHostFormat();  // convert to host order

    // Sweep through the next bucket, iterating through each
    // chain element in the bucket.
    for (int j = 0; j < bucketRec.chainNumElements; j++) {
      // Seek to chain element's position field in the bucket header.
      Verify333(fseek(file_,   bucketRec.position
                             + j*sizeof(ElementPositionRecord), SEEK_SET) == 0);

      // STEP 6.
      // Read the next element position from the bucket header.
      // and seek to the element itself.
      ElementPositionRecord elementPos;
      Verify333(fread(&elementPos, sizeof(ElementPositionRecord), 1, file_)
                == 1);  // read next element position record
      elementPos.toHostFormat();  // convert to host order
      Verify333(fseek(file_, elementPos.position, SEEK_SET) == 0);  // seek pos

      // STEP 7.
      // Read in the docid and number of positions from the element.
      DocIDElementHeader element;
      Verify333(fread(&element, sizeof(DocIDElementHeader), 1, file_) == 1);
      element.toHostFormat();  // convert to host order

      // Append it to our result list.
      docidlist.push_back(element);
    }
  }

  // Done!  Return the result list.
  return docidlist;
}

}  // namespace hw3
