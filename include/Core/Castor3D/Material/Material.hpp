/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MATERIAL_H___
#define ___C3D_MATERIAL_H___

#include "MaterialModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/Resource.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Helper class to retrieve a pass type from a MaterialType.
	\remarks	Specialisation for MaterialType::ePhong.
	\~french
	\brief		Classe d'aide permettant de récupérer le type de passe depuis un MaterialType.
	\remarks	Spécialisation pour MaterialType::ePhong.
	*/
	template<>
	struct PassTyper< MaterialType::ePhong >
	{
		using Type = PhongPass;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Helper class to retrieve a pass type from a MaterialType.
	\remarks	Specialisation for MaterialType::eMetallicRoughness.
	\~french
	\brief		Classe d'aide permettant de récupérer le type de passe depuis un MaterialType.
	\remarks	Spécialisation pour MaterialType::eMetallicRoughness.
	*/
	template<>
	struct PassTyper< MaterialType::eMetallicRoughness >
	{
		using Type = MetallicRoughnessPbrPass;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Helper class to retrieve a pass type from a MaterialType.
	\remarks	Specialisation for MaterialType::eMetallicRoughness.
	\~french
	\brief		Classe d'aide permettant de récupérer le type de passe depuis un MaterialType.
	\remarks	Spécialisation pour MaterialType::eMetallicRoughness.
	*/
	template<>
	struct PassTyper< MaterialType::eSpecularGlossiness >
	{
		using Type = SpecularGlossinessPbrPass;
	};

	class Material
		: public castor::Resource< Material >
		, public std::enable_shared_from_this< Material >
		, public castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief Material loader
		\remark
		\~french
		\brief Loader de Material
		*/
		class TextWriter
			: public castor::TextWriter< Material >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief			Writes a material into a text file
			 *\param[in]		material	The material to save
			 *\param[in,out]	file		The file where to save the material
			 *\~french
			 *\brief			Ecrit un matériau dans un fichier texte
			 *\param[in]		material	Le matériau
			 *\param[in,out]	file		Le fichier
			 */
			C3D_API bool operator()( Material const & material
				, castor::TextFile & file )override;
		};

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
			, MaterialType type );
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
		 *\~french
		 *\brief		Initialise le matériau et toutes ses passes.
		 */
		C3D_API void initialise();
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
		 *\return		The passes count.
		 *\~french
		 *\return		Le nombre de passes.
		 */
		inline uint32_t getPassCount()const
		{
			return uint32_t( m_passes.size() );
		}
		/**
		 *\~english
		 *\return		The constant iterator on the beginning of the passes array.
		 *\~french
		 *\return		L'itérateur constant sur le début du tableau de passes.
		 */
		inline PassPtrArrayConstIt begin()const
		{
			return m_passes.begin();
		}
		/**
		 *\~english
		 *\return		The iterator on the beginning of the passes array.
		 *\~french
		 *\return		L'itérateur sur le début du tableau de passes.
		 */
		inline PassPtrArrayIt begin()
		{
			return m_passes.begin();
		}
		/**
		 *\~english
		 *\return		The constant iterator on the end of the passes array.
		 *\~french
		 *\return		L'itérateur constant sur la fin du tableau de passes.
		 */
		inline PassPtrArrayConstIt end()const
		{
			return m_passes.end();
		}
		/**
		 *\~english
		 *\return		The iterator on the end of the passes array.
		 *\~french
		 *\return		L'itérateur sur la fin du tableau de passes.
		 */
		inline PassPtrArrayIt end()
		{
			return m_passes.end();
		}
		/**
		 *\~english
		 *\return		The material type.
		 *\~french
		 *\return		Le type de matériau.
		 */
		inline MaterialType getType()const
		{
			return m_type;
		}
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
		template< MaterialType Type >
		inline std::shared_ptr< typename PassTyper< Type >::Type > getTypedPass( uint32_t index )const
		{
			auto pass = getPass( index );
			CU_Require( m_type == Type );
			return std::static_pointer_cast< typename PassTyper< Type >::Type >( pass );
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
		//!\~english	The passes.
		//!\~french		Les passes.
		PassPtrArray m_passes;
		//!\~english	The material type.
		//!\~french		Le type de matériau.
		MaterialType m_type{ MaterialType::ePhong };
		//!\~english	The connections to the pass changed signals.
		//!\~french		Les connections aux signaux de passe changée.
		std::map< PassSPtr, OnPassChangedConnection > m_passListeners;
	};
}

#endif
