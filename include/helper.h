#ifndef __HELPER_H
#define __HELPER_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector> 
#include <algorithm> 

typedef std::vector<cv::Point> CONTOUR;
typedef std::vector<cv::Point2f> CONTOUR2f;
typedef std::vector<cv::Point> POINTS;
typedef std::vector<cv::Point2f> POINTS2f;

namespace helper{
    // cv::Mat find_obj(cv::Mat image, int min, int max);
    void show(cv::Mat image, std::string name);
    void show(cv::Mat image);
    void show(std::vector<cv::Mat> images);
    void shows(std::vector<cv::Mat> images);
    void shows(std::vector<cv::Mat> images, std::vector<std::string> names);

    cv::Mat rotate(cv::Mat image, double angle);
    cv::Mat rotate(cv::Mat image, double angle, cv::Point2f centre); 

    CONTOUR2f rotate_contour(CONTOUR2f cnt, double angle, cv::Point2f centre);
    CONTOUR2f rotate_points(CONTOUR2f pts, double angle);
    cv::Mat resize(cv::Mat image, double width, double height);
    
}

namespace imageProcesser{
    cv::Mat cvtGray(cv::Mat image);

    CONTOUR2f find_contour(cv::Mat image);
    cv::Point2f find_centre(CONTOUR2f contour);
    POINTS2f find_corner(cv::Mat image);
    POINTS2f find_goodFeatures(cv::Mat image);
    std::vector<CONTOUR2f> find_edges(CONTOUR2f contour2f, POINTS2f corner);
    std::vector<CONTOUR2f> refine_edges(std::vector<CONTOUR2f> edges);
    std::vector<CONTOUR2f> transform_edges(std::vector<CONTOUR2f> edges);

    double find_angle(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);
    void reorder_points(POINTS2f & pts);
    bool ccw_comparator(cv::Point2f left,cv::Point2f cent, cv::Point2f  right);
    POINTS2f find_rectangle(POINTS2f candidates, double area,cv::Mat image);
}

// namespace videoProcesser{
//     cv::Mat find_obj(cv::Mat image);
// }

#endif