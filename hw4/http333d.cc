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

#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <list>

#include "./ServerSocket.h"
#include "./HttpServer.h"

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::string;

static const int MIN_ARGS = 4;
static const int MIN_PORT = 1024;
static const int MAX_PORT = 65535;

// Print out program usage, and exit() with EXIT_FAILURE.
static void Usage(char *progname);

// Parses the command-line arguments, invokes Usage() on failure.
// "port" is a return parameter to the port number to listen on,
// "path" is a return parameter to the directory containing
// our static files, and "indices" is a return parameter to a
// list of index filenames.  Ensures that the path is a readable
// directory, and the index filenames are readable, and if not,
// invokes Usage() to exit.
static void GetPortAndPath(int argc,
                    char **argv,
                    uint16_t *port,
                    string *path,
                    list<string> *indices);

int main(int argc, char **argv) {
  // Print out welcome message.
  cout << "Welcome to http333d, the UW cse333 web server!" << endl;
  cout << "  Copyright 2012 Steven Gribble" << endl;
  cout << "  http://www.cs.washington.edu/homes/gribble" << endl;
  cout << endl;
  cout << "initializing:" << endl;
  cout << "  parsing port number and static files directory..." << endl;

  // Ignore the SIGPIPE signal, otherwise we'll crash out if a client
  // disconnects unexpectedly.
  signal(SIGPIPE, SIG_IGN);

  // Get the port number and list of index files.
  uint16_t portnum;
  string staticdir;
  list<string> indices;
  GetPortAndPath(argc, argv, &portnum, &staticdir, &indices);
  cout << "    port: " << portnum << endl;
  cout << "    path: " << staticdir << endl;

  // Run the server.
  hw4::HttpServer hs(portnum, staticdir, indices);
  if (!hs.Run()) {
    cerr << "  server failed to run!?" << endl;
  }

  cout << "server completed!  Exiting." << endl;
  return EXIT_SUCCESS;
}


static void Usage(char *progname) {
  cerr << "Usage: " << progname << " port staticfiles_directory indices+";
  cerr << endl;
  exit(EXIT_FAILURE);
}

static void GetPortAndPath(int argc,
                    char **argv,
                    uint16_t *port,
                    string *path,
                    list<string> *indices) {
  // Be sure to check a few things:
  //  (a) that you have a sane number of command line arguments
  //  (b) that the port number is reasonable
  //  (c) that "path" (i.e., argv[2]) is a readable directory
  //  (d) that you have at least one index, and that all indices
  //      are readable files.
  char *progName = argv[0];
  // STEP 1:
  // Check we have valid number of args (at least 4)
  // ex: ./http333d 5555 ../projdocs unit_test_indices/*
  if (argc < MIN_ARGS) {  // if invalid # of args
    Usage(progName);
  }  // Cool, check for reasonable port number (1024 <= portNum < 65536 (2^16))
  uint16_t portNum = atol(argv[1]);  // port number at 1st index
  if (portNum < MIN_PORT || portNum > MAX_PORT) {  // if invalid portNum
    Usage(progName);
  }  // Cool, check for path (argv[2]) is readable/reachable directory
  *port = portNum;  // update port
  char *dirName = argv[2];
  struct stat dirStat;
  if (stat(dirName, &dirStat) == -1) {  // error getting stat/not readable
    Usage(progName);
  } else {  // We're able to reach path; see if is directory
    if (!S_ISDIR(dirStat.st_mode)) {  // is not a directory
      Usage(progName);
    }
  }
  *path = string(dirName);  // update path
  list<string> files;
  // Last check on at least one index and indices are readable files
  for (int i = MIN_ARGS - 1; i < argc; i++) {  // iterate indices
    string fileName(argv[i]);
    struct stat fileStat;
    if (stat(fileName.c_str(), &fileStat) == -1) {  // can't read file
      Usage(progName);
    }  // Cool, see if it's a file
    if (!S_ISREG(fileStat.st_mode)) {  // is not a file
      Usage(progName);
    }  // Yay!!  We've verified this index as a readable file!  So add..
    files.push_back(argv[i]);
  }
  if (files.size() == 0) {  // we don't have at least one index
    Usage(progName);
  }  // Yay!!!!!!!  We've checked all requirements.
  *indices = files;
}

