#include "MorphingUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	String const MorphingUbo::BufferMorphing = cuT( "Morphing" );
	String const MorphingUbo::Time = cuT( "c3d_time" );

	MorphingUbo::MorphingUbo( Engine & engine )
		: m_ubo{ MorphingUbo::BufferMorphing
			, *engine.getRenderSystem()
			, MorphingUbo::BindingPoint }
		, m_time{ *m_ubo.createUniform< UniformType::eFloat >( MorphingUbo::Time ) }
	{
	}

	MorphingUbo::~MorphingUbo()
	{
	}

	void MorphingUbo::update( float p_time )const
	{
		m_time.setValue( p_time );
		m_ubo.update();
		m_ubo.bindTo( MorphingUbo::BindingPoint );
	}
}
