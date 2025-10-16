#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/gtc\type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

		class Shader
		{
		public:
			// program ID
			unsigned int ID;

			Shader() {
				ID = 0;
			}


			//comp shader
			Shader(const char* compPath)
			{
				isCompute = true;

				// RETRIEVAL
				std::string compCode;
				std::ifstream cShaderFile;

				// ensure ifstream objects can throw exceptions
				cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
				try
				{
					// open files
					cShaderFile.open(compPath);
					std::stringstream cShaderStream;
					// read file's buffer into streams
					cShaderStream << cShaderFile.rdbuf();
					// close file handlers
					cShaderFile.close();
					// convert streams to string
					compCode = cShaderStream.str();
				}
				catch (std::ifstream::failure e)
				{
					std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ ==> " << compPath << ' ' << std::endl;
				}

				const char* cShaderCode = compCode.c_str();

				// COMPILING
				unsigned int comp;
				int success;
				char infoLog[512];

				// comp shader
				comp = glCreateShader(GL_COMPUTE_SHADER);
				glShaderSource(comp, 1, &cShaderCode, NULL);
				glCompileShader(comp);
				// error check
				glGetShaderiv(comp, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(comp, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED "
						<< compPath << '\n' << infoLog << std::endl;
				}

				// shader program
				ID = glCreateProgram();
				glAttachShader(ID, comp);
				glLinkProgram(ID);

				// print linking error
				glGetProgramiv(ID, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(ID, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
						<< infoLog << std::endl;
				}

				// delete shaders
				glDeleteShader(comp);
			}

			// constructor
			Shader(const char* vertexPath, const char* fragmentPath)
			{
				isCompute = false;

				// RETRIEVAL
				std::string vertexCode;
				std::string fragmentCode;
				std::ifstream vShaderFile;
				std::ifstream fShaderFile;

				// ensure ifstream objects can throw exceptions
				vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
				fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
				try
				{
					// open files
					vShaderFile.open(vertexPath);
					fShaderFile.open(fragmentPath);
					std::stringstream vShaderStream, fShaderStream;
					// read file's buffer into streams
					vShaderStream << vShaderFile.rdbuf();
					fShaderStream << fShaderFile.rdbuf();
					// close file handlers
					vShaderFile.close();
					fShaderFile.close();
					// convert streams to string
					vertexCode = vShaderStream.str();
					fragmentCode = fShaderStream.str();
				}
				catch (std::ifstream::failure e)
				{
					std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ ==> " << vertexPath << ' ' << fragmentPath << std::endl;
				}

				const char* vShaderCode = vertexCode.c_str();
				const char* fShaderCode = fragmentCode.c_str();

				// COMPILING
				unsigned int vertex, fragment;
				int success;
				char infoLog[512];

				// vertex shader
				vertex = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(vertex, 1, &vShaderCode, NULL);
				glCompileShader(vertex);
				// error check
				glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(vertex, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
						<< vertexPath << infoLog << std::endl;
				}

				// fragment shader
				fragment = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fragment, 1, &fShaderCode, NULL);
				glCompileShader(fragment);
				glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(vertex, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
						<< fragmentPath << infoLog << std::endl;
				}

				// shader program
				ID = glCreateProgram();
				glAttachShader(ID, vertex);
				glAttachShader(ID, fragment);
				glLinkProgram(ID);

				// print linking error
				glGetProgramiv(ID, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(ID, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
						<< infoLog << std::endl;
				}

				// delete shaders
				glDeleteShader(vertex);
				glDeleteShader(fragment);
			}

			void use()
			{
				glUseProgram(ID);
			}
			void useCompute(int groups_x, int groups_y, int groups_z)
			{
					glUseProgram(ID);
					glDispatchCompute(groups_x, groups_y, groups_z);
					glMemoryBarrier(GL_ALL_BARRIER_BITS);
				
			}
			void setBool(const std::string& name, bool value) const
			{
				glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
			}
			void setInt(const std::string& name, int value) const
			{
				glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
			}
			void setFloat(const std::string& name, float value) const
			{
				glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
			}
			void setVec3(const std::string& name, float v1, float v2, float v3) const
			{
				glUniform3f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3);
			}

			void setVec3(const std::string& name, glm::vec3 vec1) const
			{
				glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(vec1));
			}

			void setVec4(const std::string& name, glm::vec4 vec1) const
			{
				glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(vec1));
			}

			void setMat4(const std::string& name, glm::mat4 mat1) const
			{
				unsigned int location = glGetUniformLocation(ID, name.c_str());
				glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(mat1));
			}
			void setIvec2(const std::string& name, glm::ivec2 ivec2) const
			{
				unsigned int location = glGetUniformLocation(ID, name.c_str());
				glUniform2iv(location, 1, value_ptr(ivec2));
			}

		private:
			bool isCompute{ false };

		};

#endif