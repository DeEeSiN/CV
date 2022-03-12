#include <opencv2/opencv.hpp>

int add_video(std::string name_of_video, int threshold, int Binarizationr_type,int w1,int h1, int w2, int h2)
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
		cv::threshold(framesGS[i], framesBin[i], threshold, 255, Binarizationr_type);
		cv::imwrite("frames/" + name_of_video + "_BIN_" + std::to_string(i + 1) + ".png", framesBin[i]);


		//morphology + saving
		cv::Mat structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(w1, h1));
		cv::morphologyEx(framesBin[i], framesMorph[i], cv::MORPH_CLOSE, structuring_element);
		cv::morphologyEx(framesMorph[i], framesMorph[i], cv::MORPH_OPEN, structuring_element);

		structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(w2, h2));
		cv::morphologyEx(framesMorph[i], framesMorph[i], cv::MORPH_DILATE, structuring_element);

		cv::imwrite("frames/" + name_of_video + "_MORPH_" + std::to_string(i + 1) + ".png", framesMorph[i]);


	
		//arrays for connecteedComponentsWithStats output
		cv::Mat labeledImage(framesMorph[i].size(), CV_32S), stats, centroids;
		//labeling connected components and gathering stats
		int nLabels = cv::connectedComponentsWithStats(framesMorph[i], labeledImage, stats, centroids, 8, CV_32S);

		//writing info about CCs
		std::cout << "\nnLables: " << nLabels << "\n";
		std::cout << name_of_video << std::endl;
		for (int j = 0; j < nLabels; j++) {
			std::cout << "frame: " << i + 1 << " label: " << j + 1 << " area: " << stats.at<int>(j, cv::CC_STAT_AREA) << std::endl;
		}

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


	}


	cv::waitKey(0);
	video.release();
	return 0;
}

int main() {
	add_video("old.MOV",115, cv::THRESH_BINARY_INV,6,10,95,95); 
	add_video("1k.MOV", 180, cv::THRESH_BINARY,6,10,100,10);
	add_video("5k.MOV", 90, cv::THRESH_BINARY_INV, 6, 10, 130, 155);
	add_video("500.MOV", 150, cv::THRESH_BINARY, 6, 10, 30, 30);
	add_video("100.mp4", 160, cv::THRESH_BINARY, 10, 15, 55, 55); 
	
}