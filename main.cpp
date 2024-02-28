#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <ctime>
#include <time.h>
#include <helper.h>
#include <match.h>
#include <puzzle.h>

int main(int argc, char **argv){

    srand((unsigned int)time(NULL));
    time_t start, end;
    start = time(NULL);
    char file[50];
    std::vector<puzzlePiece> pieces;
    std::vector<int> ren = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};    
    std::vector<int> answerMap;
    std::deque<std::deque<int>> mapinit = {{0,1,2,3}, {4,5,6,7}, {8,9,10,11}, {12,13,14,15}}; // the map for the initial order of scanned pieces

    /*** SCANNING PUZZLE ***/
    do{
        int i=rand() % ren.size();                              
        sprintf(file, "../images/colour%d.png",ren[i]);
        cv::Mat image = cv::imread(file);

        puzzlePiece piece;
        piece.setPiece(image);
        pieces.push_back(piece);
        answerMap.push_back(ren[i]);
        ren.erase(ren.begin() + i);
    }while(!ren.empty());

    /*** PRINT ANSWER INDEX ***/ 
    for(int i=0; i<answerMap.size(); i++){
        std::vector<int>::iterator it = std::find(answerMap.begin(), answerMap.end(), i+1);
        std::cout << it-answerMap.begin() << " ";
        if(i%4 == 3){std::cout << std::endl;}
    }

    /*** SCORING AND MAPPING ***/
    std::cout << "SCANING taken : " << time(NULL)-start << "s" <<std::endl;
    start = time(NULL);
    std::deque<std::deque<int>> map = matchPuzzle::scoring_and_mapping(pieces);
    end = time(NULL);
    std::cout << "SCORING taken : " << end-start << "s" << std::endl;

    /*** DRAWING PUZZLE ***/
    cv::Mat org =  matchPuzzle::draw_deque_map2(mapinit, pieces,800);
    cv::Mat res = matchPuzzle::draw_deque_map2(map, pieces, 500);
    std::vector<cv::Mat> ans = {org, res};
    std::vector<std::string> names = {"original", "result"};
    helper::shows(ans,names);

    return 0;
}
