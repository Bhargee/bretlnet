#include <opencv2/opencv.hpp>
#include <iostream>
#include "client.hpp"

using namespace cv;
using namespace std;

int main() {
    Client c(Client::Protocol::UDP, "127.0.0.1", 3456);
    try {
        c.Connect();
    } catch (exception &e) {
        cout << e.what() << endl;
        return -1;
    }

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened()) 
        return -1;

    Mat frame;
    Mat resized;
    for(;;)
    {
        cap >> frame;
        resize(frame, resized, Size(.1*480, .1*640));
        c.Send(resized.data, resized.total() * resized.elemSize());
    }
    return 0;
}
