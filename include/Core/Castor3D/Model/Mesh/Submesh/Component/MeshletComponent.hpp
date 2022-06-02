﻿/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshletComponent_H___
#define ___C3D_MeshletComponent_H___

#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>

#include <unordered_map>

namespace castor3d
{
	class MeshletComponent
		: public SubmeshComponent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		C3D_API explicit MeshletComponent( Submesh & submesh );
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( ShaderFlags const & shaderFlags
			, ProgramFlags const & programFlags
			, SubmeshFlags const & submeshFlags
			, MaterialRPtr material
			, TextureFlagsArray const & mask
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, uint32_t & currentBinding
			, uint32_t & currentLocation )override
		{
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentSPtr clone( Submesh & submesh )const override;
		C3D_API void createDescriptorSet( Geometry const & geometry );
		C3D_API ashes::DescriptorSet const & getDescriptorSet( Geometry const & geometry )const;

		ashes::DescriptorSetLayout const & getDescriptorLayout()const
		{
			return *m_descriptorLayout;
		}

		GpuBufferOffsetT< Meshlet > const & getMeshletsBuffer()const
		{
			return m_meshletBuffer;
		}

		std::vector< Meshlet > const & getMeshletsData()const
		{
			return m_data;
		}

		std::vector< Meshlet > & getMeshletsData()
		{
			return m_data;
		}

		uint32_t getMeshletsCount()const
		{
			return uint32_t( m_data.size() );
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;
		void doCreateDescriptorLayout( RenderDevice const & device );

	public:
		C3D_API static castor::String const Name;

	private:
		GpuBufferOffsetT< Meshlet > m_meshletBuffer;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		std::map< Geometry const *, ashes::DescriptorSetPtr > m_descriptorSets;
		std::vector< Meshlet > m_data;
	};
}

#endif
