#pragma once
#include <gui/panel.h>
#include <event.h>

#include <imgui.h>
using namespace ImGui;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <algorithm>

//todo: make these configurable
#define MOUSE_BTN_DRAG_VIEW_HOLD_TIME 0.1f
#define MOUSE_BTN_SELECT_ACTOR_MAX_CLICK_HOLD_TIME 0.4f

namespace t4editor {
    class level_view : public ui_panel {
        public:
            level_view() {
                setName("Level View");
                setFlagsManually(ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
                m_level = 0;
                m_fov = 65.0f;
                m_camPos = vec3(0.0f, 0.0f, 0.0f);
                m_camAngles = vec2(0.0f, 0.0f);
                memset(&m_mouseBtnDown, 0, sizeof(bool) * 3);
                memset(&m_keyDown, 0, sizeof(bool) * GLFW_KEY_LAST);
                m_actorUnderCursor.actorId = -1;
                m_actorUnderCursor.actorSubmeshId = -1;
                m_actorUnderCursor.actorSubmeshChunkId = -1;
                m_selectedActor.actorId = -1;
                m_selectedActor.actorSubmeshId = -1;
                m_selectedActor.actorSubmeshChunkId = -1;
            }
            ~level_view() {
            }
        
            virtual void onAttach(ui_panel* toPanel) {
                setPosition(vec2(0.0f, 20.0f));
                vec2 sz = vec2(m_app->getWindow()->getSize().x * LEVEL_VIEW_WIDTH_FRACTION, m_app->getWindow()->getSize().y * LEVEL_VIEW_HEIGHT_FRACTION);
                setSize(sz);
                
                vec2 sizeOff = getSizeCorrection();
                vec2 fbSize = sz - sizeOff;
                m_app->getFrame()->resize(fbSize.x, fbSize.y);
                    
                m_proj = perspective(radians(m_fov), fbSize.x / fbSize.y, 1.0f, 1000.0f);
                m_app->set_view(m_view, m_proj);
                
                double cx, cy;
                glfwGetCursorPos(m_app->getWindow()->getWindow(), &cx, &cy); //heh, getWindow()->getWindow()...
                m_curCursor = vec2(cx, cy);
            }
            virtual void onEvent(event* e) {
                if(e->name == "closed") {
                    m_level = 0;
                }
                else if(e->name == "window_resize") {
                    vec2 sz = vec2(m_app->getWindow()->getSize().x * LEVEL_VIEW_WIDTH_FRACTION, m_app->getWindow()->getSize().y * LEVEL_VIEW_HEIGHT_FRACTION);
                    setSize(sz);
                    
                    vec2 sizeOff = getSizeCorrection();
                    vec2 fbSz = sz - sizeOff;
                    m_app->getFrame()->resize(fbSz.x, fbSz.y);
                    m_proj = perspective(radians(m_fov), fbSz.x / fbSz.y, 1.0f, 1000.0f);
                    m_app->set_view(m_view, m_proj);
                }
                else if(e->name == "show_level_view") {
                    open();
                }
                else if(e->name == "level_loaded") {
                    m_level = m_app->getLevel();
                }
                else if(e->name == "level_unloaded") {
                    m_level = 0;
                }
                else if(e->name == "input_event") {
                    input_event* input = (input_event*)e;
                    //printf("input: %d\n", input->type);
                    switch(input->type) {
                        case input_event::ET_MOUSE_MOVE:
                            if(m_mouseBtnDown[0] != 0.0f && glfwGetTime() - m_mouseBtnDown[0] > MOUSE_BTN_DRAG_VIEW_HOLD_TIME && cursorOverView()) { //left mouse
                                vec2 delta = input->cursorPosition - m_curCursor;
                                float fac = std::max((m_fov - 10.0f) / (170.0f - 10.0f), 0.03f);
                                m_camAngles.x -= delta.x * 0.005f * fac;
                                m_camAngles.y -= delta.y * 0.005f * fac;
                                if(m_camAngles.y < -85.0f) m_camAngles.y = -85.0f;
                                if(m_camAngles.y >  85.0f) m_camAngles.y =  85.0f;
                                if(m_camAngles.x > 360.0f) m_camAngles.x -= 360.0f;
                                if(m_camAngles.x < -360.0f) m_camAngles.x += 360.0f;
                                
                                m_view = eulerAngleXYZ(m_camAngles.y, m_camAngles.x, 0.0f) * translate(m_camPos);
                                m_app->set_view(m_view, m_proj);
                            }
                            m_curCursor = input->cursorPosition;
                            break;
                        case input_event::ET_MOUSE_LEFT_DOWN:
                            m_mouseBtnDown[0] = glfwGetTime();
                            break;
                        case input_event::ET_MOUSE_LEFT_UP: {
                            float delta = glfwGetTime() - m_mouseBtnDown[0];
                            m_mouseBtnDown[0] = 0.0f;
                            
                            if(m_level != 0 && cursorOverView()) {
                                if(delta > 0.0f && delta < MOUSE_BTN_SELECT_ACTOR_MAX_CLICK_HOLD_TIME) {
                                    bool sendSelected = false;
                                    bool sendDeselected = false;
                                    if(m_actorUnderCursor.actorId != -1) {
                                        if(m_selectedActor.actorId != -1) {
                                            sendDeselected = true;
                                        }
                                        sendSelected = true;
                                    } else {
                                        if(m_selectedActor.actorId != -1) sendDeselected = true;
                                    }
                                    
                                    if(sendDeselected) {
                                        actor* a = m_level->actors()[m_selectedActor.actorId];
                                        actor_mesh* m = m_level->actors()[m_selectedActor.actorId]->meshes[m_selectedActor.actorSubmeshId];
                                        actor_selection_event e(m_level, a, m, m_selectedActor.actorId, m_selectedActor.actorSubmeshId, m_selectedActor.actorSubmeshChunkId, true);
                                        m_app->onEvent(&e);
                                    }
                                    
                                    if(sendSelected) {
                                        actor* a = m_level->actors()[m_actorUnderCursor.actorId];
                                        actor_mesh* m = a->meshes[m_actorUnderCursor.actorSubmeshId];
                                        actor_selection_event e(m_level, a, m, m_actorUnderCursor.actorId, m_actorUnderCursor.actorSubmeshId, m_actorUnderCursor.actorSubmeshChunkId, false);
                                        m_app->onEvent(&e);
                                    }
                                    
                                    if(sendSelected) m_selectedActor = m_actorUnderCursor;
                                    else if(sendDeselected) {
                                        m_selectedActor.actorId = -1;
                                        m_selectedActor.actorSubmeshId = -1;
                                        m_selectedActor.actorSubmeshChunkId = -1;
                                    }
                                    m_app->set_picked_actor_info(m_actorUnderCursor, m_selectedActor);
                                }
                            }
                            break;
                        }
                        case input_event::ET_MOUSE_MIDDLE_DOWN:
                            m_mouseBtnDown[1] = glfwGetTime();
                            break;
                        case input_event::ET_MOUSE_MIDDLE_UP:
                            m_mouseBtnDown[1] = 0.0f;
                            break;
                        case input_event::ET_MOUSE_RIGHT_DOWN:
                            m_mouseBtnDown[2] = glfwGetTime();
                            break;
                        case input_event::ET_MOUSE_RIGHT_UP:
                            m_mouseBtnDown[2] = 0.0f;
                            if(input->key == GLFW_KEY_ESCAPE && cursorOverView()) {
                                actor* a = m_level->actors()[m_selectedActor.actorId];
                                actor_mesh* m = m_level->actors()[m_selectedActor.actorId]->meshes[m_selectedActor.actorSubmeshId];
                                actor_selection_event e(m_level, a, m, m_selectedActor.actorId, m_selectedActor.actorSubmeshId, m_selectedActor.actorSubmeshChunkId, true);
                                m_app->onEvent(&e);
                                
                                m_selectedActor.actorId = -1;
                                m_selectedActor.actorSubmeshId = -1;
                                m_selectedActor.actorSubmeshChunkId = -1;
                                m_app->set_picked_actor_info(m_actorUnderCursor, m_selectedActor);
                            }
                            break;
                        case input_event::ET_KEY_DOWN:
                            m_keyDown[input->key] = glfwGetTime();
                            break;
                        case input_event::ET_KEY_UP:
                            m_keyDown[input->key] = 0.0f;
                            break;
                        case input_event::ET_SCROLL: {
                            if(cursorOverView()) {
                                float fac = std::max((m_fov - 1.0f) / (110.0f - 1.0f), 0.01f);
                                m_fov += input->scrollDelta * fac;
                                if(m_fov > 110.0f) m_fov = 110.0f;
                                if(m_fov < 1.0f) m_fov = 1.0f;
                                //printf("fov: %f | scroll fac: %f\n", m_fov, fac);
                                vec2 dims = getActualSize();
                                m_proj = perspective(radians(m_fov), dims.x / dims.y, 1.0f, 10000.0f);
                                m_app->set_view(m_view, m_proj);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        
            virtual void renderContent() {
                min_bound = ImGui::GetWindowContentRegionMin();
                max_bound = ImGui::GetWindowContentRegionMax();
                vec2 wPos = getPosition();
                min_bound.x += wPos.x;
                min_bound.y += wPos.y;
                max_bound.x += wPos.x;
                max_bound.y += wPos.y;

                if(m_level) {
                    if(cursorOverView()) {
                        float moveSpeed = 1.0f; // units per frame
                        vec3 moveDir = vec3(0.0f, 0.0f, 0.0f);
                        if(m_keyDown[GLFW_KEY_W] > 0.0f) moveDir.z =  1;
                        if(m_keyDown[GLFW_KEY_S] > 0.0f) moveDir.z = -1;
                        if(m_keyDown[GLFW_KEY_A] > 0.0f) moveDir.x =  1;
                        if(m_keyDown[GLFW_KEY_D] > 0.0f) moveDir.x = -1;
                        
                        if(!(moveDir.x == 0 && moveDir.y == 0 && moveDir.z == 0)) {
                            moveDir = normalize(moveDir) * moveSpeed;
                            vec4 nPos = vec4(moveDir, 1.0f) * eulerAngleXYZ(m_camAngles.y, m_camAngles.x, 0.0f);
                            m_camPos.x += nPos.x;
                            m_camPos.y += nPos.y;
                            m_camPos.z += nPos.z;
                            
                            m_view = eulerAngleXYZ(m_camAngles.y, m_camAngles.x, 0.0f) * translate(m_camPos);
                            m_app->set_view(m_view, m_proj);
                        }
                        
                        picking();
                    }
                    
                    framebuffer* fb = m_app->getFrame();
                    vec2 dims = getActualSize();
                    ImGui::Image((GLuint*)fb->attachments[0]->id, ImVec2(dims.x,dims.y), ImVec2(0, 1), ImVec2(1, 0));
                } else {
                    SetCursorPos(ImVec2(getSize().x / 2,(getSize().y / 2) - 10));
                    SetWindowFontScale(1.5f);
                    Text("Select a level to edit from the file menu");
                    SetWindowFontScale(1.0f);
                }
            }
        
            bool cursorOverView() const {
                vec2 cPos = getCursorPos();
                return cPos.x > min_bound.x && cPos.x < max_bound.x && cPos.y > min_bound.y && cPos.y < max_bound.y;
            }
        
            vec2 getCursorPos() const {
                return m_curCursor - vec2(min_bound.x, min_bound.y);
            }
        
            vec2 getActualSize() const {
                return (vec2(max_bound.x, max_bound.y) - vec2(min_bound.x, min_bound.y)) - getSizeCorrection();
            }
            vec2 getSizeCorrection() const {
                return vec2(2.0f, 2.0f);
            }
        
            void picking() {
                //get actor info under cursor
                if(m_level && cursorOverView()) {
                    unsigned char asset_id[3];
                    unsigned char asset_submesh_id[3];
                    unsigned char asset_submesh_chunk_id[3];
                    
                    vec2 cPos = getCursorPos();
                    int x = cPos.x;
                    int y = ((max_bound.y - min_bound.y) - 1 - cPos.y);
                    //printf("%d, %d\n", x, y);
                    
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_app->getFrame()->fbo);
                    glReadBuffer(GL_COLOR_ATTACHMENT1);
                    glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &asset_id);
                    m_actorUnderCursor.actorId = asset_id[0] + (asset_id[1] * 256) + (asset_id[2] * 256 * 256);
                    if(m_actorUnderCursor.actorId < 0 || m_actorUnderCursor.actorId >= m_level->actors().size()) {
                        m_actorUnderCursor.actorId = -1;
                        m_actorUnderCursor.actorSubmeshId = -1;
                        m_actorUnderCursor.actorSubmeshChunkId = -1;
                    } else {
                        glReadBuffer(GL_COLOR_ATTACHMENT2);
                        glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &asset_submesh_id);
                        m_actorUnderCursor.actorSubmeshId = asset_submesh_id[0] + (asset_submesh_id[1] * 256) + (asset_submesh_id[2] * 256 * 256);
                        actor* actor = m_level->actors()[m_actorUnderCursor.actorId];
                        if(m_actorUnderCursor.actorSubmeshId < 0 || m_actorUnderCursor.actorSubmeshId >= actor->meshes.size()) {
                            m_actorUnderCursor.actorId = -1;
                            m_actorUnderCursor.actorSubmeshId = -1;
                            m_actorUnderCursor.actorSubmeshChunkId = -1;
                        } else {
                            glReadBuffer(GL_COLOR_ATTACHMENT3);
                            glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &asset_submesh_chunk_id);
                            m_actorUnderCursor.actorSubmeshChunkId = asset_submesh_chunk_id[0] + (asset_submesh_chunk_id[1] * 256) + (asset_submesh_chunk_id[2] * 256 * 256);
                            actor_mesh* mesh = actor->meshes[m_actorUnderCursor.actorSubmeshId];
                            if(m_actorUnderCursor.actorSubmeshChunkId < 0 || m_actorUnderCursor.actorSubmeshChunkId >= mesh->chunkIndices.size()) {
                                m_actorUnderCursor.actorId = -1;
                                m_actorUnderCursor.actorSubmeshId = -1;
                                m_actorUnderCursor.actorSubmeshChunkId = -1;
                            }
                        }
                    }
                } else {
                    m_actorUnderCursor.actorId = -1;
                    m_actorUnderCursor.actorSubmeshId = -1;
                    m_actorUnderCursor.actorSubmeshChunkId = -1;
                }
                
                m_app->set_picked_actor_info(m_actorUnderCursor, m_selectedActor);
            }
        
            ImVec2 min_bound;
            ImVec2 max_bound;
        
        protected:
            actorUnderCursor m_actorUnderCursor;
            actorUnderCursor m_selectedActor;
            level* m_level;
            vec2 m_curCursor;
            vec2 m_camAngles;
            vec3 m_camPos;
            mat4 m_view;
            mat4 m_proj;
            float m_fov;
            float m_mouseBtnDown[3];
            float m_keyDown[GLFW_KEY_LAST];
    };
}
