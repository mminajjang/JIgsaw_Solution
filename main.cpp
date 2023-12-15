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
    std::vector<int> ren = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};     
    std::vector<int> test;
    do{
        int i=0;//rand() % ren.size();
        sprintf(file, "../images/cropped_image_%d.png",ren[i]);
        cv::Mat image = cv::imread(file);
        // if(ren[i] == 1){
        //     image = helper::rotate(image, 90);
        // }
        
        puzzlePiece piece;
        piece.setPiece(image);
        pieces.push_back(piece);
        test.push_back(ren[i]);
        ren.erase(ren.begin() + i);
    }while(!ren.empty());
    // std::cout << std::get<0>(pieces[0].piece_type) << " "<< std::get<1>(pieces[0].piece_type) << " " << std::get<2>(pieces[0].piece_type)<<std::endl;
    /* Visualizing answer map */
    for(int i=0; i<test.size(); i++){
        std::vector<int>::iterator it = std::find(test.begin(), test.end(), i+1);
        std::cout << it-test.begin() << " ";
        if(i%4 == 3){std::cout << std::endl;}
    }

    cv::Mat org = matchPuzzle::get_puzzle_original(pieces);
    // // helper::show(org);
    std::cout << "SCANING taken : " << time(NULL)-start << "s" <<std::endl;
    start = time(NULL);
    // matchPuzzle::find_min_score(pieces);
    std::deque<std::deque<int>> map = matchPuzzle::scoring_and_mapping(pieces);
    // std::deque<std::deque<int>> map = matchPuzzle::scoring_and_mapping(pieces);
    end = time(NULL);
    std::cout << "SCORING taken : " << end-start << "s" << std::endl;
    // // std::deque<std::deque<int>> map = matchPuzzle::find_map(pieces);
    cv::Mat res = matchPuzzle::draw_deque_map(map, pieces);
    std::vector<cv::Mat> ans = {org, res};
    std::vector<std::string> names = {"original", "result"};
    helper::shows(ans,names);

    return 0;
}