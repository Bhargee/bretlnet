#include <iostream>
#include <raspicam/raspicam_cv.h>
#include "client.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std; 
 
int main () {
   
    // first set up networking
    Client sender(Client::Protocol::UDP, "127.0.0.1", 3456);
    try {
        sender.Connect();
    } catch (exception &e) {
        cerr << e.what() << endl;
        return -1;
    }
    // then the raspi camera
    raspicam::RaspiCam_Cv Camera;
    cv::Mat image;
    cv::Mat resized;
    int nCount=100;
    //set camera settings to capture black and white images
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
    cout << "Opening Camera..." << endl;
    if (!Camera.open()) {
        cerr << "Error opening the camera" << endl;
        return -1;
    }
    //Start capture
    cout << "Capturing " << nCount << " frames ...." << endl;
    for ( int i=0; i<nCount; i++ ) {
        Camera.grab();
        Camera.retrieve (image);
        cv::resize(image, resized, cv::Size(165, 240));
        sender.Send(resized.data, resized.total() * resized.elemSize());
    }
    cout<<"Stop camera..."<<endl;
    Camera.release();
}
