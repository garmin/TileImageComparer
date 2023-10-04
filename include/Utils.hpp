#pragma once

#include <opencv2/core.hpp>

bool SaveImage(cv::Mat &aMat, const std::string &aDstFilePng);

struct Result {
    double score;
    bool different;
};

struct CupResult {
    std::string image1, image2;
    double score;
    bool different;
    int time;
};

std::string PrintResult(CupResult result);

std::string PrintCSVResult(CupResult result);

CupResult GetResultfromCSVLine(std::string csv);

bool CheckResultWithGroundTruth(CupResult result);