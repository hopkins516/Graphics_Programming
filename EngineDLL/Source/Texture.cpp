#include "../Headers/Texture.h"
#include "../Headers/stb_image.h"
#include "../Headers/Renderer.h"
#include <iostream>

namespace marchinGL {
	Texture::Texture(unsigned int width, unsigned int height) {
		GLCall(glGenTextures(1, &m_texture));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_texture));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
			0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}

	Texture::Texture(const char* imgPath, const bool flipVertical, const GLint  TextureWrap_S, const GLint  TextureWrap_T,
		const GLint  TextureMinFilter, const GLint  TextureMagFilter) {

		GLCall(glGenTextures(1, &m_texture));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_texture));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrap_S));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrap_T));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureMinFilter));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureMagFilter));
		stbi_set_flip_vertically_on_load(flipVertical);

		int nrChannels;
		unsigned char* data = stbi_load(imgPath, &m_width, &m_height, &nrChannels, 0);
		if (data) {
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;
			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height,
				0, format, GL_UNSIGNED_BYTE, data));
			GLCall(glGenerateMipmap(GL_TEXTURE_2D));
		} else {
			std::cout << "Failed to load texture" << std::endl;
		}
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		stbi_image_free(data);
	}

	Texture::~Texture() {
		GLCall(glDeleteTextures(1, &m_texture));
	}

	void Texture::Bind(unsigned int slot) const {
		GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_texture));
	}

	unsigned int Texture::GetID() const {
		return m_texture;
	}

	unsigned int Texture::GetWidth() {
		return m_width;
	}

	unsigned int Texture::GetHeight() {
		return m_height;
	}

}