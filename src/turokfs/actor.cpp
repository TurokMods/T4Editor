#include <GL/glew.h>
#include <turokfs/actor.h>

namespace t4editor {
    actor_mesh::actor_mesh(SubMesh* mesh) {
        for(size_t i = 0;i < mesh->GetVertexCount();i++) {
            mesh_vert vert;
            mesh->GetTexCoord(i,&vert.texcoord.x);
            mesh->GetNormal(i,&vert.normal.x);
            mesh->GetVertex(i,&vert.position.x);
            vertices.push_back(vert);
        }
        
        for(size_t i = 0;i < mesh->GetIndexCount();i++) {
            indices.push_back(mesh->GetIndex(i));
        }
        
        if(vertices.size() == 0) return;
        
        int err = 0;
        err = glGetError(); if(err != 0) printf("err: %d | %s\n", err, glewGetErrorString(err));
        
        // Setup test openGL triangle
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_vert) * vertices.size(), &vertices[0].position.x, GL_STATIC_DRAW);
        
        if(indices.size() > 0) {
            glGenBuffers(1, &ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indices.size(), &indices[0], GL_STATIC_DRAW);
        }
        
        err = glGetError(); if(err != 0) printf("err: %d | %s\n", err, glewGetErrorString(err));
    }
    actor_mesh::~actor_mesh() {
    }
    
    void actor_mesh::render() {
        if(vertices.size() == 0) return;
        int err = 0;
        err = glGetError(); if(err != 0) printf("err: %d | %s\n", err, glewGetErrorString(err));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vert), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vert), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vert), (void*)(6 * sizeof(float)));
        
        if(indices.size() > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_SHORT, 0);
        } else {
            glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        err = glGetError(); if(err != 0) printf("err: %d | %s\n", err, glewGetErrorString(err));
    }
    
    actor::actor(const Actor* def) {
        ActorMesh* mesh = def->GetMesh();
        if(mesh) {
            for(size_t i = 0;i < mesh->GetSubMeshCount();i++) {
                SubMesh* sm = mesh->GetSubMesh(i);
                meshes.push_back(new actor_mesh(sm));
            }
        }
    }
    actor::~actor() {
        for(size_t i = 0;i < meshes.size();i++) {
            delete meshes[i];
        }
        meshes.clear();
    }
}
