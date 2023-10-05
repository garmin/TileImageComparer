#include "demoalgorithms/MSEImageComparator.hpp"
#include "Utils.hpp"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

MSEImageComparator::MSEImageComparator() {
    std::cout << "The MSE comparator measures the average squared difference between pixels in the two images. A lower MSE indicates greater similarity between the images." << std::endl;
}

Result MSEImageComparator::compare(const cv::Mat& img1, const cv::Mat& img2) {
    this->image1 = img1;
    this->image2 = img2;
    auto mse = getMSE(img1, img2);
    mse = std::round(mse * 100000.0) / 100000.0; // round to 6 dec places for machine precision reasons
    std::cout << "MSE score = " << mse << std::endl;
    return {mse, mse > 0.0};
}

void MSEImageComparator::dumpImages() {}

cv::Mat MSEImageComparator::getResult() const {
    std::cout << "In the results image, the brighter the higher the difference. Black means equal, white means very high difference (>= 255)." << std::endl;
    return this->result;
}

double MSEImageComparator::getMSE(const cv::Mat& image1, const cv::Mat& image2) {
    cv::Mat diff;
    cv::absdiff(image1, image2, diff);
    diff.convertTo(diff, CV_32F);
    diff = diff.mul(diff);
    cv::Scalar mse = cv::mean(diff);
    this->result = diff;
    return mse.val[0];
}