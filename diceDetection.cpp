#include <iostream>
#include <sstream>
#include <opencv2/core.hpp>
#include "opencv2/objdetect.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <array>
using namespace cv;
using namespace std;

int countPips(cv::Mat dice){

  cv::resize(dice, dice, cv::Size(150, 150));
  cvtColor(dice, dice, CV_BGR2GRAY);

  // threshold
  // 150, 255

  cv::threshold(dice, dice, 150, 255, cv::THRESH_BINARY | CV_THRESH_OTSU );

  namedWindow("Processed", true);
  imshow("Processed", dice);

  cv::floodFill(dice, cv::Point(0,0), cv::Scalar(255));
  cv::floodFill(dice, cv::Point(0,149), cv::Scalar(255));
  cv::floodFill(dice, cv::Point(149,0), cv::Scalar(255));
  cv::floodFill(dice, cv::Point(149,149), cv::Scalar(255));

  cv::SimpleBlobDetector::Params params;

  params.filterByInertia = true;
  params.minInertiaRatio = .5;

  std::vector<cv::KeyPoint> keypoints;

  cv::Ptr<cv::SimpleBlobDetector> blobDetector = cv::SimpleBlobDetector::create(params);

  blobDetector->detect(dice, keypoints);

 // cout << keypoints.size();
  return keypoints.size();
}


std::vector<int>  ret7(std::vector<int> pips ) {
  std::vector<int> pipsModded = pips;
  std::vector<int> sevens;


  for(int i = 0; i < pips.size()-1; i++) {
	  for(int x = i + 1; x < pips.size(); x++) {
		if(pips.at(i) + pips.at(x) == 7){
		//	cout << i<< "i" << endl;
		//	cout << x<< "x" << endl;
			sevens.push_back(i);
			sevens.push_back(x);
			return sevens;
		}
  	}
  }
  return sevens;
}

