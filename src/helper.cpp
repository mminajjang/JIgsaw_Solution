#include <helper.h>

namespace helper
{ 
    void show(cv::Mat image, std::string name)
    {
        try
        {
            while (cv::waitKey() != 27)
            {
                cv::imshow(name, image);
            }
            cv::destroyWindow(name);
        }
        catch (cv::Exception &e)
        {
            std::cout << "ERROR :" << e.what() << std::endl;
            std::cout << "in " << e.file << "   at " << e.line << std::endl;
        }
    }

    void show(cv::Mat image)
    {
        show(image, "image");
    }

    void show(std::vector<cv::Mat> images)
    {
        char name[20];
        for (int i = 0; i < images.size(); i++)
        {
            sprintf(name, "image %d", i);
            show(images[i], name);
        }
    }

    void shows(std::vector<cv::Mat> images)
    {
        char name[20];

        while(cv::waitKey()!=27){
            for (int i = 0; i < images.size(); i++)
            {
                sprintf(name, "image %d", i);
                cv::imshow(name, images[i]);
            }
        }cv::destroyAllWindows();
    }

    void shows(std::vector<cv::Mat> images, std::vector<std::string> names)
    {
        while(cv::waitKey()!=27){
            for (int i = 0; i < images.size(); i++)
            {
                // sprintf(name, "image %d", i);
                cv::imshow(names[i], images[i]);
            }
        }cv::destroyAllWindows();
    }
    
    cv::Mat rotate(cv::Mat image, double angle){
        
        cv::Point centre(round((image.cols-1)/2.0), round((image.rows-1)/2.0));
        cv::Mat rot = cv::getRotationMatrix2D(centre, angle, 1.0);
        cv::Rect2f box = cv::RotatedRect(cv::Point2f(), image.size(), angle).boundingRect2f();
        rot.at<double>(0,2) += box.width/2.0 - image.cols/2.0;
        rot.at<double>(1,2) += box.height/2.0 - image.rows/2.0;
        cv::Mat result;
        cv::warpAffine(image, result, rot, box.size(), cv::WARP_FILL_OUTLIERS, cv::BORDER_CONSTANT, cv::Scalar::all(255));
        return result;
    }

    cv::Mat rotate(cv::Mat image, double angle, cv::Point2f centre){
        cv::Mat img = image.clone();
        cv::Mat rotationMatrix = cv::getRotationMatrix2D(centre, angle, 1.0);
        cv::Rect2f rect = cv::RotatedRect(cv::Point2f(), img.size(), angle).boundingRect2f();
        rotationMatrix.at<float>(0,2) += rect.width/2.0 - img.cols/2.0;
        rotationMatrix.at<float>(1,2) += rect.height/2.0 - img.rows/2.0;
        cv::Mat result;
        cv::warpAffine(img, result, rotationMatrix, rect.size());
        return result;
    }

    CONTOUR2f rotate_contour(CONTOUR2f cnt, double angle, cv::Point2f centre){
        CONTOUR2f transformed_contour;
        for(int i=0; i<cnt.size(); i++){
            cv::Point2f newPt;
            newPt.x = (cos(angle)*cnt[i].x) - (sin(angle)*cnt[i].y);
            newPt.y = (sin(angle)*cnt[i].x) + (cos(angle)*cnt[i].y);
            transformed_contour.push_back(newPt);
        }
        return transformed_contour;
    }

    CONTOUR2f rotate_points(CONTOUR2f pts, double angle){
        CONTOUR2f rotated_points;
        double deg = angle*CV_PI/180;
        for(int i=0; i<pts.size(); i++){
            rotated_points.push_back(cv::Point2f((cos(deg)*pts[i].x)-(sin(deg)*pts[i].y), (sin(deg)*pts[i].x) +(cos(deg)*pts[i].y)));
        }
        return rotated_points;
    }

    cv::Mat resize(cv::Mat image, double width, double height){
        if(image.cols < width && image.rows < height){
            cv::Mat result = cv::Mat(height, width, image.type(), cv::Scalar::all(255));
            cv::Mat ROI = result(cv::Rect(0, 0, image.cols, image.rows));
            cv::addWeighted(ROI, 0.0, image, 1.0, 0.0, ROI);
            return result;
        }
        else if (image.cols == width && image.rows == height){
            return image;
        }
        else if (image.cols > width || image.rows > height){
            // while(image.cols > width || image.rows > height){
                // std::cout << "1/1" <<std::endl;
            cv::Mat min;
            resize(image, min, cv::Size((int)image.cols/3, (int)image.rows/3));
            std::cout << min.cols << " " << min.rows << std::endl;
            // // }
            cv::Mat result = cv::Mat(height, width, image.type(), cv::Scalar::all(255));
            cv::Mat ROI = result(cv::Rect(0, 0, min.cols, min.rows));
            cv::addWeighted(ROI, 0.0, min, 1.0, 0.0, ROI);
            return result;
        }
        return image;
    }
}

