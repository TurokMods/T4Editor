#pragma once
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <EngineTypes.h>
#include <glm/glm.hpp>
using namespace glm;

namespace t4editor {
    class application;
    class shader {
        public:
            shader(application* app);
            ~shader();
        
            bool loadFromFile(const string& vert, const string& frag);
            bool loadFromMemory(const string& vert, const string& frag);
        
            void attribute(const string& name, uint32_t idx);
            void uniform(const string& name, const mat4& m4);
            void uniform(const string& name, const vec4& v4);
            void uniform(const string& name, const vec3& v3);
            void uniform(const string& name, const vec2& v2);
            void uniformi(const string& name, int int32);
            void uniformui(const string& name, unsigned int uint32);
            void uniform(const string& name, float f);
            GLint getUniformLoc(const string& name);
        
            void bind();
        
        protected:
            GLuint m_vert;
            GLuint m_frag;
            GLuint m_shader;
            unordered_map<string,GLint> m_uniform_locs;
            typedef struct {
                uint32_t idx;
                string name;
            } attrib_mapping;
            vector<attrib_mapping> m_attribs;
            application* m_app;
    };
}
