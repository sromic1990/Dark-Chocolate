// src/Renderer/RenderCommand.cpp
#include "renderer/RenderCommand.h"

namespace DC
{
	// Static member must be defined in EXACTLY ONE .cpp file.
	IRenderBackend* RenderCommand::s_Backend = nullptr;
}
