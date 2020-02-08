/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshComponent_H___
#define ___C3D_SubmeshComponent_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		Component for a submesh.
	\~french
	\brief		Composant pour un sous-maillage.
	*/
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
		C3D_API bool initialise();
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
		C3D_API void fill();
		/**
		 *\~english
		 *\brief		Uploads data on VRAM.
		 *\~french
		 *\brief		Met les données en VRAM.
		 */
		C3D_API void upload();
		/**
		 *\~english
		 *\brief		Sets the material.
		 *\param[in]	oldMaterial	The old material.
		 *\param[in]	newMaterial	The new material.
		 *\param[in]	update		Tells if the buffers must be updated.
		 *\~french
		 *\brief		Définit le materiau.
		 *\param[in]	oldMaterial	Le matériau précédent.
		 *\param[in]	newMaterial	Le nouveau matériau.
		 *\param[in]	update		Dit si les tampons doivent être mis à jour.
		 */
		C3D_API virtual void setMaterial( MaterialSPtr oldMaterial
			, MaterialSPtr newMaterial
			, bool update );
		/**
		 *\~english
		 *\brief		Gathers buffers that need to go in a VAO.
		 *\~french
		 *\brief		Récupère les tampons qui doivent aller dans un VAO.
		 */
		C3D_API virtual void gather( MaterialSPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts ) = 0;
		/**
		 *\~english
		 *\return		The shader program flags.
		 *\~french
		 *\return		Les indicateurs de shader.
		 */
		C3D_API virtual ProgramFlags getProgramFlags( MaterialSPtr material )const = 0;
		/**
		 *\~english
		 *\return		The component type name.
		 *\~french
		 *\return		Le nom du type de composant.
		 */
		inline castor::String const & getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the component to be updated.
		 *\~french
		 *\return		Dit que le composant doit être mis à jour.
		 */
		inline void needsUpdate()
		{
			m_dirty = true;
		}

	private:
		C3D_API virtual bool doInitialise() = 0;
		C3D_API virtual void doCleanup() = 0;
		C3D_API virtual void doFill() = 0;
		C3D_API virtual void doUpload() = 0;

	private:
		castor::String m_type;
		bool m_initialised{ false };
		bool m_dirty{ true };
	};
}

#endif
