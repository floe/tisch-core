/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
 *     Copyright (c) 2012 by Norbert Wiedermann, <wiederma@in.tum.de>      *
 *   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
 \*************************************************************************/

#include "MarkerTracker.h"

/*
* Constructor
*/
MarkerTracker::MarkerTracker(unsigned char _thresh, unsigned char _bw_thresh, unsigned char _max, int _imgwidth, int _imgheight) :
	thresh(_thresh), bw_thresh(_bw_thresh), max(_max)
{
	picSize.width = _imgwidth;
	picSize.height = _imgheight;

	isFirstStripe = true;
	isFirstMarker = true;

	cv_red		= CV_RGB(255,0,0);
	cv_green	= CV_RGB(0,255,0);
	cv_blue		= CV_RGB(0,0,255);
	cv_yellow	= CV_RGB(0,255,255);
	cv_magenta	= CV_RGB(255,0,255);
	cv_cyan		= CV_RGB(255,255,0);


#ifdef MY_DEBUG
	cv::namedWindow("marker", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("thresholded", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("stripe", CV_WINDOW_AUTOSIZE);
#endif
}

MarkerTracker::~MarkerTracker() {
#ifdef MY_DEBUG
	cv::destroyAllWindows();
#endif
}

/*
* used for UI interaction
*/
void MarkerTracker::update_thresh(unsigned char new_thresh) {
	thresh = new_thresh;
}

/*
* used for UI interaction
*/
void MarkerTracker::update_bw_thresh(unsigned char new_bw_thresh) {
	bw_thresh = new_bw_thresh;
}

/*
*
*/
int MarkerTracker::subpixSampleSafe ( cv::Mat pSrc, CvPoint2D32f p )
{
	int x = int( floorf ( p.x ) );
	int y = int( floorf ( p.y ) );

	if ( x < 0 || x >= pSrc.cols - 1 ||
		 y < 0 || y >= pSrc.rows - 1 )
		return 127;

	int dx = int ( 256 * ( p.x - floorf ( p.x ) ) );
	int dy = int ( 256 * ( p.y - floorf ( p.y ) ) );

	unsigned char* i = ( unsigned char* ) ( ( pSrc.data + y * pSrc.step ) + x );
	int a = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );
	i += pSrc.step;
	int b = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );

	return a + ( ( dy * ( b - a) ) >> 8 );
}

