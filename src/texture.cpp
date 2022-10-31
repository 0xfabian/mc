#include <texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const char* path, int _unit)
{
    uint8_t* data = stbi_load(path, &width, &height, &chan_num, 0);

    if (!data)
    {
        printf("Failed to load texture\n");
        exit(5);
    }

    unit = _unit;

    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);

    GLint format = (chan_num == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture::bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::use_mipmap(const char* path, int level)
{
    int mipmap_width;
    int mipmap_height;
    int mipmap_chan_num;

    uint8_t* mipmap_data = stbi_load(path, &mipmap_width, &mipmap_height, &mipmap_chan_num, 0);

    if (!mipmap_data)
    {
        printf("Failed to load mimap texture\n");
        exit(6);
    }

    GLint format = (mipmap_chan_num == 4) ? GL_RGBA : GL_RGB;

    bind();

    glTexImage2D(GL_TEXTURE_2D, level, format, mipmap_width, mipmap_height, 0, format, GL_UNSIGNED_BYTE, mipmap_data);

    stbi_image_free(mipmap_data);
}

void Texture::destroy()
{
    glDeleteTextures(1, &id);
}