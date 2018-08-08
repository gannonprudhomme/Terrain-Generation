#ifndef SHADER_H // Tells the compiler to only include and compile this header file if it hasn't been already
#define SHADER_H //		- Prevents linking conflicts

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "texture.h"

class Shader {
	public:
		// Program ID
		unsigned int id;

		Shader() : id(0) {
			
		}

		// Constructor reads and builds the shader
		Shader(const char* vertexPath, const char* fragmentPath) {
			// Retrieve the vertex/fragment source code from filePath
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;

			// Ensure ifstream objects can throw exceptions - wut
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try {
				// Open files
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;

				// Read file's buffer contents into streams
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				// Close file handlers
				vShaderFile.close();
				fShaderFile.close();

				// Convert stream into string
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			} catch(std::ifstream::failure e) {
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
			}

			// Once we're done loading in the code, compile the shaders

			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			unsigned int vertex, fragment;

			// Set up vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "VERTEX");

			// Set up fragment shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "FRAGMENT");

			// Link both shader objects into a shader program that we can use for rendering
			// Shader program object is the final linked version of multiple shaders combined
			id = glCreateProgram(); // Create the ID
			glAttachShader(id, vertex); // Attach the vertex shader
			glAttachShader(id, fragment); // Attach the fragment shader
			glLinkProgram(id); // Link the shaders altogether
			checkCompileErrors(id, "PROGRAM");

			// Delete the shaders we created, the useful part is already in the shader program
			glDeleteShader(vertex);
			glDeleteShader(fragment);

			std::cout << "Shader " << id << " created successfully from " << vertexPath << " and " << fragmentPath << std::endl;
		}

		Shader(const char* vertexPath, const char* tessControlPath, const char* tessEvalPath, const char* geomPath, const char* fragPath) {
			unsigned int vertex, tessControl, tessEval, geometry, fragment;
			vertex = compileShader(GL_VERTEX_SHADER, vertexPath);
			tessControl = compileShader(GL_TESS_CONTROL_SHADER, tessControlPath);
			tessEval = compileShader(GL_TESS_EVALUATION_SHADER, tessEvalPath);
			geometry = compileShader(GL_GEOMETRY_SHADER, geomPath);
			fragment = compileShader(GL_FRAGMENT_SHADER, fragPath);

			id = glCreateProgram();
			glAttachShader(id, vertex);
			glAttachShader(id, tessControl);
			glAttachShader(id, tessEval);
			glAttachShader(id, geometry);
			glAttachShader(id, fragment);
			glLinkProgram(id);
			checkCompileErrors(id, "PROGRAM");

			glDeleteShader(vertex);
			glDeleteShader(tessControl);
			glDeleteShader(tessEval);
			glDeleteShader(geometry);
			glDeleteShader(fragment);

			std::cout << "Tessellation Shader Created" << std::endl;
		}

		Shader(const char* vertexPath, const char* tessControlPath, const char* tessEvalPath, const char* fragPath) {
			unsigned int vertex, tessControl, tessEval, fragment;
			vertex = compileShader(GL_VERTEX_SHADER, vertexPath);
			tessControl = compileShader(GL_TESS_CONTROL_SHADER, tessControlPath);
			tessEval = compileShader(GL_TESS_EVALUATION_SHADER, tessEvalPath);
			fragment = compileShader(GL_FRAGMENT_SHADER, fragPath);

			id = glCreateProgram();
			glAttachShader(id, vertex);
			glAttachShader(id, tessControl);
			glAttachShader(id, tessEval);
			glAttachShader(id, fragment);
			glLinkProgram(id);
			checkCompileErrors(id, "PROGRAM");

			glDeleteShader(vertex);
			glDeleteShader(tessControl);
			glDeleteShader(tessEval);
			glDeleteShader(fragment);

			std::cout << "Tessellation Shader Created" << std::endl;
		}

