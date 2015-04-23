#ifndef CONSTANTS_H
#define CONSTANTS_H

// Debugging
const bool plotVectorField = false;

// Size constants
const int eyePercentTop     =   25;
const int eyePercentSide    =   13;
const int eyePercentHeight  =   30;
const int eyePercentWidth   =   35;

// Preprocessing
const bool smoothFaceImage      =   false;
const float smoothFaceFactor    =   0.005;

// Algorithm Parameters
const int fastEyeWidth          = 50;
const int weightBlurSize        = 5;
const bool enableWeight         = true;
const float weightDivisor       = 1.0;
const double gradientThreshold  = 50.0;

// Postprocessing
const bool enablePostProcess        = true;
const float postProcessThreshold    = 0.97;

#endif // CONSTANTS_H

