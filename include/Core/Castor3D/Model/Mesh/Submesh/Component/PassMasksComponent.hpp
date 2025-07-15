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

namespace c3d
{
	class PassMasksComponent
		: public SubmeshComponent
	{
	public:
		struct SurfaceShader
			: public shader::SubmeshVertexSurfaceShader
		{
			void fillSurfaceType( sdw::type::Struct & type
				, uint32_t * index )const override;
		};

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
				, uint32_t & currentLocation )override;
			/**
			 *\copydoc		SubmeshComponentData::copy
			 */
			void copy( SubmeshComponentDataRPtr data )const override;
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
			C3D_API void addDatas( Vector< PassMasks > const & data );
			/**
			 *\~english
			 *\brief		Adds masks datas.
			 *\param[in]	data	The masks datas.
			 *\~french
			 *\brief		Ajoute des données de masques.
			 *\param[in]	data	Les données de masques.
			 */
			template< size_t Count >
			void addDatas( Array< PassMasks, Count > const & data )
			{
				addDatas( data.data(), data.data() + data.size() );
			}

			bool hasData()const
			{
				return !m_data.empty();
			}

			void setData( Vector< PassMasks > data )
			{
				m_data = c3d::move( data );
			}

			Vector< PassMasks > & getData()
			{
				return m_data;
			}

			Vector< PassMasks > const & getData()const
			{
				return m_data;
			}

		private:
			bool doInitialise( RenderDevice const & device )override;
			void doCleanup( RenderDevice const & device )override;
			void doUpload( UploadData & uploader )override;

		private:
			HashMap< size_t, ashes::PipelineVertexInputStateCreateInfo > m_layouts;
			Vector< PassMasks > m_data;
			Vector< Point4ui > m_up;
		};

		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			using SubmeshComponentPlugin::SubmeshComponentPlugin;

			SubmeshComponentUPtr createComponent( Submesh & submesh )const override
			{
				return makeUniqueDerived< SubmeshComponent, PassMasksComponent >( submesh );
			}

			SubmeshComponentFlag getPassMaskFlag()const noexcept override
			{
				return getComponentFlags();
			}

			shader::SubmeshVertexSurfaceShaderPtr createVertexSurfaceShader()const override
			{
				return makeRawUnique< SurfaceShader >();
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
		C3D_API explicit PassMasksComponent( Submesh & submesh );
		/**
		 *\copydoc		SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;
		/**
		 *\copydoc		SubmeshComponent::getSubmeshFlags
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
		C3D_API static String const TypeName;

	private:
		friend class BinaryWriter< PassMasksComponent >;
		friend class BinaryParser< PassMasksComponent >;
	};
}

#endif
