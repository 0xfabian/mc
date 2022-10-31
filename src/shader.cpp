#include <shader.h>

Shader::Shader(const char* vs_path, const char* fs_path)
{
    char* vs_data = read_from_file(vs_path);

    if (!vs_data)
    {
        printf("Failed to read vertex shader source\n");
        exit(1);
    }

    GLuint vs_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs_id, 1, &vs_data, 0);
    free(vs_data);

    GLint compiled;
    glCompileShader(vs_id);
    glGetShaderiv(vs_id, GL_COMPILE_STATUS, &compiled);

    if (compiled != GL_TRUE)
    {
        printf("Failed to compile vertex shader\n");
        exit(2);
    }

    char* fs_data = read_from_file(fs_path);

    if (!fs_data)
    {
        printf("Failed to read fragment shader source\n");
        exit(3);
    }

    GLuint fs_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs_id, 1, &fs_data, 0);
    free(fs_data);

    glCompileShader(fs_id);
    glGetShaderiv(fs_id, GL_COMPILE_STATUS, &compiled);

    if (compiled != GL_TRUE)
    {
        printf("Failed to compile fragment shader\n");
        exit(4);
    }

    id = glCreateProgram();
    glAttachShader(id, vs_id);
    glAttachShader(id, fs_id);
    glLinkProgram(id);

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);
}

void Shader::bind()
{
    glUseProgram(id);
}

void Shader::upload_float(const char* var_name, float value)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform1f(loc, value);
}

void Shader::upload_int(const char* var_name, int value)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform1i(loc, value);
}

void Shader::upload_vec3(const char* var_name, const glm::vec3& vec)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Shader::upload_vec4(const char* var_name, const glm::vec4& vec)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void Shader::upload_mat4(const char* var_name, const glm::mat4& matrix)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::upload_texture(const char* var_name, const Texture& tex)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform1i(loc, tex.unit);
}

void Shader::destroy()
{
    glDeleteProgram(id);
}