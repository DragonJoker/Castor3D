#include "GlRenderTarget.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"
#include "GlFrameBuffer.hpp"
#include "GlRenderBufferAttachment.hpp"
#include "GlTextureAttachment.hpp"

#include <Scene.hpp>
#include <Logger.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlRenderTarget::GlRenderTarget( OpenGl & p_gl, GlRenderSystem * p_renderSystem, eTARGET_TYPE p_type )
	: RenderTarget( *p_renderSystem->GetOwner(), p_type )
	, Holder( p_gl )
	, m_renderSystem( p_renderSystem )
{
}

GlRenderTarget::~GlRenderTarget()
{
}

RenderBufferAttachmentSPtr GlRenderTarget::CreateAttachment( RenderBufferSPtr p_renderBuffer )
{
	return std::make_shared< GlRenderBufferAttachment >( GetOpenGl(), p_renderBuffer );
}

TextureAttachmentSPtr GlRenderTarget::CreateAttachment( DynamicTextureSPtr p_texture )
{
	return std::make_shared< GlTextureAttachment >( GetOpenGl(), p_texture );
}

FrameBufferSPtr GlRenderTarget::CreateFrameBuffer()
{
	return std::make_shared< GlFrameBuffer >( GetOpenGl(), *m_renderSystem->GetOwner() );
}
