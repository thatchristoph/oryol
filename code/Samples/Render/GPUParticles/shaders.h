#pragma once
//-----------------------------------------------------------------------------
/*  #version:1#
    machine generated, do not edit!
*/
#include "Render/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class DrawParticles {
    public:
        static const int32 ModelViewProjection = 0;
        static Render::ProgramBundleSetup CreateSetup();
    };
    class EmitParticles {
    public:
        static const int32 Time = 0;
        static Render::ProgramBundleSetup CreateSetup();
    };
    class UpdateParticles {
    public:
        static Render::ProgramBundleSetup CreateSetup();
    };
}
}
