#include "PixelWiseImageComparator.hpp"
#include "Utils.hpp"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

PixelWiseImageComparator::PixelWiseImageComparator() {
    std::cout << "The PixelWise comparator simply counts how many pixels differs between the two images. Not very robust to image compression!" << std::endl;
}

Result PixelWiseImageComparator::compare(const cv::Mat& img1, const cv::Mat& img2) {
    this->image1 = img1;
    this->image2 = img2;
    auto pixels = getDifferentPixels(img1, img2) * 1.0;
    std::cout << "Number of different pixels = " << pixels << std::endl;
    return {pixels, pixels > 0};
}

void PixelWiseImageComparator::dumpImages() {}

cv::Mat PixelWiseImageComparator::getResult() const {
    std::cout << "In the results image, black pixels are equal, white pixels are different." << std::endl;
    return this->result;
}

int PixelWiseImageComparator::getDifferentPixels(const cv::Mat& image1, const cv::Mat& image2) {
    if (image1.size() != image2.size()) {
        return -1;
    }

    this->result = cv::Mat::zeros(image1.size(), CV_8UC3);
    int differingPixels = 0;

    for (int y = 0; y < image1.rows; ++y) {
        for (int x = 0; x < image1.cols; ++x) {
            cv::Vec3b pixel1 = image1.at<cv::Vec3b>(y, x);
            cv::Vec3b pixel2 = image2.at<cv::Vec3b>(y, x);

            if (pixel1 != pixel2) {
                ++differingPixels;
                this->result.at<cv::Vec3b>(y, x) = {255, 255, 255};
            }
        }
    }

    return differingPixels;
}