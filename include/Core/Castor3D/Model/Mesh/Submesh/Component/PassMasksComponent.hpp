/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassMasksComponent_H___
#define ___C3D_PassMasksComponent_H___

#include "SubmeshComponent.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

namespace castor3d
{
	class PassMasksComponent
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
		C3D_API explicit PassMasksComponent( Submesh & submesh );
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( PipelineFlags const & flags
			, MaterialObs material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, uint32_t & currentBinding
			, uint32_t & currentLocation )override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;
		/**
		 *\~english
		 *\brief		Adds bone datas.
		 *\param[in]	begin	The bones data begin.
		 *\param[in]	end		The bones data end.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	begin	Le début des données de bones.
		 *\param[in]	end		La fin des données de bones.
		 */
		C3D_API void addDatas( PassMasks const * const begin
			, PassMasks const * const end );
		/**
		 *\~english
		 *\brief		Adds masks datas.
		 *\param[in]	data	The masks.
		 *\~french
		 *\brief		Ajoute des données de masques.
		 *\param[in]	data	Les données de masques.
		 */
		C3D_API void addDatas( std::vector< PassMasks > const & data );
		/**
		 *\~english
		 *\brief		Adds masks datas.
		 *\param[in]	data	The masks datas.
		 *\~french
		 *\brief		Ajoute des données de masques.
		 *\param[in]	data	Les données de masques.
		 */
		template< size_t Count >
		void addDatas( std::array< PassMasks, Count > const & data )
		{
			addDatas( data.data(), data.data() + data.size() );
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getSubmeshFlags
		 */
		SubmeshFlags getSubmeshFlags( Pass const * pass )const override
		{
			return hasData()
				? SubmeshFlag::ePassMasks
				: SubmeshFlag( 0 );
		}

		bool hasData()const
		{
			return !m_data.empty();
		}

		void setData( std::vector< PassMasks > data )
		{
			m_data = std::move( data );
		}

		std::vector< PassMasks > & getData()
		{
			return m_data;
		}

		std::vector< PassMasks > const & getData()const
		{
			return m_data;
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr Id = getIndex( SubmeshFlag::ePassMasks ) - 1u;

	private:
		std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
		std::vector< PassMasks > m_data;

		friend class BinaryWriter< PassMasksComponent >;
		friend class BinaryParser< PassMasksComponent >;
	};
}

#endif
