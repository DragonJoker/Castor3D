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
		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			explicit Plugin( SubmeshComponentRegister const & submeshComponents )
				: SubmeshComponentPlugin{ submeshComponents }
			{
			}

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
		 *\return		The skeleton.
		 *\~french
		 *\return		Le squelette.
		 */
		C3D_API SkeletonRPtr getSkeleton()const;
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( PipelineFlags const & flags
			, MaterialObs material
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
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;
		/**
		 *\~english
		 *\brief		adds bone datas.
		 *\param[in]	boneData	The bone datas.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	boneData	Les données de bones.
		 */
		C3D_API void addDatas( std::vector< VertexBoneData > const & boneData );
		/**
		 *\~english
		 *\brief		adds bone datas.
		 *\param[in]	boneData	The bone datas.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	boneData	Les données de bones.
		 */
		template< size_t Count >
		void addDatas( std::array< VertexBoneData, Count > const & boneData )
		{
			addDatas( boneData.data(), boneData.data() + boneData.size() );
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getSubmeshFlags
		 */
		SubmeshComponentFlag getSubmeshFlags()const noexcept override
		{
			return makeSubmeshComponentFlag( hasData() ? getId() : 0u );
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

	public:
		C3D_API static castor::String const TypeName;

	private:
		std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > m_bonesLayouts;
		VertexBoneDataArray m_bones;

		friend class BinaryWriter< SkinComponent >;
		friend class BinaryParser< SkinComponent >;
	};
}

#endif
