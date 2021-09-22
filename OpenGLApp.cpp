#include "OpenGLApp.h"

using namespace std;
using namespace glm;


vec4 RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
		if (l2Norm(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return vec4(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = normalize(cross(start, dest));

	return vec4(
		acos(cosTheta),
		rotationAxis
	);

}
void fatalError(string errorString) {
	cout << errorString << endl;
	cout << "Enter any key to quit..." << endl;
	int tmp;
	cin >> tmp;

	SDL_Quit();
}

OpenGLApp::OpenGLApp()
{
	//allocate kinect sensor
	this->kinect = new Sensor;

	int errorCode = setWindowSize(-1, -1);
	if (errorCode != 0)
		exit(errorCode);


	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		fatalError("Glew initialization failed!");
		exit(4);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Link the shaders
	DefaultShader.link("Resources/Shader/DefaultVertexShader.vert", "Resources/Shader/DefaultFragShader.frag");
	AlignShader.link("Resources/Shader/SkeletonAlignModel.vert", "Resources/Shader/SkeletonAlignShader.frag");
	LightShader.link("Resources/Shader/light.vert", "Resources/Shader/light.frag");
	NoTextureShader.link("Resources/Shader/NoTextureShader.vert", "Resources/Shader/NotextureShader.frag");
	//GENERATE REFERENCES FOR VAO(Vertex Array Object) AND VBO(Vertex Buffer Object)
	//Generate the VAO and VBO with only 1 object each

	// Generates Vertex Array Object and binds it
	defaultVAO.giveID();
	defaultVAO.Bind();
	defaultVBO.giveID();
	defaultEBO.giveID();

	// Links VBO  to VAO
	defaultVAO.LinkAttrib(defaultVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
	defaultVAO.LinkAttrib(defaultVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	defaultVAO.LinkAttrib(defaultVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	defaultVAO.LinkAttrib(defaultVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	defaultVAO.Unbind();

	// Generates Vertex Array Object and binds it
	lightVAO.giveID();
	lightVAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	lightVBO.giveID();
	lightVBO.fill(lightVertices, sizeof(lightVertices));
	// Generates Element Buffer Object and links it to indices
	lightEBO.giveID();
	lightEBO.fill(lightIndices, sizeof(lightIndices));

	// Links VBO attributes such as coordinates and colors to VAO
	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	// Unbind all to prevent accidentally modifying them
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();


	modelVAO.giveID();
	modelVAO.Bind();
	modelVBO.giveID();
	modelEBO.giveID();

	// Links VBO  to VAO
	modelVAO.LinkAttrib(modelVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	modelVAO.LinkAttrib(modelVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	modelVAO.LinkAttrib(modelVBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	modelVAO.LinkAttrib(modelVBO, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(8 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	modelVAO.Unbind();

	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(3.0f, 6.0f, 8.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);


	LightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(LightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform3f(glGetUniformLocation(LightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

	DefaultShader.Activate();
	glUniform3f(glGetUniformLocation(DefaultShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(DefaultShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	AlignShader.Activate();
	glUniform3f(glGetUniformLocation(AlignShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(AlignShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	NoTextureShader.Activate();
	glUniform3f(glGetUniformLocation(NoTextureShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(NoTextureShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	//TEXTURE
	defaultTexture.LinkTexture("Resources/Texture/paperino.jpeg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	floorTexture.LinkTexture("Resources/Texture/grass.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	avatarTexture.LinkTexture("Resources/Texture/colors/blu.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	woodTexture.LinkTexture("Resources/Texture/Legno.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	//OPENGL STUFF
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//CAMERA INITIALIZATION
	camera.Init(windowWidth, windowHeight, glm::vec3(0.0f, 1.0f, 2.0f));
}


void OpenGLApp::Run()
{
	int status = 0;
	int iteration = 0;
	//ComplexModel MrToast = loadOBJ("Resources/Models/MrToast/MrToast.obj");
	ComplexModel floor = loadOBJ("Resources/Models/Floor/floor.obj");
	ComplexModel avatar = loadOBJ("Resources/Models/Avatar/avatar.obj");
	ComplexModel tree = loadOBJ("Resources/Models/Tree/tree.obj");
	ComplexModel bush = loadOBJ("Resources/Models/Bush/bush.obj");

	SDL_Event event;
	while (true) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			return;
		if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			return;

		if (steady_clock::now() - lastSkeletonRead > 10ms)
		{
			vector<Skeleton> temp_sks;
			if (kinect->getJoints(temp_sks) > 0) {
				skeletons.clear();
				skeletons = temp_sks;
				lastSkeletonRead = steady_clock::now();
			}
		}
		// Specify the color of the background
		glClearColor(0.90196f, 1.0f, 1.0f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//pressing the cross or pressing escape will quit the application

		camera.Inputs(event);
		camera.updateMatrix(55.0f, 0.1f, 100.0f);

		drawLight();
		drawAvatarSkeleton(avatar, translate(vec3(-1.0f, 0.0f, 0.0f)));
		drawCubeSkeleton(translate(vec3(1.0f, 0.0f, 0.0f)));
		drawSkeleton(translate(vec3(-1.0f, 0.0f, 0.0f)));
		drawComplexModel(tree, translate(vec3(4.0f, 0.0f, -8.0f)));
		drawComplexModel(bush, translate(vec3(3.5f, 0.0f, -8.0f)));
		drawComplexModel(tree, translate(rotate(radians(45.0f), vec3(0.0f, 1.0f, 0.0f)), vec3(-2.0f, 0.0f, -8.0f)));
		drawComplexModel(bush, translate(rotate(radians(45.0f), vec3(0.0f, 1.0f, 0.0f)), vec3(-2.5f, 0.0f, -8.0f)));
		drawComplexModel(avatar, translate(vec3(4.5f, 0.0f, -6.0f)));
		drawComplexModel(avatar, translate(rotate(radians(45.0f), vec3(0.0f, 1.0f, 0.0f)), vec3(2.5f, 0.0f, -5.5f)));
		drawComplexModel(avatar, translate(rotate(radians(50.0f), vec3(0.0f, 1.0f, 0.0f)), vec3(3.5f, 0.0f, -5.0f)));
		drawComplexModel(avatar, translate(rotate(radians(-30.0f), vec3(0.0f, 1.0f, 0.0f)), vec3(1.5f, 0.0f, -4.5f)));
		drawComplexModel(floor, floorTexture);
		SDL_GL_SwapWindow(window);
	}
}

int OpenGLApp::SDL_initialization()
{
	//initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//create a window
	window = SDL_CreateWindow("KinectProject3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		fatalError("Window opening failed!");
		return 2;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr) {
		fatalError("The context creation is failed!");
		return 3;
	}

	return 0;
}

int OpenGLApp::setWindowSize(int w, int h)
{
	// -1 -> reset to screen size
	// 0 -> keep current size
	if (w == -1)
	{
		windowWidth = SCRWIDTH;
	}
	else {
		if (w >= 0 && w <= SCRWIDTH) {
			if (w != 0)
				windowWidth = w;
		}
		else
		{
			std::cout << "You can't set these sizes!" << std::endl;
			exit(10);
		}
	}

	if (h == -1)
	{
		windowHeight = SCRHEIGHT;
	}
	else {
		if (h >= 0 && h <= SCRHEIGHT) {
			if (h != 0)
				windowHeight = h;
		}
		else
		{
			std::cout << "You can't set these sizes!" << std::endl;
			exit(10);
		}
	}

	glViewport(0, 0, windowWidth, windowHeight);
	return SDL_initialization();
}

void OpenGLApp::drawGLfloat(GLfloat* array, Texture texture) {

	// Tells OpenGL which Shader Program we want to use
	DefaultShader.Activate();
	// Exports the camera Position to the Fragment Shader for specular lighting
	glUniform3f(glGetUniformLocation(DefaultShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// Export the camMatrix to the Vertex Shader of the pyramid
	glUniformMatrix4fv(glGetUniformLocation(DefaultShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	camera.Matrix(DefaultShader, "camMatrix");

	texture.Bind();

	// Make the VAO the current Vertex Array Object by binding it
	defaultVAO.Bind();

	defaultVBO.fill(array, sizeof(array));

	// Draw primitives, number of indices, datatype of indices, index of indices
	glDrawArrays(GL_QUADS, 0, sizeof(array));
	DefaultShader.DeActivate();

}
void OpenGLApp::drawLight() {
	// Tells OpenGL which Shader Program we want to use
	LightShader.Activate();
	glUniform3f(glGetUniformLocation(LightShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// Export the camMatrix to the Vertex Shader of the light cube
	camera.Matrix(LightShader, "camMatrix");

	// Bind the VAO so OpenGL knows to use it
	lightVAO.Bind();
	// Draw primitives, number of indices, datatype of indices, index of indices
	glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
	lightVAO.Unbind();
	LightShader.DeActivate();
}

void OpenGLApp::drawSkeleton(mat4 transformation) {

	GLfloat sizes[10];  // Store supported line width range
	GLfloat step;     // Store supported line width increments

	glGetFloatv(GL_LINE_WIDTH_RANGE, sizes);
	glGetFloatv(GL_LINE_WIDTH_GRANULARITY, &step);
	glLineWidth(sizes[1]);

	for (int body = 0; body < skeletons.size(); body++) {
		const unsigned int elements = 25 * (3 + 3 + 2 + 3);
		GLfloat temp[elements];
		GLuint joint_indices[24 * 2];
		for (int i = 0; i < elements; i += 11) {
			//Coordinates
			temp[i] = skeletons[body].joints[int(i / 11)].Position.X;
			temp[i + 1] = skeletons[body].joints[int(i / 11)].Position.Y;
			temp[i + 2] = skeletons[body].joints[int(i / 11)].Position.Z;
			//Color
			temp[i + 3] = 1.0f;
			temp[i + 4] = 1.0f;
			temp[i + 5] = 1.0f;
			//Texture Coordinates
			temp[i + 6] = 0.0f;
			temp[i + 7] = 0.0f;

			//Normals Coordinates
			temp[i + 8] = skeletons[body].orientations[int(i / 11)].Orientation.x;
			temp[i + 9] = skeletons[body].orientations[int(i / 11)].Orientation.y;
			temp[i + 10] = skeletons[body].orientations[int(i / 11)].Orientation.z;
		}

		for (int i = 0; i < 24 * 2; i += 2) {
			joint_indices[i] = bones[int(i / 2)].get<0>();
			joint_indices[i + 1] = bones[int(i / 2)].get<1>();
		}

		DefaultShader.Activate();
		defaultTexture.texUnit(DefaultShader, "tex0", 0);
		glUniform3f(glGetUniformLocation(DefaultShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		// Export the camMatrix to the Vertex Shader
		camera.Matrix(DefaultShader, "camMatrix");
		glUniformMatrix4fv(glGetUniformLocation(DefaultShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(translate(skeletons[body].transformation_matrix * transformation, vec3(1.0f, 0.0f, 0.0f))));

		defaultTexture.Bind();
		// Make the VAO the current Vertex Array Object by binding it
		defaultVAO.Bind();
		defaultVBO.fill(temp, sizeof(temp));
		defaultEBO.fill(joint_indices, sizeof(joint_indices));
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_LINES, sizeof(joint_indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		defaultTexture.Unbind();
		defaultVAO.Unbind();
		defaultEBO.Unbind();
		defaultVBO.Unbind();
		DefaultShader.DeActivate();
	}
}

void OpenGLApp::drawAvatarSkeleton(ComplexModel avatar, mat4 transformation) {

	for (int body = 0; body < skeletons.size(); body++) {
		AlignShader.Activate();
		avatarTexture.texUnit(AlignShader, "tex0", 0);
		glUniform3f(glGetUniformLocation(AlignShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(AlignShader, "camMatrix");
		//Read all sub-models
		for (const auto& it : avatar.models) {
			mat4 temp_trans = transformation;
			glm::vec3 model_axis = it.second.model_axis;
			glm::vec3 model_mid_point = it.second.model_mid_point;
			glm::vec3 bone_mid_point;
			glm::vec3 bone_axis;
			tuple<int, int> joints_indices;
			Model model = it.second;
			string name = it.first.substr(0, it.first.find_last_of("_"));

			if (name == "HEAD")
				joints_indices = make_tuple(3, 20);
			else if (name == "LEFT_UPPER_ARM")
				joints_indices = bones_map["LEFT_HUMERUS"];
			else if (name == "LEFT_DOWN_ARM")
				joints_indices = bones_map["LEFT_RADIUS"];
			else if (name == "LEFT_HAND")
				joints_indices = bones_map["LEFT_HAND"];
			else if (name == "LEFT_THUMB")
				joints_indices = bones_map["LEFT_THUMB"];
			else if (name == "RIGHT_UPPER_ARM")
				joints_indices = bones_map["RIGHT_HUMERUS"];
			else if (name == "RIGHT_DOWN_ARM")
				joints_indices = bones_map["RIGHT_RADIUS"];
			else if (name == "RIGHT_HAND")
				joints_indices = bones_map["RIGHT_HAND"];
			else if (name == "RIGHT_THUMB")
				joints_indices = bones_map["RIGHT_THUMB"];
			else if (name == "TORSO")
				joints_indices = make_tuple(2, 0);
			else if (name == "LEFT_UPPER_LEG")
				joints_indices = bones_map["LEFT_THIGH_BONE"];
			else if (name == "LEFT_DOWN_LEG")
				joints_indices = bones_map["LEFT_SHIN_BONE"];
			else if (name == "LEFT_FOOT")
				joints_indices = bones_map["LEFT_FOOT"];
			else if (name == "RIGHT_UPPER_LEG")
				joints_indices = bones_map["RIGHT_THIGH_BONE"];
			else if (name == "RIGHT_DOWN_LEG")
				joints_indices = bones_map["RIGHT_SHIN_BONE"];
			else if (name == "RIGHT_FOOT")
				joints_indices = bones_map["RIGHT_FOOT"];

			glm::vec3 JointA = vec3(skeletons[body].transformation_matrix * vec4(vec3(skeletons[body].joints[get<0>(joints_indices)].Position.X, skeletons[body].joints[get<0>(joints_indices)].Position.Y, skeletons[body].joints[get<0>(joints_indices)].Position.Z), 1));
			glm::vec3 JointB = vec3(skeletons[body].transformation_matrix * vec4(vec3(skeletons[body].joints[get<1>(joints_indices)].Position.X, skeletons[body].joints[get<1>(joints_indices)].Position.Y, skeletons[body].joints[get<1>(joints_indices)].Position.Z), 1));
			bone_mid_point = (JointA + JointB) / 2.0f;
			bone_axis = normalize(JointB - JointA);
			glm::vec4 result = RotationBetweenVectors(model_axis, bone_axis);
			float alpha = result[0];
			vec3 rotation_axis = vec3(result[1], result[2], result[3]);
			float bone_lenght = l2Norm(JointA - JointB);
			float model_lenght = l2Norm(it.second.max_point - it.second.min_point);
			float scale = bone_lenght / model_lenght;
			glm::mat4 scale_mat = glm::scale(glm::vec3(0.6 * scale, scale, 0.6 * scale));

			if (name == "TORSO") {
				alpha += radians(-90.0f);
				scale_mat = glm::scale(glm::vec3(scale * 0.7, scale*0.95, scale * 0.7));
				temp_trans = translate(transformation, vec3(0.0f, 0.08f, 0.0f));
			}
			if (name == "HEAD")
				scale_mat = glm::scale(glm::vec3(0.9 * scale, scale, 0.9 * scale));

			name = it.first.substr(0, it.first.find_first_of("_"));
			if (name == "RIGHT")
				alpha += radians(180.0f);

			//Rendering the model
			AlignShader.Activate();
			avatarTexture.Bind();
			modelVAO.Bind();
			modelVBO.Bind();
			modelVBO.fill(it.second.vertices);

			glUniform1f(glGetUniformLocation(AlignShader.ID, "angle"), alpha);
			glUniform3fv(glGetUniformLocation(AlignShader.ID, "rotation_axis"), 1, glm::value_ptr(rotation_axis));
			glUniform3fv(glGetUniformLocation(AlignShader.ID, "model_mid_point"), 1, glm::value_ptr(it.second.model_mid_point));
			glUniform3fv(glGetUniformLocation(AlignShader.ID, "bone_mid_point"), 1, glm::value_ptr(bone_mid_point));
			glUniformMatrix4fv(glGetUniformLocation(AlignShader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(scale_mat));
			glUniform1i(glGetUniformLocation(AlignShader.ID, "noTexture"), -1);
			glUniformMatrix4fv(glGetUniformLocation(AlignShader.ID, "transformation"), 1, GL_FALSE, glm::value_ptr(temp_trans));

			glDrawArrays(GL_QUADS, 0, it.second.vertices.size());

			avatarTexture.Unbind();
			modelVAO.Unbind();
			modelEBO.Unbind();
			modelVBO.Unbind();
			AlignShader.DeActivate();
		}
	}
}

void OpenGLApp::drawCubeSkeleton(mat4 transformation) {

	for (int body = 0; body < skeletons.size(); body++) {
		AlignShader.Activate();
		avatarTexture.texUnit(AlignShader, "tex0", 0);
		glUniform3f(glGetUniformLocation(AlignShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(AlignShader, "camMatrix");

		for (const auto& it : bones_map) {
			mat4 temp_trans = transformation;
			glm::vec3 bone_mid_point;
			glm::vec3 bone_axis;
			tuple<int, int> joints_indices = it.second;

			glm::vec3 JointA = vec3(skeletons[body].transformation_matrix * vec4(vec3(skeletons[body].joints[get<0>(joints_indices)].Position.X, skeletons[body].joints[get<0>(joints_indices)].Position.Y, skeletons[body].joints[get<0>(joints_indices)].Position.Z), 1));
			glm::vec3 JointB = vec3(skeletons[body].transformation_matrix * vec4(vec3(skeletons[body].joints[get<1>(joints_indices)].Position.X, skeletons[body].joints[get<1>(joints_indices)].Position.Y, skeletons[body].joints[get<1>(joints_indices)].Position.Z), 1));
			bone_mid_point = (JointA + JointB) / 2.0f;
			bone_axis = normalize(JointA - JointB);
			glm::vec4 result = RotationBetweenVectors(vec3(0.0f, 1.0f, 0.0f), bone_axis);
			float alpha = result[0];
			vec3 rotation_axis = vec3(result[1], result[2], result[3]);
			float bone_lenght = l2Norm(JointA - JointB);

			vec3 scale = vec3(0.2 * bone_lenght, bone_lenght, 0.2 * bone_lenght);

			string name = it.first;
			avatarTexture.Bind();

			if (name == "HEAD")
				scale = vec3(bone_lenght, bone_lenght, bone_lenght);
			else if (name == "NECK")
				scale = vec3(0.65 * bone_lenght, bone_lenght, 0.65 * bone_lenght);
			else if (name == "TORSO") {
				scale = vec3(0.30 * bone_lenght, bone_lenght, 0.30 * bone_lenght);
				avatarTexture.Unbind();
				defaultTexture.Bind();
			}

			glm::mat4 scale_mat = glm::scale(glm::vec3(scale.x, scale.y, scale.z));

			//Rendering the model
			AlignShader.Activate();
			defaultVAO.Bind();
			defaultVBO.Bind();
			defaultVBO.fill(cube_vertices, sizeof(cube_vertices));

			glUniform1f(glGetUniformLocation(AlignShader.ID, "angle"), alpha);
			glUniform3fv(glGetUniformLocation(AlignShader.ID, "rotation_axis"), 1, glm::value_ptr(rotation_axis));
			glUniform3fv(glGetUniformLocation(AlignShader.ID, "bone_mid_point"), 1, glm::value_ptr(bone_mid_point));
			glUniform3fv(glGetUniformLocation(AlignShader.ID, "model_mid_point"), 1, glm::value_ptr(vec3(0.0f, 0.0f, 0.0f)));
			glUniformMatrix4fv(glGetUniformLocation(AlignShader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(scale_mat));
			glUniformMatrix4fv(glGetUniformLocation(AlignShader.ID, "transformation"), 1, GL_FALSE, glm::value_ptr(temp_trans));
			glUniform1i(glGetUniformLocation(AlignShader.ID, "noTexture"), 1);

			glDrawArrays(GL_QUADS, 0, sizeof(cube_vertices));

			avatarTexture.Unbind();
			modelVAO.Unbind();
			modelEBO.Unbind();
			modelVBO.Unbind();
			AlignShader.DeActivate();
		}
	}
}


void OpenGLApp::drawComplexModel(ComplexModel complexModel, Texture& texture, glm::mat4 transformation) {
	DefaultShader.Activate();
	texture.texUnit(DefaultShader, "tex0", 0);
	glUniform3f(glGetUniformLocation(DefaultShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(DefaultShader, "camMatrix");

	for (const auto& iter : complexModel.models) {
		glUniformMatrix4fv(glGetUniformLocation(DefaultShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(transformation));
		glUniformMatrix4fv(glGetUniformLocation(DefaultShader.ID, "inverse_model"), 1, GL_FALSE, glm::value_ptr(inverse(transformation)));

		texture.Bind();
		modelVAO.Bind();
		modelVBO.Bind();
		modelVBO.fill(iter.second.vertices);

		glDrawArrays(GL_QUADS, 0, iter.second.vertices.size());

		texture.Unbind();
		modelVAO.Unbind();
		modelEBO.Unbind();
		modelVBO.Unbind();
	}
	AlignShader.DeActivate();
}

void OpenGLApp::drawComplexModel(ComplexModel complexModel, mat4 transformation) {
	NoTextureShader.Activate();
	glUniform3f(glGetUniformLocation(NoTextureShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(NoTextureShader, "camMatrix");
	for (const auto& iter : complexModel.models) {
		glUniformMatrix4fv(glGetUniformLocation(NoTextureShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(transformation));
		glUniformMatrix4fv(glGetUniformLocation(NoTextureShader.ID, "inverse_model"), 1, GL_FALSE, glm::value_ptr(inverse(transformation)));

		modelVAO.Bind();
		modelVBO.Bind();
		modelVBO.fill(iter.second.vertices);

		glDrawArrays(GL_QUADS, 0, iter.second.vertices.size());

		modelVAO.Unbind();
		modelEBO.Unbind();
		modelVBO.Unbind();
	}
	NoTextureShader.DeActivate();
}

void OpenGLApp::drawFromVector(vector<float> vertices)
{
	defaultTexture.Bind();
	// Make the VAO the current Vertex Array Object by binding it
	defaultVAO.Bind();
	defaultVBO.fill(vertices);
	glDrawArrays(GL_QUADS, 0, vertices.size());

	// Swap the back buffer with the front buffer
	SDL_GL_SwapWindow(window);
}


void OpenGLApp::Shutdown()
{
	defaultVAO.Delete();
	defaultVBO.Delete();
	defaultEBO.Delete();

	lightVAO.Delete();
	lightVBO.Delete();
	lightEBO.Delete();

	modelVAO.Delete();
	modelVBO.Delete();
	modelEBO.Delete();

	LightShader.Delete();
	DefaultShader.Delete();
	NoTextureShader.Delete();
	AlignShader.Delete();

	defaultTexture.Delete();
	floorTexture.Delete();
	avatarTexture.Delete();
	woodTexture.Delete();

	SDL_DestroyWindow(window);
	SDL_Quit();
}



