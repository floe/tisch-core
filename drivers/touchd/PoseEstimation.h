#pragma once
#include "opencv/cv.h"
/** 
 * computes the orientation and translation of a square
 * @param result result as 4x4 matrix
 * @param p2D coordinates of the four corners in clock-wise order. 
 *        the origin is assumed to be at the camera's center of projection
 * @param markerSize side-length of marker. Origin is at marker center.
 */
void estimateSquarePose( float* result, const CvPoint2D32f* p2D, float markerSize );

/**
 * Returns Matrix in Row-major format
 * @param result a 3x3 homogeneous matrix
 * @param quadrangle the coordinates of the corners counter-clockwise
 */
void calcHomography( float* pResult, const CvPoint2D32f* qQuad );