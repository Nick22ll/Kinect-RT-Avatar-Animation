#pragma once
#include <chrono>
#include <thread>
#include <iostream>
#include <thread>
#include <memory>

#include <vector>
#include<tuple>
#include<map>
#include <Kinect.h>
#include <boost/assign/list_of.hpp>
#include <boost/tuple/tuple.hpp>
//OpenGl stuff
#include<glm.hpp>

#include"Camera.h"
#include"shaderClass.h"
using namespace std;

static vector<boost::tuple<int, int>> bones = boost::assign::tuple_list_of(0, 1) (1, 20) (20, 2) (2, 3)(20, 4)(20, 8)(4, 5)(5, 6)(6, 7)(7, 21)(6, 22)(8, 9)(9, 10)(10, 11)(11, 23)(10, 24)(0, 12)(12, 13)(13, 14)(14, 15)(0, 16)(16, 17)(17, 18)(18, 19);
static map<string, tuple<int, int>> bones_map = {{"TORSO", std::make_tuple(20, 0)},{"NECK", std::make_tuple(2, 20)}, {"HEAD", std::make_tuple(3, 2)},{"LEFT_SHOULDER", std::make_tuple(20, 4)},{"RIGHT_SHOULDER", std::make_tuple(20, 8)},{"LEFT_HUMERUS", std::make_tuple(4, 5)},{"LEFT_RADIUS", std::make_tuple(5, 6)},{"LEFT_HAND", std::make_tuple(6, 7)},{"LEFT_HAND_TIP", std::make_tuple(7, 21)},{"LEFT_THUMB", std::make_tuple(6, 22)},{"RIGHT_HUMERUS", std::make_tuple(8, 9)},{"RIGHT_RADIUS", std::make_tuple(9, 10)},{"RIGHT_HAND", std::make_tuple(10, 11)},{"RIGHT_HAND_TIP", std::make_tuple(11, 23)},{"RIGHT_THUMB", std::make_tuple(10, 24)},{"LEFT_HIP", std::make_tuple(0, 12)},{"LEFT_THIGH_BONE", std::make_tuple(12, 13)},{"LEFT_SHIN_BONE", std::make_tuple(13, 14)},{"LEFT_FOOT", std::make_tuple(14, 15)},{"RIGHT_HIP", std::make_tuple(0, 16)},{"RIGHT_THIGH_BONE", std::make_tuple(16, 17)},{"RIGHT_SHIN_BONE", std::make_tuple(17, 18)},{"RIGHT_FOOT", std::make_tuple(18, 19)}};


class Skeleton
{
	public:
		Skeleton(vector<Joint> joints, vector<JointOrientation> orientations);
		Skeleton();
		//Attributes
		vector<Joint> joints;
		vector<JointOrientation> orientations;
		glm::vec3 gravity_center;
		glm::vec3 lower_point;
		glm::mat4 transformation_matrix;
};

