#include "WhiteLineImageComparator.hpp"
#include "Utils.hpp"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

WhiteLineImageComparator::WhiteLineImageComparator() {
    std::cout << "The WhiteLine comparator tries to detect white lines in both images and returns true if either image has a white line." << std::endl;
}

Result WhiteLineImageComparator::compare(const cv::Mat& img1, const cv::Mat& img2) {
    this->image1 = img1;
    this->image2 = img2;
    auto WhiteLine = hasWhiteLine(img1) && hasWhiteLine(img2);
    std::cout << "Has white line? " << (WhiteLine ? "Yes" : "No") << std::endl;
    return {(WhiteLine ? 1.0 : 0.0), WhiteLine};
}

void WhiteLineImageComparator::dumpImages() {}

cv::Mat WhiteLineImageComparator::getResult() const {
    std::cout << "In the results image, the detected lines are highlighted in red." << std::endl;
    return this->result;
}

bool WhiteLineImageComparator::hasWhiteLine(const cv::Mat& image) {
    // Convert the input image to grayscale
    cv::Mat grayscaleImage;
    cv::cvtColor(image, grayscaleImage, cv::COLOR_BGR2GRAY);

    // Apply Canny edge detection
    cv::Mat edges;
    cv::Canny(grayscaleImage, edges, 200, 250, 3);

    // Detect lines using the Hough Line Transform
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180, 100);

    // Filter and draw only nearly vertical and horizontal lines
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0], theta = lines[i][1];

        // Check if the line is nearly vertical or horizontal
        if (theta < CV_PI / 4 || theta > 3 * CV_PI / 4) {  // Vertical line
            cv::Point pt1(cvRound(rho / cos(theta)), 0);
            cv::Point pt2(cvRound(rho / cos(theta)), image.rows);
            cv::line(image, pt1, pt2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        } else if (theta > CV_PI / 4 && theta < 3 * CV_PI / 4) {  // Horizontal line
            cv::Point pt1(0, cvRound(rho / sin(theta)));
            cv::Point pt2(image.cols, cvRound(rho / sin(theta)));
            cv::line(image, pt1, pt2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        }
    }
    this->result = image;

    // Check if any white line contours are detected
    return !lines.empty();
}