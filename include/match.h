#ifndef __MATCH_H
#define __MATCH_H

#include <helper.h>
#include <puzzle.h>
#include <tuple>
#include <deque>
#include <numeric>
#include <algorithm>

enum direction{
    LEFT,
    BOTTOM,
    RIGHT,
    TOP,
};

struct couple{

    std::pair<int, direction> reference;
    std::pair<int, direction> matched;

    double score;
};


namespace matchPuzzle{

    cv::Mat compareShape(puzzlePiece A, puzzlePiece B);
    
    std::vector<std::vector<double>> DTW_graphing(std::vector<double> line1, std::vector<double> line2, bool reverse);
    
    std::vector<std::vector<double>> DTW_graphing(std::vector<cv::Point2f> line1, std::vector<cv::Point2f> line2, bool reverse);
    
    std::vector<double> DTW_searching(std::vector<std::vector<double>> graph);
    
    double scoring(std::vector<double> line1, std::vector<double> line2);
    
    double scoring(std::vector<cv::Point2f> line1, std::vector<cv::Point2f> line2);
    
    cv::Mat get_puzzle_original(std::vector<puzzlePiece> ps);
   
    void find_min_score(std::vector<puzzlePiece> & ps);
    
    void set_score(std::vector<puzzlePiece> & ps);
    
    int find_reference(std::vector<puzzlePiece> & ps);
    
    std::deque<std::deque<int>> find_map(std::vector<puzzlePiece> &ps);
    
    void update_deque_map(std::deque<std::deque<int>> & map, int num, direction dir, int &curRow, int & curCol);
    
    cv::Mat draw_deque_map(std::deque<std::deque<int>> map, std::vector<puzzlePiece> ps);
    cv::Mat draw_deque_map2(std::deque<std::deque<int>> map, std::vector<puzzlePiece> ps, int gap);
    
    std::pair<int,int> find_couple(std::vector<puzzlePiece> & ps, int refIdx, int edgeIdx);
    void check(std::vector<puzzlePiece> & ps);
    std::deque<std::deque<int>> scoring_and_mapping(std::vector<puzzlePiece> & ps);

    std::vector<couple> find_couples(std::vector<puzzlePiece> ps);
    std::deque<std::deque<int>> newMapping(std::vector<puzzlePiece> ps);
}


#endif