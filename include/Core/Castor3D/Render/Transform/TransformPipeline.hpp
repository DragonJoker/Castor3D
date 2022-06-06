/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TransformPipeline_H___
#define ___C3D_TransformPipeline_H___

#include "TransformModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

namespace castor3d
{
	struct TransformPipeline
	{
		TransformPipeline( uint32_t index );

		C3D_API std::string getName()const;
		C3D_API static std::string getName( SubmeshFlags const & submeshFlags
			, MorphFlags const & morphFlags
			, bool meshletsBounds );

		SubmeshFlags submeshFlags;
		MorphFlags morphFlags;
		bool meshletsBounds;
		uint32_t index;

		ShaderModule shader;
		ashes::DescriptorSetLayoutPtr descriptorSetLayout;
		ashes::PipelineLayoutPtr pipelineLayout;
		ashes::ComputePipelinePtr pipeline;
		ashes::DescriptorSetPoolPtr descriptorSetPool;
	};

	struct BoundsTransformPipeline
	{
		C3D_API static std::string getName();

		ShaderModule shader;
		ashes::DescriptorSetLayoutPtr descriptorSetLayout;
		ashes::PipelineLayoutPtr pipelineLayout;
		ashes::ComputePipelinePtr pipeline;
		ashes::DescriptorSetPoolPtr descriptorSetPool;
	};
}

#endif
