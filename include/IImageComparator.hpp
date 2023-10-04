#ifndef IIMAGECOMPARATOR_HPP
#define IIMAGECOMPARATOR_HPP

#include "Utils.hpp"
#include <opencv2/core.hpp>

class IImageComparator {
    public:
        virtual ~IImageComparator() = default;
        virtual Result compare(const cv::Mat& image1, const cv::Mat& image2) = 0;
        virtual void dumpImages() = 0;
        virtual cv::Mat getResult() const = 0;
};

#endif //IIMAGECOMPARATOR_HPP