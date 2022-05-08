/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshComponent_H___
#define ___C3D_SubmeshComponent_H___

#include "ComponentModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

namespace castor3d
{
	class SubmeshComponent
		: public castor::OwnedBy< Submesh >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\param[in]	type	The component type.
		 *\param[in]	id		The component ID.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 *\param[in]	type	Le type de composant.
		 *\param[in]	id		L'ID composant.
		 */
		C3D_API SubmeshComponent( Submesh & submesh
			, castor::String const & type
			, uint32_t id );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SubmeshComponent()noexcept = default;
		/**
		 *\~english
		 *\brief		Initialises the submesh
		 *\~french
		 *\brief		Initialise le sous-maillage
		 */
		C3D_API bool initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans the submesh
		 *\~french
		 *\brief		Nettoie le sous-maillage
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Uploads data on VRAM.
		 *\remarks		For host visible buffers.
		 *\~french
		 *\brief		Met les données en VRAM.
		 *\remarks		Pour les buffers host visible.
		 */
		C3D_API void upload();
		/**
		 *\~english
		 *\brief			Gathers buffers that need to go in a vertex layout.
		 *\param[in]		shaderFlags		The shader flags.
		 *\param[in]		submeshFlags	The submesh flags.
		 *\param[in]		material		The material.
		 *\param[in,out]	buffers			Receives the buffers.
		 *\param[in,out]	offsets			Receives the buffers offsets.
		 *\param[in,out]	layouts			Receives the vertex layouts.
		 *\param[in]		mask			The textures mask.
		 *\param[in,out]	currentLocation	The current attribute location.
		 *\~french
		 *\brief		Récupère les tampons qui doivent aller dans un vertex layout.
		 *\param[in]		shaderFlags		Les shader flags.
		 *\param[in]		submeshFlags	Les submesh flags.
		 *\param[in]		material		Les matériau.
		 *\param[in,out]	buffers			Reçoit les buffers.
		 *\param[in,out]	offsets			Reçoit les offsets dans les buffers.
		 *\param[in,out]	layouts			Reçoit les vertex layouts.
		 *\param[in]		mask			Le masque de textures.
		 *\param[in,out]	currentLocation	La position actuelle des attributs.
		 */
		C3D_API virtual void gather( ShaderFlags const & shaderFlags
			, SubmeshFlags const & submeshFlags
			, MaterialRPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, TextureFlagsArray const & mask
			, uint32_t & currentLocation ) = 0;
		/**
		 *\~english
		 *\return		The submesh flags.
		 *\param[in]	material	The material.
		 *\~french
		 *\return		Les indicateurs de submesh.
		 *\param[in]	material	Les matériau.
		 */
		C3D_API virtual SubmeshFlags getSubmeshFlags( Material const & material )const = 0;
		/**
		 *\~english
		 *\return		The buffer usage flags.
		 *\~french
		 *\return		Les flags d'utilisation du buffer.
		 */
		C3D_API virtual VkBufferUsageFlags getUsageFlags()const
		{
			return {};
		}
		/**
		 *\~english
		 *\return			Clones this component into given submesh.
		 *\param[in,out]	submesh	Receives the cloned component.
		 *\~french
		 *\return			Clone ce composant dans le submesh donné.
		 *\param[in,out]	submesh	Reçoit le composant cloné.
		 */
		C3D_API virtual SubmeshComponentSPtr clone( Submesh & submesh )const = 0;
		/**
		 *\~english
		 *\return		The component type name.
		 *\~french
		 *\return		Le nom du type de composant.
		 */
		castor::String const & getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the component to be updated.
		 *\~french
		 *\brief		Dit que le composant doit être mis à jour.
		 */
		void needsUpdate()
		{
			m_dirty = true;
		}
		/**
		 *\~english
		 *\return		The component binding ID.
		 *\~french
		 *\return		L'ID de binding du composant.
		 */
		uint32_t getID()
		{
			return m_id;
		}

	private:
		C3D_API virtual bool doInitialise( RenderDevice const & device ) = 0;
		C3D_API virtual void doCleanup( RenderDevice const & device ) = 0;
		C3D_API virtual void doUpload() = 0;

	private:
		castor::String m_type;
		uint32_t m_id;
		bool m_initialised{ false };
		bool m_dirty{ true };
	};
}

#endif
