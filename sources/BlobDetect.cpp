//! BlobDetect.cpp
//! Copyright 2023 Garmin Ltd. or its subsidiaries
#include <algorithm>

#include "BlobDetect.hpp"
#include "Constants.hpp"

/*
std::vector<std::pair<int, int>> BlobDetect::FindBiggestBlob(std::vector<std::pair<int, int>> &aPointList) {
  std::vector<std::vector<std::pair<int, int>>> listOfPointLists{ {} };

  // For each error point, find which blob it gets assigned to
  for (std::pair<int, int> point : aPointList) {
    AssignPointToBlob(point, listOfPointLists);
    listOfPointLists = WeedOutMeaninglessBlobs(point, listOfPointLists);
    auto blob = LargestBlob(listOfPointLists);
    if (blob.size() == kMaxBlobSize) {
      // Escape early
      return blob;
    }
  }
  return LargestBlob(listOfPointLists);
}
*/

void BlobDetect::AssignPointToBlob(const std::pair<int, int> &aPoint, std::vector<std::vector<std::pair<int, int>>> &aList) {
  // Create theoretical test points to compare to existing error points.  Left, above-left, above and above-right of the point.
  std::vector<std::pair<int, int>> testPoints = { {aPoint.first - 1, aPoint.second}, {aPoint.first - 1, aPoint.second - 1},
  {aPoint.first, aPoint.second - 1}, {aPoint.first + 1, aPoint.second - 1} };

  std::vector<int> indexes;

  int index = 0;
  std::size_t sum = 0;
  for (auto &&i : aList[0]) {
    sum++;
  }
  if (sum == 0)
  {
    aList[index].push_back(std::make_pair(aPoint.first, aPoint.second));
  }
  else {
    int index = 0;
    bool match = false;
    for (std::vector<std::pair<int, int>> list : aList) {
      for (std::pair<int, int> listPoint : list) {
        for (std::pair<int, int> testPoint : testPoints) { // each blob point
          if (testPoint.first == listPoint.first && testPoint.second == listPoint.second) {
            // This is a match.
            match = true;
            break;
          }
        }
        
        if (match) {
          aList[index].push_back(std::make_pair(aPoint.first, aPoint.second));
          if (std::find(indexes.begin(), indexes.end(), index) == indexes.end()) {
            indexes.push_back(index);
          }
          break;
        }
      }
      index++;
      match = false;
    }

    if (indexes.size() == 0) {
      if (aList.back().size() == 0) {
        aList.back().push_back({ aPoint.first, aPoint.second });
      }
      else {
        aList.push_back({ {aPoint.first, aPoint.second} });
      }
    }
  }

  if (indexes.size() == 2) {
    auto x = indexes.back();
    for (std::pair<int, int> thisPoint : aList[indexes.back()]) {
      if (std::find(aList[indexes.front()].begin(), aList[indexes.front()].end(), thisPoint) == aList[indexes.front()].end()) {
        aList[indexes.front()].push_back(std::make_pair(aPoint.first, aPoint.second));
      }
    }
    aList[indexes.back()].erase(aList[indexes.back()].begin(), aList[indexes.back()].end());
  }
}

std::vector<std::pair<int, int>> BlobDetect::LargestBlob(const std::vector<std::vector<std::pair<int, int>>> &aList) {
  std::vector<std::pair<int, int>> max = {};
  for (std::vector<std::pair<int, int>> list : aList) {
    if (list.size() > max.size()) {
      max = list;
    }
  }
  return max;
}

std::vector<std::vector<std::pair<int, int>>> BlobDetect::WeedOutMeaninglessBlobs(const std::pair<int, int> &aPoint, const std::vector<std::vector<std::pair<int, int>>> &aList) {
  std::vector<std::vector<std::pair<int, int>>> tempList;
  for (int i = 0; i < aList.size(); ++i) {
    if (aList[i].size() == 1) {
      if (aList[i][0].second < aPoint.second - 2) {
        int x = 1;
      }
      else {
        tempList.push_back({ aList[i] });
      }
    }
    else {
      tempList.push_back({ aList[i] });
    }
  }
  return tempList;
}