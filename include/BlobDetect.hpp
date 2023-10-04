//! BlobDetect.h
//! Copyright 2023 Garmin Ltd. or its subsidiaries

#pragma once
#include <vector>

class BlobDetect {
public:
  void AssignPointToBlob(const std::pair<int, int> &aPoint, std::vector<std::vector<std::pair<int, int>>> &aList);
  std::vector<std::pair<int, int>> LargestBlob(const std::vector<std::vector<std::pair<int, int>>> &aList);
  std::vector<std::vector<std::pair<int, int>>> WeedOutMeaninglessBlobs(const std::pair<int, int> &aPoint, const std::vector<std::vector<std::pair<int, int>>> &aList);
  // std::vector<std::pair<int, int>> FindBiggestBlob(std::vector<std::pair<int, int>> &aPointList);
};