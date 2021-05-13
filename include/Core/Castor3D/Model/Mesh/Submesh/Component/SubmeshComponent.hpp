/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshComponent_H___
#define ___C3D_SubmeshComponent_H___

#include "ComponentModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

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
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 *\param[in]	type	Le type de composant.
		 */
		C3D_API SubmeshComponent( Submesh & submesh
			, castor::String const & type );
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
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Initialises the submesh
		 *\~french
		 *\brief		Initialise le sous-maillage
		 */
		C3D_API void fill( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Uploads data on VRAM.
		 *\~french
		 *\brief		Met les données en VRAM.
		 */
		C3D_API void upload();
		/**
		 *\~english
		 *\brief		Gathers buffers that need to go in a VAO.
		 *\~french
		 *\brief		Récupère les tampons qui doivent aller dans un VAO.
		 */
		C3D_API virtual void gather( ShaderFlags const & flags
			, MaterialSPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, uint32_t instanceMult
			, TextureFlagsArray const & mask ) = 0;
		/**
		 *\~english
		 *\return		The shader program flags.
		 *\~french
		 *\return		Les indicateurs de shader.
		 */
		C3D_API virtual ProgramFlags getProgramFlags( MaterialSPtr material )const = 0;
		/**
		 *\~english
		 *\return		Clones this component.
		 *\~french
		 *\return		Clone ce composant.
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
		 *\return		Dit que le composant doit être mis à jour.
		 */
		void needsUpdate()
		{
			m_dirty = true;
		}

	private:
		C3D_API virtual bool doInitialise( RenderDevice const & device ) = 0;
		C3D_API virtual void doCleanup() = 0;
		C3D_API virtual void doFill( RenderDevice const & device ) = 0;
		C3D_API virtual void doUpload() = 0;

	private:
		castor::String m_type;
		bool m_initialised{ false };
		bool m_dirty{ true };
	};
}

#endif