/*
* main function called by process function in MarkerTrackerFilter from Filter.cc
*/
void MarkerTracker::findMarker(RGBImage* rgbimage, IntensityImage* image, std::vector<markerData>* foundMarkers) {
	unsigned char* org_rgb_image_data = rgbimage->getData(); // pointer to unsigned char array
	//unsigned char* org_intensity_image_data = image->getData();

	isFirstStripe = true;
	isFirstMarker = true;

	cv::Mat matRGBImage		= cv::Mat( picSize.height, picSize.width, CV_8UC3, org_rgb_image_data );
	cv::Mat matConverted	= cv::Mat( picSize.height, picSize.width, CV_8UC1 );
	cv::Mat matThresholded	= cv::Mat( picSize.height, picSize.width, CV_8UC1 );

	// convert RGB to gray scale image
	cv::cvtColor( matRGBImage, matConverted, CV_RGB2GRAY ); // libTISCH RGB images bytes are ordered in RGB not BGR like in openCV!
	// threshold the gray scale image
	cv::threshold( matConverted, matThresholded, thresh, max, CV_THRESH_BINARY);

#ifdef MY_DEBUG
	cv::imshow("thresholded", matThresholded);
#endif

	std::vector<MarkerTracker::markerData> tmpMarkers;

	// find contours in thresholded image
	vector< vector<cv::Point> > mCvContours;
	cv::findContours( matThresholded, mCvContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	
	vector<cv::Point> approx;

	// main loop looking for contours
//====================================================================================
	for( size_t i = 0; i < mCvContours.size(); i++) {
		cv::approxPolyDP(cv::Mat(mCvContours[i]), approx, cv::arcLength(cv::Mat(mCvContours[i]), true)*0.02, true);

		cv::Rect r = cv::boundingRect(approx);
		if (r.height < 40 || r.width < 40 || r.height >= picSize.height - 20 || r.width >= picSize.width - 20) {
			continue;
		}
//------------------------------------------------------------------------------------
		if( approx.size() == 4 ) {

			//-draw------------------

			const cv::Point* pts = &approx[0];
			int npts = (int)approx.size();
			// draw lines for each border
			cv::polylines(matRGBImage, &pts, &npts, 1, true, cv_red, 2);
			
			//-----------------------

			vector<cv::Vec4f> lineParams;
			
			// iterate all 4 sides of each contour
			for (int i = 0; i < 4; ++i) {
			
				//-draw------------------
			
				// draw a circle around each corner
				cv::circle(matRGBImage, approx[i], 3, cv_green, -1);
			
				//-----------------------

				double dx = (double)(approx[(i+1)%4].x - approx[i].x)/7.0;
				double dy = (double)(approx[(i+1)%4].y - approx[i].y)/7.0;
				int stripeLength = (int)(0.8*sqrt (dx*dx+dy*dy));
				if (stripeLength < 5)
					stripeLength = 5;

				// make stripeLength odd (because of the shift in nStop)
				stripeLength |= 1;

				//e.g. stripeLength = 5 --> from -2 to 2
				int nStop  = stripeLength >> 1;
				int nStart = -nStop;

				CvSize stripeSize;
				stripeSize.width = 3;
				stripeSize.height = stripeLength;

				CvPoint2D32f stripeVecX;
				CvPoint2D32f stripeVecY;

				// normalize vectors
				double diffLength = sqrt ( dx*dx + dy*dy );
				stripeVecX.x = (float)(dx / diffLength);
				stripeVecX.y = (float)(dy / diffLength);

				stripeVecY.x =  stripeVecX.y;
				stripeVecY.y = -stripeVecX.x;

				cv::Mat* matStripe = new cv::Mat( stripeSize, CV_8UC1 );

				// Vector for edge point centers
				vector<cv::Vec2f> vec_points(6);

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				// iterate all 6 intermediate steps of each side
				for(int j = 1; j < 7; ++j) {
					double px = (double)approx[i].x + (double)j * dx;
					double py = (double)approx[i].y + (double)j * dy;

					cv::Point p;
					p.x = (int)px;
					p.y = (int)py;

					cv::circle(matRGBImage, p, 2, cv_blue, -1);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					// create data for stripe
					for( int m = -1; m <= 1; ++m ) {
						for ( int n = nStart; n <= nStop; ++n )	{
							CvPoint2D32f subPixel;

							subPixel.x = (float)((double)p.x + ((double)m * stripeVecX.x) + ((double)n * stripeVecY.x));
							subPixel.y = (float)((double)p.y + ((double)m * stripeVecX.y) + ((double)n * stripeVecY.y));

							cv::Point p2;
							p2.x = (int)subPixel.x;
							p2.y = (int)subPixel.y;

							if ( isFirstStripe )
								cv::circle(matRGBImage, p2, 1, cv_magenta, -1);
							else
								cv::circle(matRGBImage, p2, 1, cv_yellow, -1);

							// calc subpixSampleSafe
							int pixel = subpixSampleSafe (matConverted, subPixel);
						
							int w = m + 1; //add 1 to shift to 0..2
							int h = n + ( stripeLength >> 1 ); //add stripelenght>>1 to shift to 0..stripeLength

							//*(iplStripe->imageData + h * iplStripe->widthStep  + w) =  pixel; //set pointer to correct position and safe subpixel intensity
							// Mat src(1, 2, CV_32FC2);
							// src.at<Vec2f>(0,1)[1] = 123; // (0,1) means row 0, col 1. [1] means channel 1. 
							//cout << "w: " << w << " h: " << h << " x: " << x << " y: " << y << " pixel " << pixel << endl;
							matStripe->at<unsigned char>(h,w) = pixel; //(y,x)
						
						} // for ( int n = nStart; n <= nStop; ++n ) {
					} // for( int m = -1; m <= 1; ++m ) {
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

					//use sobel operator on stripe
					// ( -1 , -2, -1 )
					// (  0 ,  0,  0 )
					// (  1 ,  2,  1 )
				
					double* sobelValues = new double[stripeLength - 2];

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					for (int n = 1; n < (stripeLength - 1); n++) {
						unsigned char stripePtr[] = {
							matStripe->at<unsigned char>((n-1),0), // (y,x) y=rows, x=cols
							matStripe->at<unsigned char>((n-1),1), 
							matStripe->at<unsigned char>((n-1),2),
							matStripe->at<unsigned char>((n+1),0),
							matStripe->at<unsigned char>((n+1),1),
							matStripe->at<unsigned char>((n+1),2),
						};
						double r1 = -stripePtr[ 0 ] - 2 * stripePtr[ 1 ] - stripePtr[ 2 ];
						double r3 =  stripePtr[ 3 ] + 2 * stripePtr[ 4 ] + stripePtr[ 5 ];

						sobelValues[n-1] = r1+r3;
					} // for (int n = 1; n < (stripeLength - 1); n++) {
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					double maxVal = -1;
					int maxIndex = 0;
					for (int n = 0; n < stripeLength - 2; ++n) {
						if ( sobelValues[n] > maxVal ) {
							maxVal = sobelValues[n];
							maxIndex = n;
						}
					}

					double y0, y1, y2; // y0 .. y1 .. y2
					y0 = (maxIndex <= 0) ? 0 : sobelValues[ maxIndex - 1 ];
					y1 = sobelValues[ maxIndex ];
					y2 = (maxIndex >= stripeLength - 3) ? 0 : sobelValues[ maxIndex + 1 ];

					delete sobelValues; // no longer needed, free memory

					//formula for calculating the x-coordinate of the vertex of a parabola, given 3 points with equal distances 
					//(xv means the x value of the vertex, d the distance between the points): 
					//xv = x1 + (d / 2) * (y2 - y0)/(2*y1 - y0 - y2)

					double pos = ( y2 - y0 ) / ( 4 * y1 - 2 * y0 - 2 * y2 ); //d = 1 because of the normalization and x1 will be added later
					/*
					if (std::isinf(pos)) {
					// value is infinity
					continue;
					}

					if (std::isnan(pos)) {
					// value is not a number
					continue;
					}*/

					// This would be a valid check, too
					//if (std::isinf(pos)) {
					//	// value is infinity
					//	continue;
					//}

					if (pos != pos) {
						// value is not a number
						continue;
					}

					CvPoint2D32f edgeCenter; //exact point with subpixel accuracy
					int maxIndexShift = maxIndex - (stripeLength >> 1);

					//shift the original edgepoint accordingly
					edgeCenter.x = (float)((double)p.x + (((double)maxIndexShift + pos) * stripeVecY.x));
					edgeCenter.y = (float)((double)p.y + (((double)maxIndexShift + pos) * stripeVecY.y));

					CvPoint p_tmp;
					p_tmp.x = (int)edgeCenter.x;
					p_tmp.y = (int)edgeCenter.y;
					cv::circle( matRGBImage, p_tmp, 1, cv_blue, -1 );

					vec_points[ j-1 ] = cv::Vec2f( edgeCenter.x, edgeCenter.y );

					if ( isFirstStripe ) {
						#ifdef MY_DEBUG
							cv::Mat matTmp = cv::Mat( 100, 60, CV_8UC1);
							cv::resize( *matStripe, matTmp, matTmp.size(), 0, 0, cv::INTER_NEAREST);
							cv::imshow("stripe", matTmp);
						#endif
						isFirstStripe = false;
					}


				} //for(int j = 1; j < 7; ++j) {
				// end of loop over edge points of one edge
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

				delete matStripe;
				//cout << "i: " << i << endl;
				//cout << vec_points[0][0] << " " << vec_points[0][1] << endl;
				//cout << vec_points[1][0] << " " << vec_points[1][1] << endl;
				//cout << vec_points[2][0] << " " << vec_points[2][1] << endl;
				//cout << vec_points[3][0] << " " << vec_points[3][1] << endl;
				//cout << vec_points[4][0] << " " << vec_points[4][1] << endl;
				//cout << vec_points[5][0] << " " << vec_points[5][1] << endl;
				//cout << "----------------------" << endl;

				// we now have the array of exact edge centers stored in "vec_points"
				cv::Vec4f line;
				cv::fitLine( cv::Mat(vec_points), line, CV_DIST_L2, 0.0, 0.01, 0.01 );
				// cvFitLine stores the calculated line in lineParams in the following way:
				// vec.x, vec.y, point.x, point.y

				lineParams.push_back(line);
				
				//cout << "i: " << i << endl;
				//cout << lineParams.at(i)[0] << " " << lineParams.at(i)[1] << endl;
				//cout << lineParams.at(i)[2] << " " << lineParams.at(i)[3] << endl;
				//cout << "======================" << endl;
				/*cout << "i: " << i << endl;
				cout << line[0] << " " << line[1] << endl;
				cout << line[2] << " " << line[3] << endl;
				cout << "======================" << endl;*/
				
				cv::Point p;
				p.x = (int)(lineParams.at(i)[2] - 50.0 * lineParams.at(i)[0]);
				p.y = (int)(lineParams.at(i)[3] - 50.0 * lineParams.at(i)[1]);

				//CvPoint p2;
				cv::Point p2;
				p2.x = (int)(lineParams.at(i)[2] + 50.0 * lineParams.at(i)[0]);
				p2.y = (int)(lineParams.at(i)[3] + 50.0 * lineParams.at(i)[1]);

				cv::line(matRGBImage, p, p2, cv_green, 1, 8, 0);


			} //for (int i = 0; i < 4; ++i) {
			// end of loop over the 4 edges


			// so far we stored the exact line parameters and show the lines in the image
			// now we have to calculate the exact corners
			cv::Point2f corners[4];
// ##################################################################
			for (int i = 0; i < 4; ++i) {
				int j = (i+1)%4;
				double x0,x1,y0,y1,u0,u1,v0,v1;
				x0 = (double)lineParams.at(i)[2]; y0 = (double)lineParams.at(i)[3];
				x1 = (double)lineParams.at(j)[2]; y1 = (double)lineParams.at(j)[3];

				u0 = (double)lineParams.at(i)[0]; v0 = (double)lineParams.at(i)[1];
				u1 = (double)lineParams.at(j)[0]; v1 = (double)lineParams.at(j)[1];

				// (x|y) = p + s * vec
				// s = Ds / D (see cramer's rule)
				// (x|y) = p + (Ds / D) * vec
				// (x|y) = (p * D / D) + (Ds * vec / D)
				// (x|y) = (p * D + Ds * vec) / D
				// (x|y) = a / c;
				double a =  x1 * u0 * v1 - y1 * u0 * u1 - x0 * u1 * v0 + y0 * u0 * u1;
				double b = -x0 * v0 * v1 + y0 * u0 * v1 + x1 * v0 * v1 - y1 * v0 * u1;
				double c =  v1 * u0 - v0 * u1;
				
				if ( fabs(c) < 0.001 ) { //lines parallel?
					std::cout << "lines parallel" << std::endl;
					continue;
				}

				a /= c;
				b /= c;

				//exact corner
				corners[i].x = (float)a; 
				corners[i].y = (float)b;
				//CvPoint p;
				cv::Point p;
				p.x = (int)corners[i].x;
				p.y = (int)corners[i].y;

				cv::circle(matRGBImage, p, 5, CV_RGB(i*60, i*60, 0), -1);
			} // for (int i = 0; i < 4; ++i) {
			//finished the calculation of the exact corners
// ##################################################################

			cv::Point2f targetCorners[4];
			targetCorners[0].x = -0.5; targetCorners[0].y = -0.5;
			targetCorners[1].x =  5.5; targetCorners[1].y = -0.5;
			targetCorners[2].x =  5.5; targetCorners[2].y =  5.5;
			targetCorners[3].x = -0.5; targetCorners[3].y =  5.5;

			//create and calculate the matrix of perspective transform
			cv::Mat projMat = cv::Mat( 3, 3, CV_32F );
			//CvMat* projMat = cvCreateMat (3, 3, CV_32F );

			// #define cvWarpPerspectiveQMatrix cvGetPerspectiveTransform
			//cvWarpPerspectiveQMatrix ( corners, targetCorners, projMat); // src dst map_matrix
			// CvMat* cvGetPerspectiveTransform(const CvPoint2D32f* src, const CvPoint2D32f* dst, CvMat* mapMatrix)
			// Mat getPerspectiveTransform(const Point2f* src, const Point2f* dst)
			
			projMat = cv::getPerspectiveTransform(corners, targetCorners);

			//create image for the marker
			CvSize markerSize;
			markerSize.width  = 6;
			markerSize.height = 6;
			//IplImage* iplMarker = cvCreateImage( markerSize, IPL_DEPTH_8U, 1 );
			matMarker = cv::Mat(markerSize, CV_8UC1);

			//change the perspective in the marker image using the previously calculated matrix
			//cvWarpPerspective(matConverted, iplMarker, projMat, CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
			
			cv::warpPerspective(matConverted, matMarker, projMat, markerSize, CV_WARP_FILL_OUTLIERS, 0, cvScalarAll(0));
			//cv::warpPerspective(matConverted, matConverted, projMat, tmp_size, CV_WARP_FILL_OUTLIERS, 0, cvScalarAll(0));
			
			//cvWarpPerspective(const CvArr* src, CvArr* dst, const CvMat* mapMatrix, int flags=CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, CvScalar fillval=cvScalarAll(0) )
			//warpPerspective(InputArray src, OutputArray dst, InputArray M, Size dsize, int flags=INTER_LINEAR, int borderMode=BORDER_CONSTANT, const Scalar& borderValue=Scalar())
			//cv::imshow("converted", matConverted);	
			
			cv::threshold( matMarker, matMarker, bw_thresh, 255, CV_THRESH_BINARY);
			//cvThreshold(iplMarker, iplMarker, bw_thresh, 255, CV_THRESH_BINARY);
			


//now we have a B/W image of a supposed Marker

			// check if border is black
			int code = 0;
			for (int i = 0; i < 6; ++i) {
				int pixel1 = matMarker.at<unsigned char>(0, i); //top
				int pixel2 = matMarker.at<unsigned char>(5, i); //bottom
				int pixel3 = matMarker.at<unsigned char>(i, 0); //left
				int pixel4 = matMarker.at<unsigned char>(i, 5); //right
				if ( ( pixel1 > 0 ) || ( pixel2 > 0 ) || ( pixel3 > 0 ) || ( pixel4 > 0 ) ) {
					code = -1;
					break;
				}
			}

			if ( code < 0 ) continue;

			//copy the BW values into cP
			int cP[4][4];
			
			for ( int i = 0; i < 4; ++i) {
				for ( int j = 0; j < 4; ++j) {
					cP[i][j] = matMarker.at<unsigned char>((i+1),(j+1));
					cP[i][j] = (cP[i][j] == 0) ? 1 : 0; //if black then 1 else 0
				}
			}

			//save the ID of the marker
			int codes[4];
			codes[0] = codes[1] = codes[2] = codes[3] = 0;
			for (int i = 0; i < 16; i++)
			{
				int row = i >> 2;
				int col = i % 4;

				codes[0] <<= 1;
				codes[0] |= cP[row][col]; // 0°

				codes[1] <<= 1;
				codes[1] |= cP[3-col][row]; // 90°

				codes[2] <<= 1;
				codes[2] |= cP[3-row][3-col]; // 180°

				codes[3] <<= 1;
				codes[3] |= cP[col][3-row]; // 270°
			}

			// cout << codes[0] << " " << codes[1] << " " << codes[2] << " " << codes[3] << endl;

			if ( (codes[0]==0) || (codes[0]==0xffff) )
				continue;

			// account for symmetry
			code = codes[0];
			int angle = 0;
			for ( int i = 1; i < 4; ++i ) {
				if ( codes[i] < code ) {
					code = codes[i];
					angle = i;
				}
			}

			//correct order of corners
			if(angle != 0) {
				CvPoint2D32f corrected_corners[4];
				for(int i = 0; i < 4; i++)	corrected_corners[(i + angle)%4] = corners[i];
				for(int i = 0; i < 4; i++)	corners[i] = corrected_corners[i];
			}

			//	printf ("Found: %04x\n", code);

			//cout << "found marker " << std::hex << setfill('0') << setw(2) << nouppercase << code << endl;

#ifdef MY_DEBUG
			if ( isFirstMarker ) {
				isFirstMarker = false;
				cv::Mat matTmp = cv::Mat( 100, 100, CV_8UC1);
				cv::resize( matMarker, matTmp, matTmp.size(), 0, 0, cv::INTER_NEAREST);
				cv::imshow("marker", matTmp);
			}
#endif MY_DEBUG

			// transfer camera coords to screen coords
			for(int i = 0; i < 4; i++) {
				corners[i].x -= picSize.width/2;
				corners[i].y = -corners[i].y + picSize.height/2;
			}

			CvPoint2D32f c_corners[4];
			c_corners[0] = corners[0];
			c_corners[1] = corners[1];
			c_corners[2] = corners[2];
			c_corners[3] = corners[3];

			estimateSquarePose( markerPoseResultMatrix, c_corners, 0.045 );

			MarkerTracker::markerData myMarker;
			myMarker.markerID = code;
			for(int i = 0; i<16; i++) {
				myMarker.resultMatrix[i] = markerPoseResultMatrix[i];
			}

			tmpMarkers.push_back(myMarker);
		
		} // if( approx.size() == 4 ) {
//------------------------------------------------------------------------------------
	} // end main for loop looking for contours for( size_t i = 0; i < mCvContours.size(); i++) {
//====================================================================================
			
	foundMarkers->clear();

	std::vector<MarkerTracker::markerData>::iterator cp_iter;
	for(cp_iter = tmpMarkers.begin(); cp_iter < tmpMarkers.end(); cp_iter++) {
		foundMarkers->push_back(*cp_iter);
	}


}
