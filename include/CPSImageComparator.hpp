#ifndef CPSIMAGECOMPARATOR_HPP
#define CPSIMAGECOMPARATOR_HPP
#include <opencv2/core.hpp>
#include "IImageComparator.hpp"
#include "DifferencesFinder.hpp"
#include "Utils.hpp"

class CPSImageComparator : public IImageComparator {
    public:
        CPSImageComparator(int maxBlobSize, int pxStride, int compareConstant, ComparisonThresholds thresholds);
        Result compare(const cv::Mat& img1, const cv::Mat& img2) override;
        void dumpImages() override;
        cv::Mat getResult() const override;

    private:
        int mMaxBlobSize;
        int mPxStride;
        int mCompareConstant;
        ComparisonThresholds mThresholds;

        cv::Mat image1;
        cv::Mat image2;
        cv::Mat blendedImage;
        cv::Mat comparisonImage;
        cv::Mat blobsImage;
        cv::Mat largestBlobImage;
        cv::Mat bitmaskImage;
        cv::Mat diffsImage;
};

#endif //CPSIMAGECOMPARATOR_HPP