#include "SSIMImageComparator.hpp"
#include "Utils.hpp"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

SSIMImageComparator::SSIMImageComparator() {
    std::cout << "The SSIM comparator returns a value between -1 and 1, where a higher value indicates greater similarity between the images, and a value closer to -1 indicates greater dissimilarity. It takes into account both structural and luminance differences in the images." << std::endl;
}

Result SSIMImageComparator::compare(const cv::Mat& img1, const cv::Mat& img2) {
    this->image1 = img1;
    this->image2 = img2;
    auto ssim = getSSIM(img1, img2);
    std::cout << "SSIM score = " << ssim << "%" << std::endl;
    return {ssim, ssim < 99.9};
}

void SSIMImageComparator::dumpImages() {}

cv::Mat SSIMImageComparator::getResult() const {
    std::cout << "In the results image, the darker the higher the difference. Black means very different pixels, white means that the pixels are equal." << std::endl;
    return this->result;
}

double SSIMImageComparator::getSSIM(const cv::Mat& i1, const cv::Mat& i2) {
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d = CV_32F;
    cv::Mat I1, I2;
    i1.convertTo(I1, d);            // cannot calculate on one byte large values
    i2.convertTo(I2, d);
    cv::Mat I2_2   = I2.mul(I2);        // I2^2
    cv::Mat I1_2   = I1.mul(I1);        // I1^2
    cv::Mat I1_I2  = I1.mul(I2);        // I1 * I2
    /*************************** END INITS **********************************/
    cv::Mat mu1, mu2;                   // PRELIMINARY COMPUTING
    cv::GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
    cv::GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
    cv::Mat mu1_2   =   mu1.mul(mu1);
    cv::Mat mu2_2   =   mu2.mul(mu2);
    cv::Mat mu1_mu2 =   mu1.mul(mu2);
    cv::Mat sigma1_2, sigma2_2, sigma12;
    cv::GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;
    cv::GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;
    cv::GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;
    cv::Mat t1, t2, t3;
    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);                 // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);                 // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
    cv::Mat ssim_map;
    cv::divide(t3, t1, ssim_map);        // ssim_map =  t3./t1;
    cv::Scalar mssim = mean(ssim_map);   // mssim = average of ssim map
    auto ssim = ((mssim.val[0] + mssim.val[1] + mssim.val[2])/3) * 100;
    this->result = ssim_map;
    SaveImage(ssim_map, "intermediates/ssim_map.png");
    return ssim;
}