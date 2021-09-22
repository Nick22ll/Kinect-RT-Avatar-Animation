
//STD Libs
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<sstream>
#include<algorithm>
#include<map>
#include<string>
//GLEW
#include<glew.h>
#include "dataanalysis.h"
//OpenGL Math libs
#include<glm.hpp>
#include<vec3.hpp>
#include<vec4.hpp>
#include<mat4x4.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>

using namespace std;


struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texcoord;
	glm::vec3 normal;
};


struct Model {
	string name;
	vector<Vertex> vertices;
	string material;
	GLuint smooth_shading;  //Controllare il tipo
	glm::vec3 model_axis, min_point, max_point;
	glm::vec3 model_mid_point = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Kd, Ks, Ka;
};


class ComplexModel {
public:
	ComplexModel() {};
	vector<float> toUniqueVector();
	map<string, Model> models;
};


static ComplexModel loadOBJ(string path)
{
	std::cout << "Loading... " << path << std::endl;
	ComplexModel complexModel;
	std::stringstream ss;
	std::ifstream in_file(path);
	unsigned int vertex_count = 0, texture_count = 0, normal_count = 0;
	unsigned int correctiveVertex = 0, correctiveTexture = 0, correctiveNormal = 0;

	//File open error check
	if (!in_file.is_open())
	{
		throw "ERROR::OBJLOADER::Could not open file.";
	}
	std::string line = "";
	std::string prefix = "";
	glm::vec3 temp_vec3;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 temp_vec2;
	GLint temp_glint = 0;

	//Temporary Model
	Model tempModel;

	//Vertex portions
	std::vector<glm::fvec3> vertex_positions;
	std::vector<glm::fvec2> vertex_texcoords;
	std::vector<glm::fvec3> vertex_normals;

	//Face vectors
	std::vector<GLint> vertex_position_indicies;
	std::vector<GLint> vertex_texcoord_indicies;
	std::vector<GLint> vertex_normal_indicies;

	//Vertex array
	std::vector<Vertex> vertices;

	//Read one line at a time
	while (std::getline(in_file, line))
	{
		//Get the prefix of the line
		ss.clear();
		ss.str(line);
		ss >> prefix;
		
		if (prefix == "#")
		{

		}
		else if (prefix == "o")
		{
			//Load in all indices
			if (vertex_position_indicies.size() > 0) 
			{
				for (size_t i = 0; i < vertex_position_indicies.size(); ++i)
				{
					Vertex temp;
					temp.position = vertex_positions[vertex_position_indicies[i] - 1];
					if(vertex_texcoords.size()>0)
						temp.texcoord = vertex_texcoords[vertex_texcoord_indicies[i] - 1];
					temp.normal = vertex_normals[vertex_normal_indicies[i] - 1];
					temp.color = color;
					tempModel.vertices.push_back(temp);
				}
				//Calculate model middle point and axis
				alglib::real_2d_array Array2D;
				float min = 12346789237, max = -1217534278;
				Array2D.setlength(tempModel.vertices.size(), 3);
				for (int i = 0; i < tempModel.vertices.size(); i++) {
					for (int j = 0; j < 3; j++) {
						Array2D[i][j] = tempModel.vertices[i].position[j];
					}
					tempModel.model_mid_point += glm::vec3(tempModel.vertices[i].position[0], tempModel.vertices[i].position[1], tempModel.vertices[i].position[2]);
				
					if (tempModel.vertices[i].position[1] < min)
					{
						min = tempModel.vertices[i].position[1];
						tempModel.min_point = tempModel.vertices[i].position;
					}

					if (tempModel.vertices[i].position[1] > max)
					{
						max = tempModel.vertices[i].position[1];
						tempModel.max_point = tempModel.vertices[i].position;
					}
				}
				tempModel.model_mid_point /= tempModel.vertices.size();

				alglib::real_1d_array variances;
				alglib::real_2d_array basisVectors;
				alglib::pcatruncatedsubspace(Array2D, tempModel.vertices.size(), 3, 1, 0, 0, variances, basisVectors);
				tempModel.model_axis = glm::normalize(glm::vec3(basisVectors[0][0], basisVectors[1][0], basisVectors[2][0]));


				complexModel.models[tempModel.name] = tempModel;
				
			}

			correctiveVertex += vertex_positions.size();
			correctiveTexture += vertex_texcoords.size();
			correctiveNormal += vertex_normals.size();

			//Clearing all temporary variables
			vertex_positions.clear();
			vertex_texcoords.clear();
			vertex_normals.clear();

			vertex_position_indicies.clear();
			vertex_texcoord_indicies.clear();
			vertex_normal_indicies.clear();
			tempModel.vertices.clear();
			vertices.clear();
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			

			//Initialization of tempModel
			ss >> tempModel.name;

		}
		else if (prefix == "s")
		{
			ss >> tempModel.smooth_shading;
		}
		else if (prefix == "use_mtl" || prefix == "usemtl")
		{
			
			ss >> tempModel.material;
			std::stringstream cc;
			string mtl_path = path.substr(0,path.find_last_of("."))+".mtl";

			std::ifstream mtl_file(mtl_path);
			//File open error check
			if (!mtl_file.is_open())
			{
				throw "ERROR::OBJLOADER::Could not open mtl_file.";
			}
			std::string mtl_line = "";
			std::string mtl_prefix = "";
			std::string material_name = "";
			bool found = false;
			while (std::getline(mtl_file, mtl_line))
			{
				cc.clear();
				cc.str(mtl_line);
				cc >> mtl_prefix;
				cc >> material_name;
				if (mtl_prefix == "newmtl" && material_name == tempModel.material ) {
					found = true;
					while (mtl_line != "") {
						std::getline(mtl_file, mtl_line);
						cc.clear();
						cc.str(mtl_line);
						cc >> mtl_prefix;
						if (mtl_prefix == "Kd") {
							//cc >> tempModel.Kd.x >> tempModel.Kd.y >> tempModel.Kd.z;
							cc >> color.x >> color.y >> color.z;
							tempModel.Kd.x = color.x;
							tempModel.Kd.y = color.y;
							tempModel.Kd.z = color.z;
						}
							
						if (mtl_prefix == "Ka")
							cc >> tempModel.Ka.x >> tempModel.Ka.y >> tempModel.Ka.z;
						if (mtl_prefix == "Ks")
							cc >> tempModel.Ks.x >> tempModel.Ks.y >> tempModel.Ks.z;
					}
					
				}
				
			}

		}
		else if (prefix == "v") //Vertex position
		{
			ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
			vertex_positions.push_back(temp_vec3);
		}
		else if (prefix == "vt")
		{
			ss >> temp_vec2.x >> temp_vec2.y;
			vertex_texcoords.push_back(temp_vec2);
		}
		else if (prefix == "vn")
		{
			ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
			vertex_normals.push_back(temp_vec3);
		}
		else if (prefix == "f")
		{
			int counter = 0;
			while (ss >> temp_glint)
			{
				//Pushing indices into correct arrays
				if (counter == 0)
					vertex_position_indicies.push_back(temp_glint - correctiveVertex);
				else if (counter == 1)
					vertex_texcoord_indicies.push_back(temp_glint - correctiveTexture);
				else if (counter == 2)
					vertex_normal_indicies.push_back(temp_glint - correctiveNormal);

				//Handling characters
				if (ss.peek() == '/')
				{
					++counter;
					ss.ignore(1, '/');
					if (ss.peek() == '/')
					{
						vertex_texcoord_indicies.push_back(1.0f);
						++counter;
						ss.ignore(1, '/');
					}
				}
				else if (ss.peek() == ' ')
				{
					++counter;
					ss.ignore(1, ' ');
				}

				//Reset the counter
				if (counter > 2)
					counter = 0;
			}
		}
		else
		{
		}

	}
	if (vertex_position_indicies.size() > 0 )
	{
		for (size_t i = 0; i < vertex_position_indicies.size(); ++i)
		{
			Vertex temp;
			temp.position = vertex_positions[vertex_position_indicies[i] - 1];
			if (vertex_texcoords.size() > 0)
				temp.texcoord = vertex_texcoords[vertex_texcoord_indicies[i] - 1];
			temp.normal = vertex_normals[vertex_normal_indicies[i] - 1];
			temp.color = color;
			tempModel.vertices.push_back(temp);
		}


		//Calculate model middle point, max_point, min_point and axis
		float min = 12346789237, max = -1217534278;
		alglib::real_2d_array Array2D;
		Array2D.setlength(tempModel.vertices.size(), 3);
		for (int i = 0; i < tempModel.vertices.size(); i++) {
			for (int j = 0; j < 3; j++) {
				Array2D[i][j] = tempModel.vertices[i].position[j];
			}
			tempModel.model_mid_point += glm::vec3(tempModel.vertices[i].position[0], tempModel.vertices[i].position[1], tempModel.vertices[i].position[2]);
			
			if (tempModel.vertices[i].position[1] < min)
			{
				min = tempModel.vertices[i].position[1];
				tempModel.min_point = tempModel.vertices[i].position;
			}

			if (tempModel.vertices[i].position[1] > max)
			{
				max = tempModel.vertices[i].position[1];
				tempModel.max_point = tempModel.vertices[i].position;
			}
		}
		tempModel.model_mid_point /= tempModel.vertices.size();

		alglib::real_1d_array variances;
		alglib::real_2d_array basisVectors;
		alglib::pcatruncatedsubspace(Array2D, tempModel.vertices.size(), 3, 1, 0, 0, variances, basisVectors);
		tempModel.model_axis = glm::normalize(glm::vec3(basisVectors[0][0], basisVectors[1][0], basisVectors[2][0]));


		complexModel.models[tempModel.name] = tempModel;
	}
	//DEBUG
	std::cout << "Nr of models: " << complexModel.models.size() << "\n";

	//Loaded success
	std::cout << "OBJ file loaded!" << "\n";
	return complexModel;
}
