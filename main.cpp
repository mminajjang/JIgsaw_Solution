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
    std::vector<int> ren = {0,1,9,3}; int cnt = 0;
    /*** SCANNING PUZZLE ***/
    do{             
        sprintf(file, "../images/colour%d.png",ren[cnt]);
        cv::Mat image = cv::imread(file);

        puzzlePiece piece;
        piece.setPiece(image);
        pieces.push_back(piece);
        cnt++;
    }while(cnt<ren.size());
    int even = 1; char nnn[50];
    std::vector<cv::Mat> imgs;
    std::vector<std::vector<cv::Point2f>> lines;
    for(auto piece : pieces){
        cv::Mat tempImg = cv::Mat(piece.image.rows, piece.image.cols, piece.image.type(), cv::Scalar::all(255));
        std::vector<cv::Point2f> line;
        if (even%2 == 0){
            std::cout<<"left  " << piece.edges[0].size() << std::endl;
            int index = 0;
            for(auto pt:piece.edges[0]){
                
                if(index==40){
                    // std::cout << "x : " << pt.x <<  " y : " << pt.y << std::endl; 
                    cv::circle(tempImg, pt, 4, cv::Scalar(0,0,255), 4, 8);index = 0;
                    line.push_back(cv::Point2f((double)pt.x, (double)pt.y));
                }
                else{
                    cv::circle(tempImg, pt, 2, cv::Scalar::all(0), 2, 8);
                }
                index++;line.push_back(cv::Point2f((double)pt.x, (double)pt.y));
            }
            
        }
        else{
            std::cout<<"right  " << piece.edges[2].size() << std::endl;
            int index = 0;
            for(auto pt:piece.edges[2]){
                if(index==40){
                    // std::cout << "x : " << pt.x <<  " y : " << pt.y << std::endl; 
                    cv::circle(tempImg, pt, 4, cv::Scalar(0,0,255), 4, 8);index = 0;
                    // line.push_back(cv::Point2f((double)pt.x, (double)pt.y));
                }
                else{
                    cv::circle(tempImg, pt, 2, cv::Scalar::all(0), 2, 8);
                }
                index++;line.push_back(cv::Point2f((double)pt.x, (double)pt.y));
            }
            
        }
        even++;
        sprintf(nnn, "../images/edge%d.png", even);
        cv::imwrite(nnn, tempImg);
        imgs.push_back(tempImg);
        lines.push_back(line);
    }

    std::vector<std::vector<double>> pair1 = matchPuzzle::DTW_graphing(lines[0],lines[1],0);
    for(int i=0; i<pair1.size(); i++){
        for(int j=0; j<pair1[i].size(); j++){
            std::cout << pair1[i][j] << " " ;
        }std::cout << std::endl;
    }
    std::vector<double> weight = matchPuzzle::DTW_searching(pair1);
    std::cout << "weight  : " << std::accumulate(weight.begin(), weight.end(),0) / weight.size() << std::endl;
    // std::vector<std::vector<double>> pair2 = matchPuzzle::DTW_graphing(lines[2],lines[3], 0);
    // for(int i=0; i<pair2.size(); i++){
    //     for(int j=0; j<pair2[i].size(); j++){
    //         std::cout << pair2[i][j] << " " ;
    //     }std::cout << std::endl;
    // }
    // weight =matchPuzzle::DTW_searching(pair2);
    // std::cout << "weight  : " << std::accumulate(weight.begin(), weight.end(),0) / weight.size() << std::endl;
    // helper::shows(imgs);
    return 0;
}


// int main(int argc, char **argv){

//     srand((unsigned int)time(NULL));
//     time_t start, end;
//     start = time(NULL);
//     char file[50];
//     std::vector<puzzlePiece> pieces;
//     std::vector<int> ren = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};    
//     std::vector<int> answerMap;
//     std::deque<std::deque<int>> mapinit = {{0,1,2,3}, {4,5,6,7}, {8,9,10,11}, {12,13,14,15}}; // the map for the initial order of scanned pieces

//     /*** SCANNING PUZZLE ***/
//     do{
//         int i=rand() % ren.size();                              
//         sprintf(file, "../images/colour%d.png",ren[i]);
//         cv::Mat image = cv::imread(file);

//         puzzlePiece piece;
//         piece.setPiece(image);
//         pieces.push_back(piece);
//         answerMap.push_back(ren[i]);
//         ren.erase(ren.begin() + i);
//     }while(!ren.empty());

//     /*** PRINT ANSWER INDEX ***/ 
//     for(int i=0; i<answerMap.size(); i++){
//         std::vector<int>::iterator it = std::find(answerMap.begin(), answerMap.end(), i+1);
//         std::cout << it-answerMap.begin() << " ";
//         if(i%4 == 3){std::cout << std::endl;}
//     }

//     /*** SCORING AND MAPPING ***/
//     std::cout << "SCANING taken : " << time(NULL)-start << "s" <<std::endl;
//     start = time(NULL);
//     std::deque<std::deque<int>> map = matchPuzzle::scoring_and_mapping(pieces);
//     end = time(NULL);
//     std::cout << "SCORING taken : " << end-start << "s" << std::endl;

//     /*** DRAWING PUZZLE ***/
//     cv::Mat org =  matchPuzzle::draw_deque_map2(mapinit, pieces,800);
//     cv::Mat res = matchPuzzle::draw_deque_map2(map, pieces, 500);
//     std::vector<cv::Mat> ans = {org, res};
//     std::vector<std::string> names = {"original", "result"};
//     helper::shows(ans,names);

//     return 0;
// }
