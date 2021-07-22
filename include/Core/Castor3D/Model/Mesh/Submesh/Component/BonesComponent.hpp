/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BonesComponent_H___
#define ___C3D_BonesComponent_H___

#include "SubmeshComponent.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

namespace castor3d
{
	class BonesComponent
		: public SubmeshComponent
	{
	private:
		CU_DeclareList( castor::ByteArray, BytePtr );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		C3D_API explicit BonesComponent( Submesh & submesh );
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
		C3D_API void addBoneDatas( VertexBoneData const * const begin
			, VertexBoneData const * const end );
		/**
		 *\~english
		 *\return		The skeleton.
		 *\~french
		 *\return		Le squelette.
		 */
		C3D_API SkeletonSPtr getSkeleton()const;
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( ShaderFlags const & flags
			, MaterialSPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, uint32_t instanceMult
			, TextureFlagsArray const & mask )override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentSPtr clone( Submesh & submesh )const override;
		/**
		 *\~english
		 *\brief		adds bone datas.
		 *\param[in]	boneData	The bone datas.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	boneData	Les données de bones.
		 */
		C3D_API void addBoneDatas( std::vector< VertexBoneData > const & boneData );
		/**
		 *\~english
		 *\brief		adds bone datas.
		 *\param[in]	boneData	The bone datas.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	boneData	Les données de bones.
		 */
		template< size_t Count >
		void addBoneDatas( std::array< VertexBoneData, Count > const & boneData )
		{
			addBoneDatas( boneData.data(), boneData.data() + boneData.size() );
		}
		/**
		 *\~english
		 *\return		\p true if the submesh has bone data.
		 *\~french
		 *\return		\p true si le sous-maillage a des données d'os.
		 */
		bool hasBoneData()const
		{
			return !m_bones.empty();
		}
		/**
		 *\~english
		 *\return		The bone data.
		 *\~french
		 *\return		Les données d'os.
		 */
		VertexBoneDataArray const & getBonesData()const
		{
			return m_bones;
		}
		/**
		 *\~english
		 *\return		The bones VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer des bones.
		 */
		ashes::VertexBuffer< VertexBoneData > const & getBonesBuffer()const
		{
			return *m_bonesBuffer;
		}
		/**
		 *\~english
		 *\return		The bones VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer des bones.
		 */
		ashes::VertexBuffer< VertexBoneData > & getBonesBuffer()
		{
			return *m_bonesBuffer;
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		ProgramFlags getProgramFlags( MaterialSPtr material )const override
		{
			return hasBoneData()
				? ProgramFlag::eSkinning
				: ProgramFlag( 0 );
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup()override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr BindingPoint = 3u;

	private:
		ashes::VertexBufferPtr< VertexBoneData > m_bonesBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_bonesLayout;
		VertexBoneDataArray m_bones;

		friend class BinaryWriter< BonesComponent >;
		friend class BinaryParser< BonesComponent >;
	};
}

#endif
