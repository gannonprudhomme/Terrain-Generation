#include "texture.h"

Texture::Texture(std::string fileName) : filePath(fileName) {
	this->textureID = load();

	// Set default texture coordinates
	texCoords[0] = 0; texCoords[1] = 0; // bottom left
	texCoords[2] = 0; texCoords[3] = 1; // top left
	texCoords[4] = 1; texCoords[5] = 1; // top right
	texCoords[6] = 1; texCoords[7] = 0; // bottom right

	this->x = 0;
	this->y = 0;
}

Texture::Texture(std::string fileName, std::vector<float> tc) : filePath(fileName) {
	this->textureID = load();

	// Set default texture coordinates
	texCoords[0] = tc[0]; texCoords[1] = tc[1]; // bottom left
	texCoords[2] = tc[2]; texCoords[3] = tc[3]; // top left
	texCoords[4] = tc[4]; texCoords[5] = tc[5]; // top right
	texCoords[6] = tc[6]; texCoords[7] = tc[7]; // bottom right
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::load() {
	unsigned int id;
	glGenTextures(1, &id);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrComponents, 0);
	if(data) {
		GLenum format;
		if(nrComponents == 1)
			format = GL_RED;
		else if(nrComponents == 3)
			format = GL_RGB;
		else if(nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); 

		// Set the class variables to the respective size of the image retrieved form stbi lib
		this->width = width;
		this->height = height;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << filePath << std::endl;
	}

	return id;
}

GLvoid* Texture::getImageData() {
	int width, height, nrComponents;
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrComponents, 0);

	return data;
}
