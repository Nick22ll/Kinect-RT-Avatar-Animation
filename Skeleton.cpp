#include "Skeleton.h"

Skeleton::Skeleton(vector<Joint> joints, vector<JointOrientation> orientations)
{
	this->joints = joints;
	this->orientations = orientations;
	this->gravity_center = glm::vec3(0.0f, 0.0f, 0.0f);	
	float min = 112345678;
	for (auto& it : joints) {
		if (it.Position.Y < min) {
			lower_point = glm::vec3(0.0f, it.Position.Y, 0.0f);
			min = it.Position.Y;
		}
		this->gravity_center += glm::vec3(it.Position.X, it.Position.Y, it.Position.Z);
	}
	this->gravity_center /= joints.size();
	transformation_matrix = glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	transformation_matrix = glm::translate(transformation_matrix, -lower_point);
	this->gravity_center = glm::vec3(transformation_matrix * glm::vec4(this->gravity_center, 1));
}


Skeleton::Skeleton() {
	gravity_center = glm::vec3(0.0f, 0.0f, 0.0f);
	transformation_matrix = glm::mat4(1.0f);
	lower_point = glm::vec3(0.0f, 0.0f, 0.0f);
}