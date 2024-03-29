#include <GL/glew.h>
#include <turokfs/actor.h>
#include <app.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <gui/ImGuizmo.h>

#include <render/shader.h>
#include <render/texture.h>

using namespace glm;

namespace t4editor {
    vec3 int_to_vec3(int i) {
        int r = (i & 0x000000FF) >>  0;
        int g = (i & 0x0000FF00) >>  8;
        int b = (i & 0x00FF0000) >> 16;
        //printf("%d -> %d, %d, %d -> %f, %f, %f\n", i, r, g, b, float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f);
        return vec3(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f);
    }

    actor_mesh::actor_mesh(SubMesh* mesh, texture* tex, vector<texture*>sm_textures) {
        submesh_textures = sm_textures;
        m_Texture = tex;
		vao = 0;
		vbo = 0;

        for(size_t i = 0;i < mesh->GetVertexCount();i++) {
            mesh_vert vert;
            memset(&vert, 0, sizeof(mesh_vert));
            mesh->GetTexCoord(i,&vert.texcoord.x);
            mesh->GetNormal(i,&vert.normal.x);
            mesh->GetVertex(i,&vert.position.x);
            vertices.push_back(vert);
        }

        if(vertices.size() > 0) {
            m_min = m_max = vec3(vertices[0].position.x, vertices[0].position.y, vertices[0].position.z);
            for(int i = 0;i < vertices.size();i++) {
                vec3 pos = vec3(vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
                if(pos.x < m_min.x) m_min.x = pos.x;
                if(pos.x > m_max.x) m_max.x = pos.x;
                if(pos.y < m_min.y) m_min.y = pos.y;
                if(pos.y > m_max.y) m_max.y = pos.y;
                if(pos.z < m_min.z) m_min.z = pos.z;
                if(pos.z > m_max.z) m_max.z = pos.z;
            }
        }
        
        vector<unsigned short> firstChunkIndices;
        for(size_t i = 0;i < mesh->GetIndexCount();i++) {
            firstChunkIndices.push_back(mesh->GetIndex(i));
        }
        if(firstChunkIndices.size() != 0) chunkIndices.push_back(firstChunkIndices);
        
        for(size_t c = 0;c < mesh->GetChunkCount();c++) {
            MeshChunk* ch = mesh->GetChunk(c);
            vector<unsigned short> indices;
            for(size_t i = 0;i < ch->GetIndexCount();i++) {
                indices.push_back(ch->GetIndex(i));
            }
            
            if(indices.size() > 0) chunkIndices.push_back(indices);
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
        
		ibos = nullptr;
        if(chunkIndices.size() > 0) {
            ibos = new GLuint[chunkIndices.size()];
            memset(ibos, 0, chunkIndices.size() * sizeof(GLuint));
            glGenBuffers(chunkIndices.size(), ibos);
            for(size_t i = 0;i < chunkIndices.size();i++) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * chunkIndices[i].size(), &chunkIndices[i][0], GL_STATIC_DRAW);
            }
        }
        
        err = glGetError(); if(err != 0) printf("err: %d | %s\n", err, glewGetErrorString(err));
    }

    actor_mesh::~actor_mesh() {
        int err = 0;
        err = glGetError();
		if(err != 0)
			printf("err: %d | %s\n", err, glewGetErrorString(err));
        if(vao) glDeleteVertexArrays(1, &vao);
        if(vbo) glDeleteBuffers(1, &vbo);
        if(ibos) {
            glDeleteBuffers(chunkIndices.size(), ibos);
        }
        err = glGetError();
		if(err != 0)
			printf("err: %d | %s\n", err, glewGetErrorString(err));
    }
    
