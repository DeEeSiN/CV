#include <opencv2/opencv.hpp>



int add_video(std::string name_of_video)
{
	//open video from the path
	cv::VideoCapture video("../../../data/videos_lab4/" + name_of_video);

	//delete last 4 symbols of name of the video (format)
	name_of_video = name_of_video.substr(0, name_of_video.size() - 4);

	//video Error
	if (!video.isOpened()) {
		std::cout << "ERROR! THE VIDEO IS NOT OPENED.\n";
		return -1;
	}

	int nFrames = video.get(cv::CAP_PROP_FRAME_COUNT);

	//arrays for each stage of frame processing
	cv::Mat frames[3], framesGS[3], framesBin[3], framesMorph[3], framesCC[3];

	for (int i = 0; i < 3; i++) {

		//counting current frame
		int frame_number = nFrames / 5 * (i + 2);

		//getting the current frame
		video.set(cv::CAP_PROP_POS_FRAMES, frame_number);
		video >> frames[i];

		//saving orig picture
		cv::imwrite("frames/" + name_of_video + "_original_" + std::to_string(i + 1) + ".png", frames[i]);

		//reduction + saving
		cv::cvtColor(frames[i], framesGS[i], cv::COLOR_BGR2GRAY); //reduction
		cv::imwrite("frames/" + name_of_video + "_grayscale_" + std::to_string(i + 1) + ".png", framesGS[i]);

		//binarization + saving
		cv::threshold(framesGS[i], framesBin[i], 165, 255, cv::THRESH_BINARY); 

		//inverting a binarization in case of high percentage of white pixels 
		double white_pixels_percentage = 0;
		for (int j = 0; j < framesBin[i].rows; j++) {
			for (int k = 0; k < framesBin[i].cols; k++) {
				if (framesBin[i].at<uint8_t>(j, k) == 255) {
					white_pixels_percentage++;
				}
			}
		}
		white_pixels_percentage = white_pixels_percentage / (framesBin[i].rows * framesBin[i].cols) * 100;
		if (white_pixels_percentage > 50) {
			cv::bitwise_not(framesBin[i], framesBin[i]);
		}

		cv::imwrite("frames/" + name_of_video + "_BIN_" + std::to_string(i + 1) + ".png", framesBin[i]);


		//morphology + saving
		cv::Mat structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(6, 12));
		cv::morphologyEx(framesBin[i], framesMorph[i], cv::MORPH_CLOSE, structuring_element);
		cv::morphologyEx(framesMorph[i], framesMorph[i], cv::MORPH_OPEN, structuring_element);

		structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(120, 60));
		cv::morphologyEx(framesMorph[i], framesMorph[i], cv::MORPH_CLOSE, structuring_element);

		cv::imwrite("frames/" + name_of_video + "_MORPH_" + std::to_string(i + 1) + ".png", framesMorph[i]);



		//arrays for connecteedComponentsWithStats output
		cv::Mat labeledImage(framesMorph[i].size(), CV_32S), stats, centroids;
		//labeling connected components and gathering stats
		int nLabels = cv::connectedComponentsWithStats(framesMorph[i], labeledImage, stats, centroids, 8, CV_32S);


		//finding main connected components by area, providing it's the biggest except background
		int max_area = 0, max_label = 1;

		for (int j = 1; j < nLabels; j++) {
			if (max_area < stats.at<int>(j, cv::CC_STAT_AREA)) {
				max_area = stats.at<int>(j, cv::CC_STAT_AREA);
				max_label = j;
			}
		}

		//creating vector of colors
		std::vector<cv::Vec3b> colors(nLabels);
		for (int j = 0; j < nLabels; j++) {
			colors[j] = cv::Vec3b(0, 0, 0);
		}
		colors[max_label] = cv::Vec3b(255, 255, 255);

		//test
		framesCC[i] = frames[i].clone();
		framesCC[i] = 0;
		//cv::Mat dst(frames[i].size(), frames[i].type());
		//dyeing CCs with random colors
		for (int j = 0; j < labeledImage.rows; j++) {
			for (int k = 0; k < labeledImage.cols; k++) {
				int label = labeledImage.at<int>(j, k);
				cv::Vec3b& pixel = framesCC[i].at<cv::Vec3b>(j, k);
				pixel = colors[label];
			}
		}
		cv::imwrite("frames/" + name_of_video + "_CC_" + std::to_string(i + 1) + ".png", framesCC[i]); //saving img with CCs

		cv::Mat standart_mask = cv::imread("../../../data/masks_lab04/" + name_of_video + "_MASK_1.png");
		float quality = 0;
		for (int k = 0; k < standart_mask.rows; k++)
		{
			for (int j = 0; j < standart_mask.cols; j++)
			{
				if (standart_mask.at<uint8_t>(k, j) == framesCC[i].at<uint8_t>(k, j))
				{
					quality++;
				}
			}
		}
		quality = (quality / (standart_mask.rows * standart_mask.cols)) * 100;
		cv::cvtColor(standart_mask, standart_mask, cv::COLOR_BGR2GRAY);
		cv::cvtColor(framesCC[i], framesCC[i], cv::COLOR_BGR2GRAY);
		std::cout << "Quality of " + name_of_video + " " << i + 1 << ": " << quality << std::endl;

		cv::Mat mask_over_original(frames[i].size(), CV_8UC3);
		mask_over_original = 0;

		for (int j = 0; j < framesCC[i].rows; j++)
		{
			for (int k=0;k<framesCC[i].cols;k++)
				if ((standart_mask.at<uint8_t>(j, k) == 0) && (framesCC[i].at<uint8_t>(j, k) == 0))
				{
					mask_over_original.at<cv::Vec3b>(j, k) = cv::Vec3b(0, 0, 0);
				}
				else if ((standart_mask.at<uint8_t>(j, k) == 0) && (framesCC[i].at<uint8_t>(j, k) == 255))
				{
					mask_over_original.at<cv::Vec3b>(j, k) = cv::Vec3b(0, 0, 255);
				}
				else if ((standart_mask.at<uint8_t>(j, k) == 255) && (framesCC[i].at<uint8_t>(j, k) == 0))
				{
					mask_over_original.at<cv::Vec3b>(j, k) = cv::Vec3b(255, 0, 0);
				}
				else if ((standart_mask.at<uint8_t>(j, k) == 255) && (framesCC[i].at<uint8_t>(j, k) == 255))
				{
					mask_over_original.at<cv::Vec3b>(j, k) = cv::Vec3b(255, 255, 255);
				}
		}
		cv::addWeighted(frames[i], 0.5, mask_over_original, 0.5, 0.0, mask_over_original);
		cv::imwrite("frames/" + name_of_video + "_final_" + std::to_string(i + 1) + ".png", mask_over_original); // saving final img
	}
	 

	cv::waitKey(0);
	video.release();
	return 0;
}

int main() {
	add_video("1000.MOV");
	add_video("1k.MOV");
	add_video("5k.MOV");
	add_video("500.MOV");
	add_video("100.mp4");

}