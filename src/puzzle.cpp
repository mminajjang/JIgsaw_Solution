#include <puzzle.h>

void puzzlePiece::setPiece(cv::Mat img){

// TODO : 퍼즐의 회전정도에 따라 이미지 & 컨투어 & other info 회전시키기, 엣지들을 서남동북 으로 정렬
    image = img.clone(); 
    contour = imageProcesser::find_contour(image);
    corners = imageProcesser::find_corner(image);
    centre = imageProcesser::find_centre(contour);
    edges = imageProcesser::find_edges(contour, corners);
    std::vector<cv::Scalar> colours = {cv::Scalar((double)255,0,0), cv::Scalar(0,(double)255,0), cv::Scalar(0,0,(double)255), cv::Scalar((double)255,0,(double)255)};
    int c = 0;
    for(auto edge:edges){
        for(auto e: edge){
            cv::circle(image, e, 2, cv::Scalar::all(0), 2, 8);
        }c++;
    }
    edgeImgs.clear();

    piece_type = getPieceType(edges);
    transformed_edges = transformPoints(edges, image,contour);
}

void puzzlePiece::rotatePiece(int num){
    image = helper::rotate(image, 90*num);
    int back = 3;
    for(int i=0; i<num; i++){
        edges.insert(edges.begin(), edges[back]);
        edges.erase(edges.begin()+edges.size()-1);

        transformed_edges.insert(transformed_edges.begin(), transformed_edges[back]);
        transformed_edges.erase(transformed_edges.begin()+transformed_edges.size()-1);
    }
   
    if(std::get<1>(piece_type) != -1){
        std::get<1>(piece_type) = (std::get<1>(piece_type)+num)%4;
    }
    if(std::get<2>(piece_type) != -1){
        std::get<2>(piece_type) = (std::get<2>(piece_type)+num)%4;
    }
}

std::tuple<pieceType, int, int> puzzlePiece::getPieceType(std::vector<CONTOUR2f> edges){
    int edgeCounter = 0; std::vector<int> edgeIndex(2, -1);
    for(int i=0; i<edges.size(); i++){
        double arc = cv::arcLength(edges[i], false);
        double dist = hypot(edges[i].front().x - edges[i].back().x, edges[i].front().y - edges[i].back().y);
        // std::cout <<"diff : " <<  arc - dist << std::endl;
        if (abs(arc-dist) < 50){
            edgeIndex[edgeCounter] = i;    
            edgeCounter += 1;
        }
    }
    return std::make_tuple((pieceType)edgeCounter, edgeIndex[0], edgeIndex[1]);
}

std::vector<std::vector<double>> puzzlePiece::reduceDimension(std::vector<CONTOUR2f> edges){

    std::vector<std::vector<double>> result; //(4, std::vector<double>());
    // std::vector<cv::DistanceTypes> types = {cv::DIST_L1, cv::DIST_L2, cv::DIST_L12, cv::DIST_FAIR, cv::DIST_WELSCH, cv::DIST_HUBER};
    // // 파랑 -> 초록 -> 빨강 -> 노랑 -> 청록 -> 마젠타
    // std::vector<cv::Scalar> clr = {cv::Scalar(255,0,0), cv::Scalar(0,255,0), cv::Scalar(0,0,255), cv::Scalar(0,255,255), cv::Scalar(255,255,0), cv::Scalar(255,0,255)};
    // int count = 0; double minD = 1000000; double maxD = 0;
    for(auto edge : edges){
        cv::Vec4f linePt;
        cv::fitLine(edge, linePt, cv::DIST_HUBER, .0, .01, .01);
        double a = linePt[1];
        double b = -linePt[0];
        double c = (linePt[0]*linePt[3]) - (linePt[1]*linePt[2]);
        imageProcesser::reorder_points(edge);
        std::vector<double> reducedEdge;
        for(int i=0; i<edge.size(); i++){
            double d = fabs((a*edge[i].x) + (b*edge[i].y) + c) / sqrt(pow(a,2) + pow(b,2));
            reducedEdge.push_back(d);
        }
        result.push_back(reducedEdge);
    }
    return result;
}

std::vector<std::vector<cv::Point2f>> puzzlePiece::transformPoints(std::vector<CONTOUR2f> edges, cv::Mat img, CONTOUR2f contour){
    std::vector<std::vector<cv::Point2f>> result;
    cv::Moments mom = cv::moments(contour);
    cv::Point2f centrePt = cv::Point2f(mom.m10/mom.m00, mom.m01/mom.m00);

    for(auto edge : edges){
        cv::Vec4f linePt; 
        cv::fitLine(edge, linePt, cv::DIST_HUBER, .0, .01, .01);
        double vx = linePt[0]; double vy = linePt[1];
        double x = linePt[2]; double y = linePt[3];
        // cv::Point2f centre = cv::Point2f(x, y);
        double theta = atan(vy/vx);
        double rot = -theta;
        
        //trasnsform
        std::vector<cv::Point2f> newEdge;
        for(int i=0; i<edge.size(); i++){
            cv::Point2f translatedPt = cv::Point2f(edge[i].x-x, edge[i].y-y); // translated 
            cv::Point2f rotatedPt = cv::Point2f((cos(rot)*translatedPt.x)+(sin(rot)*translatedPt.y), (-sin(rot)*translatedPt.x)+(cos(rot)*translatedPt.y));
            // newEdge.push_back(translatedPt);
            newEdge.push_back(rotatedPt);
        }

        // reorder the points 
        cv::Point2f transformedCentre = cv::Point2f(centrePt.x-x, centrePt.y-y);
        cv::Point2f newCentrePt = cv::Point2f((cos(rot)*transformedCentre.x)+(sin(rot)*transformedCentre.y), (-sin(rot)*transformedCentre.x)+(cos(rot)*transformedCentre.y));
        if (newCentrePt.x<x){
            //위에서 아래로 
            if(newEdge.front().y < newEdge.back().y){
                std::reverse(newEdge.begin(), newEdge.end());
            }
        }
        else if(newCentrePt.x>x){
            //아래에서 위로
            if(newEdge.front().y > newEdge.back().y){
                std::reverse(newEdge.begin(), newEdge.end());
            }
        }
        else{
            throw std::invalid_argument(" x is invalied" );
        }
        result.push_back(newEdge);
    }

    // helper::show(img);
    return result;

}

// double puzzlePiece::check_rotated(std::tuple<pieceType, int, int> features, std::vector<CONTOUR2f> edges, POINTS2f corner){
    
//     pieceType type = std::get<0>(features);
//     if (type == pieceType::MIDDLE){
//         cv::Point c1 = corner[0];
//         cv::Point c2 = corner[1];
//         cv::Point c3 = corner[2];
//         cv::Point c4 = corner[3];
        
//         try{
//             double angle1 = atan((c1.y-c2.y)/(c1.x-c2.x));
//             return angle1;
            
//         }
//         catch(std::exception & e ){
//             return CV_PI/2;
//         }
//     }
//     else{
//         CONTOUR2f edge1 = edges[std::get<1>(features)];
//         int mid_index = edge1.size()/2;
//         try{
//             double angle1 = atan((edge1.front().y-edge1[mid_index].y)/(edge1.front().x-edge1[mid_index].x));
//             double angle2 = atan((edge1[mid_index].y-edge1.back().y)/(edge1[mid_index].x-edge1.back().x));
//             return (angle1 + angle2) / 2;
//         }
//         catch(std::exception & e ){
//             std::cout << " replaced " << e.what() << std::endl;
//             return 0;
//         }
//     }
// }
