#ifndef POLIMIIMAGECOMPARATOR_HPP
#define POLIMIIMAGECOMPARATOR_HPP
#include <opencv2/core.hpp>
#include "IImageComparator.hpp"

class PoliMiImageComparator : public IImageComparator {
    public:
        PoliMiImageComparator();
        Result compare(const cv::Mat& img1, const cv::Mat& img2) override;
        void dumpImages() override;
        cv::Mat getResult() const override;

    private:
        cv::Mat image1;
        cv::Mat image2;
};

#endif //POLIMIIMAGECOMPARATOR_HPP