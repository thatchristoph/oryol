//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

using namespace Oryol;

class ClearApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    glm::vec4 clearColor;
};
OryolMain(ClearApp);

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnRunning() {

    // render one frame
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::All, this->clearColor);
    Gfx::CommitFrame();
    
    this->clearColor += glm::vec4(0.01, 0.005, 0.0025f, 0.0);
    this->clearColor = glm::mod(this->clearColor, glm::vec4(1.0f));
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(400, 300, "Oryol Clear Sample"));
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
ClearApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
