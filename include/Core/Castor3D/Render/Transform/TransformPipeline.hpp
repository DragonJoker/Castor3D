/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TransformPipeline_H___
#define ___C3D_TransformPipeline_H___

#include "TransformModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

namespace castor3d
{
	struct TransformPipeline
	{
		C3D_API explicit TransformPipeline( uint32_t index );

		C3D_API std::string getName( Engine const & engine )const;
		C3D_API static std::string getName( Engine const & engine
			, SubmeshComponentCombineID combine
			, MorphFlags const & morphFlags
			, bool meshletsBounds
			, bool hasMorphingWeights );
		C3D_API static uint32_t getIndex( SubmeshComponentCombineID combine
			, MorphFlags const & morphFlags
			, ProgramFlags const & programFlags
			, bool morphingWeights );

		SubmeshComponentCombineID combineID;
		MorphFlags morphFlags;
		bool meshletsBounds;
		bool hasMorphingWeights;
		uint32_t index;

		ShaderModule shader;
		ashes::DescriptorSetLayoutPtr descriptorSetLayout;
		ashes::PipelineLayoutPtr pipelineLayout;
		ashes::ComputePipelinePtr pipeline;
		ashes::DescriptorSetPoolPtr descriptorSetPool;
	};

	struct BoundsTransformPipeline
	{
		C3D_API explicit BoundsTransformPipeline( bool normals );

		C3D_API std::string getName()const;
		C3D_API static std::string getName( bool normals );

		bool normals;

		ShaderModule shader;
		ashes::DescriptorSetLayoutPtr descriptorSetLayout;
		ashes::PipelineLayoutPtr pipelineLayout;
		ashes::ComputePipelinePtr pipeline;
		ashes::DescriptorSetPoolPtr descriptorSetPool;
	};
}

#endif
