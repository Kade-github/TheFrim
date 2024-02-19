#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "../../Include/stb_image.h"
#include "stbi.h"

Texture* stbi_h::stbi_load_file(std::string filePath)
{
	stbi_set_flip_vertically_on_load(true);
	int w;
	int h;
	unsigned char* c = stbi_load(filePath.c_str(), &w, &h, nullptr, 4);
	if (get_error())
		std::cout << "[Image] [Error] failure to load " << stbi_failure_reason() << std::endl;
	return new Texture(c, w, h);
}

unsigned char* stbi_h::stbi_load_file_data(std::string filePath, int* w, int* h)
{
	unsigned char* c = stbi_load(filePath.c_str(), w, h, nullptr, 4);
	return c;
}

Texture* stbi_h::stbi_load_memory(char* memory, int size)
{
	stbi_set_flip_vertically_on_load(true);
	int w = 0;
	int h = 0;

	unsigned char* data = stbi_load_from_memory((stbi_uc*)memory, size, &w, &h, NULL, 4);

	if (get_error())
		std::cout << "[Image] [Error] [Memory] failure to load " << stbi_failure_reason() << std::endl;

	return new Texture(data, w, h);
}

void stbi_h::stbi_free(unsigned char* data)
{
	stbi_image_free(data);
}

bool stbi_h::get_error()
{
	if (stbi_failure_reason())
	{
		std::string reason = std::string(stbi_failure_reason());
		if (reason != "can't fopen" && reason != "bad png sig")
		{
			return true;
		}
	}
	return false;
}