#pragma once
#include "Constants.hpp"
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

struct DiffStats {
  int mLeft, mTop, mWidth, mHeight, mArea;
};

using DiffStatsCollection = std::vector<DiffStats>;

struct ImageStats {
  int mWidth, mHeight, mChannels;

  ImageStats(const int &aWidth, const int &aHeight, const int &aChannels)
      : mWidth(aWidth), mHeight(aHeight), mChannels(aChannels) {}

  ImageStats() : ImageStats(0, 0, 0) {}
};

struct ComparisonThresholds {
  // Thresholds in decimal forms. e.g. 0.05 => 5%
  double mAreaThreshold, mExtentThreshold;

  ComparisonThresholds(const double &aAreaThreshold,
                       const double &aExtentThreshold)
      : mAreaThreshold(aAreaThreshold), mExtentThreshold(aExtentThreshold){};
  ComparisonThresholds() : ComparisonThresholds(0.0, 0.0){};
};

cv::Mat PreprocessImageChannel(cv::Mat &aImg1, cv::Mat &aImg2,
                               const std::filesystem::path &aArtefactLocation,
                               const bool aSaveIntermediateResults,
                               const std::string &aArtefactsSuffix);

bool Check(const ImageStats &aImageStats, const DiffStatsCollection &aDiffCollection, const ComparisonThresholds &aCmpParameters);

std::pair<ImageStats, DiffStatsCollection> FindDifferences(const cv::Mat &image1, const cv::Mat &aImgimage1);

DiffStatsCollection CollectDiff(cv::Mat &aBitmask);
