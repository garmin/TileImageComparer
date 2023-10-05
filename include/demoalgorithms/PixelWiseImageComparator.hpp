#ifndef PixelWiseIMAGECOMPARATOR_HPP
#define PixelWiseIMAGECOMPARATOR_HPP
#include "IImageComparator.hpp"
#include "Utils.hpp"

class PixelWiseImageComparator : public IImageComparator {
    public:
        PixelWiseImageComparator();
        Result compare(const cv::Mat& img1, const cv::Mat& img2) override;
        void dumpImages() override;
        cv::Mat getResult() const override;
    private:
        int getDifferentPixels(const cv::Mat& i1, const cv::Mat& i2);

        cv::Mat image1;
        cv::Mat image2;
        cv::Mat result;
};

#endif //PixelWiseIMAGECOMPARATOR_HPP