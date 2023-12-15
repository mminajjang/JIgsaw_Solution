#include <iostream>
#include <opencv2/opencv.hpp>
#include <helper.h>


int main(int argc, char **argv){

    char name[50];

    for (int i=0; i<2; i++){
        sprintf(name,"../images/cropped_image_%d.png", i);
        cv::Mat img = cv::imread(name);


    }

    return 0;
}