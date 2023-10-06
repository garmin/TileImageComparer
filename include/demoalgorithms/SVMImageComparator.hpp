#ifndef SVMIMAGECOMPARATOR_HPP
#define SVMIMAGECOMPARATOR_HPP
#include "IImageComparator.hpp"
#include "Utils.hpp"
#include <opencv2/ml.hpp>

class SVMImageComparator : public IImageComparator {
    public:
        SVMImageComparator();
        Result compare(const cv::Mat& img1, const cv::Mat& img2) override;
        void dumpImages() override;
        cv::Mat getResult() const override;
    private:
        double compareImages(cv::Mat& image1, cv::Mat& image2);
        cv::Ptr<cv::ml::SVM> createModel();
        cv::Mat extractFeatures(cv::Mat& image);
        void train(cv::Ptr<cv::ml::SVM> model);

        cv::Mat image1;
        cv::Mat image2;
        cv::Mat result;
        cv::Ptr<cv::ml::SVM> model;
};

#endif //SVMIMAGECOMPARATOR_HPP