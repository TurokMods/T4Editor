#pragma once
#include <Actor.h>
using namespace opent4;

#include <vector>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include <GLFW/glfw3.h>

namespace t4editor {
    class shader;
    class application;
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
    
    class actor;
    class actor_mesh {
        public:
            actor_mesh(SubMesh* mesh);
            ~actor_mesh();
        
            void render(shader* s);
        
            GLuint vao;
            GLuint vbo;
        
            vector<mesh_vert> vertices;
            GLuint* ibos;
            vector<vector<unsigned short> > chunkIndices;
            application* app;
            actor* parent;
            int submesh_id;
    };
    
    class actor {
        public:
            actor(application* app, const ActorMesh* mesh, const ActorDef* def);
            ~actor();
        
            //Expects shader to already be bound
            void render(shader* s);
        
            vector<actor_mesh*> meshes;
        
            //I know we normally use matrices, but I want to keep the transform representation
            //exactly like it is in the files until it gets sent to the GPU
            vec3 position;
            vec3 scale;
            vec3 rotation;
            int actor_id;
            int editor_id;
            
        protected:
            application* m_app;
    };
}
