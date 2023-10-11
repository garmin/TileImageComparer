#include "PoliMiImageComparator.hpp"

#include <iostream>

                                         
PoliMiImageComparator::PoliMiImageComparator() {
        std::cout << "Custom image comparator!" << std::endl;
    }

Result PoliMiImageComparator::compare(const cv::Mat& image1, const cv::Mat& image2){
    return {0.0, false};
}

 cv::Mat PoliMiImageComparator::getResult() const {
    return cv::Mat::zeros(256, 256, CV_32F);
}

void PoliMiImageComparator::dumpImages() {}