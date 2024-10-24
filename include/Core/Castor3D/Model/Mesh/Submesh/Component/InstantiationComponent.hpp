/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantiationComponent_H___
#define ___C3D_InstantiationComponent_H___

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

namespace castor3d
{
	struct InstantiationData
	{
		ObjectIdsConfiguration objectIDs;
	};

	class InstantiationComponent
		: public SubmeshComponent
	{
	public:
		class Plugin
			: public SubmeshComponentPlugin
		{
		public:
			using SubmeshComponentPlugin::SubmeshComponentPlugin;

			SubmeshComponentUPtr createComponent( Submesh & submesh )const override
			{
				return castor::makeUniqueDerived< SubmeshComponent, InstantiationComponent >( submesh );
			}

			SubmeshComponentFlag getInstantiationFlag()const noexcept override
			{
				return getComponentFlags();
			}
		};

		static SubmeshComponentPluginUPtr createPlugin( SubmeshComponentRegister const & submeshComponents )
		{
			return castor::makeUniqueDerived< SubmeshComponentPlugin, Plugin >( submeshComponents );
		}

		struct Data
		{
			Data( uint32_t count
				, GpuBufferOffsetT< InstantiationData > buffer
				, castor::Vector< InstantiationData > data = {} )
				: count{ count }
				, buffer{ castor::move( buffer ) }
				, data{ castor::move( data ) }
			{
			}
			uint32_t count;
			GpuBufferOffsetT< InstantiationData > buffer;
			castor::Vector< InstantiationData > data;
		};
		using InstanceDataMap = castor::Map< uint32_t, Data >;

		struct ComponentData
			: public SubmeshComponentData
		{
			explicit ComponentData( Submesh & submesh
				, uint32_t threshold );
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
				, uint32_t & currentLocation )override;
			/**
			 *\copydoc		castor3d::SubmeshComponentData::copy
			 */
			void copy( SubmeshComponentDataRPtr data )const override;
			/**
			 *\~english
			 *\brief		Increments instance count.
			 *\param[in]	material	The material for which the instance count is incremented.
			 *\return		\p true if the instance buffer has changed.
			 *\~french
			 *\brief		Incrémente le compte d'instances.
			 *\param[in]	material	Le matériau pour lequel le compte est incrémenté.
			 *\return		\p true Si le buffer d'instances a changé.
			 */
			C3D_API bool ref( Material const * material );
			/**
			 *\~english
			 *\brief		Decrements instance count.
			 *\param[in]	material	The material for which the instance count is decremented.
			 *\~french
			 *\brief		Décrémente le compte d'instances.
			 *\param[in]	material	Le matériau pour lequel le compte est décrémenté.
			 */
			C3D_API void unref( Material const * material );
			/**
			 *\~english
			 *\brief		Retrieves the instances count
			 *\param[in]	pass	The material pass for which the instance count is retrieved
			 *\return		The value
			 *\~french
			 *\brief		Récupère le nombre d'instances
			 *\param[in]	pass	La passe de matériau pour lequel le compte est récupéré
			 *\return		La valeur
			 */
			C3D_API uint32_t getRefCount( Pass const & pass )const;
			/**
			 *\~english
			 *\return		The maximum instances count, amongst all materials.
			 *\~french
			 *\return		Le nombre moximum d'instances, tous matériaux confondus,
			 */
			C3D_API uint32_t getMaxRefCount()const;
			/**
			*\~english
			*name
			*	Getters.
			*\~french
			*name
			*	Accesseurs.
			*/
			/**@{*/
			C3D_API InstanceDataMap::const_iterator find( Pass const & pass )const;
			C3D_API InstanceDataMap::iterator find( Pass const & pass );

			inline uint32_t getThreshold()const
			{
				return m_threshold;
			}

			inline InstanceDataMap::const_iterator end()const
			{
				return m_instances.end();
			}

			inline InstanceDataMap::iterator end()
			{
				return m_instances.end();
			}

			inline bool isInstanced( uint32_t count )const
			{
				return count >= getThreshold();
			}
			/**@}*/

		private:
			bool doInitialise( RenderDevice const & device )override;
			void doCleanup( RenderDevice const & device )override;
			void doUpload( UploadData & uploader )override;

		private:
			InstanceDataMap m_instances;
			castor::UnorderedMap< size_t, ashes::PipelineVertexInputStateCreateInfo > m_mtxLayouts;
			uint32_t m_threshold;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh		The parent submesh.
		 *\param[in]	threshold	The threshold from which a submesh will be instantiated.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh		Le sous-maillage parent.
		 *\param[in]	threshold	Le seuil à partir duquel un sous-maillage sera instancié.
		 */
		C3D_API explicit InstantiationComponent( Submesh & submesh
			, uint32_t threshold = 1u );
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentUPtr clone( Submesh & submesh )const override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		C3D_API ProgramFlags getProgramFlags( Pass const & pass )const noexcept override;
		/**
			*\~english
			*\param[in]	pass	The material pass.
			*\return		\p true if the instance count for given material is greater than the threshold.
			*\~french
			*\param[in]	pass	La passe de matériau.
			*\return		\p true si le nombre d'instances pour le matériau donné est plus grand que le seuil.
			*/
		C3D_API bool isInstanced( Pass const & pass )const;
		/**
			*\~english
			*\return		\p true if the max instance count is greater than the threshold.
			*\~french
			*\return		\p true si le nombre d'instances max est plus grand que le seuil.
			*/
		C3D_API bool isInstanced()const;

		ComponentData & getData()const noexcept
		{
			return *getDataT< ComponentData >();
		}

	public:
		C3D_API static castor::String const TypeName;
	};
}

#endif
