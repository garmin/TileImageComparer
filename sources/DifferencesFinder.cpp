#include "Constants.hpp"
#include "DifferencesFinder.hpp"
#include "Utils.hpp"
#include <filesystem>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat PreprocessImageChannel(cv::Mat &aImg1, cv::Mat &aImg2,
                               const std::filesystem::path &aArtefactLocation,
                               const bool aSaveIntermediateResults,
                               const std::string &aArtefactsSuffix) {
    assert(aImg1.channels() == aImg2.channels());
    cv::Mat diff, diffFiltered, diffBinary, diffBinaryUnion;

    // 1 single-channel difference
    cv::absdiff(aImg1, aImg2, diff);

    // 2 Opening to remove major pixel differences
    //   caused by compression
    auto kernelFilter = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(kFilteringKernelSize, kFilteringKernelSize));
    cv::morphologyEx(diff, diffFiltered, cv::MorphTypes::MORPH_OPEN, kernelFilter);

    // 3 Binarize image
    cv::threshold(diffFiltered, diffBinary, kComparisonBinarizationRgbThreshold,
                255.0, cv::ThresholdTypes::THRESH_BINARY);

    // 4 Open: connect small regions separated by a small gap.
    //   Avoid having lots of small diff areas.
    kernelFilter = cv::getStructuringElement(
        cv::MorphShapes::MORPH_RECT,
        cv::Size(kBlobFilteringKernelSize, kBlobFilteringKernelSize));
    cv::morphologyEx(diffBinary, diffBinaryUnion, cv::MorphTypes::MORPH_OPEN,
                    kernelFilter);

    assert(diffBinaryUnion.channels() == 1);

    return diffBinaryUnion;
}

bool Check(const ImageStats &aImageStats,
           const DiffStatsCollection &aDiffCollection,
           const ComparisonThresholds &aCmpParameters) {
  long diffArea{0};
  int widestDiff{0};
  int highestDiff{0};

  for (const auto &diffstats : aDiffCollection) {
    diffArea += diffstats.mArea;
    widestDiff = std::max({widestDiff, diffstats.mWidth});
    highestDiff = std::max({highestDiff, diffstats.mHeight});
  }

  // total number of pixel on a single channel
  long imageTotalArea = aImageStats.mHeight * aImageStats.mWidth;
  // normalize stats
  double widestDiffNorm = static_cast<double>(widestDiff) / aImageStats.mWidth;
  double highestDiffNorm =
      static_cast<double>(highestDiff) / aImageStats.mHeight;
  double diffAreaNorm = static_cast<double>(diffArea) / imageTotalArea;

  bool status = true;
  if (diffAreaNorm > aCmpParameters.mAreaThreshold) {
    status = false;
  }
  if ((highestDiffNorm > aCmpParameters.mExtentThreshold) ||
      (widestDiffNorm > aCmpParameters.mExtentThreshold)) {
    status = false;
  }

  return status;
}

 std::pair<ImageStats, DiffStatsCollection> FindDifferences(const cv::Mat &image1, const cv::Mat &image2) {

    ImageStats stats{image1.cols, image1.rows, image1.channels()};

    std::vector<cv::Mat> channelsImg1, channelsImg2;
    split(image1, channelsImg1);
    split(image2, channelsImg2);

    cv::Mat bitmaskUnion = cv::Mat(image1.rows, image1.cols, CV_8UC1, cv::Scalar(0));
    std::vector<std::string> suffix = {"_b", "_g", "_r"};
    auto nChannels = 3;
    for (size_t channelIdx = 0; channelIdx < nChannels; channelIdx++) {
        auto bitmask = PreprocessImageChannel(
            channelsImg1.at(channelIdx), channelsImg2.at(channelIdx),
            "intermediates/", true, suffix.at(channelIdx));
        cv::bitwise_or(bitmaskUnion, bitmask, bitmaskUnion);
    }
    auto kernelFilter = cv::getStructuringElement(
        cv::MORPH_ELLIPSE,
        cv::Size(kBlobFilteringKernelSize, kBlobFilteringKernelSize));
    cv::morphologyEx(bitmaskUnion, bitmaskUnion, cv::MorphTypes::MORPH_OPEN,
                    kernelFilter);

    auto bitmaskLocation = "intermediates/diff_bitmask.png";
    SaveImage(bitmaskUnion, bitmaskLocation);
    return std::make_pair(stats, CollectDiff(bitmaskUnion));
 }

 DiffStatsCollection CollectDiff(cv::Mat &aBitmask) {
  // 5 Identify diff areas and compute stats
  cv::Mat labels, stats, centroids;
  auto numComponents = cv::connectedComponentsWithStats(aBitmask, labels, stats, centroids, 8, CV_32S);

  DiffStatsCollection diffCollection;
  // the first label found is the background label, so...
  if (numComponents < 2) {
    return diffCollection;
  }

  // start from the second (idx = 1)
  for (size_t componentIdx = 1; componentIdx < numComponents; componentIdx++) {
    DiffStats diffStats;
    diffStats.mLeft =
        stats.at<int>(static_cast<int>(componentIdx), cv::CC_STAT_LEFT);
    diffStats.mTop =
        stats.at<int>(static_cast<int>(componentIdx), cv::CC_STAT_TOP);
    diffStats.mWidth =
        stats.at<int>(static_cast<int>(componentIdx), cv::CC_STAT_WIDTH);
    diffStats.mHeight =
        stats.at<int>(static_cast<int>(componentIdx), cv::CC_STAT_HEIGHT);
    diffStats.mArea =
        stats.at<int>(static_cast<int>(componentIdx), cv::CC_STAT_AREA);
    diffCollection.push_back(diffStats);
  }
  return diffCollection;
}