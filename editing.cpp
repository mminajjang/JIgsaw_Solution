// /***
// Colour piece check
// ***/

// #include <iostream>
// #include <opencv2/opencv.hpp>
// #include <helper.h>
// #include <cmath>
// #include <algorithm>
// #include <puzzle.h>

// int main(int argc, char **argv){
//     cv::Mat img = cv::imread("../figure_imgs/p7.png");
//     cv::Mat gray; cv::cvtColor(img,gray, cv::COLOR_BGR2GRAY);
//     cv::threshold(gray, gray, 200,255,cv::THRESH_BINARY_INV);
//     std::vector<std::vector<cv::Point>> contours;
//     std::vector<cv::Vec4i> hierarchy;

//     cv::findContours(gray, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
//     cv::drawContours(img, contours, -1, cv::Scalar(0,0,255), 2, 8);
//     while(cv::waitKey() != 27){
//         cv::imshow("img", img);
//     } 
//     std::cout << "contours : " << contours.size() <<std::endl;
//     return 0;
// }
/***
crop puzzles from canvas
***/
#include <iostream>
#include <opencv2/opencv.hpp>
#include <helper.h>
#include <cmath>
#include <algorithm>
#include <puzzle.h>

int main(int argc, char **argv){
    
    cv::Mat img = cv::imread("../figure_imgs/newblack.png");
    // cv::Mat cr = cv::imread("../figure_imgs/newcolour.png");
    cv::Mat gray; cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    char name[50];
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    int x,y,w,h;
    cv::findContours(gray, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
    for(int i=0; i< contours.size(); i++){
        if(hierarchy[i][3] != -1){
            sprintf(name, "../figure_imgs/blackP%d.png", i+20);
            std::cout << name << std::endl;
           cv::Rect rect = cv::boundingRect(contours[i]);
           cv::Rect roi = cv::Rect(rect.x-10, rect.y-10, rect.width+20, rect.height+20);
           cv::imwrite(name, img(roi));
        }
    }
    // cv::drawContours(img, contours, -1, cv::Scalar(0,0,255), 3);
    // cv::imwrite("../figure_imgs/contours.png", img);
    while(cv::waitKey() != 27){
        cv::imshow("img", img);
    }
    return 0;
}
// /***
// // Coloring all puzzle in canvas 
// ***/
// #include <iostream>
// #include <opencv2/opencv.hpp>
// #include <helper.h>
// #include <cmath>
// #include <algorithm>
// #include <puzzle.h>


// int main(int argc, char **argv){

//     char name[50];
//     char rename[50];
//     cv::Mat img = cv::imread("../figure_imgs/newblack.png");
//     int y= 0;int yi = 0; int x=0;int xi =0;
//     for(int c=0; c<img.cols; c++){
//         for(int r=0; r<img.rows; r++){
//             if(img.at<cv::Vec3b>(r,c)[0] != 255 && 
//                     img.at<cv::Vec3b>(r,c)[1] != 255 &&
//                     img.at<cv::Vec3b>(r,c)[2] != 255){
//                         xi = c; yi = r;
//                         c = img.cols+ 100;
//                         break;
//                 }
//         }
//     } 
//     x = xi;
//     for(int c=0; c<img.cols; c++){
//         y = yi; x+=1;
//         for(int r=0; r<img.rows; r++){
//             y += 1;
//             if(img.at<cv::Vec3b>(r,c)[0] != 255 && 
//                     img.at<cv::Vec3b>(r,c)[1] != 255 &&
//                     img.at<cv::Vec3b>(r,c)[2] != 255){
//                         img.at<cv::Vec3b>(r,c)[0] = 255 * ((double)y/4000);
//                         // img.at<cv::Vec3b>(r,c)[1] = 0;
//                         img.at<cv::Vec3b>(r,c)[2] = 255 * ((double)x/4000);;
//                 }
//         }
//     } 
//         cv::imwrite("../figure_imgs/newcolour.png", img);
//         // while (cv::waitKey() != 27)
//         //     {
//         //         cv::imshow("image", img);
//         //     }
    
    
//     return 0;
// }


/***
 drawing all puzzle respecting contours
***/

// #include <iostream>
// #include <opencv2/opencv.hpp>
// #include <helper.h>
// #include <cmath>
// #include <algorithm>
// #include <puzzle.h>


// int main(int argc, char **argv){

//     char name[50];
//     char rename[50];
//     cv::Mat canvas = cv::Mat(4000,4000,16, cv::Scalar(255,255,255));
//     std::vector<cv::Point> ref_points;
//     // ref_points.push_back(cv::Point(10,10));
//     int first_x = 200; int curr_x = 500;
//     int next_y = 200; int curr_y = 500;
//     std::vector<puzzlePiece> pieces;
//     for (int i=1; i<17; i++){
//         sprintf(name,"../images/cropped_image_%d.png", i);
//         cv::Mat img = cv::imread(name);
      
//         puzzlePiece piece;
//         piece.setPiece(img);
//         pieces.push_back(piece);

//         if (i == 1 || i == 5 || i == 9 || i == 13){
//             curr_y = next_y;
//             curr_x = 200;
//             next_y += abs(piece.edges[0].back().y - piece.edges[0].front().y) + 500;
//             ref_points.push_back(cv::Point(curr_x, curr_y));
//             curr_x += 500 + abs(piece.edges[3].front().x - piece.edges[3].back().x); 
//         }
//         else{
//             ref_points.push_back(cv::Point(curr_x, curr_y));
//             curr_x += 500 + abs(piece.edges[3].front().x - piece.edges[3].back().x); 
            
//         }
//     }
//     for(auto pp : ref_points){
//         std::cout << pp <<std::endl;
//     }
//     int n = 0;
//     std::vector<std::vector<cv::Point2f>> newContours;
//     for(auto ps : pieces){
//         std::vector<cv::Point2f> newContour;
//         int X = ref_points[n].x;// - ps.contour.front().x;
//         int Y = ref_points[n].y;// - ps.contour.front().y;
//         // cv::circle(canvas, cv::Point(X, Y), 4, cv::Scalar(0,0,255), 4, 8);
//         for(int i=0; i<ps.contour.size(); i++){
//             cv::Point newpt = cv::Point(ps.contour[i].x + X - ps.edges[0][0].x, ps.contour[i].y + Y - ps.edges[0][0].y);
//             newContour.push_back(newpt);
//             cv::circle(canvas, newpt, 2, cv::Scalar(0,0,0), 2, 8);
//         }
//         newContours.push_back(newContour);
//         n++;
//     }
//     int num = 0;
//     for(auto ct:newContours){
//         std::cout << "new contorus " << num <<std::endl; num++;
//         for(int r=0; r<canvas.rows; r++){
//             for(int c=0; c<canvas.cols; c++){
//                 if(cv::pointPolygonTest(ct, cv::Point(c,r), false) > -1){
//                     canvas.at<cv::Vec3b>(r, c)[0] = 0;
//                     canvas.at<cv::Vec3b>(r, c)[1] = 0;
//                     canvas.at<cv::Vec3b>(r, c)[2] = 0;
//                 }
//             }
//         }
//     }

//     // cv::resize(canvas, canvas, cv::Size(1000,1000));
//     cv::imwrite("../figure_imgs/newblack.png", canvas);
//     while (cv::waitKey() != 27)
//         {
//             cv::imshow("image", canvas);
//         }
    
    
//     return 0;
// }