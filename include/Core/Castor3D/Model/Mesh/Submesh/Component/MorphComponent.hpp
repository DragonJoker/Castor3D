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
		struct ComponentData
			: public SubmeshComponentData
		{
			explicit ComponentData( Submesh & submesh )
				: SubmeshComponentData{ submesh }
			{
			}
			/**
			 *\copydoc		castor3d::SubmeshComponentData::gather
			 */
			void gather( PipelineFlags const & flags
				, MaterialObs material
				, ashes::BufferCRefArray & buffers
				, std::vector< uint64_t > & offsets
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
			MorphFlags getMorphFlags()const noexcept
			{
				return m_flags;
			}
			/**
			 *\~english
			 *\return		The morph targets buffer.
			 *\~french
			 *\return		Le buffer de cibles de morph.
			 */
			GpuBufferOffsetT< castor::Point4f > const & getMorphTargets()const noexcept
			{
				return m_buffer;
			}
			/**
			 *\~english
			 *\return		The morph targets data.
			 *\~french
			 *\return		Les données de cibles de morph.
			 */
			std::vector< SubmeshAnimationBuffer > const & getMorphTargetsBuffers()const noexcept
			{
				return m_targets;
			}
			/**
			 *\~english
			 *\return		The morph targets data.
			 *\~french
			 *\return		Les données de cibles de morph.
			 */
			std::vector< SubmeshAnimationBuffer > & getMorphTargetsBuffers()noexcept
			{
				return m_targets;
			}
			/**
			 *\~english
			 *\return		The number of morph targets.
			 *\~french
			 *\return		Le nombre de cibles de morph.
			 */
			uint32_t getMorphTargetsCount()const noexcept
			{
				return uint32_t( m_targets.size() );
			}

		private:
			bool doInitialise( RenderDevice const & device )override;
			void doCleanup( RenderDevice const & device )override;
			void doUpload( UploadData & uploader )override;

		private:
			MorphFlags m_flags{};
			uint32_t m_targetDataCount{};
			GpuBufferOffsetT< castor::Point4f > m_buffer;
			std::vector< SubmeshAnimationBuffer > m_targets;

		private:
			friend class BinaryWriter< MorphComponent >;
			friend class BinaryParser< MorphComponent >;
		};

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
				return castor::makeUniqueDerived< SubmeshComponent, MorphComponent >( submesh );
			}

			SubmeshComponentFlag getMorphFlag()const noexcept override
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
		C3D_API explicit MorphComponent( Submesh & submesh );
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;

		ComponentData & getData()const noexcept
		{
			return *getDataT< ComponentData >();
		}

	public:
		C3D_API static castor::String const TypeName;
	};
}

#endif
