#ifndef HISTOGRAMIMAGECOMPARATOR_HPP
#define HISTOGRAMIMAGECOMPARATOR_HPP
#include "IImageComparator.hpp"
#include "Utils.hpp"

class HistogramImageComparator : public IImageComparator {
    public:
        HistogramImageComparator();
        Result compare(const cv::Mat& img1, const cv::Mat& img2) override;
        void dumpImages() override;
        cv::Mat getResult() const override;
    private:
        double calculateBhattacharyyaDistance(const cv::Mat& image1, const cv::Mat& image2);

        cv::Mat image1;
        cv::Mat image2;
};

#endif //HISTOGRAMIMAGECOMPARATOR_HPP