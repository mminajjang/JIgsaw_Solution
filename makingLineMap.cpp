#include <iostream>
#include <opencv2/opencv.hpp>
#include <helper.h>
#include <cmath>
#include <algorithm>
#include <puzzle.h>


int main(int argc, char **argv){

    char name[50];
    char rename[50];
    cv::Mat canvas = cv::Mat(6000,6000,16, cv::Scalar(255,255,255));
    std::vector<cv::Point> ref_points;
    // ref_points.push_back(cv::Point(10,10));
    int first_x = 500; int curr_x = 500;
    int next_y = 500; int curr_y = 500;
    std::vector<puzzlePiece> pieces;
    for (int i=1; i<17; i++){
        sprintf(name,"../images/cropped_image_%d.png", i);
        cv::Mat img = cv::imread(name);
      
        puzzlePiece piece;
        piece.setPiece(img);
        pieces.push_back(piece);

        if (i == 1 || i == 5 || i == 9 || i == 13){
            curr_y = next_y;
            curr_x = 500;
            next_y += abs(piece.edges[0].back().y - piece.edges[0].front().y) + 500;
            ref_points.push_back(cv::Point(curr_x, curr_y));
            curr_x += 500 + abs(piece.edges[3].front().x - piece.edges[3].back().x); 
        }
        else{
            ref_points.push_back(cv::Point(curr_x, curr_y));
            curr_x += 500 + abs(piece.edges[3].front().x - piece.edges[3].back().x); 
            
        }
    }
    for(auto pp : ref_points){
        std::cout << pp <<std::endl;
    }
    int n = 0;
    std::vector<std::vector<cv::Point2f>> newContours;
    for(auto ps : pieces){
        std::vector<cv::Point2f> newContour;
        int X = ref_points[n].x;// - ps.contour.front().x;
        int Y = ref_points[n].y;// - ps.contour.front().y;
        cv::circle(canvas, cv::Point(X, Y), 4, cv::Scalar(0,0,255), 4, 8);
        for(int i=0; i<ps.contour.size(); i++){
            cv::Point newpt = cv::Point(ps.contour[i].x + X - ps.edges[0][0].x, ps.contour[i].y + Y - ps.edges[0][0].y);
            newContour.push_back(newpt);
            cv::circle(canvas, newpt, 2, cv::Scalar(0,0,0), 2, 8);
        }
        newContours.push_back(newContour);
        n++;
    }
    int num = 0;
    // for(auto ct:newContours){
    //     std::cout << "new contorus " << num <<std::endl; num++;
    //     for(int r=0; r<canvas.rows; r++){
    //         for(int c=0; c<canvas.cols; c++){
    //             if(cv::pointPolygonTest(ct, cv::Point(c,r), false) > -1){
    //                 canvas.at<cv::Vec3b>(r, c)[0] = 0;
    //                 canvas.at<cv::Vec3b>(r, c)[1] = 0;
    //                 canvas.at<cv::Vec3b>(r, c)[2] = 0;
    //             }
    //         }
    //     }
    // }

    cv::resize(canvas, canvas, cv::Size(1000,1000));
    cv::imwrite("../figure_imgs/newblack.png", canvas);
    while (cv::waitKey() != 27)
        {
            cv::imshow("image", canvas);
        }
    
    
    return 0;
}