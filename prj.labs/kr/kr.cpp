#include <opencv2/opencv.hpp>

int main() {
    cv::Mat foto(300, 450, CV_32FC1);
    std::vector <std::vector<float>> color = { {255, 0, 127} , {0, 127, 255} };
    int width = 150; 
    int radius = 70;
    cv::Rect2d straight(0, 0, 150, 150);

    for (int i = 0; i < 2; i++) {
        straight.y = i * width;
        for (int j = 0; j < 3; j++) {
            straight.x = j * width;
            cv::Mat tmp = foto(straight);
            tmp = color[i][j] / 255;
            cv::circle(foto, cv::Point(straight.x + radius, straight.y + radius), 55, color[1 - i][j] / 255, -1);
        }
    }

    cv::imshow("foto", foto);
    cv::imwrite("foto.png", foto*255);

    cv::Mat F1(2, 2, CV_32FC1);
    F1 = 0;
    F1.at<float>(0, 1) = 1;
    F1.at<float>(1, 0) = -1;
    cv::Mat Foto1Filt;
    cv::filter2D(foto, Foto1Filt, -1, F1, cv::Point(0, 0));

    cv::Mat F2(2, 2, CV_32FC1);
    F2 = 0;
    F2.at<float>(1, 0) = 1;
    F2.at<float>(0, 1) = -1;
    cv::Mat Foto2Filt;
    cv::filter2D(foto, Foto2Filt, -1, F2, cv::Point(0, 0));

    cv::Mat mid(300, 450, CV_32FC1);
    for (int i = 0; i < mid.rows; i++) {
        for (int j = 0; j < mid.cols; j++) {
           std::sqrt(mid.at<float>(i, j) = Foto1Filt.at<float>(i, j) * Foto1Filt.at<float>(i, j) + Foto2Filt.at<float>(i, j) * Foto2Filt.at<float>(i, j));
        }
    }

    Foto1Filt = (Foto1Filt + 1) / 2;
    cv::imshow("Foto1Filt", Foto1Filt);
    cv::imwrite("Foto1Filt.png", Foto1Filt * 255);

    Foto2Filt = (Foto2Filt + 1) / 2;
    cv::imshow("Foto2Filt", Foto2Filt);
    cv::imwrite("Foto2Filt.png", Foto2Filt * 255);

    cv::imshow("mid", mid);
    cv::imwrite("mid.png", mid * 255);

    cv::waitKey(0);
}