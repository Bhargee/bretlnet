#include <opencv2/opencv.hpp>
#include "server.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void callback(char *data) {
    Mat m(Size(.1*480, .1*640), CV_8UC3, data, Mat::AUTO_STEP);
    Mat r;
    resize(m, r, Size(480, 640));
    imshow("window", r);
    waitKey(0); 
} 

int main() {
    namedWindow("window", 1);
    Server s(Server::Protocol::UDP, 3456, 9216, callback);
    s.Serve();
}

