/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IndexMapping_H___
#define ___C3D_IndexMapping_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

namespace castor3d
{
	class IndexMapping
		: public SubmeshComponent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh				The parent submesh.
		 *\param[in]	type				The component type.
		 *\param[in]	bufferUsageFlags	The buffer usage flags.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh				Le sous-maillage parent.
		 *\param[in]	type				Le type de composant.
		 *\param[in]	bufferUsageFlags	Les flags d'utilisation du buffer.
		 */
		C3D_API IndexMapping( Submesh & submesh
			, castor::String const & type
			, VkBufferUsageFlags bufferUsageFlags );
		/**
		 *\~english
		 *\return		The elements count.
		 *\~french
		 *\return		Le nombre d'éléments.
		 */
		C3D_API virtual uint32_t getCount()const = 0;
		/**
		 *\~english
		 *\return		The components count in an element.
		 *\~french
		 *\return		Le nombre de composantes d'un élément.
		 */
		C3D_API virtual uint32_t getComponentsCount()const = 0;
		/**
		 *\~english
		 *\brief		Sorts the face from farthest to nearest from the camera
		 *\param[in]	cameraPosition	The camera position, relative to submesh
		 *\~french
		 *\brief		Trie les faces des plus éloignées aux plus proches de la caméra
		 *\param[in]	cameraPosition	La position de la caméra, relative au sous-maillage
		 */
		C3D_API virtual void sortByDistance( castor::Point3f const & cameraPosition ) = 0;
		/**
		 *\~english
		 *\brief		Generates normals and tangents.
		 *\param[in]	reverted	Tells if normals are inverted.
		 *\~french
		 *\brief		Génère les normales et les tangentes.
		 *\param[in]	reverted	Dit si les normales sont inversées.
		 */
		C3D_API virtual void computeNormals( bool reverted = false ) = 0;
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		void gather( ShaderFlags const & shaderFlags
			, SubmeshFlags const & submeshFlags
			, MaterialRPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, TextureFlagsArray const & mask
			, uint32_t & currentLocation )override
		{
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getSubmeshFlags
		 */
		SubmeshFlags getSubmeshFlags( Material const & material )const override
		{
			return SubmeshFlags{};
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getUsageFlags
		 */
		VkBufferUsageFlags getUsageFlags()const override
		{
			return m_bufferUsageFlags;
		}

	private:
		bool doInitialise( RenderDevice const & device )override
		{
			return true;
		}

		void doCleanup( RenderDevice const & device )override
		{
		}

		void doUpload()override
		{
		}

	private:
		VkBufferUsageFlags m_bufferUsageFlags;

	protected:
		StagingDataUPtr m_staging;
	};
}

#endif
