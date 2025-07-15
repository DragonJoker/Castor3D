/*
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

namespace c3d
{
	class MeshletComponent
		: public SubmeshComponent
	{
	public:
		struct ComponentData
			: public SubmeshComponentData
		{
			using SubmeshComponentData::SubmeshComponentData;
			/**
			 *\copydoc		SubmeshComponentData::gather
			 */
			void gather( PipelineFlags const & flags
				, Pass const & pass
				, ObjectBufferOffset const & bufferOffsets
				, ashes::BufferCRefArray & buffers
				, Vector< uint64_t > & offsets
				, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
				, uint32_t & currentBinding
				, uint32_t & currentLocation )override
			{
			}
			/**
			 *\copydoc		SubmeshComponentData::copy
			 */
			void copy( SubmeshComponentDataRPtr data )const override;

			C3D_API void createDescriptorSet( Geometry const & geometry
				, Pass const & pass );
			C3D_API ashes::DescriptorSet const & getDescriptorSet( Geometry const & geometry
				, Pass const & pass )const;
			C3D_API void instantiate( Geometry const & geometry
				, Pass const & pass );
			C3D_API GpuBufferOffsetT< MeshletCullData > const & getFinalCullBuffer( Geometry const & geometry
				, Pass const & pass )const;

			ashes::DescriptorSetLayout const & getDescriptorLayout()const
			{
				return *m_descriptorLayout;
			}

			Vector< Meshlet > const & getMeshletsData()const
			{
				return m_meshlets;
			}

			Vector< Meshlet > & getMeshletsData()
			{
				needsUpdate();
				return m_meshlets;
			}

			Vector< MeshletCullData > & getCullData()
			{
				needsUpdate();
				return m_cull;
			}

			bool hasCullData()const noexcept
			{
				return !m_cull.empty();
			}

			uint32_t getMeshletsCount()const
			{
				return uint32_t( m_meshlets.size() );
			}

			GpuBufferOffsetT< MeshletCullData > const & getSourceCullBuffer()const
			{
				return m_sourceCullBuffer;
			}

		private:
			bool doInitialise( RenderDevice const & device )override;
			void doCleanup( RenderDevice const & device )override;
			void doUpload( UploadData & uploader )override;
			void doCreateDescriptorLayout( RenderDevice const & device );

		private:
			GpuBufferOffsetT< MeshletCullData > m_sourceCullBuffer;
			HashMap< size_t, GpuBufferOffsetT< MeshletCullData > > m_finalCullBuffers;
			Vector< Meshlet > m_meshlets;
			Vector< MeshletCullData > m_cull;
			ashes::DescriptorSetLayoutPtr m_descriptorLayout;
			ashes::DescriptorSetPoolPtr m_descriptorPool;
			HashMap< size_t, ashes::DescriptorSetPtr > m_descriptorSets;
		};

		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			using SubmeshComponentPlugin::SubmeshComponentPlugin;

			SubmeshComponentUPtr createComponent( Submesh & submesh )const override
			{
				return makeUniqueDerived< SubmeshComponent, MeshletComponent >( submesh );
			}

			SubmeshComponentFlag getMeshletFlag()const noexcept override
			{
				return getComponentFlags();
			}
		};

		static SubmeshComponentPluginUPtr createPlugin( SubmeshComponentRegister const & submeshComponents )
		{
			return makeUniqueDerived< SubmeshComponentPlugin, Plugin >( submeshComponents );
		}
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
		 *\copydoc		SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;
		/**
		 *\copydoc		SubmeshComponent::getProgramFlags
		 */
		C3D_API ProgramFlags getProgramFlags( Pass const & pass )const noexcept override;

		ComponentData & getData()const noexcept
		{
			return *getDataT< ComponentData >();
		}

	public:
		C3D_API static String const TypeName;
	};
}

#endif
