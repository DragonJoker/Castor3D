/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IndexMapping_H___
#define ___C3D_IndexMapping_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

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
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh				Le sous-maillage parent.
		 *\param[in]	type				Le type de composant.
		 */
		C3D_API IndexMapping( Submesh & submesh
			, castor::String const & type
			, SubmeshComponentDataUPtr data );
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
		 *\brief		Generates normals.
		 *\param[in]	reverted	Tells if normals are inverted.
		 *\~french
		 *\brief		Génère les normales.
		 *\param[in]	reverted	Dit si les normales sont inversées.
		 */
		C3D_API virtual void computeNormals( bool reverted = false ) = 0;
		/**
		 *\~english
		 *\brief		Generates tangents.
		 *\remarks		This function supposes the normals are defined.
		 *\~french
		 *\brief		Génère les tangentes.
		 *\remarks		Cette fonction suppose que les normales sont définies.
		 */
		C3D_API virtual void computeTangents() = 0;

	protected:
		struct ComponentData
			: public SubmeshComponentData
		{
			explicit ComponentData( Submesh & submesh
				, VkBufferUsageFlags bufferUsageFlags )
				: SubmeshComponentData{ submesh }
				, m_bufferUsageFlags{ bufferUsageFlags }
			{
			}
			/**
			 *\copydoc		castor3d::SubmeshComponentData::gather
			 */
			void gather( PipelineFlags const & flags
				, Pass const & pass
				, ObjectBufferOffset const & bufferOffsets
				, ashes::BufferCRefArray & buffers
				, std::vector< uint64_t > & offsets
				, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
				, uint32_t & currentBinding
				, uint32_t & currentLocation )override final
			{
			}
			/**
			 *\copydoc		castor3d::SubmeshComponentData::getUsageFlags
			 */
			VkBufferUsageFlags getUsageFlags()const noexcept override
			{
				return m_bufferUsageFlags;
			}

		private:
			bool doInitialise( RenderDevice const & device )override
			{
				return true;
			}

		private:
			VkBufferUsageFlags m_bufferUsageFlags;
		};
	};
}

#endif
