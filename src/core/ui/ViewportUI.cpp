#include "ViewportUI.hpp"

#include "engine/InputHandler.hpp"
#include "engine/Errorlog.hpp"
#include "engine/AppTimer.hpp"
#include "object/ObjCache.hpp"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "platform/Platform.hpp"


bool ViewportUI::initialized = false;
bool ViewportUI::initPos = true;
GLuint ViewportUI::fbo = 0;
GLuint ViewportUI::rbo = 0;
GLuint ViewportUI::viewportTex = 0;
ImVec2 ViewportUI::dimensions = ImVec2(0, 0);
ImVec2 ViewportUI::prevDimensions = ImVec2(0, 0);
ImVec2 ViewportUI::pos = ImVec2(0, 0);
std::unique_ptr<Camera> ViewportUI::camPtr = nullptr;
float ViewportUI::targetWidth = 0.0f;
float ViewportUI::targetHeight = 0.0f;
ImVec2 ViewportUI::windowPos = ImVec2(0, 0);

#define TARGET_WIDTH 0.4f
#define TARGET_HEIGHT 1.0f
#define START_X 0;
#define START_Y 0;

bool ViewportUI::initialize() {
    if (ViewportUI::initialized) {
        return false;
    }
    dimensions = ImVec2(Platform::getWindow().width / 2, Platform::getWindow().height / 2);
    pos = ImVec2(Platform::getWindow().width / 2 - Platform::getWindow().width * 0.25f, Platform::getWindow().height / 2 - Platform::getWindow().height * 0.25f);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &viewportTex);
    glBindTexture(GL_TEXTURE_2D, viewportTex);
    glTexImage2D(
        GL_TEXTURE_2D, 
        0,
        GL_RGBA,
        dimensions.x,
        dimensions.y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, 
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        viewportTex,
        0
    );

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        dimensions.x,
        dimensions.y
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        rbo
    );

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);

    ViewportUI::camPtr = std::make_unique<Camera>();

    ViewportUI::targetWidth = TARGET_WIDTH;
    ViewportUI::targetHeight = TARGET_HEIGHT;
    ViewportUI::windowPos.x = START_X;
    ViewportUI::windowPos.y = START_Y;

    ViewportUI::initialized = true;
    return true;
}

void ViewportUI::render() {
    if (!initialized) {
        return;
    }
    ViewportUI::bind();

    glClearColor(0.4f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), ViewportUI::getAspect(), 0.1f, 100.0f);
    glm::mat4 view = camPtr->GetViewMatrix();

    ObjCache::renderAll(perspective, view);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    ViewportUI::draw();
}

Camera* ViewportUI::getCamera() {
    if (!initialized) return nullptr;
    return camPtr.get();
}


ViewportUI::~ViewportUI() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &viewportTex);
    fbo = 0;
    rbo = 0;
    viewportTex = 0;
}


void ViewportUI::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, dimensions.x, dimensions.y);
}


void ViewportUI::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOWSIZE.width, WINDOWSIZE.height);
}


void ViewportUI::draw() {
    // if (initPos) {
    //     ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);
    //     ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    //     prevDimensions = dimensions;
    //     initPos = false;
    // }
    
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); //removes border
    // ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);

    float menuBarHeight = ImGui::GetFrameHeight();

    int displayWidth = ImGui::GetIO().DisplaySize.x;
    int displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;

    float width = (float)displayWidth * ViewportUI::targetWidth;
    float height = (float)displayHeight * ViewportUI::targetHeight;

    float editorOffsetX = (float)displayWidth * ViewportUI::targetWidth; // viewport and editor have same width

    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(windowPos.x + editorOffsetX, windowPos.y + menuBarHeight), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Viewport", nullptr, flags);

    dimensions = ImGui::GetWindowSize();
    pos = ImGui::GetWindowPos();
    reformat(); // relative to the imgui window, not the application

    ImVec2 vpSize = ImGui::GetContentRegionAvail();
    ImGui::Image(
        (void*)(intptr_t)viewportTex,
        vpSize,
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    // FPS overlay
    std::string fps = "FPS: " + std::to_string(AppTimer::getFPS());
    ImGui::GetWindowDrawList()->AddText(
        ImVec2(pos.x + 20, pos.y + 40),
        IM_COL32(255, 255, 255, 255),
        fps.c_str()
    );

    ImGui::End();
    // ImGui::PopStyleVar();
}


void ViewportUI::reformat() {
    if (dimensions.x == prevDimensions.x && dimensions.y == prevDimensions.y) return;
    //if (MOUSEBUTTON[GLFW_MOUSE_BUTTON_1].isDown) return; //wait until window is resized 

    glBindTexture(GL_TEXTURE_2D, viewportTex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        dimensions.x,
        dimensions.y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER, 
        GL_DEPTH24_STENCIL8,
        dimensions.x,
        dimensions.y
    );

    prevDimensions = dimensions;
}


float ViewportUI::getAspect() {
    return (float)dimensions.x / (float)dimensions.y;
}
