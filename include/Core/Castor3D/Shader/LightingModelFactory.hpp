/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingModelFactory_H___
#define ___C3D_LightingModelFactory_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <unordered_set>

namespace castor3d
{
	class LightingModelFactory
	{
	public:
		using Obj = shader::LightingModel;
		using Key = castor::String;
		using Id = LightingModelID;
		using PtrType = shader::LightingModelUPtr;
		using Creator = shader::LightingModelCreator;
		using ObjPtr = PtrType;
		struct LightingModel
		{
			LightingModel( castor::String name = {}
				, Creator create = {}
				, shader::DiffuseBrdfDesc defaultDiffuseBrdf = {}
				, shader::SpecularBrdfDesc defaultSpecularBrdf = {}
				, shader::SheenBrdfDesc defaultSheenBrdf = {}
				, shader::ClearcoatBrdfDesc defaultClearcoatBrdf = {} )
				: name{ std::move( name ) }
				, create{ std::move( create ) }
				, defaultDiffuseBrdf{ std::move( defaultDiffuseBrdf ) }
				, defaultSpecularBrdf{ std::move( defaultSpecularBrdf ) }
				, defaultSheenBrdf{ std::move( defaultSheenBrdf ) }
				, defaultClearcoatBrdf{ std::move( defaultClearcoatBrdf ) }
			{
			}

			castor::String name{};
			Creator create{};
			shader::DiffuseBrdfDesc defaultDiffuseBrdf{};
			shader::SpecularBrdfDesc defaultSpecularBrdf{};
			shader::SheenBrdfDesc defaultSheenBrdf{};
			shader::ClearcoatBrdfDesc defaultClearcoatBrdf{};
		};
		using LightingModelPtr = std::unique_ptr< LightingModel >;
		using LightingModelCont = castor::Vector< LightingModelPtr >;
		struct Entry
		{
			LightingModel const * model;
			Key name{};
			LightingModelID lightingModelId{};
			shader::DiffuseBrdfDesc diffuseBrdf{};
			shader::SpecularBrdfDesc specularBrdf{};
			shader::SheenBrdfDesc sheenBrdf{};
			shader::ClearcoatBrdfDesc clearcoatBrdf{};
		};
		using ObjCont = castor::Vector< Entry >;

