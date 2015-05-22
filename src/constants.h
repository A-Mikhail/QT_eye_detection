/*
   “Copyright 2015 Адаменко Михаил”

    This file is part of EyeDetection.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Alizee.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

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

