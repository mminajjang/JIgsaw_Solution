#ifndef __PUZZLE_H
#define __PUZZLE_H

#include <helper.h>
#include <utility>
#include <tuple>

enum pieceType{
    MIDDLE,
    RIM,
    CORNER,
};

struct puzzlePiece{
    cv::Mat image;
    double cols;// = (double)image.cols;
    cv::Mat extract_puzzle;
    CONTOUR2f contour;
    POINTS2f corners;
    cv::Point2f centre;
    std::vector<CONTOUR2f> edges;
    // std::vector<CONTOUR2f> transformed_edges;
    std::tuple<pieceType, int, int> piece_type;
    std::vector<cv::Mat> edgeImgs;
    std::vector<std::vector<double>> reduced_edges;

    std::vector<std::vector<cv::Point2f>> transformed_edges;

    void setPiece(cv::Mat img);

    /// @brief investigate the piece has straight edge as corner, rim or middle location.
    /// @param edges 
    /// @return tuple of( piece type / straighted edge indices unless -1 )
    std::tuple<pieceType, int, int> getPieceType(std::vector<CONTOUR2f> edges);

    /// @brief reduce dimension from edge points to distance between centre line 
    /// @param edges 
    /// @return 1 Dimension of each edge feature
    std::vector<std::vector<double>> reduceDimension(std::vector<CONTOUR2f> edges);
     
    /// @brief 
    // void setMatchedPiece();
    std::vector<std::vector<cv::Point2f>> transformPoints(std::vector<CONTOUR2f> edges, cv::Mat img, CONTOUR2f contour);

    // score, piece index, edge number 
    std::vector<std::tuple<double, int, int>>  scores;

    void rotatePiece(int num);
};

#endif