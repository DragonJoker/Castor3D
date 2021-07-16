/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Material_H___
#define ___C3D_Material_H___

#include "MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/Resource.hpp>

namespace castor3d
{
	class Material
		: public castor::Resource< Material >
		, public std::enable_shared_from_this< Material >
		, public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	The material name.
		 *\param[in]	engine	The core engine.
		 *\param[in]	type	The material type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Le nom du matériau.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	type	Le type de matériau.
		 */
		C3D_API Material( castor::String const & name
			, Engine & engine
			, PassTypeID type );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Material();
		/**
		 *\~english
		 *\brief		Initialises the material and all it's passes.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise le matériau et toutes ses passes.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Flushes passes.
		 *\~french
		 *\brief		Supprime les passes.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Creates a pass.
		 *\return		The created pass.
		 *\~french
		 *\brief		Crée une passe.
		 *\return		La passe créée.
		 */
		C3D_API PassSPtr createPass();
		/**
		 *\~english
		 *\brief		Removes an external pass to rhe material.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Supprime une passe externe.
		 *\param[in]	pass	La passe.
		 */
		C3D_API void removePass( PassSPtr pass );
		/**
		 *\~english
		 *\brief		Retrieves a pass and returns it.
		 *\param[in]	index	The index of the wanted pass.
		 *\return		The retrieved pass or nullptr if not found.
		 *\~french
		 *\brief		Récupère une passe.
		 *\param[in]	index	L'index de la passe voulue.
		 *\return		La passe récupére ou nullptr si non trouvés.
		 */
		C3D_API PassSPtr getPass( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Destroys the pass at the given index.
		 *\param[in]	index	The pass index.
		 *\~french
		 *\brief		Destroys the pass at the given index.
		 *\param[in]	index	L'index de la passe.
		 */
		C3D_API void destroyPass( uint32_t index );
		/**
		*\~english
		*\return		\p true if all passes needs alpha blending.
		*\~french
		*\return		\p true si toutes les passes ont besoin d'alpha blending.
		*/
		C3D_API bool hasAlphaBlending()const;
		/**
		*\~english
		*\return		\p true if at least one pass needs a reflection map.
		*\~french
		*\return		\p true si au moins une passe a besoin d'une reflection map.
		*/
		C3D_API bool hasEnvironmentMapping()const;
		/**
		 *\~english
		 *\return		Tells if the material has subsurface scattering.
		 *\~french
		 *\return		Dit si le matériau a du subsurface scattering.
		 */
		C3D_API bool hasSubsurfaceScattering()const;
		/**
		 *\~english
		 *\return		Tells if the material is textured.
		 *\param[in]	mask	A texture mask to filter out textures.
		 *\~french
		 *\return		Dit si le matériau a des textures.
		 *\param[in]	mask	Un masque de textures pour les filtrer.
		 */
		C3D_API bool isTextured( TextureFlags mask = TextureFlag::eAll )const;
		/**
		 *\~english
		 *\return		The passes count.
		 *\~french
		 *\return		Le nombre de passes.
		 */
		uint32_t getPassCount()const
		{
			return uint32_t( m_passes.size() );
		}
		/**
		 *\~english
		 *\return		The constant iterator on the beginning of the passes array.
		 *\~french
		 *\return		L'itérateur constant sur le début du tableau de passes.
		 */
		PassPtrArrayConstIt begin()const
		{
			return m_passes.begin();
		}
		/**
		 *\~english
		 *\return		The iterator on the beginning of the passes array.
		 *\~french
		 *\return		L'itérateur sur le début du tableau de passes.
		 */
		PassPtrArrayIt begin()
		{
			return m_passes.begin();
		}
		/**
		 *\~english
		 *\return		The constant iterator on the end of the passes array.
		 *\~french
		 *\return		L'itérateur constant sur la fin du tableau de passes.
		 */
		PassPtrArrayConstIt end()const
		{
			return m_passes.end();
		}
		/**
		 *\~english
		 *\return		The iterator on the end of the passes array.
		 *\~french
		 *\return		L'itérateur sur la fin du tableau de passes.
		 */
		PassPtrArrayIt end()
		{
			return m_passes.end();
		}
		/**
		 *\~english
		 *\return		The material type.
		 *\~french
		 *\return		Le type de matériau.
		 */
		PassTypeID getType()const
		{
			return m_type;
		}

	private:
		void onPassChanged( Pass const & pass );

	public:
		//!\~english	The signal raised when the material has changed.
		//!\~french		Le signal levé lorsque le matériau a changé.
		OnMaterialChanged onChanged;
		//!\~english	The default material name.
		//!\~french		Le nom du matériau par défaut.
		static const castor::String DefaultMaterialName;

	private:
		PassPtrArray m_passes;
		PassTypeID m_type{ 0u };
		std::map< PassSPtr, OnPassChangedConnection > m_passListeners;
	};
}

#endif
