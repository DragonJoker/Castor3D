/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphComponent_H___
#define ___C3D_MorphComponent_H___

#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <unordered_map>

namespace castor3d
{
	class MorphComponent
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
		C3D_API explicit MorphComponent( Submesh & submesh
			, MorphFlags flags );
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
		/**
		 *\~english
		 *\brief		Adds a morph target.
		 *\param[in]	data	The morph target.
		 *\~french
		 *\brief		Ajoute une cible de morph.
		 *\param[in]	data	La cible de morph.
		 */
		C3D_API void addMorphTarget( SubmeshAnimationBuffer data );
		/**
		 *\~english
		 *\return		The morphing flags.
		 *\~french
		 *\return		Les indicateurs de morphing.
		 */
		MorphFlags getMorphFlags()const
		{
			return m_flags;
		}
		/**
		 *\~english
		 *\return		The morph targets buffer.
		 *\~french
		 *\return		Le buffer de cibles de morph.
		 */
		GpuBufferOffsetT< castor::Point4f > const & getMorphTargets()const
		{
			return m_buffer;
		}
		/**
		 *\~english
		 *\return		The morph targets data.
		 *\~french
		 *\return		Les données de cibles de morph.
		 */
		std::vector< SubmeshAnimationBuffer > const & getMorphTargetsBuffers()const
		{
			return m_targets;
		}
		/**
		 *\~english
		 *\return		The morph targets data.
		 *\~french
		 *\return		Les données de cibles de morph.
		 */
		std::vector< SubmeshAnimationBuffer > & getMorphTargetsBuffers()
		{
			return m_targets;
		}
		/**
		 *\~english
		 *\return		The number of morph targets.
		 *\~french
		 *\return		Le nombre de cibles de morph.
		 */
		uint32_t getMorphTargetsCount()const
		{
			return uint32_t( m_targets.size() );
		}

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;

	private:
		MorphFlags m_flags{};
		uint32_t m_pointsCount{};
		uint32_t m_targetDataCount{};
		GpuBufferOffsetT< castor::Point4f > m_buffer;
		std::vector< SubmeshAnimationBuffer > m_targets;

		friend class BinaryWriter< MorphComponent >;
		friend class BinaryParser< MorphComponent >;
	};
}

#endif
