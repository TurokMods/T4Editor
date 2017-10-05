#include <GL/glew.h>
#include <render/shader.h>
#include <app.h>

namespace t4editor {
    void logShaderError(GLuint ShaderID) {
        GLint status = 0;
        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &maxLength);
            
            char errorLog[maxLength];
            glGetShaderInfoLog(ShaderID, maxLength, &maxLength, &errorLog[0]);
            glDeleteShader(ShaderID);
            printf("%s", errorLog);
        }
    }
    
    shader::shader(application* app) {
        m_shader = 0;
        m_vert = 0;
        m_frag = 0;
        m_app = app;
    }
    shader::~shader() {
    }
    
    bool shader::loadFromFile(const string &vert, const string &frag) {
        string vsPath = m_app->editorDataPath() + "/" + vert;
        string fsPath = m_app->editorDataPath() + "/" + frag;
        
        FILE* vs_fp = fopen(vsPath.c_str(),"r");
        if(!vs_fp) {
            printf("Failed to open shader %s\n", vert.c_str());
            return false;
        }
        
        fseek(vs_fp, 0, SEEK_END);
        size_t vs_sz = ftell(vs_fp);
        fseek(vs_fp, 0, SEEK_SET);
        
        char* vs_code = new char[vs_sz + 1];
        memset(vs_code, 0, vs_sz + 1);
        if(fread(vs_code,vs_sz,1,vs_fp) != 1) {
            printf("Failed to read %lu bytes from shader file %s\n", vs_sz, vert.c_str());
            delete [] vs_code;
            fclose(vs_fp);
            return false;
        }
        fclose(vs_fp);
        
        string vs_src = string(vs_code, vs_sz);
        delete [] vs_code;
        
        FILE* fs_fp = fopen(fsPath.c_str(),"r");
        if(!fs_fp) {
            printf("Failed to open shader %s\n", frag.c_str());
            return false;
        }
        
        fseek(fs_fp, 0, SEEK_END);
        size_t fs_sz = ftell(fs_fp);
        fseek(fs_fp, 0, SEEK_SET);
        
        char* fs_code = new char[fs_sz + 1];
        memset(fs_code, 0, fs_sz + 1);
        if(fread(fs_code,fs_sz,1,fs_fp) != 1) {
            printf("Failed to read %lu bytes from shader file %s\n", fs_sz, frag.c_str());
            delete [] fs_code;
            fclose(fs_fp);
            return false;
        }
        fclose(fs_fp);
        
        string fs_src = string(fs_code, fs_sz);
        delete [] fs_code;
        
        return loadFromMemory(vs_src, fs_src);
    }
    bool shader::loadFromMemory(const string &vert, const string &frag) {
        m_vert = glCreateShader(GL_VERTEX_SHADER);
        m_frag = glCreateShader(GL_FRAGMENT_SHADER);
        
        const char* vs = vert.c_str();
        glShaderSource(m_vert, 1, &vs, 0);
        glCompileShader(m_vert);
        logShaderError(m_vert);
        
        const char* fs = frag.c_str();
        glShaderSource(m_frag, 1, &fs, 0);
        glCompileShader(m_frag);
        logShaderError(m_frag);
        
        m_shader = glCreateProgram();
        glAttachShader(m_shader, m_vert);
        glAttachShader(m_shader, m_frag);
        glLinkProgram(m_shader);
        return true;
    }
    void shader::attribute(const string &name, uint32_t idx) {
        glBindAttribLocation(m_shader, idx, name.c_str());
    }
    void shader::bind() {
        glUseProgram(m_shader);
    }
}
