/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkinComponent_H___
#define ___C3D_SkinComponent_H___

#include "SubmeshComponent.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

namespace castor3d
{
	class SkinComponent
		: public SubmeshComponent
	{
	public:
		struct ComponentData
			: public SubmeshComponentData
		{
			using SubmeshComponentData::SubmeshComponentData;
			/**
			 *\copydoc		castor3d::SubmeshComponentData::gather
			 */
			void gather( PipelineFlags const & flags
				, Pass const & pass
				, ObjectBufferOffset const & bufferOffsets
				, ashes::BufferCRefArray & buffers
				, castor::Vector< uint64_t > & offsets
				, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
				, uint32_t & currentBinding
				, uint32_t & currentLocation )override
			{
			}
			/**
			 *\copydoc		castor3d::SubmeshComponentData::copy
			 */
			void copy( SubmeshComponentDataRPtr data )const override;
			/**
			 *\~english
			 *\brief		adds bone datas.
			 *\param[in]	begin	The bones data begin.
			 *\param[in]	end		The bones data end.
			 *\~french
			 *\brief		Ajoute des données de bones.
			 *\param[in]	begin	Le début des données de bones.
			 *\param[in]	end		La fin des données de bones.
			 */
			C3D_API void addDatas( VertexBoneData const * const begin
				, VertexBoneData const * const end );
			/**
			 *\~english
			 *\brief		adds bone datas.
			 *\param[in]	boneData	The bone datas.
			 *\~french
			 *\brief		Ajoute des données de bones.
			 *\param[in]	boneData	Les données de bones.
			 */
			C3D_API void addDatas( castor::Vector< VertexBoneData > const & boneData );
			/**
			 *\~english
			 *\brief		adds bone datas.
			 *\param[in]	boneData	The bone datas.
			 *\~french
			 *\brief		Ajoute des données de bones.
			 *\param[in]	boneData	Les données de bones.
			 */
			template< size_t Count >
			void addDatas( castor::Array< VertexBoneData, Count > const & boneData )
			{
				addDatas( boneData.data(), boneData.data() + boneData.size() );
			}

			bool hasData()const
			{
				return !m_bones.empty();
			}

			VertexBoneDataArray & getData()
			{
				return m_bones;
			}

			VertexBoneDataArray const & getData()const
			{
				return m_bones;
			}

		private:
			bool doInitialise( RenderDevice const & device )override;
			void doCleanup( RenderDevice const & device )override;
			void doUpload( UploadData & uploader )override;

		private:
			castor::UnorderedMap< size_t, ashes::PipelineVertexInputStateCreateInfo > m_bonesLayouts;
			VertexBoneDataArray m_bones;

		private:
			friend class BinaryWriter< SkinComponent >;
			friend class BinaryParser< SkinComponent >;
		};

		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			using SubmeshComponentPlugin::SubmeshComponentPlugin;

			SubmeshComponentUPtr createComponent( Submesh & submesh )const override
			{
				return castor::makeUniqueDerived< SubmeshComponent, SkinComponent >( submesh );
			}

			SubmeshComponentFlag getSkinFlag()const noexcept override
			{
				return getComponentFlags();
			}
		};

		static SubmeshComponentPluginUPtr createPlugin( SubmeshComponentRegister const & submeshComponents )
		{
			return castor::makeUniqueDerived< SubmeshComponentPlugin, Plugin >( submeshComponents );
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		C3D_API explicit SkinComponent( Submesh & submesh );
		/**
		 *\~english
		 *\return		The skeleton.
		 *\~french
		 *\return		Le squelette.
		 */
		C3D_API SkeletonRPtr getSkeleton()const;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::getSubmeshFlags
		 */
		SubmeshComponentFlag getSubmeshFlags()const noexcept override
		{
			return makeSubmeshComponentFlag( getDataT< ComponentData >()->hasData() ? getId() : 0u );
		}

		ComponentData & getData()const noexcept
		{
			return *getDataT< ComponentData >();
		}

	public:
		C3D_API static castor::String const TypeName;
	};
}

#endif
