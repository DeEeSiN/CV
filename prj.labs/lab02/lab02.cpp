#include <opencv2/opencv.hpp>

void getBrightness(cv::Mat bright, int* array)
{
	for (int i = 0; i < 256; i++)
	{
		array[i] = 0;
	}
	for (int i = 0; i < bright.rows; i++)
	{
		for (int j = 0; j < bright.cols; j++)
		{
			array[bright.at<uchar>(i, j)]++;
		}
	}
}

int main() {
	cv::Mat start_image_png = cv::imread("../../../data/cross_0256x0256.png");

	cv::imwrite("cross_0256x0256_025.jpg", start_image_png, {cv::IMWRITE_JPEG_QUALITY, 25});

	cv::Mat single_channel_png[3]; //BGR
	cv::split(start_image_png, single_channel_png);

	cv::Mat black(256, 256, CV_8UC1);
	black = 0;

	cv::Mat monochrome[3];
	cv::Mat blue[3] = { single_channel_png[0],black,black };
	cv::Mat green[3] = { black,single_channel_png[1],black };
	cv::Mat red[3] = { black,black,single_channel_png[2] };
	cv::merge(blue, 3, monochrome[0]);
	cv::merge(green, 3, monochrome[1]);
	cv::merge(red, 3, monochrome[2]);

	cv::Mat mosaic_png_top;
	cv::Mat mosaic_png_bot;
	cv::Mat mosaic_png_;

	cv::hconcat(start_image_png, monochrome[2], mosaic_png_top);
	cv::hconcat(monochrome[0], monochrome[1], mosaic_png_bot);
	cv::vconcat(mosaic_png_top, mosaic_png_bot, mosaic_png_);
	cv::imwrite("cross_0256x0256_png_channels.png", mosaic_png_);

	cv::imwrite("cross_0256x0256.jpg", start_image_png, { cv::IMWRITE_JPEG_QUALITY, 100 });
	cv::Mat start_image_jpg = cv::imread("cross_0256x0256.jpg");

	cv::Mat single_channel_jpg[3]; //BGR
	cv::split(start_image_jpg, single_channel_jpg);

	cv::Mat monochrome_jpg[3];
	cv::Mat jpg_blue[3] = { single_channel_jpg[0],black,black };
	cv::Mat jpg_green[3] = { black,single_channel_jpg[1],black };
	cv::Mat jpg_red[3] = { black,black,single_channel_jpg[2] };
	cv::merge(jpg_blue, 3, monochrome_jpg[0]);
	cv::merge(jpg_green, 3, monochrome_jpg[1]);
	cv::merge(jpg_red, 3, monochrome_jpg[2]);

	cv::Mat mosaic_jpg_top;
	cv::Mat mosaic_jpg_bot;
	cv::Mat mosaic_jpg_;

	cv::hconcat(start_image_jpg, monochrome_jpg[2], mosaic_jpg_top);
	cv::hconcat(monochrome_jpg[0], monochrome_jpg[1], mosaic_jpg_bot);
	cv::vconcat(mosaic_jpg_top, mosaic_jpg_bot, mosaic_jpg_);
	cv::imwrite("cross_0256x0256_jpg_channels.png", mosaic_jpg_);


	//histogram png
	cv::Mat back_screen_png(256, 256, CV_8UC3, cv::Scalar(240, 240, 240));

	int saturation_blue[256];
	int saturation_green[256];
	int saturation_red[256];
	getBrightness(single_channel_png[0], saturation_blue);
	getBrightness(single_channel_png[1], saturation_green);
	getBrightness(single_channel_png[2], saturation_red);

	int max = 0;
	for (int i = 0; i < 256; i++)
	{
		if (max < saturation_blue[i])
			max = saturation_blue[i];

		if (max < saturation_green[i])
			max = saturation_green[i];

		if (max < saturation_red[i])
			max = saturation_red[i];
	}

	for (int i = 0; i < 255; i++)
	{
		for (int j = 0; j < 255; j++)
		{
			cv::line(back_screen_png, cv::Point(i, 256-saturation_blue[i]/(max/256)), cv::Point(i+1, 256-saturation_blue[i+1]/(max/256)), cv::Scalar(255, 0, 0),1,8,0);
			cv::line(back_screen_png, cv::Point(i, 256 - saturation_green[i] / (max / 256)), cv::Point(i + 1, 256 - saturation_green[i + 1] / (max / 256)), cv::Scalar(0, 255, 0), 1, 8, 0);
			cv::line(back_screen_png, cv::Point(i, 256 - saturation_red[i] / (max / 256)), cv::Point(i + 1, 256 - saturation_red[i + 1] / (max / 256)), cv::Scalar(0, 0, 255), 1, 8, 0);
		}
	}


	//histogram jpg
	cv::Mat back_screen_jpg(256, 256, CV_8UC3, cv::Scalar(240, 240, 240));
	
	getBrightness(single_channel_jpg[0], saturation_blue);
	getBrightness(single_channel_jpg[1], saturation_green);
	getBrightness(single_channel_jpg[2], saturation_red);
	
	max = 0;
	for (int i = 0; i < 256; i++)
	{
		if (max < saturation_blue[i])
			max = saturation_blue[i];

		if (max < saturation_green[i])
			max = saturation_green[i];

		if (max < saturation_red[i])
			max = saturation_red[i];
	}

	for (int i = 0; i < 255; i++)
	{
		for (int j = 0; j < 255; j++)
		{
			cv::line(back_screen_jpg, cv::Point(i, 256 - saturation_blue[i] / (max / 256)), cv::Point(i + 1, 256 - saturation_blue[i + 1] / (max / 256)), cv::Scalar(255, 0, 0), 1, 8, 0);
			cv::line(back_screen_jpg, cv::Point(i, 256 - saturation_green[i] / (max / 256)), cv::Point(i + 1, 256 - saturation_green[i + 1] / (max / 256)), cv::Scalar(0, 255, 0), 1, 8, 0);
			cv::line(back_screen_jpg, cv::Point(i, 256 - saturation_red[i] / (max / 256)), cv::Point(i + 1, 256 - saturation_red[i + 1] / (max / 256)), cv::Scalar(0, 0, 255), 1, 8, 0);
		}
	}


	cv::hconcat(start_image_png, start_image_png, mosaic_jpg_top);
	cv::hconcat(back_screen_png, back_screen_jpg, mosaic_jpg_bot);
	cv::vconcat(mosaic_jpg_top, mosaic_jpg_bot, mosaic_jpg_);
	cv::imwrite("cross_0256x0256_hists.png", mosaic_jpg_);

	cv::waitKey(0);
	
}