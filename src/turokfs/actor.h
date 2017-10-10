#pragma once
#include <Actor.h>
using namespace opent4;

#include <vector>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include <GLFW/glfw3.h>

#include <render/texture.h>

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
            actor_mesh(SubMesh* mesh, texture* tex);
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

			texture* m_Texture;
    };
    
    class actor {
        public:
            actor(application* app, ActorMesh* mesh, ActorDef* def);
            ~actor();
        
            //Expects shader to already be bound
            void render(shader* s);
        
            vector<actor_mesh*> meshes;
        
            int actor_id;
            int editor_id;
        
            ActorDef* actorTraits;
            ActorMesh* meshTraits;
            
        protected:
            application* m_app;
    };
}