    void actor_mesh::render(shader* s, bool preview) {
        if(vertices.size() == 0) return;
        int err = 0;
        err = glGetError();
		if(err != 0)
			printf("err: %d | %s\n", err, glewGetErrorString(err));
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vert), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vert), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vert), (void*)(6 * sizeof(float)));
        
        if(chunkIndices.size() > 0) {
            if(!preview) s->uniform("debug_float", 0.0f);
            for(size_t i = 0;i < chunkIndices.size();i++) {
                texture* tex = 0;
                if(submesh_textures.size() > i) tex = submesh_textures[i];
                else if(m_Texture) tex = m_Texture;
                else tex = app->getDefaultTexture();
                tex->bind();
                glActiveTexture(GL_TEXTURE0);
                s->uniform1i("diffuse_map", 0);
                if(!preview) s->uniform("actor_submesh_chunk_id", int_to_vec3(i));
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
                glDrawElements(GL_TRIANGLE_STRIP, chunkIndices[i].size(), GL_UNSIGNED_SHORT, 0);
            }
        } else {
            if(m_Texture) m_Texture->bind();
            else app->getDefaultTexture()->bind();
            glActiveTexture(GL_TEXTURE0);

            s->uniform1i("diffuse_map", 0);
            if(!preview) s->uniform("debug_float", 1.0f);
            if(!preview) s->uniform("actor_submesh_chunk", int_to_vec3(0));
            glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        err = glGetError();
		if(err != 0)
			printf("err: %d | %s\n", err, glewGetErrorString(err));
    }

    actor::actor(application* app, ActorMesh* mesh, ActorDef* def, ATRFile* atr) {
		/* TODO: 
		* This has all kinds of dynamic allocations and other weirdness going on here.
		*/
        m_app = app;
        actorTraits = def;
        meshTraits = mesh;
		actorFile = atr;
		level* lev = app->getLevel();

        if(mesh) {
            for(size_t i = 0;i < mesh->GetSubMeshCount();i++) {
                texture* t = 0;
                if (i < mesh->m_MeshInfos.size()) {
                    MeshInfo minfo = mesh->m_MeshInfos[i];
                    if(minfo.TSNR_ID != -1)
                    {
                        int TexID = mesh->m_TXSTs[mesh->m_TSNRs[minfo.TSNR_ID].TXST_ID].TextureID;
                        if(TexID < mesh->m_Textures.size()) t = app->getTexture(mesh->m_Textures[TexID]);
                    }
                }

                SubMesh* sm = mesh->GetSubMesh(i);
                vector<texture*> sm_textures;
                for(size_t ch = 0;ch < sm->GetChunkCount();ch++) {
                    texture* t = 0;
                    if(ch < mesh->m_MTRLs.size())
                    {
                        if(mesh->m_MTRLs[ch].Unk4 >= 0 && mesh->m_MTRLs[ch].Unk4 < mesh->m_TSNRs.size())
                        {
                            int TexID = mesh->m_TXSTs[mesh->m_TSNRs[mesh->m_MTRLs[ch].Unk4].TXST_ID].TextureID;
                            if(TexID < mesh->m_Textures.size()) t = app->getTexture(mesh->m_Textures[TexID]);
                        }
                    }
                    if(t) {
                        sm_textures.push_back(t);
                    } else {
                        //to do: some kind of default texture?
                    }
                }

                meshes.push_back(new actor_mesh(sm, t, sm_textures));
                meshes[i]->app = app;
                meshes[i]->parent = this;
                meshes[i]->submesh_id = i;
            }

            if(meshes.size() > 0) {
                m_min = meshes[0]->getMinBound();
                m_max = meshes[0]->getMaxBound();
                for(size_t i = 1; i < meshes.size();i++) {
                    vec3 pos = meshes[i]->getMinBound();
                    if(pos.x < m_min.x) m_min.x = pos.x;
                    if(pos.x > m_max.x) m_max.x = pos.x;
                    if(pos.y < m_min.y) m_min.y = pos.y;
                    if(pos.y > m_max.y) m_max.y = pos.y;
                    if(pos.z < m_min.z) m_min.z = pos.z;
                    if(pos.z > m_max.z) m_max.z = pos.z;

                    pos = meshes[i]->getMaxBound();
                    if(pos.x < m_min.x) m_min.x = pos.x;
                    if(pos.x > m_max.x) m_max.x = pos.x;
                    if(pos.y < m_min.y) m_min.y = pos.y;
                    if(pos.y > m_max.y) m_max.y = pos.y;
                    if(pos.z < m_min.z) m_min.z = pos.z;
                    if(pos.z > m_max.z) m_max.z = pos.z;
                }
            }

            glm::vec3 position;
            if (actorTraits)
            {
                ActorVec3 pos = actorTraits->Position;
                position = glm::vec3(pos.x, pos.y, pos.z);
            }
            m_AABB.min = position + m_min;
            m_AABB.max = position + m_max;
        }
        
        if(def) {
            actor_id = def->ID;
        }
    }

    actor::~actor() {
        for(size_t i = 0;i < meshes.size();i++) {
            delete meshes[i];
        }
        meshes.clear();
    }

    void actor::render(t4editor::shader *s, const mat4& view, const mat4& viewproj, bool preview, mat4* modelOverride) {
        vec3 position, rotation;
        vec3 scale = vec3(1.0f, 1.0f, 1.0f);

        if (actorTraits) {
            ActorVec3 pos = actorTraits->Position;
            ActorVec3 rot = actorTraits->Rotation;
            ActorVec3 scl = actorTraits->Scale;
            position = vec3(pos.x, pos.y, pos.z);

            rotation = vec3(rot.x, rot.y, rot.z);
            scale = vec3(scl.x, scl.y, scl.z);
        }

        if (position != m_lastFramePos || scale != m_lastFrameScale || rotation != m_lastFrameRot) {
            mat4 T = translate(position);
            mat4 R = eulerAngleXYZ(radians(rotation.x), radians(rotation.y), radians(rotation.z));
            mat4 S = glm::scale(scale);

            m_transform = T * R * S;
            m_AABB.transform(m_transform, m_min, m_max);
            m_lastFramePos = position;
            m_lastFrameScale = scale;
            m_lastFrameRot = rotation;
        }

        s->uniform("model", m_transform);

		mat4 model = mat4(1);
		if(modelOverride) model = *modelOverride;
		else {
			mat4 T = translate(position);
			mat4 R = eulerAngleXYZ(radians(rotation.x),radians(rotation.y),radians(rotation.z));
			mat4 S = glm::scale(scale);
			model = T * R * S;
			
			if(m_app->getSelectedActor().actorId == editor_id && actorTraits) {
				mat4 v = view;
				mat4 p = m_app->GetCamera()->GetProjection();
				mat4 m = model;
				ImGuizmo::MODE mode = m_app->get_transform_operation() == ImGuizmo::SCALE ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
				ImGuizmo::Manipulate(&v[0][0], &p[0][0], m_app->get_transform_operation(), mode, &m[0][0]);

				vec3 pos, rot, scl, skw;
				vec4 prs;
				quat qrot;
				decompose(m, scl, qrot, pos, skw, prs);
				rot = degrees(eulerAngles(qrot));
					
				actorTraits->Position.x = pos.x;
				actorTraits->Position.y = pos.y;
				actorTraits->Position.z = pos.z;
				actorTraits->Scale.x = scl.x;
				actorTraits->Scale.y = scl.y;
				actorTraits->Scale.z = scl.z;
				actorTraits->Rotation.x = -rot.x;
				actorTraits->Rotation.y = -rot.y;
				actorTraits->Rotation.z = -rot.z;
			}
		}

        s->uniform("model", model);
        s->uniform("view", view);
        s->uniform("mvp", viewproj * m_transform);
        if(!preview) s->uniform("actor_id", int_to_vec3(editor_id));
        
        if(m_app->getSelectedActor().actorId == editor_id) {
            s->uniform("actor_selected", 1.0f);
        } else s->uniform("actor_selected", 0.0f);
        
        for(size_t i = 0;i < meshes.size();i++) {
            s->uniform("actor_submesh_id", int_to_vec3(i));
            meshes[i]->render(s);
        }
    }
}
