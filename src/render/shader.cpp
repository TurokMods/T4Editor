#include <render/shader.h>
#include <app.h>
#include <logger.h>

namespace t4editor {
	void logShaderError(GLuint ShaderID) {
		GLint status = 0;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

			char* errorLog = new char[maxLength];
			glGetShaderInfoLog(ShaderID, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(ShaderID);
			printf("%s", errorLog);
			delete[] errorLog;
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
		fread(vs_code, vs_sz, 1, vs_fp);
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
		fread(fs_code, fs_sz, 1, fs_fp);
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
        for(size_t i = 0;i < m_attribs.size();i++) {
            glBindAttribLocation(m_shader, m_attribs[i].idx, m_attribs[i].name.c_str());
        }
        glLinkProgram(m_shader);
        return true;
    }
    void shader::attribute(const string &name, uint32_t idx) {
        attrib_mapping map;
        map.idx = idx;
        map.name = name;
        m_attribs.push_back(map);
    }
    void shader::uniform(const string& name, const mat4& m4) {
        GLint loc = getUniformLoc(name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, &m4[0][0]);
    }
    void shader::uniform(const string& name, const vec4& v4) {
        GLint loc = getUniformLoc(name);
        glUniform4fv(loc, 1, &v4[0]);
    }
    void shader::uniform(const string& name, const vec3& v3) {
        GLint loc = getUniformLoc(name);
        glUniform3fv(loc, 1, &v3[0]);
    }
    void shader::uniform(const string& name, const vec2& v2) {
        GLint loc = getUniformLoc(name);
        glUniform2fv(loc, 1, &v2[0]);
    }
    void shader::uniformi(const string& name, int int32) {
        GLint loc = getUniformLoc(name);
        glUniform1iv(loc, 1, &int32);
    }
	void shader::uniform1i(const string&name, int int32) {
		GLint loc = getUniformLoc(name);
		glUniform1i(loc, int32);
	}
    void shader::uniformui(const string& name, unsigned int uint32) {
        GLint loc = getUniformLoc(name);
        glUniform1uiv(loc, 1, &uint32);
    }
    void shader::uniform(const string& name, float f) {
        GLint loc = getUniformLoc(name);
        glUniform1fv(loc, 1, &f);
    }
    GLint shader::getUniformLoc(const string &name) {
        auto it =  m_uniform_locs.find(name);
        GLint loc = 0;
        if(it == m_uniform_locs.end()) {
            loc = glGetUniformLocation(m_shader, name.c_str());
            m_uniform_locs[name] = loc;
        } else return m_uniform_locs[name];
        return loc;
    }
    void shader::bind() {
        glUseProgram(m_shader);
    }
}
