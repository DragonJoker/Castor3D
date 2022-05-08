/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantiationComponent_H___
#define ___C3D_InstantiationComponent_H___

#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

namespace castor3d
{
	struct InstantiationData
	{
		castor::Point4ui m_objectIDs;
	};

	class InstantiationComponent
		: public SubmeshComponent
	{
	public:
		struct Data
		{
			Data( uint32_t count
				, GpuBufferOffsetT< InstantiationData > buffer
				, std::vector< InstantiationData > data = {} )
				: count{ count }
				, buffer{ std::move( buffer ) }
				, data{ std::move( data ) }
			{
			}
			uint32_t count;
			GpuBufferOffsetT< InstantiationData > buffer;
			std::vector< InstantiationData > data;
		};
		using InstanceDataMap = std::map< Material const *, Data >;

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
		 *\~english
		 *\brief		Increments instance count.
		 *\param[in]	material	The material for which the instance count is incremented.
		 *\return		\p true if the instance buffer has changed.
		 *\~french
		 *\brief		Incrémente le compte d'instances.
		 *\param[in]	material	Le matériau pour lequel le compte est incrémenté.
		 *\return		\p true Si le buffer d'instances a changé.
		 */
		C3D_API bool ref( MaterialRPtr material );
		/**
		 *\~english
		 *\brief		Decrements instance count.
		 *\param[in]	material	The material for which the instance count is decremented.
		 *\~french
		 *\brief		Décrémente le compte d'instances.
		 *\param[in]	material	Le matériau pour lequel le compte est décrémenté.
		 */
		C3D_API void unref( MaterialRPtr material );
		/**
		 *\~english
		 *\brief		Retrieves the instances count
		 *\param[in]	material	The material for which the instance count is retrieved
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre d'instances
		 *\param[in]	material	Le matériau pour lequel le compte est récupéré
		 *\return		La valeur
		 */
		C3D_API uint32_t getRefCount( MaterialRPtr material )const;
		/**
		 *\~english
		 *\param[in]	material	The material.
		 *\return		\p true if the instance count for given material is greater than the threshold.
		 *\~french
		 *\param[in]	material	Le matériau.
		 *\return		\p true si le nombre d'instances pour le matériau donné est plus grand que le seuil.
		 */
		C3D_API bool isInstanced( MaterialRPtr material )const;
		/**
		 *\~english
		 *\return		\p true if the max instance count is greater than the threshold.
		 *\~french
		 *\return		\p true si le nombre d'instances max est plus grand que le seuil.
		 */
		C3D_API bool isInstanced()const;
		/**
		 *\~english
		 *\return		The maximum instances count, amongst all materials.
		 *\~french
		 *\return		Le nombre moximum d'instances, tous matériaux confondus,
		 */
		C3D_API uint32_t getMaxRefCount()const;
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( ShaderFlags const & shaderFlags
			, SubmeshFlags const & submeshFlags
			, MaterialRPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, TextureFlagsArray const & mask
			, uint32_t & currentLocation )override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentSPtr clone( Submesh & submesh )const override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API SubmeshFlags getSubmeshFlags( Material const & material )const override;

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

		inline InstanceDataMap::const_iterator find( Material const & material )const
		{
			return m_instances.find( &material );
		}

		inline InstanceDataMap::iterator find( Material const & material )
		{
			needsUpdate();
			return m_instances.find( &material );
		}
		/**@}*/

	private:
		bool doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;
		inline bool doCheckInstanced( uint32_t count )const
		{
			return count >= getThreshold();
		}

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr BindingPoint = 2u;

	private:
		InstanceDataMap m_instances;
		std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > m_mtxLayouts;
		uint32_t m_threshold;
	};
}

#endif
