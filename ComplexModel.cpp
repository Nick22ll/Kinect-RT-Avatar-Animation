#include "ComplexModel.h"


vector<float> ComplexModel::toUniqueVector()
{
	vector<float> points;
	for (auto const& iter : models) {
		Model model = iter.second;
		for (int i = 0; i < model.vertices.size(); i++) {
			points.push_back(model.vertices[i].position[0]);
			points.push_back(model.vertices[i].position[1]);
			points.push_back(model.vertices[i].position[2]);
			points.push_back(model.vertices[i].color[0]);
			points.push_back(model.vertices[i].color[1]);
			points.push_back(model.vertices[i].color[2]);
			points.push_back(model.vertices[i].texcoord[0]);
			points.push_back(model.vertices[i].texcoord[1]);
		}
	}
	return points;
}
