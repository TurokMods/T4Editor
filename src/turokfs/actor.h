#pragma once
#include <Actor.h>
using namespace opent4;

#include <vector>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include <GLFW/glfw3.h>

#include <render/texture.h>
#include <render/frustum.h>

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
            actor_mesh(SubMesh* mesh, texture* tex, vector<texture*> sm_textures);
            ~actor_mesh();
            
            void render(shader* s, bool preview = false);
            
            GLuint vao;
            GLuint vbo;
            
            vector<mesh_vert> vertices;
            GLuint* ibos;
            vector<vector<unsigned short> > chunkIndices;
            application* app;
            actor* parent;
            int submesh_id;
            
            texture* m_Texture;
            vector<texture*> submesh_textures;

            void SetPosition(glm::vec3 pos);
            void SetScale(glm::vec3 scale);
            void SetRotation(glm::vec3 rot);
            
            vec3 getMinBound() const { return m_min; }
            vec3 getMaxBound() const { return m_max; }
            
        protected:
            vec3 m_min;
            vec3 m_max;
    };
    
    class actor {
        public:
            actor(application* app, ActorMesh* mesh, ActorDef* def, ATRFile* atr);
            ~actor();
            
            //Expects shader to already be bound
            void render(shader* s, const mat4& view, const mat4& viewproj, bool preview = false, mat4* overrideModel = 0);
        
            vector<actor_mesh*> meshes;
            
            int actor_id;
            int editor_id;

			ATRFile* actorFile;
            ActorDef* actorTraits;
            ActorMesh* meshTraits;
            
            vec3 getMinBound() const { return m_min; }
            vec3 getMaxBound() const { return m_max; }
            AABB getBoundingBox() const { return m_AABB; }
            
        protected:
            application* m_app;

            glm::vec3 m_lastFramePos;
            glm::vec3 m_lastFrameScale;
            glm::vec3 m_lastFrameRot;

            glm::mat4 m_transform;
            vec3 m_min;
            vec3 m_max;
            AABB m_AABB;
    };
}
