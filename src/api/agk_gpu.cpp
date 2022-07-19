#include "agk_gpu.hpp"
#include "agk_util.hpp"

void gpu::program::set1f(const std::string &uniform_name, float val) {
    glUniform1f(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void gpu::program::set1i(const std::string &uniform_name, int32_t val) {
    glUniform1i(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void gpu::program::set1ui(const std::string &uniform_name, uint32_t val) {
    glUniform1ui(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void gpu::program::set4f(const std::string &uniform_name, const float* val) {
    glUniform4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, val);
}

void gpu::program::set4fm(const std::string &uniform_name, const float *val) {
    glUniformMatrix4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, GL_FALSE, val);
}

bool gpu::compile_shader(GLuint &shader, GLuint mode, const char *shader_src) {
    shader = glCreateShader(mode);

    glShaderSource(shader, 1, &shader_src, NULL);
    glCompileShader(shader);

    GLint compile_status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

    if (!compile_status) {
        char log[256];
        glGetShaderInfoLog(shader, 256, NULL, log);
        util::log(log);
    }

    return compile_status;
}

void gpu::create_program(gpu::program &program, const char *vsh_path, const char *fsh_path) {
    agk_source vsh_source, fsh_source;
    bool flag = true;
    GLuint vsh, fsh;

    flag = util::open_file(vsh_source, vsh_path) && util::open_file(fsh_source, fsh_path);
    flag = flag && gpu::compile_shader(vsh, GL_VERTEX_SHADER, vsh_source.data1.c_str()) && gpu::compile_shader(fsh, GL_FRAGMENT_SHADER, fsh_source.data1.c_str());

    if (flag) {
        program.program = glCreateProgram();

        glAttachShader(program.program, vsh);
        glAttachShader(program.program, fsh);
        glLinkProgram(program.program);

        GLint link_status = GL_FALSE;
        glGetProgramiv(program.program, GL_LINK_STATUS, &link_status);

        if (!link_status) {
            char log[256];
            glGetProgramInfoLog(program.program, 256, NULL, log);
            util::log(log);
        }
    }

    program.validation = flag;
}
