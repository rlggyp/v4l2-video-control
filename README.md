# v4l2-video-control
Controlling a Video Camera with v4l2 and OpenCV

## Dependencies
| Dependency      | Version  |
| --------------- | -------- |
| libv4l-dev      |          |
| libyaml-cpp-dev |          |
| OpenCV          | >=4.2.0  |
| C++             | >=14     |
| CMake           | >=3.12.0 |

## Documentation Reference
- [User Controls](https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/control.html)
- [Extended Controls](https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/extended-controls.html)

## Build 
```bash
git clone https://github.com/rlggyp/v4l2-video-control.git
cd v4l2-video-control/v4l2

mkdir build
cd build
cmake ..
make

```
## Usage
```bash
./main <camera_id>
```
## Contributing
Contributions are welcome! If you have any suggestions, bug reports, or feature requests, please open an issue or submit a pull request.

## License
This project is licensed under the [MIT License](LICENSE). See the LICENSE file for details.
