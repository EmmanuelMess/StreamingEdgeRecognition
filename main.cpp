#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
	auto windowName = "MORE CASIO WATCH MODS (LCD COLORS, TRANSPARENT DISPLAY, MICRO SD, STRAP REMOVER).mp4";

	auto filename = windowName;
	cv::VideoCapture capture(filename);
	cv::Mat frame;

	if( !capture.isOpened() )
		std::cerr << "Error when reading video";

	cv::namedWindow( windowName, 1);

	const int amountOfFramesToSmudge = 3;

	auto buffer = std::array<cv::Mat, amountOfFramesToSmudge>();
	buffer.fill(cv::Mat(720, 1280, CV_8UC1, cv::Scalar(0)));
	using BufferType = uint8_t;

	for( ; ; ) {
		auto startTicks = cv::getTickCount();
		auto beginTime = std::chrono::high_resolution_clock::now();

		capture >> frame;
		if(frame.empty())
			break;

		cv::Mat currentFrameEdges;

		Canny(frame, currentFrameEdges, 400, 600, 5, true);

		std::rotate(buffer.begin(), buffer.begin() + 1, buffer.end());

		buffer[amountOfFramesToSmudge-1]  = currentFrameEdges.clone() / static_cast<float>(static_cast<BufferType>(-1));

		cv::Mat finalEdges = buffer[0].clone();

		for (int i = 1; i < amountOfFramesToSmudge; ++i) {
			finalEdges += buffer[i];
		}

		cv::Mat result(720, 1280, CV_8UC1);

		result.forEach<uint8_t>([&finalEdges](uint8_t & pixel, const int position[]) -> void {
			pixel = finalEdges.at<BufferType>(position) >= amountOfFramesToSmudge ? static_cast<uint8_t>(-1) : 0;
		});

		cv::Mat bgrCanny;
		cv::merge(std::array<cv::Mat, 3>{result, result, result}, bgrCanny);

		auto halfBGR = frame(cv::Rect(0, 0, frame.cols / 2, frame.rows));
		auto halfCanny = bgrCanny(cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows));

		cv::Mat twoHalfs(frame.rows, frame.cols, frame.type());
		halfBGR.copyTo(twoHalfs(cv::Rect(0, 0, frame.cols / 2, frame.rows)));
		halfCanny.copyTo(twoHalfs(cv::Rect(twoHalfs.cols / 2, 0, twoHalfs.cols / 2, twoHalfs.rows)));

		auto endTicks = cv::getTickCount();
		auto endTime = std::chrono::high_resolution_clock::now();

		auto ticks = static_cast<double>(endTicks - startTicks) / cv::getTickFrequency();
		auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime-beginTime).count();

		std::cout << "Ticks: " << ticks << " Time: " << time <<  "ms\n";

		cv::imshow(windowName, twoHalfs);
		cv::waitKey(20); // waits to display frame
	}
	cv::waitKey(0); // key press to close window
	// releases and window destroy are automatic in C++ interface
}
