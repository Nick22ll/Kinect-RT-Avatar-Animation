#include "Sensor.h"

//safe way of deleting a COM object
template<typename T>
void safeRelease(T& ptr) { if (ptr) { ptr->Release(); ptr = nullptr; } }


Sensor::Sensor() 
{
    HRESULT errorCode;

    //get the kinect sensor
    errorCode = GetDefaultKinectSensor(&sensor);
    if (FAILED(errorCode))
    {
        printf("Failed to find the kinect sensor!\n");
        exit(10);
    }
    sensor->Open();

    //INITIALIZING DEPTH SENSOR
    //get the depth frame source
    IDepthFrameSource* depthFrameSource;
    errorCode = sensor->get_DepthFrameSource(&depthFrameSource);
    if (FAILED(errorCode))
    {
        printf("Failed to get the depth frame source.\n");
        exit(10);
    }
    //get the depth frame reader
    errorCode = depthFrameSource->OpenReader(&depthFrameReader);
    if (FAILED(errorCode))
    {
        printf("Failed to open the depth frame reader!\n");
        exit(10);
    }
    

    //get depth frame description
    
    IFrameDescription* frameDesc;
    depthFrameSource->get_FrameDescription(&frameDesc);
    frameDesc->get_Width(&depthWidth);
    frameDesc->get_Height(&depthHeight);

    //release depth frame source
    safeRelease(depthFrameSource);

    //allocate depth buffer
    this->depthBuffer = new uint16[depthWidth * depthHeight];
    

    //INITIALIZING BODY SENSOR
    //get skeleton joints source
    IBodyFrameSource* bodyFrameSource = nullptr;
    errorCode = sensor->get_BodyFrameSource(&bodyFrameSource);
    if (FAILED(errorCode))
    {
        printf("Failed to get the depth frame source.\n");
        exit(10);
    }

    errorCode = bodyFrameSource->OpenReader(&bodyFrameReader);

    if (FAILED(errorCode))
    {
        printf("Failed to open the body frame reader!\n");
        exit(10);
    }
    safeRelease(bodyFrameSource);

    //INITIALIZING RGB CAMERA SENSOR
    //get color frame source
    IColorFrameSource* colorFrameSource;
    errorCode = sensor->get_ColorFrameSource(&colorFrameSource);
    if (FAILED(errorCode))
    {
        printf("Failed to get color frame source!\n");
        exit(10);
    }

    //get color frame reader
    errorCode = colorFrameSource->OpenReader(&colorFrameReader);
    if (FAILED(errorCode))
    {
        printf("Failed to open color frame reader!\n");
        exit(10);
    }

    //get color frame descriprtion
    colorFrameSource->get_FrameDescription(&frameDesc);
    frameDesc->get_Width(&colorWidth);
    frameDesc->get_Height(&colorHeight);

    //release the color frame source
    safeRelease(colorFrameSource);

    //allocate color buffer
    colorBuffer = new uint32[colorWidth * colorHeight];

    //get the coordinate mapper
    errorCode = sensor->get_CoordinateMapper(&coordinateMapper);
    if (FAILED(errorCode))
    {
        printf("Failed to get coordinate mapper!\n");
        exit(10);
    }

    //allocate a buffer of color space points
    colorSpacePoints = new ColorSpacePoint[depthWidth * depthHeight];
}


void Sensor::getDepthFrame(uint16** depthOutputFrame)
{
    memcpy(*depthOutputFrame, depthBuffer, depthWidth * depthHeight * 2); // la copia avviene a byte e a noi servono uint16 = 2 byte
}

void Sensor::readDepthFrame()
{

    HRESULT errorCode;

    //depth stuff
    IDepthFrame* depthFrame;
    errorCode = depthFrameReader->AcquireLatestFrame(&depthFrame);
    if (FAILED(errorCode)) return;
    errorCode = depthFrame->CopyFrameDataToArray(depthWidth * depthHeight, depthBuffer);
    safeRelease(depthFrame);
    
    
    if (FAILED(errorCode))
    {
        printf("oh no, something went wrong while copying depth frame to the buffer!\n");
        exit(10);
    }
    
}

void Sensor::getColorFrame(uint32** colorOutputBuffer)
{
    
    memcpy(*colorOutputBuffer, colorBuffer, colorWidth * colorHeight  * 4); //si moltiplica per 4 perchè la copia avviene a byte
    
    
}

void Sensor::readColorFrame()
{
    
    IColorFrame* colorFrame;
    HRESULT errorCode = colorFrameReader->AcquireLatestFrame(&colorFrame);
    if (FAILED(errorCode)) {
        safeRelease(colorFrame);
        return;
    }

    errorCode = colorFrame->CopyConvertedFrameDataToArray(
        colorWidth * colorHeight * 4, (BYTE*)this->colorBuffer, ColorImageFormat_Bgra);

    
    safeRelease(colorFrame);
    
}

void Sensor::DepthMapper(ColorSpacePoint** colorOutputSpacePoints, int resize) {

    HRESULT errorCode = coordinateMapper->MapDepthFrameToColorSpace(depthWidth * depthHeight, depthBuffer, resize, colorSpacePoints);

    if (FAILED(errorCode))
    {
        printf("Oh no! Failed map the depth frame to color space!\n");
        return;
    }
}

int Sensor::getJoints(vector<Skeleton>& skeletons) {
    //put update and drawing stuff here

    HRESULT hr;

    //skeleton stuff
    IBodyFrame* bodyFrame = nullptr;
    hr = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
    if (FAILED(hr)) {
        safeRelease(bodyFrame);
        //printf("oh no, something went wrong in skeleton retrieve!\n");
        return -1;
    }
    IBody* bodies[BODY_COUNT] = { 0 };
    hr = bodyFrame->GetAndRefreshBodyData(_countof(bodies), bodies);

    int body_num = processBodies(BODY_COUNT, bodies, skeletons);

    //After body processing is done, we're done with our bodies so release them.
    for (unsigned int bodyIndex = 0; bodyIndex < _countof(bodies); bodyIndex++) {
        safeRelease(bodies[bodyIndex]);
    }

    safeRelease(bodyFrame);
    return body_num;
}

int Sensor::processBodies(const unsigned int& bodyCount, IBody** bodies, vector<Skeleton>& skeletons)
{
    for (unsigned int bodyIndex = 0; bodyIndex < bodyCount; bodyIndex++) {
        IBody* body = bodies[bodyIndex];

        //Get the tracking status for the body, if it's not tracked we'll skip it
        BOOLEAN isTracked = false;
        HRESULT hr = body->get_IsTracked(&isTracked);
        if (FAILED(hr) || isTracked == false) {
            continue;
        }

        //If we're here the body is tracked so lets get the joint properties for this skeleton
        Joint temp_joints[JointType_Count];
        JointOrientation temp_orientations[JointType_Count];
        vector<Joint> joint_array(0);
        vector<JointOrientation> orientation_array(0);
        body->GetJointOrientations(_countof(temp_orientations), temp_orientations);
        body->GetJoints(_countof(temp_joints), temp_joints);
        for (int i = 0; i < JointType_Count; i++) {
            orientation_array.push_back(temp_orientations[i]);
            joint_array.push_back(temp_joints[i]);
        }
        Skeleton skeleton(joint_array, orientation_array);
        skeletons.push_back(skeleton);
    }
    return skeletons.size();
}


