//! Constants.h
//! Copyright 2023 Garmin Ltd. or its subsidiaries

#pragma once

const int kImageCompareConstant = 21;  // Manhattan Norm constant, no lower than 20
const int kMaxBlobSize = 500;
const int kResizedImageDimension = 1024;  // 1024
const int kFilteringKernelSize = 3;
const int kBlobFilteringKernelSize = 9;  // Remove any blob smaller than this.
const double kComparisonBinarizationRgbThreshold = 10.0;
