#include 	"video_control.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <yaml-cpp/yaml.h>
#include <opencv2/opencv.hpp>

#include <cctype>
#include <fstream>

std::string LowerCase(const unsigned char *word, const size_t &word_length);

namespace v4l2 {
VideoControl::VideoControl(const std::string &config_path, const unsigned char camera_id) {
	device_ = "/dev/video" + std::to_string(camera_id);
	config_path_ = config_path;
	window_name_ = "VideoControl";

	GetAvailableControls();
	std::vector<Config> yaml_config = GetConfigFromYAMLFile();
	MergeConfigAndSetAllControls(yaml_config);

	cv::namedWindow(window_name_, cv::WINDOW_AUTOSIZE);
	CreateTrackbar();
}

void VideoControl::GetAvailableControls() {
	int fd = open(device_.c_str(), O_RDWR);

	if (fd == -1) {
		std::cerr << "Failed to open " << device_ << std::endl;
		return;
	}

	v4l2_queryctrl queryctrl{};
	queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;

	while (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == 0) {
		if (queryctrl.minimum == 0 && queryctrl.maximum == 0) {
			queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
			continue;
		}

		v4l2_control control;
    control.id = queryctrl.id;

		if (ioctl(fd, VIDIOC_G_CTRL, &control) == -1) {
     std::cerr << "Failed to get control value" << std::endl;
    }

		Config config;
		config.control_id = queryctrl.id;
		config.name = LowerCase(queryctrl.name, sizeof(queryctrl.name));
		config.min_value = queryctrl.minimum;
		config.max_value = queryctrl.maximum;
		config.value = control.value;
		config.available = true;

		config_.push_back(config);
		queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
	}

	close(fd);
}

std::vector<Config> VideoControl::GetConfigFromYAMLFile() {
	std::vector<Config> yaml_config;
  std::ifstream checkfile(config_path_);

  if (!checkfile.is_open()) {
    std::cerr << "Unable to open file: " << config_path_ << std::endl;
    return yaml_config;
  }

	checkfile.close();

	YAML::Node config = YAML::LoadFile(config_path_);

 	for (YAML::const_iterator it = config.begin(); it != config.end(); ++it) {
		Config temp;
		temp.name = it->first.as<std::string>();
		temp.value = it->second.as<int>();

		yaml_config.push_back(temp);
 	}

	return yaml_config;
}

void VideoControl::MergeConfigAndSetAllControls(const std::vector<Config> &yaml_config) {
	for (const Config &yaml_item : yaml_config) {
		bool replaced = false;

		for (Config &config_item : config_) {
			if (config_item.name != yaml_item.name)
				continue;

			config_item.value = yaml_item.value;
			replaced = true;
			break;
		}

		if (!replaced)
			config_.push_back(yaml_item);
	}

	int fd = open(device_.c_str(), O_RDWR);

	if (fd == -1) {
		std::cerr << "Failed to open " << device_ << std::endl;
		return;
	}

	for (Config &config_item : config_) {
		if (!config_item.available)
			continue;

		SetControl(fd, config_item);
		config_item.previous_value = config_item.value;
	}

	close(fd);
}

void VideoControl::SetControl(const int &fd, const Config &config) {
	v4l2_control control{};
	control.id = config.control_id;
	control.value = config.value;

	if (ioctl(fd, VIDIOC_S_CTRL, &control) < 0) {
		std::cerr << "Failed to set control " << config.name << ": " << config.value << std::endl;
	}
}

void VideoControl::SetAllControls() {
	int fd = open(device_.c_str(), O_RDWR);

	if (fd == -1) {
		std::cerr << "Failed to open " << device_ << std::endl;
		return;
	}

	for (Config &config_item : config_) {
		if (!config_item.available || (config_item.previous_value == config_item.value))
			continue;

		SetControl(fd, config_item);
		config_item.previous_value = config_item.value;
	}

	close(fd);
}

void VideoControl::CreateTrackbar() {
	for (const Config &config_item : config_) {
		if (!config_item.available)
			continue;

		if (config_item.min_value < 0) {
			cv::createTrackbar(config_item.name, window_name_, NULL, abs(config_item.min_value - config_item.max_value));
			cv::setTrackbarPos(config_item.name, window_name_, config_item.value + abs(config_item.min_value));
		} else {
			cv::createTrackbar(config_item.name, window_name_, NULL, config_item.max_value);
			cv::setTrackbarPos(config_item.name, window_name_, config_item.value);
		}
	}
}

void VideoControl::GetTrackbarValue() {
	for (Config &config : config_) {
		if (!config.available)
			continue;

		config.value = cv::getTrackbarPos(config.name, window_name_);

		if (config.value < config.min_value)
			config.value = config.min_value;

		cv::setTrackbarPos(config.name, window_name_, config.value);

		if (config.min_value < 0)
			config.value -= abs(config.min_value);
	}

	SetAllControls();
}
void VideoControl::WriteConfigToYAMLFile() {
 std::ofstream file(config_path_);

  YAML::Emitter emitter;
  emitter << YAML::BeginMap;

	for (const Config &config : config_) {
  	emitter << YAML::Key << config.name;
  	emitter << YAML::Value << config.value;
	}

  emitter << YAML::EndMap;

  file << emitter.c_str();
  file.close();
	std::cout << "Config saved successfully to: " << config_path_ << std::endl;
}

} // namespace v4l2

std::string LowerCase(const unsigned char *word, const size_t &word_length) {
	std::string result;

	for (size_t i = 0; i < word_length; ++i) {
		if (std::isalpha(word[i])) {
			result += std::tolower(word[i]);
		} else if (word[i] == ' ') {
			result += '_';
		}
	}

	return result;
}