		// Use/activate the shader
		void use() {
			glUseProgram(id);
		}

		// Utility uniform functions
		void setBool(const std::string &name, bool value) const {
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
		}
		void setInt(const std::string &name, int value) const {
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}

		void setFloat(const std::string &name, float value) const {
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		}

		void setVec2(const std::string &name, glm::vec2 vec) const {
			glUniform2f(glGetUniformLocation(id, name.c_str()), vec.x, vec.y);
		}

		void setVec2(const std::string &name, float x, float y) const {
			glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
		}

		void setVec3(const std::string &name, glm::vec3 vec) {
			glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vec));
		}

		void setVec3(const std::string &name, float x, float y, float z) const {
			glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
		}

		void setVec4(const std::string &name, glm::vec4 vec) {
			glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vec));
		}

		void setMat3(const std::string &name, glm::mat4 mat) const {
			glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
		}

		void setMat4(const std::string &name, glm::mat4 mat) const {
			glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
		}

		// Basically just setSampler
		void setTexture(const std::string &name, unsigned int textureLocation) {
			glUniform1i(glGetUniformLocation(id, name.c_str()), textureLocation);
		}

	private:
		unsigned int compileShader(GLenum type, const char* path) {
			std::string code;
			std::ifstream shaderFile;

			shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try {
				// Open file
				shaderFile.open(path);
				std::stringstream shaderStream;

				// Read file's buffer contents into streams
				shaderStream << shaderFile.rdbuf();

				// Close file handler
				shaderFile.close();

				// Convert stream into string
				code = shaderStream.str();
			} catch(std::ifstream::failure e) {
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ at path " << path << std::endl;
			}

			// Compile the shader
			const char* shaderCode = code.c_str();

			unsigned int shader;

			shader = glCreateShader(type);
			glShaderSource(shader, 1, &shaderCode, NULL);
			glCompileShader(shader); 
			//unsigned int shader;
			//shader = testCompileShader(type, path);
			switch(type) {
				case GL_VERTEX_SHADER:
					checkCompileErrors(shader, "VERTEX");
					break;
				case GL_TESS_CONTROL_SHADER:
					checkCompileErrors(shader, "TESS_CONTROL");
					break;
				case GL_TESS_EVALUATION_SHADER:
					checkCompileErrors(shader, "TESS_EVAL");
					break;
				case GL_GEOMETRY_SHADER:
					checkCompileErrors(shader, "GEOMETRY");
					break;
				case GL_FRAGMENT_SHADER:
					checkCompileErrors(shader, "FRAGMENT");
					break;
			}

			return shader;
		}

		unsigned int testCompileShader(GLenum type, std::string fileName) {
			FILE* fp = fopen(fileName.c_str(), "rt");
			if(!fp)
				return 0;
			// Get all lines from a file
			std::vector<std::string> sLines;
			char sLine[255];
			while(fgets(sLine, 255, fp))
				sLines.push_back(sLine);
			fclose(fp);

			const char** sProgram = new const char*[sLines.size()];
			for(int i = 0; i < sLines.size(); i++) {
				sProgram[i] = sLines[i].c_str();
			}

			unsigned int shader;
			shader = glCreateShader(type);
			glShaderSource(shader, sizeof(sProgram), (const GLchar**)sProgram, NULL);
			glAttachShader(id, shader);
			glCompileShader(shader);

			return shader;
		}

		void checkCompileErrors(unsigned int shader, std::string type) {
			int success;
			char infoLog[1024];
			if(type != "PROGRAM") {
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if(!success) {
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "SHADER COMPILATION ERROR of type: " << type << "\n" << infoLog << "\n -- ---------------------------------------------------- -- " << std::endl;
				}
			} else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if(!success) {
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "PROGRAM LINKING ERROR of type " << type << "\n" << infoLog << "\n -- -------------------------------------------------------- -- " << std::endl;
				}
			}
		}
};

#endif