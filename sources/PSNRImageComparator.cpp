#include "PSNRImageComparator.hpp"
#include "Utils.hpp"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

PSNRImageComparator::PSNRImageComparator() {
    std::cout << "The PSNR comparator (Peak Signal-to-Noise Ratio) uses the PSNR metric, commonly used to quantify reconstruction quality for images and video subject to lossy compression." << std::endl;
}

Result PSNRImageComparator::compare(const cv::Mat& img1, const cv::Mat& img2) {
    this->image1 = img1;
    this->image2 = img2;
    auto psnr = getPSNR(img1, img2);
    psnr = std::round(psnr * 100000.0) / 100000.0; // round to 6 dec places for machine precision reasons
    std::cout << "PSNR score = " << psnr << std::endl;
    return {psnr, psnr > 0.0};
}

void PSNRImageComparator::dumpImages() {}

cv::Mat PSNRImageComparator::getResult() const {
    std::cout << "In the results image, the brighter the higher the difference. Black means equal, white means very high difference (>= 255)." << std::endl;
    return this->result;
}

double PSNRImageComparator::getPSNR(const cv::Mat& image1, const cv::Mat& image2) {
    cv::Mat mseImage;
    cv::absdiff(image1, image2, mseImage);
    mseImage.convertTo(mseImage, CV_32F); // Convert to 32-bit float
    mseImage = mseImage.mul(mseImage);    // Square the differences
    double mse = cv::mean(mseImage)[0];    // Calculate Mean Squared Error (MSE)
    double psnr = 10.0 * log10((255 * 255) / mse); // Calculate PSNR
    this->result = mseImage;
    return psnr;
}