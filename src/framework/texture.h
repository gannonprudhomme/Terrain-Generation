#pragma once

#include <string>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <iostream>
#include "../utils/stb_image.h"

class Texture {
	public:
		GLfloat texCoords[8];
	protected:
		std::string &filePath;
		unsigned int textureID; // texture ID itself
		unsigned int width, height; // normalized
		unsigned int x, y; // coordinates(normalized) in the texture. if this texture isn't a sprite sheet, should be (0, 0)
	public:
		Texture(std::string fileName);
		Texture(std::string fileName, std::vector<float> tc);

		void bind();
		void unbind();

		inline unsigned int getWidth() const { return width; }
		inline unsigned int getHeight() const { return height; }
		inline unsigned int getID() const { return textureID; }

		inline std::string getFilePath() const { return filePath; }

		GLvoid* getImageData();
	private:
		unsigned int load();
};