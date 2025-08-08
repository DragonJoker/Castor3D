/*
See LICENSE file in root folder
*/
#ifndef ___C3DED_DepthNormalEdgeDetection_H___
#define ___C3DED_DepthNormalEdgeDetection_H___

#include <Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ashespp/Buffer/Buffer.hpp>

#include <ShaderAST/Shader.hpp>

namespace draw_edges
{
	class DepthNormalEdgeDetection
	{
	public:
		enum Bindings : uint32_t
		{
			eMaterials,
			eModels,
			eDepthObj,
			eNmlOcc,
			eDepthRange,
			eSpecifics,
		};

	public:
		DepthNormalEdgeDetection( crg::FramePassGroup & graph
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, c3d::PassBuffer const & passBuffer
			, c3d::Texture const & depthObj
			, c3d::Texture const & nmlOcc
			, c3d::BufferBase const & depthRange
			, bool const * enabled );
		~DepthNormalEdgeDetection();

		void accept( c3d::ConfigurationVisitorBase & visitor );

		c3d::Texture const & getResult()const
		{
			return m_result;
		}

	protected:
		c3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		c3d::Extent3D m_extent;
		c3d::Texture m_result;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