int main( int argc, char** argv )
{
  cout << "test" << endl;
  cv::Mat frame;
  cv::Mat unprocessFrame;
  cv::Mat finalFrame;
  for(;;) {
    //picture filename
    String fileName = "dice_1.jpg";
    std::vector<int> allPips;
    frame = imread(fileName, CV_LOAD_IMAGE_UNCHANGED);
    if(frame.empty()) {
      cout <<"Error" << endl;
      system("pause");
      return -1;
    }

    namedWindow("orig", CV_WINDOW_AUTOSIZE);
    imshow("orig", frame);

    unprocessFrame = frame.clone();
    cvtColor(frame, frame, CV_BGR2GRAY);

    cv::threshold(frame, frame, 150, 255, cv::THRESH_BINARY | CV_THRESH_OTSU );

    cv::Canny( frame, frame, 2, 2*2, 3, false );

    std::vector<std::vector<cv::Point> > diceContours;
    // std::vector<std::vector<cv::Point> > actualDiceContours;
    std::vector<cv::Vec4i> diceHierarchy;
   // cv::findContours( frame.clone(), actualDiceContours, diceHierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE );
 cv::findContours( frame.clone(), diceContours, diceHierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE );


   /* for(int i = 0; i < actualDiceContours.size(); i++){ 
	    double diceContourArea = cv::contourArea(actualDiceContours[i]);

      //THIS STRATIFIES BY SIZE BE CAREUL WITH THIS LINEEEEEE
      if(diceContourArea > 100) {
	      diceContours.push_back(actualDiceContours.at(i));
      }

    }*/
// cout << diceContours.size();
    for(int i=0; i < diceContours.size(); i++){

      double diceContourArea = cv::contourArea(diceContours[i]);

      //THIS STRATIFIES BY SIZE BE CAREUL WITH THIS LINEEEEEE
      if(diceContourArea > 100) {

        cv::Rect diceBoundsRect = cv::boundingRect( cv::Mat(diceContours[i]) );

        cv::Mat diceROI = unprocessFrame(diceBoundsRect);

        int numberOfPips = countPips(diceROI);
        if(numberOfPips > 0) {
          allPips.push_back(numberOfPips);

        }
	 /*  std::ostringstream diceText;
	    diceText << i ;
	    // draw value
	    cv::putText(unprocessFrame, diceText.str(),cv::Point(diceBoundsRect.x, diceBoundsRect.y + diceBoundsRect.height + 20 ),cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar::all(255), 1, 8);
	    // draw rect
	   cv::Scalar color = cv::Scalar( 0, 153, 255 );
	 //  cv::rectangle( unprocessFrame, diceBoundsRect.br(), diceBoundsRect.tl(), color, 2,8, 0 );

	   cv::rectangle( unprocessFrame, diceBoundsRect.tl(), diceBoundsRect.br(), color, 2,8, 0 );

        	cv::imshow("frame", unprocessFrame);*/
      }
    }


    std::vector<int> sevens = ret7(allPips);
    if(!sevens.empty()) {
    for(int i = 0; i < sevens.size(); i++){
	    finalFrame = unprocessFrame.clone();
	     cv::Rect diceBoundsRect = cv::boundingRect(cv::Mat(diceContours[54]));

	  //  cv::Rect diceBoundsRect = cv::boundingRect(cv::Mat(diceContours[sevens.at(i)]));
	    int numberOfPips = allPips.at(sevens.at(i));
	    
	    std::ostringstream diceText;
	    diceText << numberOfPips;
	    // draw value
	    cv::putText(unprocessFrame, diceText.str(),cv::Point(diceBoundsRect.x, diceBoundsRect.y + diceBoundsRect.height - 50 ),cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar::all(255), 1, 8);
	    // draw rect
	   cv::Scalar color = cv::Scalar( 0, 153, 255 );
	 //  cv::rectangle( unprocessFrame, diceBoundsRect.br(), diceBoundsRect.tl(), color, 2,8, 0 );

	   cv::rectangle( unprocessFrame, diceBoundsRect.tl(), diceBoundsRect.br(), color, 2,8, 0 );
	   namedWindow("Final", CV_WINDOW_AUTOSIZE);
	   imshow("FINAL", unprocessFrame);
    }

   /* for(int i = 0; i<sevens.size(); i++) {
      finalFrame = unprocessFrame.clone();
      //SEVEN IS INT FIX THE BOTTOM LINE
      cv::Rect diceBoundsRect = cv::boundingRect( cv::Mat(diceContours[sevens.at(i)]) );

      cv::Mat diceROI = unprocessFrame(diceBoundsRect);

      int numberOfPips = countPips(diceROI);

      std::ostringstream diceText;
      diceText << "val: " << numberOfPips;
      // draw value
      cv::putText(unprocessFrame, diceText.str(),cv::Point(diceBoundsRect.x, diceBoundsRect.y + diceBoundsRect.height + 20 ),cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar::all(255), 1, 8);
      // draw rect
      cv::Scalar color = cv::Scalar( 0, 153, 255 );
      cv::rectangle( unprocessFrame, diceBoundsRect.tl(), diceBoundsRect.br(), color, 2,8, 0 );

      namedWindow("Final", CV_WINDOW_AUTOSIZE);
      imshow("FINAL", unprocessFrame);
    }*/

    /*
    for(int i = 0; i < sevens.size(); i++) {

    std::vector<std::vector<cv::Point> > diceContours;
    std::vector<cv::Vec4i> diceHierarchy;
    cv::findContours( frame.clone(), diceContours, diceHierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE );

    cv::Rect diceBoundsRect = cv::boundingRect( cv::Mat(diceContours[i]) );

    cv::Mat diceROI = unprocessFrame(diceBoundsRect);
    int numberOfPips = countPips(diceROI);

    std::ostringstream diceText;
    diceText << "val: " <<numberOfPips;
    // draw value
    cv::putText(unprocessFrame, diceText.str(),cv::Point(diceBoundsRect.x, diceBoundsRect.y + diceBoundsRect.height + 20 ),cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar::all(255), 1, 8);
    // draw rect
    cv::Scalar color = cv::Scalar( 0, 153, 255 );
    cv::rectangle( unprocessFrame,diceBoundsRect.tl(), diceBoundsRect.br(), color, 2,8, 0 );

    cv::imshow("frame", unprocessFrame);


    if(cv::waitKey(30) >= 0) {
    break;
  }
}
*/

if(cv::waitKey(30) >- 0) {
  break;
}

}
else {
	cout << "empty";
}
}
}
