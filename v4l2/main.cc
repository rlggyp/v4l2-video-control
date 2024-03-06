#include "video_control.h"

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "usage: " << argv[0] << " <camera_id>" << std::endl;
		return 1;
	}

	unsigned char camera_id = std::atoi(argv[1]);
	cv::VideoCapture capture(camera_id);

	if (!capture.isOpened()) {
		std::cerr << std::endl << "can't open camera: " << (int)camera_id << std::endl;
		return 1;
	}

	cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);

	cv::Mat frame;
	v4l2::VideoControl video_control("config.yaml", camera_id);

	char key;
	char escape = 27;
	char save = 's';

	while (key != escape) {
		key = cv::waitKey(1);
		if (key == save)
			video_control.WriteConfigToYAMLFile();

		video_control.GetTrackbarValue();
		capture.read(frame);
		cv::imshow("frame", frame);
	}

	cv::destroyAllWindows();
	
	return 0;
}
