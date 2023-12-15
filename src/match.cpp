#include <match.h>

namespace matchPuzzle{

    cv::Mat compareShape(puzzlePiece A, puzzlePiece B){
        cv::Mat img = A.image.clone();
        for(int b=0; b<B.edgeImgs.size(); b++){
            cv::Mat temp = B.edgeImgs[b].clone();
            cv::threshold(temp, temp, 200,255,cv::THRESH_BINARY_INV);
            cv::Mat res;
            cv::matchTemplate(img, temp, res, cv::TM_CCOEFF_NORMED);
            // cv::normalize(res,res,0,1,cv::NORM_MINMAX,-1, cv::Mat());
            double minVal, maxVal; 
            cv::Point minLoc, maxLoc;
            // cv::Point matchLoc;
            cv::minMaxLoc( res, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
            if (maxVal > 0.85 && maxVal<0.97){ // if over 0.97, it detects straight line 
                cv::Point topLeft = maxLoc;
                cv::Point bottomRight = cv::Point(topLeft.x+temp.cols, topLeft.y+temp.rows);
                cv::rectangle(img, topLeft, bottomRight, cv::Scalar(0,255,0), 2, 8, 0);
                std::cout << maxVal <<std::endl;
                // helper::show(img);
                // return maxVal;
            }
            // else{
            //     std::cout << "NOT VALID: " << maxVal <<std::endl;
            // }
        }
        return img;
    }

    std::vector<std::vector<double>> DTW_graphing(std::vector<double> line1, std::vector<double> line2, bool reverse){
        int sizeA = line1.size();
        int sizeB = line2.size();

        std::vector<std::vector<double>> graph(sizeA, std::vector<double>(sizeB, -1));
        if(reverse){
            std::reverse(line2.begin(), line2.end());
        }
        
        for(int i=0; i<sizeA; i++){
            for(int j=0; j<sizeB; j++){
                if (i == 0 && j == 0){
                    graph[i][j] = sqrt(pow(line1[i]-line2[j], 2));//hypot(line1[i], line2[j]);
                }
                else if (i == 0 && j != 0){
                    graph[i][j] = sqrt(pow(line1[i]-line2[j], 2)) + graph[i][j-1];
                }
                else if (i != 0 && j == 0){
                    graph[i][j] = sqrt(pow(line1[i]-line2[j], 2)) + graph[i-1][j];
                }
                else {
                    graph[i][j] = sqrt(pow(line1[i]-line2[j], 2)) + std::min({graph[i-1][j-1], graph[i-1][j], graph[i][j-1]});
                }
            }
        }
        return graph;

    }

    std::vector<std::vector<double>> DTW_graphing(std::vector<cv::Point2f> line1, std::vector<cv::Point2f> line2, bool reverse){
        int sizeA = line1.size();
        int sizeB = line2.size();

        std::vector<std::vector<double>> graph(sizeA, std::vector<double>(sizeB, -1));
        // TODO :  회전에 따라 reverse 바꿔야함

        if(reverse){
            std::reverse(line2.begin(), line2.end());
        }
        
        for(int i=0; i<sizeA; i++){
            for(int j=0; j<sizeB; j++){
                if (i == 0 && j == 0){
                    graph[i][j] = hypot(line1[i].x - line2[j].x, line1[i].y - line2[j].y);
                }
                else if (i == 0 && j != 0){
                    graph[i][j] = hypot(line1[i].x - line2[j].x, line1[i].y - line2[j].y) + graph[i][j-1];
                }
                else if (i != 0 && j == 0){
                    graph[i][j] = hypot(line1[i].x - line2[j].x, line1[i].y - line2[j].y) + graph[i-1][j];
                }
                else {
                    graph[i][j] = hypot(line1[i].x - line2[j].x, line1[i].y - line2[j].y) + std::min({graph[i-1][j-1], graph[i-1][j], graph[i][j-1]});
                }
            }
        }

        return graph;
    }

    std::vector<double> DTW_searching(std::vector<std::vector<double>> graph){
        std::vector<double> weight;
        int i=graph.size()-1; int j=graph[0].size()-1;
        weight.push_back(graph[i][j]);
        cv::Mat img = cv::Mat(i, j, 22, cv::Scalar::all(255));
        while(i > 0 && j > 0){
            if(graph[i-1][j-1] <= graph[i-1][j] && graph[i-1][j-1] <= graph[i][j-1]){
                cv::circle(img, cv::Point(j-1,i-1), 2, cv::Scalar(0,0,255), 2, 8);
                weight.push_back(graph[i-1][j-1]);
                i -=1 ; j-= 1;
            }
            else{
                if(graph[i-1][j] <= graph[i][j-1]){
                    cv::circle(img, cv::Point(j,i-1), 2, cv::Scalar(0,0,255), 2, 8);
                    weight.push_back(graph[i-1][j]);
                    i -= 1;
                }
                else{
                    cv::circle(img, cv::Point(j-1,i), 2, cv::Scalar(0,0,255), 2, 8);
                    weight.push_back(graph[i][j-1]);
                    j -=1;
                }
            }
        }
        if (i != 0 || j != 0){
            weight.push_back(graph[0][0]);
            cv::circle(img, cv::Point(0,0), 2, cv::Scalar(0,0,255), 2, 8);
        }
        // helper::show(img);

        return weight;
    }
    
    double scoring(std::vector<double> line1, std::vector<double> line2){
        std::vector<std::vector<double>> graph = DTW_graphing(line1, line2, true);
        std::vector<double> weight = DTW_searching(graph);
        double score = std::accumulate(weight.begin(), weight.end(),0) / weight.size();

        if( score <= 1 ){score = 100000000000;}

        graph = DTW_graphing(line1, line2, false);
        weight = DTW_searching(graph);
        double scoreR = std::accumulate(weight.begin(), weight.end(),0) / weight.size();

        if( scoreR <= 1 ){scoreR = 100000000000;}

        return score; //std::min({score, scoreR});
    }

    double scoring(std::vector<cv::Point2f> line1, std::vector<cv::Point2f> line2){
        std::vector<std::vector<double>> graph = DTW_graphing(line1, line2, true);
        std::vector<double> weight = DTW_searching(graph);
        double score = std::accumulate(weight.begin(), weight.end(),0) / weight.size();

        if( score <= 1 ){score = 100000000000;}

        graph = DTW_graphing(line1, line2, false);
        weight = DTW_searching(graph);
        double scoreR = std::accumulate(weight.begin(), weight.end(),0) / weight.size();

        if( scoreR <= 1 ){scoreR = 100000000000;}

        return std::min({score, scoreR});
    }
    
    cv::Mat get_puzzle_original(std::vector<puzzlePiece> ps){
        int maxCol =0; int maxRow = 0;
        std::vector<cv::Mat> resized;
        for(int i=0; i<ps.size(); i++){
            cv::Mat resizedImg;
            cv::resize(ps[i].image, resizedImg, cv::Size(ps[i].image.cols/4, ps[i].image.rows/4));
            if(resizedImg.cols > maxCol){maxCol = resizedImg.cols;}
            if(resizedImg.rows > maxRow){maxRow = resizedImg.rows;}
            resized.push_back(resizedImg);
        }
        int nn = sqrt(ps.size());
        double canvas_rows = maxRow*nn;
        double canvas_cols = maxCol * nn;
        cv::Mat canvas = cv::Mat(canvas_rows, canvas_cols, resized[0].type(), cv::Scalar::all(255));
        

        for(int i=0; i< resized.size(); i++){
            cv::Mat roi = canvas(cv::Rect((i%nn)*maxCol, (i/nn)*maxRow, resized[i].cols, resized[i].rows));
            cv::addWeighted(roi, 0.0, resized[i], 1.0, 0.0, roi);
        }
        return canvas;
    }

    void find_min_score(std::vector<puzzlePiece> & ps){

        double threshold = 2500.0;
        for(int i=0; i<ps.size(); i++){
            std::vector<std::tuple<double, int, int>> setScore(4);
            // std::cout << " \r SCORING  ... [ " << 100*i/ps.size() << "% completed ]" <<std::flush;
            std::cout << "SCORING ... " << i << std::endl;
            for(int a=0; a<ps[i].transformed_edges.size(); a++){
                if (a != std::get<1>(ps[i].piece_type) && a != std::get<2>(ps[i].piece_type)){
                    double minScore = 10000000000000000;
                    int jIdx = -1; int bIdx = -1;
                    for(int j=0; j<ps.size(); j++){
                        if(i != j){
                          for(int b=0; b<ps[j].transformed_edges.size(); b++){
                            double score = scoring(ps[i].transformed_edges[a], ps[j].transformed_edges[b]);
                                if (score<minScore){
                                    minScore = score;
                                    jIdx = j;    bIdx = b;
                                }
                            }  
                        }
                    }

                    if (minScore < threshold){
                        setScore[a] = std::make_tuple(minScore, jIdx, bIdx);
                    }
                    else{
                        setScore[a] = std::make_tuple(-1,-1, -1);
                    }
                    std::cout << i << " piece  at edge " << a << " = " << jIdx << " piece at edge " << bIdx << " score : " << minScore << std::endl;
                }
                else{
                    setScore[a] = std::make_tuple(0, 0, 0);
                }
            }
            ps[i].scores = setScore;
        }
        std::cout << " \r SCORING  ... [ " << 100 << "% completed ]" <<std::endl;
    }

    void set_score(std::vector<puzzlePiece> & ps){
        std::vector<std::vector<double>> s = {
            {0}, {-1}, {435}, {0},
            {556}, {375}, {408}, {0},
            {375}, {560}, {360}, {0},
            {295}, {850}, {0}, {0},
            {0}, {1084}, {550}, {-1},
            {608}, {842}, {440}, {469},
            {466}, {1061}, {-1}, {625},
            {-1}, {657}, {0}, {908},
            {0}, {294}, {308}, {1317},
            {373}, {264}, {1069}, {927},
            {1278}, {1340}, {811}, {863},
            {601}, {344}, {0}, {713},
            {0}, {0}, {614}, {240},
            {529}, {0}, {1677}, {252},
            {1707}, {0}, {2378}, {1545},
            {2414}, {0}, {0}, {379}
            };
        std::vector<std::vector<int>> p = {
            {0}, {-1}, {1}, {0},
            {0}, {5}, {2}, {0},
            {1}, {6}, {3}, {0},
            {2}, {7}, {0}, {0},
            {0}, {8}, {5}, {-1},
            {4}, {9}, {6}, {1},
            {5}, {10}, {-1}, {2},
            {-1}, {11}, {0}, {3},
            {0}, {12}, {9}, {4},
            {8}, {13}, {10}, {5},
            {9}, {14}, {11}, {6},
            {10}, {15}, {0}, {7},
            {0}, {0}, {13}, {8},
            {12}, {0}, {14}, {9},
            {13}, {0}, {15}, {10},
            {14}, {0}, {0}, {11}
            };
        std::vector<std::vector<int>> e = {
            {0}, {-1}, {0}, {0},
            {2}, {3}, {0}, {0},
            {2}, {3}, {0}, {0},
            {2}, {3}, {0}, {0},
            {0}, {3}, {0}, {-1},
            {2}, {3}, {0}, {1},
            {2}, {3}, {-1}, {1},
            {-1}, {3}, {0}, {1},
            {0}, {3}, {0}, {1},
            {2}, {3}, {0}, {1},
            {2}, {3}, {0}, {1},
            {2}, {3}, {0}, {1},
            {0}, {0}, {0}, {1},
            {2}, {0}, {0}, {1},
            {2}, {0}, {0}, {1},
            {2}, {0}, {0}, {1}
            };

        for(int i=0; i<ps.size(); i++){
            std::vector<std::tuple<double, int, int>> setScore;
            for(int j=0; j<4; j++){
                setScore.push_back(std::make_tuple(s[(i*4)+j][0], p[(i*4)+j][0], e[(i*4)+j][0]));
            }
            ps[i].scores = setScore;
        }
        // for(int i=0; i<ps.size(); i++){
        //     std::cout << "======= "<< i << " =======" << std::endl;
        //     for(int j=0; j<4; j++){
        //         std::cout << std::get<0>(ps[i].scores[j]) <<"  " <<  std::get<1>(ps[i].scores[j]) << "  " << std::get<2>(ps[i].scores[j]) <<std::endl;
        //     }
        // }
    }

    int find_reference(std::vector<puzzlePiece> & ps){
        double minScore = 100000; int ref_idx = -1;
        for(int i=0; i<ps.size(); i++){ 
            // std::cout << "======= " << i << " =======" << std::endl;
            double total_score = 0; int cnt = 0;
            for(int j=0; j<ps[i].scores.size(); j++){
                // std::cout << std::get<0>(ps[i].scores[j]) <<"  " <<  std::get<1>(ps[i].scores[j]) << "  " << std::get<2>(ps[i].scores[j]) <<std::endl;
                if(std::get<0>(ps[i].scores[j]) == -1){
                    total_score += 1000000;
                }
                else if (std::get<0>(ps[i].scores[j]) == 0){
                    continue;
                }
                else{
                    cnt += 1;
                    total_score += std::get<0>(ps[i].scores[j]);
                }
            }
            if (minScore > (total_score/cnt)){
                minScore = (total_score/cnt);
                ref_idx = i;
            }
        }
        return ref_idx;
    }

    std::deque<std::deque<int>> find_map(std::vector<puzzlePiece> &ps){
        std::deque<int> searchIdx = {find_reference(ps)}; 
        int left, right, top, bottom;
        int left_closed = 0; int right_closed = 0;
        int top_closed = 0; int bottom_closed = 0; bool all_closed = false;

        std::deque<int> row; row.push_front(searchIdx[0]);
        std::deque<std::deque<int>> map; map.push_front(row);
        int col_r = 0; int row_r = 0;                               // current reference index in map
        std::vector<int> visited = {searchIdx[0]};
        do{
            int ref = searchIdx.front();
            for(int r = 0; r<map.size(); r++){
                std::deque<int> row = map[r];
                std::deque<int>::iterator it = std::find(row.begin(), row.end(), ref);
                if (it != row.end()){
                    row_r = r; col_r = it-row.begin();
                    break;
                }
            }
            // std::cout << " referece : " << ref << "   row : " << row_r << "   col : " << col_r << std::endl;
            for(int i=0; i<4; i++){
                double score = std::get<0>(ps[ref].scores[i]);
                if (score == -1){               // unmatched 
                    update_deque_map(map, -1, (direction)i, row_r, col_r);
                }
                else if (score == 0){           // rim or corner edges
                    if ((direction)i==direction::LEFT)          { left_closed += 1; }
                    else if((direction)i==direction::BOTTOM)    { bottom_closed += 1; }
                    else if((direction)i==direction::RIGHT)     { right_closed += 1; }
                    else if ((direction)i==direction::TOP)      { top_closed += 1; }
                    else{ throw std::out_of_range(" Error : edge"); }
                }
                else{                                           // matched
                    int num = std::get<1>(ps[ref].scores[i]);
                    
                    update_deque_map(map, num, (direction)i, row_r, col_r);
                    if(std::find(visited.begin(), visited.end(), num) == visited.end()){
                        // std::cout << "pushed : " <<num <<std::endl;
                        searchIdx.push_back(num);
                        visited.push_back(num);
                    }
                }
            }
            
            // std::cout << " closed : "  << left_closed << "  " << bottom_closed << "  " << right_closed << "  " << top_closed << std::endl;

            if (left_closed == 4 && bottom_closed == 4 && right_closed == 4 && top_closed == 4){
                all_closed = true;
            }
            searchIdx.pop_front();

        }while(!all_closed || !searchIdx.empty());

        std::cout << "====  Ending Map  ==== " << std::endl;
        for(int r=0; r<map.size(); r++){
            for (int c=0; c<map[r].size(); c++){
                std::cout << map[r][c] << "  " ;
                if (map[r][c] < 0){
                    throw std::invalid_argument( " ERROR : Map had invalid index ");
                }
            }std::cout << std::endl;
        }
        return map;
    }

    void update_deque_map(std::deque<std::deque<int>> & map, int num, direction dir, int & curRow, int & curCol){

        int row_tot = map.size();
        int col_tot = map[0].size();
        switch(dir){
            case LEFT:
                if (curCol == 0){               // new generate left
                    for(int r=0; r<row_tot; r++){
                        if (r != curRow){
                            map[r].push_front(-1);
                        }
                        else{
                            map[r].push_front(num);
                        }
                    }
                    curCol+=1;
                }
                else{
                    if (num != -1){
                        if(map[curRow][curCol-1] == -1 || map[curRow][curCol-1] == num){map[curRow][curCol-1] = num;}
                        // else{ throw std::invalid_argument( " certain location is not empty (left)");}
                    }
                }
                break;

            case BOTTOM:
                if (row_tot-1 == curRow){      //  new generate bottom
                    std::deque<int> row(col_tot, -1); 
                    row[curCol] = num;
                    map.push_back(row);
                }
                else {
                    if (num != -1){
                        if(map[curRow+1][curCol] == -1 || map[curRow+1][curCol] == num){map[curRow+1][curCol] = num;}
                        // else{ throw std::invalid_argument( " certain location is not empty (bottom)");}
                    }
                }
                break;

            case RIGHT:
                if (col_tot-1 == curCol){       // new generate right
                    for(int r=0; r<row_tot; r++){
                        if(r != curRow){
                            map[r].push_back(-1);
                        }
                        else{
                            map[r].push_back(num);
                        }
                    }
                }
                else{
                    if (num != -1){
                        if (map[curRow][curCol+1] == -1 || map[curRow][curCol+1] == num){map[curRow][curCol+1] = num;}
                        // else{ throw std::invalid_argument( " certain location is not empty (right)");}
                    }
                }
                break;

            case TOP:
                if (curRow == 0){               // new generate top
                    std::deque<int> row(col_tot, -1);
                    row[curCol] = num;
                    map.push_front(row);
                    curRow+=1;
                }
                else{
                    if(num != -1){
                        if(map[curRow-1][curCol] == -1 || map[curRow-1][curCol] == num){map[curRow-1][curCol] = num;}
                        // else{ throw std::invalid_argument( " certain location is not empty (top)");}
                    }
                    
                }
                break;

            default :
                std::cout << " No  direction provided " << std::endl;
                break;
        }
    }

    cv::Mat draw_deque_map(std::deque<std::deque<int>> map, std::vector<puzzlePiece> ps){
        // Find max size of piece 
        // std::cout << " GETTING CANVAS ... " << std::endl;
        double maxCol = 0; double maxRow = 0;
        for( auto piece : ps){
            cv::resize(piece.image, piece.image, cv::Size(piece.image.cols/4, piece.image.rows/4));
            if (piece.image.cols > maxCol){
                maxCol = piece.image.cols;
            }

            if (piece.image.rows > maxRow){
                maxRow = piece.image.rows;
            }
        }

        int s = sqrt(ps.size());
        double canvas_rows = s * maxRow;
        double canvas_cols = s * maxCol;

        cv::Mat canvas = cv::Mat(canvas_rows, canvas_cols, ps[0].image.type(), cv::Scalar::all(255));

        std::cout <<" DRAWING CANVAS ... " << std::endl;

        int sY= (s - 1) * maxRow;
        int sX = (s - 1) * maxCol * (s%2);
        int C, R; 
        for(int row=0; row<map.size(); row++){
            for(int col=0; col<map[row].size(); col++){
                cv::Mat pieceImg = ps[map[row][col]].image;
                cv::resize(pieceImg, pieceImg, cv::Size(pieceImg.cols/4, pieceImg.rows/4));
                C = pieceImg.cols;
                R = pieceImg.rows;
                cv::Mat roi = canvas(cv::Rect(col*maxCol,row*maxRow,C, R));
                cv::addWeighted(roi, 0.0, pieceImg, 1.0, 0.0, roi);
            }
        }

        return canvas;
    }

    std::pair<int,int> find_couple(std::vector<puzzlePiece> & ps, int ref, int edgeIdx){
        double threshold = 2500.0;
        int a = edgeIdx;
        if (a != std::get<1>(ps[ref].piece_type) && a != std::get<2>(ps[ref].piece_type)){
            double minScore = 10000000000000000;
            int jIdx = -1; int bIdx = -1;
            for(int j=0; j<ps.size(); j++){
                if(ref != j){
                    for(int b=0; b<ps[j].transformed_edges.size(); b++){
                        double score = scoring(ps[ref].transformed_edges[a], ps[j].transformed_edges[b]);
                        if (score<minScore){
                            minScore = score;
                            jIdx = j;    bIdx = b;
                        }
                    }  
                }
            }

            if (minScore < threshold){
                return std::make_pair(jIdx, bIdx);
            }
            else{
                return std::make_pair(-1,-1);
            }
        }
        else{
            return std::make_pair(-2,-2);
        }
        
    }

    std::deque<std::deque<int>> scoring_and_mapping(std::vector<puzzlePiece> & ps){
        std::deque<int> searchIdx = {0}; 
        int left, right, top, bottom;
        int left_closed = 0; int right_closed = 0;
        int top_closed = 0; int bottom_closed = 0; bool all_closed = false;

        std::deque<int> row; row.push_front(searchIdx[0]);
        std::deque<std::deque<int>> map; map.push_front(row);
        int col_r = 0; int row_r = 0;                               // current reference index in map
        std::vector<int> visited = {searchIdx[0]};
        int cnt = 0;
        do{
            int ref = searchIdx.front();
            // update current reference's index on map
            for(int r = 0; r<map.size(); r++){
                std::deque<int> row = map[r];
                std::deque<int>::iterator it = std::find(row.begin(), row.end(), ref);
                if (it != row.end()){
                    row_r = r; col_r = it-row.begin();
                    break;
                }
            }
            std::cout << " current referece : " << ref << std::endl;
            for(int i=0; i<4; i++){ // search 4edges of reference piece
            
                std::pair<int,int> matched = find_couple(ps, ref, i);
                int matchedIdx = matched.first;
                int edgeIdx = matched.second;
                if (matchedIdx == -1 && matchedIdx == -1){               // unmatched 
                    update_deque_map(map, -1, (direction)i, row_r, col_r);
                }
                else if (matchedIdx == -2 && edgeIdx == -2){           // rim or corner edges
                // std::cout << " something closed " << std::endl;
                    if ((direction)i==direction::LEFT)          { left_closed += 1; }
                    else if((direction)i==direction::BOTTOM)    { bottom_closed += 1; }
                    else if((direction)i==direction::RIGHT)     { right_closed += 1; }
                    else if ((direction)i==direction::TOP)      { top_closed += 1; }
                    else{ throw std::out_of_range(" Error : edge"); }
                }
                else if ( matchedIdx >= 0 && matchedIdx < ps.size() && edgeIdx >= 0 && edgeIdx < 4){    
                    int rotate = (((i+2)%4) - edgeIdx + 4)%4;

                    int newRow = row_r+((i%2)*pow(-1, (i+1)%3)); 
                    int newCol = col_r+((i+1)%2 * pow(-1, (i+1)%3));
                    // std::cout << "newRow : " << newRow << "  newCol : " << newCol << "  direction : " << i << std::endl;
                    // update_deque_map(map, matchedIdx, (direction)i, row_r, col_r);
                    // if(std::find(visited.begin(), visited.end(), matchedIdx) == visited.end()){
                    //     searchIdx.push_back(matchedIdx);
                    //     visited.push_back(matchedIdx);
                    // }

                    std::cout << "rot : " << rotate << "  " << ref << " at " << i << " matched with " << matchedIdx << " at " << edgeIdx << std::endl;
                    if(newRow < 0 || newCol < 0 || newRow >= map.size() || newCol>= map[0].size()){
                        std::cout << "map extended  " << i <<std::endl;
                        ps[matchedIdx].rotatePiece(rotate);
                        update_deque_map(map, matchedIdx, (direction)i, row_r, col_r);
                        if(std::find(visited.begin(), visited.end(), matchedIdx) == visited.end()){
                            searchIdx.push_back(matchedIdx);
                            visited.push_back(matchedIdx);
                        }
                        std::cout << " ==== map ==== " <<std::endl;
                        for(int r=0; r<map.size(); r++){
                                for (int c=0; c<map[r].size(); c++){
                                    std::cout << map[r][c] << "  " ;
                            }std::cout << std::endl;
                        } 
                            
                    }
                    else {
                        if (map[newRow][newCol] == -1){    // check if map has value on 
                            ps[matchedIdx].rotatePiece(rotate);
                            update_deque_map(map, matchedIdx, (direction)i, row_r, col_r);
                            if(std::find(visited.begin(), visited.end(), matchedIdx) == visited.end()){
                                searchIdx.push_back(matchedIdx);
                                visited.push_back(matchedIdx);
                            }
                        }
                        std::cout << " ==== map ==== " <<std::endl;
                        for(int r=0; r<map.size(); r++){
                                for (int c=0; c<map[r].size(); c++){
                                    std::cout << map[r][c] << "  " ;
                            }std::cout << std::endl;
                        } 
                    }
                }
                
                else{
                    std::cout << matchedIdx << " , " << edgeIdx << std::endl;
                    throw std::out_of_range( " matched couple return invalid value");
                }
            }
            
            if (left_closed == 4 && bottom_closed == 4 && right_closed == 4 && top_closed == 4){
                all_closed = true;
            }
            searchIdx.pop_front();
            
        }while(!all_closed && !searchIdx.empty());

        std::cout << " closed : "  << left_closed << "  " << bottom_closed << "  " << right_closed << "  " << top_closed << std::endl;

        std::cout << "====  Ending Map  ==== " << std::endl;
        for(int r=0; r<map.size(); r++){
            for (int c=0; c<map[r].size(); c++){
                std::cout << map[r][c] << "  " ;
                // if (map[r][c] < 0){
                //     std::invalid_argument( " ERROR : Map had invalid index ");
                // }
            }std::cout << std::endl;
        }
        return map;
    }

    std::vector<couple> find_couples(std::vector<puzzlePiece> ps){
        std::vector<couple> couples;
        double threshold = 2500.0;
        for(int i=0; i<ps.size(); i++){
            std::cout << " \r SCORING  ... [ " << 100*i/ps.size() << "% completed ]" <<std::flush;
            for(int a=0; a<ps[i].transformed_edges.size(); a++){

                if (a != std::get<1>(ps[i].piece_type) && a != std::get<2>(ps[i].piece_type)){
                    
                    double minScore = 10000000000000000;
                    int jIdx = -1; int bIdx = -1;
                    for(int j=0; j<ps.size(); j++){
                        if(i != j){
                          for(int b=0; b<ps[j].transformed_edges.size(); b++){
                            if (b != std::get<1>(ps[j].piece_type) && b != std::get<2>(ps[j].piece_type)){
                                double score = scoring(ps[i].transformed_edges[a], ps[j].transformed_edges[b]);
                                    if (score<minScore){
                                        minScore = score;
                                        jIdx = j;    bIdx = b;
                                    }
                                }  
                            }
                        }
                    }
                    if (minScore < threshold){
                        couple cp; 
                        cp.reference = std::make_pair(i, (direction)a);
                        cp.matched = std::make_pair(jIdx, (direction)bIdx);
                        cp.score = minScore;
                        couples.push_back(cp);
                    }
                    
                }
                
            }
        }
        std::cout << " \r SCORING  ... [ " << 100 << "% completed ]" <<std::endl;

        for(int i=0; i<couples.size()-1; i++){
            for(int j=i+1; j<couples.size(); j++){
                if (couples[i].score > couples[j].score){
                    couple temp = couples[i];
                    couples[i] = couples[j];
                    couples[j] = temp;
                }

                if(couples[i].reference == couples[j].matched && couples[i].matched == couples[j].reference){
                    couples.erase(couples.begin()+j); j-=1;
                }
            }
        }

        for(auto cp: couples){
            std::cout << cp.reference.first << "-"<< cp.reference.second << "  " << cp.matched.first << "-"<< cp.matched.second << " score : " << cp.score << std::endl;
        }
        return couples;
    }

    std::deque<std::deque<int>> newMapping(std::vector<puzzlePiece> ps){
        std::vector<couple> couples = find_couples(ps);
        std::deque<int> searchIdx = {couples[0].reference.first}; 
        int left, right, top, bottom;
        int left_closed = 0; int right_closed = 0;
        int top_closed = 0; int bottom_closed = 0; bool all_closed = false;

        std::deque<int> row; row.push_front(searchIdx[0]);
        std::deque<std::deque<int>> map; map.push_front(row);
        int col_r = 0; int row_r = 0;                               // current reference index in map
        std::vector<int> visited = {searchIdx[0]};

        do{
            int ref = searchIdx.front();
            for(int r = 0; r<map.size(); r++){
                std::deque<int> row = map[r];
                std::deque<int>::iterator it = std::find(row.begin(), row.end(), ref);
                if (it != row.end()){
                    row_r = r; col_r = it-row.begin();
                    break;
                }
            }
            for(int cp=0; cp<couples.size(); cp++){
                if (couples[cp].reference.first == ref){
                    int match = couples[cp].matched.first;
                    update_deque_map(map, match, couples[cp].reference.second, row_r, col_r);
                    
                    if(std::find(visited.begin(), visited.end(), match) == visited.end()){
                        searchIdx.push_back(match);
                        visited.push_back(match);  
                    }
                    couples.erase(couples.begin()+cp);
                    cp-=1;
                }
                else if(couples[cp].matched.first == ref){
                    int match = couples[cp].reference.first;
                    update_deque_map(map, match, couples[cp].matched.second, row_r, col_r);
                    
                    if(std::find(visited.begin(), visited.end(), match) == visited.end()){
                        searchIdx.push_back(match);
                        visited.push_back(match);  
                    }
                    couples.erase(couples.begin()+cp);
                    cp-=1;
                }
            }
            
            searchIdx.pop_front();

        }while(!couples.empty() && !searchIdx.empty());

        std::cout << "====  Ending Map  ==== " << std::endl;
        for(int r=0; r<map.size(); r++){
            for (int c=0; c<map[r].size(); c++){
                std::cout << map[r][c] << "  " ;
                if (map[r][c] < 0){
                    throw std::invalid_argument( " ERROR : Map had invalid index ");
                }
            }std::cout << std::endl;
        }
        return map;
    }   
}

