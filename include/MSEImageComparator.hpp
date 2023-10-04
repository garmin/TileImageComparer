#ifndef MSEIMAGECOMPARATOR_HPP
#define MSEIMAGECOMPARATOR_HPP
#include "IImageComparator.hpp"
#include "Utils.hpp"

class MSEImageComparator : public IImageComparator {
    public:
        MSEImageComparator();
        Result compare(const cv::Mat& img1, const cv::Mat& img2) override;
        void dumpImages() override;
        cv::Mat getResult() const override;
    private:
        double getMSE(const cv::Mat& i1, const cv::Mat& i2);

        cv::Mat image1;
        cv::Mat image2;
        cv::Mat result;
};

#endif //MSEIMAGECOMPARATOR_HPP