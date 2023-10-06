#include "demoalgorithms/SVMImageComparator.hpp"
#include "Utils.hpp"

#include <iostream>
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>

SVMImageComparator::SVMImageComparator() {
    std::cout << "The SVM comparator..." << std::endl;
    this->model = createModel();
    train(model);
}

Result SVMImageComparator::compare(const cv::Mat& img1, const cv::Mat& img2) {
    this->image1 = img1;
    this->image2 = img2;

    auto SVM = compareImages(this->image1, this->image2);
    std::cout << "SVM similarity score = " << SVM << std::endl;
    return {SVM, SVM > 0.0};
}

void SVMImageComparator::dumpImages() {}

cv::Mat SVMImageComparator::getResult() const {
    std::cout << "In the results image..." << std::endl;
    return this->result;
}

double SVMImageComparator::compareImages(cv::Mat& image1, cv::Mat& image2) {
    cv::Mat featureVector;
    cv::hconcat(extractFeatures(image1), extractFeatures(image2), featureVector);
    float similarityScore = this->model->predict(featureVector);
    this->result = featureVector;
    return similarityScore;
}

cv::Ptr<cv::ml::SVM> SVMImageComparator::createModel() {
    cv::Ptr<cv::ml::SVM> model = cv::ml::SVM::create();
    model->setType(cv::ml::SVM::C_SVC);
    model->setKernel(cv::ml::SVM::RBF);
    model->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));

    return model;
}

cv::Mat SVMImageComparator::extractFeatures(cv::Mat& image) {
    cv::Mat res;
    cv::cvtColor(image, res, cv::COLOR_BGR2GRAY);
    cv::resize(res, res,
                cv::Size(512, 1),
                cv::INTER_LINEAR);
    res.convertTo(res, CV_32F);
    return res;
}

void SVMImageComparator::train(cv::Ptr<cv::ml::SVM> model) {
    // Define the path to the directory containing your labeled image pairs
    std::string modelFile = "svm_model.xml";

    if (std::filesystem::exists(modelFile) && cv::ml::SVM::load(modelFile.c_str()) != nullptr) {
        std::cout << "Loading pre-trained model from " << modelFile << "..." << std::endl;
        this->model = cv::ml::SVM::load(modelFile.c_str());
    } else {
        std::string datasetPath = "trainingSet";

        // Create data structures for features and labels
        std::vector<cv::Mat> featureVectors;
        std::vector<int> labels;

        // List the subdirectories (labels) in the dataset directory
        std::vector<std::string> labelDirs;
        for (const auto & entry : std::filesystem::directory_iterator(datasetPath))
            labelDirs.push_back(entry.path());

        // Loop through each label directory
        for (const std::string& labelDir : labelDirs) {
            std::cout << "Loading from " << labelDir << "..." << std::endl;
            // Extract the label from the directory name (e.g., "same" or "different")
            std::string label = labelDir.substr(labelDir.find_last_of('/') + 1);

            // List image files in the label directory
            std::vector<cv::String> imageFiles;
            cv::glob(labelDir + "/*.png", imageFiles, false);

            // Loop through image pairs in the label directory
            for (size_t i = 0; i < imageFiles.size(); i += 2) {
                std::cout << "Loading " << imageFiles[i] << " and " << imageFiles[i + 1] << std::endl;
                // Load the two images in the pair
                cv::Mat image1 = cv::imread(imageFiles[i]);
                cv::Mat image2 = cv::imread(imageFiles[i + 1]);
                if (image1.empty() || image2.empty()) {
                    std::cerr << "Error loading images." << std::endl;
                    continue; // Skip this pair if loading fails
                }
                std::cout << "Loaded " << imageFiles[i] << " and " << imageFiles[i + 1] << std::endl;

                // Extract features from the images (e.g., using HOG)
                cv::Mat featureVector;
                cv::hconcat(extractFeatures(image1), extractFeatures(image2), featureVector);

                // Convert label to integer (e.g., "same" = 1, "different" = 0)
                int labelInt = (label == "same") ? 1 : 0;

                // Add the feature vector and label to the data structures
                featureVectors.push_back(featureVector);
                labels.push_back(labelInt);
            }
        }

        std::cout << "Converting data..." << std::endl;
        // Convert data structures to OpenCV format
        cv::Mat trainingData, trainingLabels;
        cv::vconcat(featureVectors, trainingData);
        cv::Mat(labels).copyTo(trainingLabels);

        std::cout << "Training phase..." << std::endl;
        model->train(trainingData, cv::ml::ROW_SAMPLE, trainingLabels);

        // Save the trained SVM model to a file
        std::cout << "Saving model..." << std::endl;
        model->save(modelFile);
    }
}