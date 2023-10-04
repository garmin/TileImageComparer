#include "HistogramImageComparator.hpp"
#include "Utils.hpp"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

HistogramImageComparator::HistogramImageComparator() {
    std::cout << "The Bhattacharyya Distance comparator measures the similarity between two histograms, with lower values indicating greater similarity." << std::endl;
}

Result HistogramImageComparator::compare(const cv::Mat& img1, const cv::Mat& img2) {
    this->image1 = img1;
    this->image2 = img2;
    auto bhattacharyya = calculateBhattacharyyaDistance(img1, img2);
    bhattacharyya = std::round(bhattacharyya * 100000.0) / 100000.0; // round to 6 dec places for machine precision reasons
    std::cout << "Bhattacharyya score = " << bhattacharyya << std::endl;
    return {bhattacharyya, bhattacharyya > 0.0005};
}

void HistogramImageComparator::dumpImages() {}

cv::Mat HistogramImageComparator::getResult() const {
    std::cout << "In the results image, the brighter the higher the difference. Black means equal, white means very high difference (>= 255)." << std::endl;
    cv::Mat diff;
    cv::absdiff(image1, image2, diff);
    return diff;
}

double HistogramImageComparator::calculateBhattacharyyaDistance(const cv::Mat& image1, const cv::Mat& image2) {
    // Convert images to grayscale
    cv::Mat image1_gray, image2_gray;
    cv::cvtColor(image1, image1_gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(image2, image2_gray, cv::COLOR_BGR2GRAY);

    // Compute histograms
    cv::Mat hist1, hist2;
    int histSize = 256;  // Number of bins
    float range[] = {0, 256};
    const float* histRange = {range};
    bool uniform = true, accumulate = false;

    cv::calcHist(&image1_gray, 1, nullptr, cv::Mat(), hist1, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&image2_gray, 1, nullptr, cv::Mat(), hist2, 1, &histSize, &histRange, uniform, accumulate);

    // Normalize histograms
    cv::normalize(hist1, hist1, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(hist2, hist2, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    // Calculate Bhattacharyya Distance
    double bhattacharyya = cv::compareHist(hist1, hist2, cv::HISTCMP_BHATTACHARYYA);

    return bhattacharyya;
}