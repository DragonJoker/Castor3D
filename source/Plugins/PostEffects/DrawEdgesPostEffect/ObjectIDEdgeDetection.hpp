/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_ObjectIDEdgeDetection_H___
#define ___C3DDE_ObjectIDEdgeDetection_H___

#include <Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderAST/Shader.hpp>

namespace draw_edges
{
	class ObjectIDEdgeDetection
	{
	public:
		ObjectIDEdgeDetection( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, c3d::PassBuffer const & passBuffer
			, crg::ImageViewId const & depthObj
			, bool const * enabled );
		~ObjectIDEdgeDetection();

		void accept( c3d::ConfigurationVisitorBase & visitor );

		crg::ImageViewId const & getResult()const
		{
			return m_result.sampledViewId;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	private:
		c3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		c3d::Extent3D m_extent;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::Texture m_result;
		crg::FramePass & m_pass;
	};
}

#endif
