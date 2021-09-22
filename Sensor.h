#include <iostream>
#include <memory>
#include"Skeleton.h"
using namespace std;

//usefull typedefs for explicit type sizes
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

//some often used STL header files

//some constants



class Sensor
{

public:
	Sensor();
	void readDepthFrame();
	void getDepthFrame(uint16** depthOutputFrame);
	void readColorFrame();
	void getColorFrame();

	int getJoints(vector<Skeleton>& skeletons);
	int processBodies(const unsigned int& bodyCount, IBody** bodies, vector<Skeleton>& skeletons);

	void getColorFrame(uint32** colorOutputBuffer);
	void DepthMapper(ColorSpacePoint** colorOutputSpacePoints, int resize);
	int depthWidth = 0, depthHeight = 0;
	int colorWidth = 0, colorHeight = 0;
private:
	IKinectSensor* sensor = nullptr;
	IDepthFrameReader* depthFrameReader = nullptr;
	IColorFrameReader* colorFrameReader = nullptr;
	IBodyFrameReader* bodyFrameReader = nullptr;
	ICoordinateMapper* coordinateMapper = nullptr;
	ColorSpacePoint* colorSpacePoints = nullptr;

	uint16* depthBuffer = nullptr;
	uint32* colorBuffer = nullptr;
};

