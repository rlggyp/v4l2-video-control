#ifndef V4L2_VIDEO_CONTROL_H_
#define V4L2_VIDEO_CONTROL_H_

#include <string>
#include <vector>

namespace v4l2 {
struct Config {
	std::string name;
	int control_id;
	int min_value;
	int max_value;
	int value;
	int previous_value;
	bool available;

	Config()
		: name(""),
			control_id(0),
			min_value(0),
			max_value(0),
			value(0),
			previous_value(0),
			available(false) {}
};

class VideoControl {
 public:
	VideoControl() {}
	VideoControl(const std::string &config_path, unsigned char camera_id);
	void GetTrackbarValue();
	void WriteConfigToYAMLFile();

 private:
	void GetAvailableControls();
	std::vector<Config> GetConfigFromYAMLFile();
	void MergeConfigAndSetAllControls(const std::vector<Config> &yaml_config);
	void SetControl(const int &fd, const Config &config);
	void SetAllControls();
	void CreateTrackbar();

	std::string config_path_;
	std::string device_;
	std::string window_name_;
	std::vector<Config> config_;
};
} // namespace v4l2

#endif // V4L2_VIDEO_CONTROL_H_
