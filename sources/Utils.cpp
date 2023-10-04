#include "Utils.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>

bool SaveImage(cv::Mat &aMat, const std::string &aDstFilePng) {
  return cv::imwrite(aDstFilePng, aMat);
}

std::string PrintResult(CupResult result) {
    std::string out = "Comparison result on " + result.image1 + " and " + result.image2 + " = " + std::string(result.different ? "True" : "False") + std::string(result.different ? " (Differences detected)" : " (Equal)");
    out += "\nScore = " + std::to_string(result.score);
    out += "\nTime = " + std::to_string(result.time) + " ms";
    return out;
}

std::string PrintCSVResult(CupResult result) {
    std::string out = result.image1 + ";" + result.image2 + ";" + std::to_string(result.different) + ";" + std::to_string(result.score) + ";" + std::to_string(result.time);
    return out;
}

CupResult GetResultfromCSVLine(std::string csv) {
    CupResult result;

    std::istringstream iss(csv);
    std::string token;

    getline(iss, token, ';');
    result.image1 = token;

    getline(iss, token, ';');
    result.image2 = token;

    getline(iss, token, ';');
    result.different = (token == "1");

    getline(iss, token, ';');
    result.score = std::stod(token);

    getline(iss, token, ';');
    result.time = std::stod(token);

    return result;
}


bool CheckResultWithGroundTruth(CupResult result) {
    std::string filename = "groundtruth.csv";
    std::ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        CupResult groundTruth = GetResultfromCSVLine(line);
        if (result.image1 == groundTruth.image1 && result.image2 == groundTruth.image2) {
            file.close();
            return result.different == groundTruth.different;
        }
    }

    file.close(); // Close the file when done

    return false;
}