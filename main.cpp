#include <opencv2/opencv.hpp>
#include "LFFD_MNN.h"
#include <chrono>

using namespace cv;
using namespace std;


int main(int argc, char** argv) {
    float f;
    float FPS[16];
    int i, Fcnt=0;
    Mat frame;
    chrono::steady_clock::time_point Tbegin, Tend;

    for(i=0;i<16;i++) FPS[i]=0.0;

	LFFD* face_detector = new LFFD(5); //5 scales
//	LFFD* face_detector = new LFFD(8); //8 scales (more accurate, but slower)

    VideoCapture cap("Walks2.mp4");
    if (!cap.isOpened()) {
        cerr << "ERROR: Unable to open the video" << endl;
        return 0;
    }

    cout << "Start grabbing, press ESC on Live window to terminate" << endl;
    while(1){
//        frame=imread("25.jpg");  //need to refresh frame before dnn class detection
        cap >> frame;
        if (frame.empty()) {
            cerr << "ERROR: Unable to grab from the camera" << endl;
            break;
        }

        Tbegin = chrono::steady_clock::now();

        std::vector<FaceInfo> finalBox;

        face_detector->detect(frame,finalBox,frame.rows,frame.cols);

        for(size_t i = 0; i < finalBox.size(); i++) {
            FaceInfo facebox = finalBox[i];
            cv::Rect box=cv::Rect(facebox.x1,facebox.y1,facebox.x2-facebox.x1,facebox.y2-facebox.y1);
            cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);
        }

        //cv::imwrite("result_25.jpg", frame); //save the result

        Tend = chrono::steady_clock::now();
        //calculate frame rate
        f = chrono::duration_cast <chrono::milliseconds> (Tend - Tbegin).count();
        if(f>0.0) FPS[((Fcnt++)&0x0F)]=1000.0/f;
        for(f=0.0, i=0;i<16;i++){ f+=FPS[i]; }
        putText(frame, format("FPS %0.2f", f/16),Point(10,20),FONT_HERSHEY_SIMPLEX,0.6, Scalar(0, 0, 255));

        //show output
        imshow("RPi 4 - 1,95 GHz - 2 Mb RAM", frame);

        char esc = waitKey(5);
        if(esc == 27) break;
    }

    cout << "Closing the camera" << endl;
    destroyAllWindows();
    delete face_detector;
    cout << "Bye!" << endl;
    return 0;
}
