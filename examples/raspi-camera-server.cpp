#include <opencv2/opencv.hpp>
#include "server.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void callback(char *data) {
    Mat m(Size(165, 240), CV_8UC1, data, Mat::AUTO_STEP);
    Mat r;
    resize(m, r, Size(330, 480));
    imshow("window", r);
    waitKey(0); 
} 

int main() {
    namedWindow("window", 1);
    Server s(Server::Protocol::UDP, 3456, 39600, callback);
    s.Serve();
}

