#include "video_control.h"

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << std::endl << "usage: " << argv[0] << " <camera_id>" << std::endl;
		return 1;
	}

	int8_t camera_id = std::atoi(argv[1]);
	cv::VideoCapture capture(camera_id);

	if (!capture.isOpened()) {
		std::cerr << std::endl << "can't open camera: " << (int)camera_id << std::endl;
		return 1;
	}

	cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("brightness", "frame", NULL, 255);

	cv::Mat frame;
	short last_brightness = 0;
	short brightness = 1;

	v4l2::VideoControl video_control("config.yaml", camera_id);
	char key;
	char escape = 27;
	char save = 's';

	while (key != 27) {
		key = cv::waitKey(1);
		capture.read(frame);
		brightness = cv::getTrackbarPos("brightness", "frame");
		video_control.GetTrackbarValue();

		cv::imshow("frame", frame);
		last_brightness = brightness;

		if (key == save) {
			video_control.WriteConfigToYAMLFile();
		}
	}

	cv::destroyAllWindows();
	
	return 0;
}
