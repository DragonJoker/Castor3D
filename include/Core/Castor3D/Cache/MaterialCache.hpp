/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialCache_H___
#define ___C3D_MaterialCache_H___

#include "Castor3D/Cache/CacheBase.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/PassBuffer/PassBufferModule.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBufferModule.hpp"

namespace castor3d
{
	/**
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Material collection, with additional functions
	\~french
	\brief		Collection de matériaux, avec des fonctions additionnelles
	*/
	class MaterialCache
		: public CacheBase< Material, castor::String >
	{
	public:
		using MyCacheType = CacheBase< Material, castor::String >;
		using MyCacheTraits = typename MyCacheType::MyCacheTraits;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using LockType = typename MyCacheType::LockType;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	produce		The element producer.
		 *\param[in]	initialise	The element initialiser.
		 *\param[in]	clean		The element cleaner.
		 *\param[in]	merge		The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	produce		Le créateur d'objet.
		 *\param[in]	initialise	L'initialiseur d'objet.
		 *\param[in]	clean		Le nettoyeur d'objet.
		 *\param[in]	merge		Le fusionneur de collection d'objets.
		 */
		MaterialCache( Engine & engine
			, Producer && produce
			, Initialiser && initialise
			, Cleaner && clean
			, Merger && merge )
			: MyCacheType( engine
				, std::move( produce )
				, std::move( initialise )
				, std::move( clean )
				, std::move( merge ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~MaterialCache()
		{
		}
		/**
		 *\~english
		 *\brief		Intialises the default material.
		 *\param[in]	device	The GPU device.
		 *\param[in]	type	The materials type.
		 *\~french
		 *\brief		Initialise le matériau par défaut.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	type	Le type des matériaux.
		 */
		C3D_API void initialise( RenderDevice const & device
			, MaterialType type );
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Deletes the default material, flush the collection
		 *\~french
		 *\brief		Supprime le matériau par défaut, vide la collection
		 */
		C3D_API void clear();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\param[in]	element	The element.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	element	L'élément.
		 */
		C3D_API MaterialSPtr add( Key const & name, MaterialSPtr element );
		/**
		 *\~english
		 *\brief		Creates an element.
		 *\param[in]	name	The element name.
		 *\param[in]	type	The material type.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un élément.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	type	Le type de matéeiau.
		 *\return		L'élément créé.
		 */
		C3D_API MaterialSPtr add( Key const & name, MaterialType type );
		/**
		 *\~english
		 *\brief		Puts all the materials names in the given array
		 *\param[out]	names	The array of names to be filled
		 *\~french
		 *\brief		Remplit la liste des noms de tous les matériaux
		 *\param[out]	names	La liste de noms
		 */
		C3D_API void getNames( castor::StringArray & names );
		/**
		 *\~english
		 *\brief		Retrieves the default material
		 *\~french
		 *\brief		Récupère le matériau par défaut
		 */
		MaterialSPtr getDefaultMaterial()const
		{
			return m_defaultMaterial;
		}
		/**
		 *\~english
		 *\return		The pass buffer.
		 *\~french
		 *\return		Le tampon de passes.
		 */
		PassBuffer const & getPassBuffer()const
		{
			CU_Require( m_passBuffer );
			return *m_passBuffer;
		}
		/**
		 *\~english
		 *\return		The textures configurations buffer.
		 *\~french
		 *\return		Le tampon de configurations de textures.
		 */
		TextureConfigurationBuffer const & getTextureBuffer()const
		{
			CU_Require( m_textureBuffer );
			return *m_textureBuffer;
		}

	private:
		void doAddMaterial( Material const & material );

	private:
		MaterialSPtr m_defaultMaterial;
		PassBufferSPtr m_passBuffer;
		TextureConfigurationBufferSPtr m_textureBuffer;
	};
}

#endif
