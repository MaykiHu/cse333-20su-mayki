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

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using std::vector;

namespace hw4 {

static const char *kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::GetNextRequest(HttpRequest *request) {
  // Use "WrappedRead" to read data into the buffer_
  // instance variable.  Keep reading data until either the
  // connection drops or you see a "\r\n\r\n" that demarcates
  // the end of the request header. Be sure to try and read in
  // a large amount of bytes each time you call WrappedRead.
  //
  // Once you've seen the request header, use ParseRequest()
  // to parse the header into the *request argument.
  //
  // Very tricky part:  clients can send back-to-back requests
  // on the same socket.  So, you need to preserve everything
  // after the "\r\n\r\n" in buffer_ for the next time the
  // caller invokes GetNextRequest()!

  // STEP 1:
  size_t found = buffer_.find(kHeaderEnd);  // check if header exists
  if (found == string::npos) {  // no header end was found, we need to read
    unsigned char buf[1024];
    int bytesRead = -1;
    // Keep reading data until connection drops or header end found
    while (bytesRead != 0 && found == string::npos) {
      bytesRead = WrappedRead(fd_, buf, 1024);
      if (bytesRead == -1) {  // fatal error occured
        return false;
      } else if (bytesRead == 0) {  // connection dropped / EOF
        continue;  // no need to add to buffer_
      } else {  // we read the bytes; need to add to buffer_
        buffer_ += std::string(reinterpret_cast<char *>(buf), bytesRead);
        found = buffer_.find(kHeaderEnd);  // update if we've found header end
      }
    }
  }  // finished reading
  if (found == string::npos) {  // if we haven't read header end
    return false;
  }  // else, we have seen the request header so parse it
  *request = ParseRequest(buffer_.substr(0, found + kHeaderEndLen));
  buffer_ = buffer_.substr(found + kHeaderEndLen);  // preserve after header

  return true;  // You may want to change this.
}

bool HttpConnection::WriteResponse(const HttpResponse &response) {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         (unsigned char *) str.c_str(),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string &request) {
  HttpRequest req("/");  // by default, get "/".

  // Split the request into lines.  Extract the URI from the first line
  // and store it in req.URI.  For each additional line beyond the
  // first, extract out the header name and value and store them in
  // req.headers_ (i.e., HttpRequest::AddHeader).  You should look
  // at HttpRequest.h for details about the HTTP header format that
  // you need to parse.
  //
  // You'll probably want to look up boost functions for (a) splitting
  // a string into lines on a "\r\n" delimiter, (b) trimming
  // whitespace from the end of a string, and (c) converting a string
  // to lowercase.
  //
  // Note that you may assume the request you are parsing is correctly
  // formatted. If for some reason you encounter a header that is
  // malformed, you may skip that line.

  // STEP 2:
  std::vector<std::string> lines;  // split request into lines
  boost::split(lines, request, boost::is_any_of("\r\n"),
               boost::token_compress_on);
  for (uint32_t i = 0; i < lines.size(); i++) {  // trim whitespace from lines
    boost::trim(lines[i]);
  }
  std::vector<std::string> firstLine;  // first line of req
  boost::split(firstLine, lines[0], boost::is_any_of(" "),
               boost::token_compress_on);  // parse first line
  if (firstLine.size() == 3) {  // request first line should have 3 args
    req.set_uri(firstLine[1]);  // the param at index 1 is the URI
  }  // now we extract the header info
  std::vector<std::string> header;
  for (uint32_t i = 1; i < lines.size(); i++) {  // iterate through header lines
    boost::split(header, lines[i], boost::is_any_of(": "),
                 boost::token_compress_on);  // [headername], [headerval]
    if (header.size() == 2) {  // is correctly formed, assume format is correct
      boost::to_lower(header[0]);  // maker header name lowercase
      req.AddHeader(header[0], header[1]);
    }  // otherwise request is malformed and we can skip this line
  }
  return req;
}

}  // namespace hw4
