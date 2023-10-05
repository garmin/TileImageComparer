#ifndef WhiteLineIMAGECOMPARATOR_HPP
#define WhiteLineIMAGECOMPARATOR_HPP
#include "IImageComparator.hpp"
#include "Utils.hpp"

class WhiteLineImageComparator : public IImageComparator {
    public:
        WhiteLineImageComparator();
        Result compare(const cv::Mat& img1, const cv::Mat& img2) override;
        void dumpImages() override;
        cv::Mat getResult() const override;
    private:
        bool hasWhiteLine(const cv::Mat& image);

        cv::Mat image1;
        cv::Mat image2;
        cv::Mat result;
};

#endif //WhiteLineIMAGECOMPARATOR_HPP