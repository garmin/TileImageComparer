//! ComparerPlayground.cpp
//! Copyright 2023 Garmin Ltd. or its subsidiaries

#include <iomanip>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <cstdio>
#include <argparse/argparse.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "Constants.hpp"
#include "Utils.hpp"
#include "IImageComparator.hpp"
#include "CPSImageComparator.hpp"
#include "demoalgorithms/SSIMImageComparator.hpp"
#include "demoalgorithms/MSEImageComparator.hpp"
#include "demoalgorithms/PSNRImageComparator.hpp"
#include "demoalgorithms/PixelWiseImageComparator.hpp"
#include "demoalgorithms/HistogramImageComparator.hpp"
#include "demoalgorithms/CVBlobsImageComparator.hpp"
#include "demoalgorithms/WhiteLineImageComparator.hpp"
#include "demoalgorithms/SVMImageComparator.hpp"

ComparisonThresholds kDefaultThresholds{ 0.05,    // 5% of the area
                                         0.025};  // 2.5% of the edge

int main(int argc, char *argv[]) {
    // Disable OpenCV2 terminal logging
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    // Command line arguments definition
    argparse::ArgumentParser argParser("Comparer Playground", "0.1");
    argParser.add_argument("--algorithm")
            .help("Algorithm to be used. Options available: custom, ssim, mse, hist, pixel, psnr, cvblobs, whiteline, svm.")
            .default_value("custom");

    argParser.add_argument("-f1")
            .required()
            .help("First IMG file path");

    argParser.add_argument("-f2")
            .required()
            .help("Second IMG file path");

    argParser.add_argument("-s")
            .help("Size in pixel of the image. Images will be resized before processing. Defaul value " + std::to_string(kResizedImageDimension));

    argParser.add_argument("-b")
            .help("Benchmark mode, suppresses verbosity and only outputs the result in CSV format")
            .default_value(false)
            .implicit_value(true);

    // Command line arguments parsing
    try {
        argParser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << argParser;
        return EXIT_FAILURE;
    }

    std::string algorithm;
    bool benchmarkMode = false;
    int dimension = kResizedImageDimension;
    std::string image1FileName = argParser.get<std::string>("-f1");
    std::string image2FileName = argParser.get<std::string>("-f2");
    if(argParser.is_used("-s")) dimension = std::stoi(argParser.get<std::string>("-s"));
    if(argParser.is_used("-b")) {
        if (argParser.get<bool>("-b")) {
            std::cout.setstate(std::ios_base::failbit);
            benchmarkMode = true;
        }
    }
    if(argParser.is_used("--algorithm")) {
        algorithm = argParser.get<std::string>("--algorithm");
    }

    // Software banner
    std::cout << "**** Image comparer playground v0.1 ****" << std::endl;
    std::cout << "Copyright 2023 Garmin Ltd. or its subsidiaries" << std::endl << std::endl;

    // Reading image files
    cv::Mat image1 = cv::imread(image1FileName);
    if (image1.empty()) {
        std::cerr << "Error: Could not open or find " << image1FileName << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << image1FileName << " size: " << image1.rows << "x" << image1.cols << " (" << image1.rows*image1.cols << " pixels)" << std::endl;
    }

    cv::Mat image2 = cv::imread(image2FileName);
    if (image2.empty()) {
        std::cerr << "Error: Could not open or find " << image2FileName << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << image2FileName << " size: " << image2.rows << "x" << image2.cols << " (" << image2.rows*image2.cols << " pixels)" << std::endl;
    }

    cv::resize(image1, image1,
                cv::Size(dimension, dimension),
                cv::INTER_LINEAR);
    cv::resize(image2, image2,
                cv::Size(dimension, dimension),
                cv::INTER_LINEAR);

    std::unique_ptr<IImageComparator> comparator;

    if (algorithm == "ssim" || algorithm == "SSIM") {
        comparator = std::make_unique<SSIMImageComparator>();
    } else if (algorithm == "mse" || algorithm == "MSE") {
        comparator = std::make_unique<MSEImageComparator>();
    } else if (algorithm == "hist" || algorithm == "HIST") {
        comparator = std::make_unique<HistogramImageComparator>();
    } else if (algorithm == "psnr" || algorithm == "PSNR") {
        comparator = std::make_unique<PSNRImageComparator>();
    } else if (algorithm == "pixel" || algorithm == "pixelwise") {
        comparator = std::make_unique<PixelWiseImageComparator>();
    } else if (algorithm == "cvblobs" || algorithm == "blobs") {
        comparator = std::make_unique<CVBlobsImageComparator>();
    } else if (algorithm == "line" || algorithm == "whiteline" || algorithm == "lines") {
        comparator = std::make_unique<WhiteLineImageComparator>();
    } else if (algorithm == "svm" || algorithm == "SVM") {
        comparator = std::make_unique<SVMImageComparator>();
    } else {
        // Custom Algorithm here
        comparator = std::make_unique<CPSImageComparator>(kMaxBlobSize, 1, kImageCompareConstant, kDefaultThresholds);
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto compResult = comparator->compare(image1, image2);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

    CupResult resultData;
    resultData.image1 = image1FileName;
    resultData.image2 = image2FileName;
    resultData.score = compResult.score;
    resultData.different = compResult.different;
    resultData.time = duration.count();
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    std::cout << PrintResult(resultData) << std::endl;
    if (benchmarkMode) {
        bool groundTruthComparison = CheckResultWithGroundTruth(resultData);
        std::cout.clear();
        std::cout << PrintCSVResult(resultData) << (groundTruthComparison ? " OK" : " NO") << std::endl;
        std::cout.setstate(std::ios_base::failbit);

        if (groundTruthComparison) return EXIT_SUCCESS;
        else return EXIT_FAILURE;
    }

    comparator->dumpImages();
    cv::Mat result = comparator->getResult();
    SaveImage(result, "result.png");

    cv::imshow("Result", result);
    cv::waitKey(0);

    return EXIT_SUCCESS;
}