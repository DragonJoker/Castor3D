/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_EdgeDetection_H___
#define ___C3DSMAA_EdgeDetection_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderAST/Shader.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

namespace smaa
{
	class SmaaUbo;
	struct SmaaData;

	template< sdw::var::Flag FlagT >
	using EDVertexStructT = sdw::IOStructInstanceHelperT< FlagT
		, "SMAAED_Vertex"
		, sdw::IOVec2Field< "texcoord", 0u >
		, sdw::IOVec4ArrayField< "offset", 1u, 3u > >;

	template< sdw::var::Flag FlagT >
	struct EDVertexT
		: public EDVertexStructT< FlagT >
	{
		EDVertexT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: EDVertexStructT< FlagT >{ writer, c3d::move( expr ), enabled }
		{
		}

		auto texcoord()const{ return this->template getMember< "texcoord" >(); }
		auto offset()const{ return this->template getMember< "offset" >(); }
	};

	class EdgeDetection
	{
	public:
		EdgeDetection( crg::FramePassGroup & graph
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, SmaaConfig const & config
			, c3d::ShaderPtr shader
			, bool const * enabled
			, uint32_t const * passIndex
			, uint32_t passCount );
		~EdgeDetection();

		void accept( c3d::ConfigurationVisitorBase & visitor )const;

		static void getVertexProgram( sdw::TraditionalGraphicsWriter & writer
			, SmaaData const & smaaData );

		c3d::Texture const & getColourResult()const
		{
			return m_outColour;
		}

		c3d::Texture const & getDepthResult()const
		{
			return m_outDepth;
		}

	protected:
		c3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		SmaaConfig const & m_config;
		c3d::Extent3D m_extent;
		c3d::Texture m_outColour;
		c3d::Texture m_outDepth;
		crg::ImageViewId m_outStencilView;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass & m_pass;
	};
}

#endif
