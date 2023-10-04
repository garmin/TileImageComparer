#include "CVBlobsImageComparator.hpp"
#include "Utils.hpp"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

CVBlobsImageComparator::CVBlobsImageComparator() {
    std::cout << "The CVBlobs comparator uses OpenCV's SimpleBlobDetector and uses the average blob size to score the differences between images" << std::endl;
}

Result CVBlobsImageComparator::compare(const cv::Mat& img1, const cv::Mat& img2) {
    this->image1 = img1;
    this->image2 = img2;
    auto CVBlobs = getCVBlobs(img1, img2);
    std::cout << "CVBlobs score = " << CVBlobs << std::endl;
    return {CVBlobs, CVBlobs > 50};
}

void CVBlobsImageComparator::dumpImages() {}

cv::Mat CVBlobsImageComparator::getResult() const {
    std::cout << "In the results image, the detected blobs are highligted with a red circle with a diameter proportional to the size of the detected blob." << std::endl;
    return this->result;
}

double CVBlobsImageComparator::getCVBlobs(const cv::Mat& i1, const cv::Mat& i2) {
     // Setup SimpleBlobDetector parameters.
    cv::SimpleBlobDetector::Params params;

    params.filterByArea = true;
    params.minDistBetweenBlobs = 50.0f;
    params.filterByInertia = false;
    params.filterByConvexity = false;
    params.filterByColor = false;
    params.filterByCircularity = false;
    params.filterByArea = true;
    params.minArea = 20.0f;
    params.maxArea = 5000000000.0f;

    auto detector = cv::SimpleBlobDetector::create(params);

    cv::Mat img = cv::Mat::zeros(image1.size(), CV_8UC3);

    for (int y = 0; y < image1.rows; ++y) {
        for (int x = 0; x < image1.cols; ++x) {
            cv::Vec3b pixel1 = image1.at<cv::Vec3b>(y, x);
            cv::Vec3b pixel2 = image2.at<cv::Vec3b>(y, x);

            if (pixel1 != pixel2) {
                img.at<cv::Vec3b>(y, x) = {255, 255, 255};
            }
        }
    }

    std::vector<cv::KeyPoint> keypoints;
    detector->detect(img, keypoints);

    std::cout << "OpenCV2 found " << keypoints.size() << " blobs" << std::endl;

    cv::drawKeypoints(img, keypoints, this->result, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    double size = 0;

    if (keypoints.size() > 0) {
        for (auto keypoint : keypoints) {
            size += keypoint.size;
        }

        size /= keypoints.size();
    }

    return size;
}