	public:
		/**
		 *\~english
		 *\brief		Registers a lighting model.
		 *\param[in]	baseName				The lighting model base name.
		 *\param[in]	defaultDiffuseBrdf		The default diffuse BRDF.
		 *\param[in]	defaultSpecularBrdf		The default specular BRDF.
		 *\param[in]	defaultSheenBrdf		The default sheen BRDF.
		 *\param[in]	defaultClearcoatBrdf	The default clearcoat BRDF.
		 *\param[in]	create					The object creation function.
		 *\~french
		 *\brief		Enregistre un modèle d'éclairage.
		 *\param[in]	baseName				Le nom de base du modèle d'éclairage.
		 *\param[in]	defaultDiffuseBrdf		La BRDF de diffuse par défaut.
		 *\param[in]	defaultSpecularBrdf		La BRDF de spéculaire par défaut.
		 *\param[in]	defaultSheenBrdf		La BRDF de sheen par défaut.
		 *\param[in]	defaultClearcoatBrdf	La BRDF de clearcoat par défaut.
		 *\param[in]	create					La fonction de création d'objet.
		 */
		C3D_API void registerType( castor::String const & baseName
			, shader::DiffuseBrdfDesc const & defaultDiffuseBrdf
			, shader::SpecularBrdfDesc const & defaultSpecularBrdf
			, shader::SheenBrdfDesc const & defaultSheenBrdf
			, shader::ClearcoatBrdfDesc const & defaultClearcoatBrdf
			, Creator const & create );
		/**
		 *\~english
		 *\brief		Unregisters a lighting model.
		 *\param[in]	key					The lighting model.
		 *\~french
		 *\brief		Désenregistre un modèle d'éclairage.
		 *\param[in]	key					Le modèle d'éclairage.
		 */
		C3D_API void unregisterType( castor::String const & baseName );
		/**
		 *\~english
		 *\brief		Registers a diffuse BRDF.
		 *\param[in]	desc	The BRDF description.
		 *\~french
		 *\brief		Enregistre une BRDF de diffuse.
		 *\param[in]	desc	La description de la BRDF.
		 */
		C3D_API void registerDiffuseBrdf( shader::DiffuseBrdfDesc const & desc );
		/**
		 *\~english
		 *\brief		Unegisters a diffuse BRDF.
		 *\param[in]	name	The BRDF name.
		 *\~french
		 *\brief		Désenregistre une BRDF de diffuse.
		 *\param[in]	name	Le nom de la BRDF.
		 */
		C3D_API void unregisterDiffuseBrdf( castor::String const & name );
		/**
		 *\~english
		 *\brief		Registers a specular BRDF.
		 *\param[in]	desc	The BRDF description.
		 *\~french
		 *\brief		Enregistre une BRDF de spéculaire.
		 *\param[in]	desc	La description de la BRDF.
		 */
		C3D_API void registerSpecularBrdf( shader::SpecularBrdfDesc const & desc );
		/**
		 *\~english
		 *\brief		Unegisters a specular BRDF.
		 *\param[in]	name	The BRDF name.
		 *\~french
		 *\brief		Désenregistre une BRDF de spéculaire.
		 *\param[in]	name	Le nom de la BRDF.
		 */
		C3D_API void unregisterSpecularBrdf( castor::String const & name );
		/**
		 *\~english
		 *\brief		Registers a sheen BRDF.
		 *\param[in]	desc	The BRDF description.
		 *\~french
		 *\brief		Enregistre une BRDF de sheen.
		 *\param[in]	desc	La description de la BRDF.
		 */
		C3D_API void registerSheenBrdf( shader::SheenBrdfDesc const & desc );
		/**
		 *\~english
		 *\brief		Unegisters a sheen BRDF.
		 *\param[in]	name	The BRDF name.
		 *\~french
		 *\brief		Désenregistre une BRDF de sheen.
		 *\param[in]	name	Le nom de la BRDF.
		 */
		C3D_API void unregisterSheenBrdf( castor::String const & name );
		/**
		 *\~english
		 *\brief		Registers a clearcoat BRDF.
		 *\param[in]	desc	The BRDF description.
		 *\~french
		 *\brief		Enregistre une BRDF de clearcoat.
		 *\param[in]	desc	La description de la BRDF.
		 */
		C3D_API void registerClearcoatBrdf( shader::ClearcoatBrdfDesc const & desc );
		/**
		 *\~english
		 *\brief		Unegisters a clearcoat BRDF.
		 *\param[in]	name	The BRDF name.
		 *\~french
		 *\brief		Désenregistre une BRDF de clearcoat.
		 *\param[in]	name	Le nom de la BRDF.
		 */
		C3D_API void unregisterClearcoatBrdf( castor::String const & name );
		/**
		 *\~english
		 *\param[in]	basName			The lighting model base name.
		 *\param[in]	diffuseBrdf		The diffuse BRDF name.
		 *\param[in]	specularBrdf	The specular BRDF name.
		 *\param[in]	sheenBrdf		The sheen BRDF name.
		 *\param[in]	clearcoatBrdf	The clearcoat BRDF name.
		 *\return		The lighting model ID.
		 *\~french
		 *\param[in]	basName			Le nom de base du modèle d'éclairage.
		 *\param[in]	diffuseBrdf		Le nom de la BRDF de diffuse.
		 *\param[in]	specularBrdf	Le nom de la BRDF de speculaire.
		 *\param[in]	sheenBrdf		Le nom de la BRDF de sheen.
		 *\param[in]	clearcoatBrdf	Le nom de la BRDF de clearcoat.
		 *\return		L'ID du modèle d'éclairage.
		 */
		C3D_API Id getLightingModelId( castor::String const & basName
			, castor::String diffuseBrdf = {}
			, castor::String specularBrdf = {}
			, castor::String sheenBrdf = {}
			, castor::String clearcoatBrdf = {} );
		/**
		 *\~english
		 *\param[in]	baseName	The lighting model base name.
		 *\return		The lighting model description.
		 *\~french
		 *\param[in]	baseName	Le nom de base du modèle d'éclairage.
		 *\return		La description du modèle d'éclairage.
		 */
		C3D_API LightingModel const & getModel( castor::String const & baseName )const;
		/**
		 *\~english
		 *\param[in]	id	The lighting model ID.
		 *\return		The lighting model name.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage.
		 *\return		Le nom du modèle d'éclairage.
		 */
		C3D_API Key getBaseName( Id const & id )const;
		/**
		 *\~english
		 *\param[in]	id	The lighting model ID.
		 *\return		The diffuse BRDF name.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage pour la diffuse BRDF.
		 *\return		Le nom de la diffuse BRDF.
		 */
		C3D_API castor::String getDiffuseBrdfName( Id const & id )const;
		/**
		 *\~english
		 *\param[in]	id	The lighting model ID.
		 *\return		The specular BRDF name.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage.
		 *\return		Le nom de la specular BRDF.
		 */
		C3D_API castor::String getSpecularBrdfName( Id const & id )const;
		/**
		 *\~english
		 *\param[in]	id	The lighting model ID.
		 *\return		The sheen BRDF name.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage.
		 *\return		Le nom de la sheen BRDF.
		 */
		C3D_API castor::String getSheenBrdfName( Id const & id )const;
		/**
		 *\~english
		 *\param[in]	id	The lighting model ID.
		 *\return		The clearcoat BRDF name.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage.
		 *\return		Le nom de la clearcoat BRDF.
		 */
		C3D_API castor::String getClearcoatBrdfName( Id const & id )const;
		/**
		 *\~english
		 *\return		The unique lighting models IDs.
		 *\~french
		 *\return		Les ID uniques des modèles d'éclairage.
		 */
		C3D_API castor::Vector< LightingModelID > getLightingModelsID()const;
		/**
		 *\~english
		 *\return		The lighting models names.
		 *\~french
		 *\return		Les noms des modèles d'éclairage.
		 */
		C3D_API castor::StringArray listRegisteredTypes()const;
		/**
		 *\~english
		 *\brief		Replaces old names with registered ones.
		 *\~french
		 *\brief		Remplace les anciens nommages par ceux enregistrés.
		 */
		C3D_API static castor::String normaliseName( castor::String name );
		/**
		 *\~english
		 *\brief		Creates an object from an ID.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	params				The creation parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet à partir d'un ID.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\param[in]	params				Les paramètres de création.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		ObjPtr create( LightingModelID lightingModelId
			, Parameters && ... params )const
		{
			if ( lightingModelId == 0u )
			{
				return nullptr;
			}

			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [lightingModelId]( Entry const & lookup )
				{
					return lightingModelId == lookup.lightingModelId;
				} );

			if ( it == m_registered.end() )
			{
				CU_Exception( castor::ERROR_UNKNOWN_OBJECT );
			}

			return it->model->create( lightingModelId
				, it->diffuseBrdf
				, it->specularBrdf
				, it->sheenBrdf
				, it->clearcoatBrdf
				, castor::forward< Parameters >( params )... );
		}

	private:
		void registerType( LightingModel const & model
			, shader::DiffuseBrdfDesc diffuseBrdf
			, shader::SpecularBrdfDesc specularBrdf
			, shader::SheenBrdfDesc sheenBrdf
			, shader::ClearcoatBrdfDesc clearcoatBrdf );
		void unregisterType( castor::String const & baseName
			, castor::String const & diffuseBrdf
			, castor::String const & specularBrdf
			, castor::String const & sheenBrdf
			, castor::String const & clearcoatBrdf );

	private:
		LightingModelID m_currentId{};
		ObjCont m_registered;
		LightingModelCont m_models;
		shader::DiffuseBrdfArray m_diffuseBrdfs;
		shader::SpecularBrdfArray m_specularBrdfs;
		shader::SheenBrdfArray m_sheenBrdfs;
		shader::ClearcoatBrdfArray m_clearcoatBrdfs;
	};
}

#endif
