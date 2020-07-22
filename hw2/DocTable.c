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
#include "./DocTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libhw1/CSE333.h"
#include "libhw1/HashTable.h"

#define HASHTABLE_INITIAL_NUM_BUCKETS 2

// This structure represents a DocTable; it contains two hash tables, one
// mapping from docid to docname, and one mapping from docname to docid.
struct doctable_st {
  HashTable *docid_to_docname;  // mapping docid to docname
  HashTable *docname_to_docid;  // mapping docname to docid
  DocID_t    max_id;            // max docID allocated so far
};

// A function for freeing a DocTable's payload
static void DocValue_Free(HTValue_t value);

DocTable* DocTable_Allocate(void) {
  DocTable *dt = (DocTable *) malloc(sizeof(DocTable));

  dt->docid_to_docname = HashTable_Allocate(HASHTABLE_INITIAL_NUM_BUCKETS);
  dt->docname_to_docid = HashTable_Allocate(HASHTABLE_INITIAL_NUM_BUCKETS);
  dt->max_id = 1;  // we reserve max_id = 0 for the invalid docID

  return dt;
}

void DocTable_Free(DocTable *table) {
  Verify333(table != NULL);

  // STEP 1.
  HashTable_Free(table->docid_to_docname, &DocValue_Free);
  HashTable_Free(table->docname_to_docid, &DocValue_Free);
  table->max_id = 0;  // empty
  free(table);
}

int DocTable_NumDocs(DocTable *table) {
  Verify333(table != NULL);
  return HashTable_NumElements(table->docid_to_docname);
}

DocID_t DocTable_Add(DocTable *table, char *docname) {
  char *doccopy;
  DocID_t *docid;
  DocID_t res;
  HTKeyValue_t kv, oldkv;

  Verify333(table != NULL);

  // STEP 2.
  // Check to see if the document already exists.  Then make a copy of the
  // docname and allocate space for the new ID.
  res = DocTable_GetDocID(table, docname); 
  if (res != INVALID_DOCID) {  // doc already exists
    return res;
  }
  int nameLength = strlen(docname) + 1;  // for '\0'
  doccopy = (char *) malloc(nameLength);
  snprintf(doccopy, nameLength, "%s", docname);  // make a copy of docname
  docid = (DocID_t *) malloc(sizeof(DocID_t));  // allocate space for new ID
  *docid = table->max_id;
  table->max_id++;

  // STEP 3.
  // Set up the key/value for the id->name mapping, and do the insert.
  kv.key = (HTKey_t) *docid;
  kv.value = (HTValue_t) doccopy;
  Verify333(HashTable_Insert(table->docid_to_docname, kv, &oldkv) == false);
  // STEP 4.
  // Set up the key/value for the name->id, and/ do the insert.
  // Be careful about how you calculate the key for this mapping.
  // You want to be sure that how you do this is consistent with
  // the provided code.
  kv.key = FNVHash64((unsigned char *) doccopy, strlen(doccopy));
  kv.value = (HTValue_t) docid;
  Verify333(HashTable_Insert(table->docname_to_docid, kv, &oldkv) == false);
  return *docid;
}

DocID_t DocTable_GetDocID(DocTable *table, char *docname) {
  HTKey_t key;
  HTKeyValue_t kv;
  int res;

  Verify333(table != NULL);
  Verify333(docname != NULL);

  // STEP 5.
  // Try to find the passed-in doc in docname_to_docid table.
  key = FNVHash64((unsigned char *) docname, strlen(docname));
  res = HashTable_Find(table->docname_to_docid, key, &kv);
  if (res) {  // docname key was found
    return *(DocID_t *) kv.value;  // return found docid to this docname
  }
  return INVALID_DOCID;  // you may want to change this
}

char *DocTable_GetDocName(DocTable *table, DocID_t docid) {
  HTKeyValue_t kv;

  Verify333(table != NULL);
  Verify333(docid != INVALID_DOCID);

  // STEP 6.
  // Lookup the docid in the docid_to_docname table,
  // and either return the string (i.e., the (char *)
  // saved in the value field for that key) or
  // NULL if the key isn't in the table.
  bool hasFound = HashTable_Find(table->docid_to_docname, docid, &kv);
  if (hasFound) {  // docid key was found
    return kv.value;
  }
  return NULL;  // you may want to change this
}

HashTable* DT_GetDocidToDocnameTable(DocTable *table) {
  Verify333(table != NULL);
  return table->docid_to_docname;
}

HashTable* DT_GetDocnameToDocidTable(DocTable *table) {
  Verify333(table != NULL);
  return table->docname_to_docid;
}

static void DocValue_Free(HTValue_t value) {
  Verify333(value != NULL);
  free(value);
}
