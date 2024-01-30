#include <iostream>
#include <opencv2/opencv.hpp>
#include <helper.h>
#include <cmath>
#include <algorithm>


int main(int argc, char **argv){

    char name[50];
    char rename[50];
    int i = 14;
    // for (int i=11; i<15; i+=3){
        sprintf(name,"../images/cropped_image_%d.png", i);
        cv::Mat img = cv::imread(name);
        CONTOUR2f contour = imageProcesser::find_contour(img);
        POINTS2f corners = imageProcesser::find_corner(img);
        std::vector<CONTOUR2f> edges = imageProcesser::find_edges(contour, corners);
        
        std::vector<int> width; 
        std::vector<int> height;

        for (auto edge: edges){
            int minX = 1000000;
            int minY = 1000000;
            int maxX = 0;
            int maxY = 0;
            for (auto e:edge){
                if (e.x > maxX){
                    maxX = (int)ceil(e.x);
                }
                if (e.x < minX){
                    minX = (int)floor(e.x);
                }

                if (e.y > maxY){
                    maxY = (int)ceil(e.y);
                }
                if (e.y < minY){
                    minY = (int)floor(e.y);
                }
            }
            width.push_back(maxX-minX);
            height.push_back(maxY-minY);

            cv::Mat crop = img(cv::Rect(minX, minY, maxX-minX, maxY-minY));
            helper::show(crop);
        }
        // sprintf(rename,"../figure_imgs/figure_extract_edges%d.png", i);
        // bool res = cv::imwrite(rename, white);
        // std::cout << "res :" << res << std::endl;
    // }

    return 0;
}