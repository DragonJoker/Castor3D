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
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, castor3d::PassBuffer const & passBuffer
			, crg::ImageViewId const & depthObj
			, crg::ImageViewId const & nmlOcc
			, ashes::Buffer< int32_t > const & depthRange
			, bool const * enabled );
		~DepthNormalEdgeDetection();

		void accept( castor3d::ConfigurationVisitorBase & visitor );

		crg::ImageViewId const & getResult()const
		{
			return m_result.sampledViewId;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	protected:
		castor3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		VkExtent3D m_extent;
		castor3d::Texture m_result;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass & m_pass;
	};
}

#endif
