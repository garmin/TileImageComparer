#include "CPSImageComparator.hpp"

#include <iostream>

#include <opencv2/imgproc.hpp>

#include "BlobDetect.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
                                         
CPSImageComparator::CPSImageComparator(int maxBlobSize, int pxStride, int compareConstant, ComparisonThresholds thresholds):
    mMaxBlobSize(maxBlobSize),
    mPxStride(pxStride),
    mCompareConstant(compareConstant),
    mThresholds(thresholds) {
        std::cout << "Max blob size is " << maxBlobSize << std::endl;
        std::cout << "Pixel compare threshold is " << mCompareConstant << std::endl;
    }

Result CPSImageComparator::compare(const cv::Mat& image1, const cv::Mat& image2){
    this->image1 = image1;
    this->image2 = image2;

    // Difference detection phase
    auto processedPair = FindDifferences(image1, image2);

    bool checkStatus = !Check(processedPair.first, processedPair.second, this->mThresholds);
    std::cout << "Check result = " << (checkStatus ? "True" : "False") << std::endl;

    // Error pixels detection phase
    std::vector<std::pair<int, int>> errorPoints;

    this->blendedImage = cv::Mat::zeros(kResizedImageDimension, kResizedImageDimension, CV_8UC3);
    this->comparisonImage = cv::Mat::zeros(kResizedImageDimension, kResizedImageDimension, CV_8UC3);
    auto comparedPixels = 0;
    float manhattanNorm = 0;
    for (int y = 0; y < image1.rows; y += this->mPxStride) {
        for (int x = 0; x < image1.cols; x += this->mPxStride) {
            comparedPixels++;
            auto pixelToCompare = image1.at<cv::Vec3b>(x, y);
            auto pixel2 = image2.at<cv::Vec3b>(x, y);
            this->blendedImage.at<cv::Vec3b>(x, y) = (pixelToCompare/2 + pixel2/2);
            this->comparisonImage.at<cv::Vec3b>(x, y) = (this->blendedImage.at<cv::Vec3b>(x, y)/2 + cv::Vec3b({0, 255, 0})/3);

            if (pixelToCompare != pixel2) {
                if (pixelToCompare != pixel2) {
                    int blendedToOrigR = std::abs(pixelToCompare[0] - pixel2[0]);
                    int blendedToOrigG = std::abs(pixelToCompare[1] - pixel2[1]);
                    int blendedToOrigB = std::abs(pixelToCompare[2] - pixel2[2]);
                    float diff = (blendedToOrigR + blendedToOrigG + blendedToOrigB) / 3;
                    if (diff > this->mCompareConstant) {
                        errorPoints.push_back(std::make_pair(x, y));
                        this->comparisonImage.at<cv::Vec3b>(x, y) = (this->blendedImage.at<cv::Vec3b>(x, y)/2 + cv::Vec3b({0, 0, 255})/2);
                        manhattanNorm += diff;
                    }
                }
            }
        }
    }

    // Output data about error pixels
    std::cout << "Manhattan norm sum = " << manhattanNorm << std::endl;
    std::cout << "Compared " << comparedPixels << " pixels" << std::endl;
    std::cout << "Found " << errorPoints.size() << " bad pixels" << std::endl;
    double errorPerc = (static_cast<float>(errorPoints.size()) / comparedPixels) * 100;
    std::cout << "Error percentage: " << errorPerc << "%" << std::endl;

    if(!SaveImage(this->comparisonImage, "intermediates/comparison.png") || !SaveImage(this->blendedImage, "intermediates/blended.png")){
        std::cerr << "Error while saving intermediate image" << std::endl;
    }

    for (int y = 0; y < this->blendedImage.rows; y += this->mPxStride) {
        for (int x = 0; x < this->blendedImage.cols; x += this->mPxStride) {
            this->blendedImage.at<cv::Vec3b>(x, y) = this->blendedImage.at<cv::Vec3b>(x, y) / 3;
        }
    }

    this->diffsImage = cv::Mat::zeros(kResizedImageDimension, kResizedImageDimension, CV_8UC3);
    this->blendedImage.copyTo(this->diffsImage);
    for (auto diffStat : processedPair.second) {
        cv::Vec3b color = {0, 0, 255};
        cv::Rect rect(diffStat.mLeft, diffStat.mTop, diffStat.mWidth, diffStat.mHeight);
        cv::rectangle(this->diffsImage, rect, color, 5);
    }

    if (processedPair.second.size() == 0) cv::putText(this->diffsImage, "No diff detected", cv::Point(20, this->diffsImage.rows / 2), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(255, 0, 0), 2);
    if(!SaveImage(this->diffsImage, "intermediates/diffs.png")){
        std::cerr << "error while saving diffs image" << std::endl;
    }

    // Blob detection phase
    std::vector<std::pair<int, int>> blob = {};
    BlobDetect blobDetect;
    std::vector<std::vector<std::pair<int, int>>> listOfPointLists{ {} };

    // For each error point, find which blob it gets assigned to
    for (std::pair<int, int> point : errorPoints) {
        blobDetect.AssignPointToBlob(point, listOfPointLists);
        listOfPointLists = blobDetect.WeedOutMeaninglessBlobs(point, listOfPointLists);
        blob = blobDetect.LargestBlob(listOfPointLists);

        if (blob.size() == this->mMaxBlobSize) {
            // Escape early ?
            break;
        }
    }

    // Computation ended
    auto stop = std::chrono::high_resolution_clock::now();

    // Output blob detection results
    std::cout << "Found " << std::to_string(listOfPointLists.size()) << " blobs" << std::endl;
    std::cout << "Largest blob size is at least " << std::to_string(static_cast<int>(blob.size())) << std::endl;

    this->largestBlobImage = cv::Mat::zeros(kResizedImageDimension, kResizedImageDimension, CV_8UC3);
    this->blendedImage.copyTo(this->largestBlobImage);
    cv::Vec3b color = {0, 0, 0};
    color[0] = rand()%255;
    color[1] = rand()%255;
    color[2] = rand()%255;
    for (auto point : blob) {
        this->largestBlobImage.at<cv::Vec3b>(point.first, point.second) = color;
    }

    if(!SaveImage(this->largestBlobImage, "intermediates/blobs_largest.png")){
        std::cerr << "error while saving blobs_largest image" << std::endl;
    }

    this->blobsImage = cv::Mat::zeros(kResizedImageDimension, kResizedImageDimension, CV_8UC3);
    this->blendedImage.copyTo(this->blobsImage);
    for (auto blob : listOfPointLists) {
        cv::Vec3b color = {0, 0, 0};
        color[0] = rand()%255;
        color[1] = rand()%255;
        color[2] = rand()%255;
        for (auto point : blob) {
            this->blobsImage.at<cv::Vec3b>(point.first, point.second) = color;
        }
    }

    if(!SaveImage(this->blobsImage, "intermediates/blobs.png")){
        std::cerr << "error while saving blobs image" << std::endl;
    }

    if (blob.size() > 50) {
        std::cout << "Result = True" << std::endl;
    } else {
        std::cout << "Result = False" << std::endl;
    }

    return {100 - errorPerc, checkStatus || blob.size() > 50};
}

 cv::Mat CPSImageComparator::getResult() const {
    cv::Mat origRow, firstRow, secondRow, thirdRow, result;

    auto font = cv::FONT_HERSHEY_DUPLEX;
    auto fontColor = CV_RGB(255, 255, 255);
    auto outlineColor = CV_RGB(0, 0, 0);
    auto fontSize = 1.0;
    auto fontThickness = 2;
    auto outlineThickness = 6;
    auto position = cv::Point(blendedImage.cols / 20, blendedImage.rows / 20);

    cv::putText(this->image1, "Original image 1", position, font, fontSize, outlineColor, outlineThickness);
    cv::putText(this->image1, "Original image 1", position, font, fontSize, fontColor, fontThickness);

    cv::putText(this->image2, "Original image 2", position, font, fontSize, outlineColor, outlineThickness);
    cv::putText(this->image2, "Original image 2", position, font, fontSize, fontColor, fontThickness);

    cv::putText(this->blendedImage, "Blended", position, font, fontSize, outlineColor, outlineThickness);
    cv::putText(this->blendedImage, "Blended", position, font, fontSize, fontColor, fontThickness);

    cv::putText(this->comparisonImage, "Comparison", position, font, fontSize, outlineColor, outlineThickness);
    cv::putText(this->comparisonImage, "Comparison", position, font, fontSize, fontColor, fontThickness);

    cv::putText(this->blobsImage, "Blobs", position, font, fontSize, outlineColor, outlineThickness);
    cv::putText(this->blobsImage, "Blobs", position, font, fontSize, fontColor, fontThickness);

    cv::putText(this->largestBlobImage, "Largest blob", position, font, fontSize, outlineColor, outlineThickness);
    cv::putText(this->largestBlobImage, "Largest blob", position, font, fontSize, fontColor, fontThickness);

    cv::putText(this->diffsImage, "Diffs", position, font, fontSize, outlineColor, outlineThickness);
    cv::putText(this->diffsImage, "Diffs", position, font, fontSize, fontColor, fontThickness);
    
    cv::Mat bitmaskImage = cv::imread("intermediates/diff_bitmask.png");
    cv::putText(bitmaskImage, "Bitmask", position, font, fontSize, outlineColor, outlineThickness);
    cv::putText(bitmaskImage, "Bitmask", position, font, fontSize, fontColor, fontThickness);

    cv::hconcat(image1, image2, origRow);
    cv::hconcat(this->blendedImage, this->comparisonImage, firstRow);
    cv::hconcat(this->blobsImage, this->largestBlobImage, secondRow);
    cv::hconcat(this->diffsImage, bitmaskImage, thirdRow);
    cv::vconcat(firstRow, secondRow, result);
    cv::vconcat(result, thirdRow, result);
    cv::vconcat(origRow, result, result);

    return result;
}

void CPSImageComparator::dumpImages() {}


// if(!SaveImage(result, "result.png")){
//         std::cerr << "error while saving result image" << std::endl;
//         return false;
//     }