namespace imageProcesser{

    cv::Mat cvtGray(cv::Mat image){
        cv::Mat img = image.clone();
        try{
            cv::Mat gray;
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            return gray;
        }
        catch(cv::Exception & e){
            return img;
        }
    }
    
    CONTOUR2f find_contour(cv::Mat image){
        cv::Mat img = image.clone();
        cv::Mat gray = cvtGray(img);
        cv::blur(gray, gray, cv::Size(5,5)); 
        cv::Mat bin;
        cv::threshold(gray, bin, 200,255,cv::THRESH_BINARY_INV);
       
        std::vector<CONTOUR> contours;
        std::vector<cv::Vec4i> hierarchy;

        cv::findContours(bin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
        CONTOUR2f contour2f(contours[0].size());
        for (int i=0; i<contour2f.size(); i++){
            contour2f[i] = cv::Point2f((double)contours[0][i].x, (double)contours[0][i].y);
        }
        return contour2f;
    }

    cv::Point2f find_centre(CONTOUR2f contour2f){
        cv::Moments mom = cv::moments(contour2f);

        return cv::Point2f(mom.m10/mom.m00, mom.m01/mom.m00);
    }

    POINTS2f find_corner(cv::Mat image){
        cv::Mat img = image.clone();
        POINTS2f features = find_goodFeatures(img);
        CONTOUR2f contour = find_contour(img);
        POINTS2f candidates = find_rectangle(features, cv::contourArea(contour), img);
        
        std::vector<cv::Point2f> corners;
        for(auto candidate: candidates){
            double minDist = 100000; int idx = -1;
            for (int h=0; h<contour.size();h++){
                double dist = hypot((double)candidate.x-(double)contour[h].x, (double)candidate.y-(double)contour[h].y);
                if(dist < minDist){
                    minDist = dist;
                    idx = h;
                }
            }
            corners.push_back(cv::Point2f((double)contour[idx].x, (double)contour[idx].y));
        }

        double minDist = 10000000; int idx = -1;
        for(int i=0; i< corners.size(); i++){
            double dist = hypot(corners[i].x, corners[i].y);
            if(dist < minDist){
                minDist = dist; 
                idx = i;
            }
        }
        if (idx == 0){
            return corners;
        }
        else{
            POINTS2f newCorners;
            for(int j=0; j<corners.size(); j++){
               newCorners.push_back(corners[(idx+j)%4]);
            }
    
            return newCorners;
        }        
    }

    std::vector<CONTOUR2f> find_edges(CONTOUR2f cnt, POINTS2f corner){
       std::vector<CONTOUR2f> edges; int n = cnt.size();
        
        for(int j=0; j<corner.size(); j++){
            CONTOUR2f edge; bool flag = false;
            for(int i=0; i<cnt.size(); i++){
                if (corner[j%4] == cnt[i] && !flag){
                    flag = true;
                }

                if (corner[(j+1)%4] == cnt[i] && flag){
                    edges.push_back(edge);
                    edge.clear();
                    flag = false;
                    break;
                }

                if (flag){
                    edge.push_back(cnt[i]);
                    if (i == cnt.size()-1){
                        i = -1;
                    }
                }
            }
        }
        
        if (edges.size() != 4){
            std::cout << "ERROR : " << " edges has " << edges.size() << " detedted " << std::endl;
        }

        return edges;
    }
    
    std::vector<CONTOUR2f> refine_edges(std::vector<CONTOUR2f> edges){
        std::vector<CONTOUR2f> refined_edges;
        for(auto line : edges){
            std::vector<CONTOUR2f> clusters; CONTOUR2f cluster = {line[0]};
                for(int i=0; i<line.size()-1; i++){
                    double dist = hypot(line[i].x - line[i+1].x, line[i].y - line[i+1].y);
                    if (dist <= 1.0){
                        cluster.push_back(line[i+1]);
                    }
                    else{
                        clusters.push_back(cluster);
                        cluster.clear();
                    }
                }
                if(clusters.size() >1){
                    std::vector<cv::Point2f> newLine;
                    for(int i=0; i<clusters.size(); i++){
                        int X =0; int Y=0; 
                        for(int j=0; j<clusters[i].size(); j++){
                            X += clusters[i][j].x;
                            Y += clusters[i][j].y;
                        }
                        if(X != 0 || Y != 0){
                        newLine.push_back(cv::Point2f((X/clusters[i].size()), (Y/clusters[i].size())));}
                    }
                    refined_edges.push_back(newLine);
                }
                else{
                    refined_edges.push_back(line);
                } 
        }
        return refined_edges;
    }
    
    std::vector<CONTOUR2f> transform_edges(std::vector<CONTOUR2f> edges){
        std::vector<CONTOUR2f> transformed_edges;
        for(int i=0; i<edges.size(); i++){
            cv::Vec4f centreLine;
            cv::fitLine(edges[i], centreLine, cv::DIST_L2, 0, 0.01, 0.01);
            CONTOUR2f transformed_edge;
            for(int j=0; j<edges[i].size(); j++){
                transformed_edge.push_back(cv::Point2f(edges[i][j].x-centreLine[2], edges[i][j].y-centreLine[3]));
            }
            transformed_edges.push_back(transformed_edge);
        }
        return transformed_edges;
    }
    
    POINTS2f find_goodFeatures(cv::Mat image){
        cv::Mat img = image.clone();
        cv::Mat imgGray = cvtGray(img);
        cv::Mat imgBin;
        cv::bilateralFilter(imgGray, imgBin, 3,200,200);
        cv::threshold(imgBin, imgBin, 200, 255, cv::THRESH_BINARY_INV);
        imgBin.convertTo(imgBin, CV_32FC1);
        POINTS2f corner;
        double minLength = 10;//std::sqrt(cv::contourArea(find_contour(img))) / 6;
        cv::goodFeaturesToTrack(imgBin, corner, 0,0.1,minLength,cv::noArray(), 3, true);
        return corner;
    }

    POINTS2f find_rectangle(POINTS2f candidates, double area, cv::Mat image){

        double minDiff = 0.05;
        tryAgain:
        double minArea = area * 3/4;
        int bestIdx = -1; double score = 1000000; double maxArea = 0;
        std::vector<POINTS2f> result; result.clear();
        for(int i=0; i<candidates.size()-3; i++){
            for(int j=i+1; j<candidates.size()-2; j++){
                for(int k=j+1; k<candidates.size()-1; k++){
                    for(int l=k+1; l<candidates.size(); l++){
                        
                            POINTS2f rect = {candidates[i], candidates[j], candidates[k], candidates[l]};
                            //  std::cout <<rect[0] << "  "<< rect[1] <<"  "<< rect[2] << "  "<< rect[3] << std::endl;
                            reorder_points(rect);
                            
                            double corner1 = find_angle(rect[0], rect[1], rect[2]);
                            double corner2 = find_angle(rect[1], rect[2], rect[3]);
                            double corner3 = find_angle(rect[2], rect[3], rect[0]);
                            double corner4 = find_angle(rect[3], rect[0], rect[1]);
                        
                            double diff1 = fabs(CV_PI/2 - corner1);
                            double diff2 = fabs(CV_PI/2 - corner2);
                            double diff3 = fabs(CV_PI/2 - corner3);
                            double diff4 = fabs(CV_PI/2 - corner4);
                            // double sum = diff1+diff2+diff3+diff4;


                            if (diff1 <= 0.1 && diff2 <= 0.1 && diff3 <= 0.1 && diff4 <= 0.1){
                                double area = cv::contourArea(rect);
                                if (area > minArea){
                                    result.push_back(rect);
                                    if(area > maxArea){
                                        maxArea = area;
                                        bestIdx = result.size()-1;
                                    }
                                }
                            }
                            
                            // if (sum < minDiff){
                            //     if(cv::contourArea(rect) > minArea){
                            //         result.push_back(rect);
                            //         if (sum < score){
                            //             bestIdx = result.size()-1;
                            //             score = sum;
                            //         }
                            //     }
                            // }
                        }
                }
            }
        }
        // std::cout << "max area " << maxArea << std::endl;
        if (result.size() < 1){
            minDiff += 0.01;
            area = minArea;
            goto tryAgain;
        }
        else if (result.size() > 1){
            return result[bestIdx];
        }
        return result[0];
    }

    double find_angle(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3){
        cv::Point2f vec1((double)p1.x-(double)p2.x, (double)p1.y-(double)p2.y);
        cv::Point2f vec2((double)p3.x-(double)p2.x, (double)p3.y-(double)p2.y);
        double atheta =( vec1.x*vec2.x + vec1.y*vec2.y )/ (hypot(vec1.x, vec1.y)*hypot(vec2.x, vec2.y));
        double angle = acos(atheta);
        return angle;
    }

    void reorder_points(POINTS2f & pts){
        int n = pts.size();
        int repeat = 1;
        for(int r=0;r<n;r++){
            repeat *= (n-r);
        }
        for(int i=0; i<repeat; i++){
            if(!ccw_comparator(pts[i%n], pts[(i+1)%n], pts[(i+2)%n])){
                swap(pts[(i+1)%n], pts[(i+2)%n]);
            }
        }
    }

    bool ccw_comparator(cv::Point2f left,cv::Point2f cent, cv::Point2f  right){
        double dx1 = left.x - cent.x;
        double dy1 = left.y - cent.y;
        double dx2 = right.x - cent.x;
        double dy2 = right.y - cent.y;

        int direction = (dx1*dy2) - (dy1*dx2);
        if(direction < 0){return 0;}
        else if (direction > 0){return 1;}
        else {return (hypot(dx1,dy1) <hypot(dx2, dy2));}
    }
}
