#pragma once
#include <Actor.h>
using namespace opent4;

#include <vector>
using namespace std;

#include <GLFW/glfw3.h>

namespace t4editor {
    struct mesh_vec3 {
        float x, y, z;
    };
    
    struct mesh_vec2 {
        float x, y;
    };
    
    struct mesh_vert {
        mesh_vec3 position;
        mesh_vec3 normal;
        mesh_vec2 texcoord;
    };
    
    class actor_mesh {
        public:
            actor_mesh(SubMesh* mesh);
            ~actor_mesh();
        
            void render();
        
            GLuint vao;
            GLuint vbo;
            GLuint ibo;
        
            vector<mesh_vert> vertices;
            vector<unsigned short> indices;
    };
    
    class actor {
        public:
            actor(const Actor* def);
            ~actor();
        
            vector<actor_mesh*> meshes;
    };
}
