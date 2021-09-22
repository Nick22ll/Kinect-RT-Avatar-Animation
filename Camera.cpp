#include"Camera.h"



Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::Init(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Sets new camera matrix
	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}


void Camera::Inputs(const SDL_Event& event)
{
	
	switch(event.type)
	{
		case SDL_KEYDOWN:
		{
			//Handles keyboard inputs
			SDL_Keycode keycode = event.key.keysym.sym;
			switch (keycode) {
			case SDLK_w:
				Position += speed * (Orientation);
				break;
			case SDLK_a:
				Position += speed * -glm::normalize(glm::cross(Orientation, Up));
				break;
			case SDLK_s:
				Position += speed * -Orientation;
				break;
			case SDLK_d:
				Position += speed * glm::normalize(glm::cross(Orientation, Up));
				break;
			case SDLK_SPACE:
				Position += speed * Up;
				break;
			case SDLK_LCTRL:
				Position += speed * -Up;
				break;
			case SDLK_LSHIFT:
				speed = 0.4f;
				break;
			case SDLK_o:
				Position = glm:: vec3(0.0f,1.0f, 2.0f);
				Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
				break;
			default:
				break;
			}
		}break;
		case SDL_KEYUP:
		{	
			SDL_Keycode keycode = event.key.keysym.sym;
			switch (keycode) {
				case SDLK_LSHIFT:
					speed = 0.1f;
					break;
				default:
					break;
			}
		}
		break;
		case SDL_MOUSEBUTTONDOWN: {
			switch (event.button.button) {
			case SDL_BUTTON_LEFT: {
				// Hides mouse cursor
				SDL_ShowCursor(SDL_DISABLE);

				// Prevents camera from jumping on the first click
				if (firstClick)
				{
					SDL_WarpMouseInWindow(nullptr, (width / 2), (height / 2));
					firstClick = false;
				}
			}break;
			case SDL_BUTTON_RIGHT:{
				// Hides mouse cursor
				SDL_ShowCursor(SDL_DISABLE);

				// Prevents camera from jumping on the first click
				if (firstClick)
				{
					SDL_WarpMouseInWindow(nullptr, (width / 2), (height / 2));
					firstClick = false;
				}
			}
				break;
			default:
				break;
			}
		}break;
		case SDL_MOUSEBUTTONUP:
		{
			switch (event.button.button) {
				case SDL_BUTTON_LEFT:
					// Unhides cursor since camera is not looking around anymore
					SDL_ShowCursor(SDL_ENABLE);
					// Makes sure the next time the camera looks around it doesn't jump
					firstClick = true;
					break;
				case SDL_BUTTON_RIGHT:
					// Unhides cursor since camera is not looking around anymore
					SDL_ShowCursor(SDL_ENABLE);
					// Makes sure the next time the camera looks around it doesn't jump
					firstClick = true;
					break;
				default:
					break;
			}
		}break;
		case SDL_MOUSEMOTION:
		{
			switch (event.button.button) 
			{
				case SDL_BUTTON_LEFT:
					if (firstClick == false) 
					{
						// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
						// and then "transforms" them into degrees 
						float rotX = sensitivity * (float)(event.motion.y - (height / 2)) / height;
						float rotY = sensitivity * (float)(event.motion.x - (width / 2)) / width;

						// Calculates upcoming vertical change in the Orientation
						glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

						// Decides whether or not the next vertical Orientation is legal or not
						if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
						{
							Orientation = newOrientation;
						}

						// Rotates the Orientation left and right
						Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

						// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
						SDL_WarpMouseInWindow(nullptr, (width / 2), (height / 2));
					}
					break;
				case SDL_BUTTON_X1:  //ATTENZIONE... IL TASTO DESTRO DEL MOUSE MI VIENE LETTO COME IL TASTO X1
					if (firstClick == false) {
						
						// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
						// and then "transforms" them into degrees 
						float rotX = sensitivity * (float)(event.motion.y - (height / 2)) / height;
						float rotY = sensitivity * (float)(event.motion.x - (width / 2)) / width;

					 
						glm::mat4x4 transformation = glm::mat4(1.0f);
						transformation = glm::translate(transformation, -Orientation);
						// Calculates upcoming vertical change in the Orientation
						transformation = glm::rotate(transformation, glm::radians(-rotX), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));

						// Rotates the Orientation left and right
						transformation = glm::rotate(transformation, glm::radians(-rotY), Up);
						
						  
						transformation = glm::translate(transformation, glm::vec3(0.0f,0.0f,-0.1f));

						Position =glm::vec3( transformation * glm::vec4(Position,1));
						Orientation = glm::vec3(transformation * glm::vec4(Orientation,1));

						 
						// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
						SDL_WarpMouseInWindow(nullptr, (width / 2), (height / 2));
					}
					break;
				default:
					break;
			}
		}
		break;
		case SDL_MOUSEWHEEL:
		{
			if(event.wheel.y > 0)
				Position += speed * Orientation;
			else
				Position -= speed * (Orientation);
		}
		return;
		default:
			break;
	}

}