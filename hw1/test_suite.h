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

#ifndef HW1_TEST_SUITE_H_
#define HW1_TEST_SUITE_H_

#include "gtest/gtest.h"

class HW1Environment : public ::testing::Environment {
 public:
  static void AddPoints(int points);

  // These are run once for the entire test environment:
  virtual void SetUp();
  virtual void TearDown();

 private:
  static constexpr int HW1_MAXPOINTS = 215;
  static int points_;
};


#endif  // HW1_TEST_SUITE_H_
