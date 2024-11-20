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
		using PtrType = shader::LightingModelPtr;
		using Creator = shader::LightingModelCreator;
		using ObjPtr = PtrType;
		struct LightingModel
		{
			explicit LightingModel( castor::String name = {}
				, Creator create = {}
				, shader::LightingModelDesc defaultDesc = {} )
				: name{ std::move( name ) }
				, create{ std::move( create ) }
				, defaultDesc{ std::move( defaultDesc ) }
			{
			}

			castor::String name{};
			Creator create{};
			shader::LightingModelDesc defaultDesc{};
		};
		using LightingModelPtr = std::unique_ptr< LightingModel >;
		using LightingModelCont = castor::Vector< LightingModelPtr >;
		struct Entry
		{
			LightingModel const * model;
			Key name{};
			LightingModelID lightingModelId{};
			shader::LightingModelDesc desc{};
		};
		using ObjCont = castor::Vector< Entry >;

	public:
		/**
		 *\~english
		 *\brief		Registers a lighting model.
		 *\param[in]	baseName	The lighting model base name.
		 *\param[in]	defaultDesc	The default model description.
		 *\param[in]	create		The object creation function.
		 *\~french
		 *\brief		Enregistre un modèle d'éclairage.
		 *\param[in]	baseName	Le nom de base du modèle d'éclairage.
		 *\param[in]	defaultDesc	La description du modèle par défaut.
		 *\param[in]	create		La fonction de création d'objet.
		 */
		C3D_API void registerType( castor::String const & baseName
			, shader::LightingModelDesc const & defaultDesc
			, Creator const & create );
		/**
		 *\~english
		 *\brief		Unregisters a lighting model.
		 *\param[in]	baseName	The lighting model base name.
		 *\~french
		 *\brief		Désenregistre un modèle d'éclairage.
		 *\param[in]	baseName	Le nom de base du modèle d'éclairage.
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
		 *\brief		Registers a scattering model.
		 *\param[in]	desc	The model description.
		 *\~french
		 *\brief		Enregistre modèle de scattering.
		 *\param[in]	desc	La description du modèle.
		 */
		C3D_API void registerScatteringModel( shader::ScatteringModelDesc const & desc );
		/**
		 *\~english
		 *\brief		Unegisters a scattering model.
		 *\param[in]	name	The model name.
		 *\~french
		 *\brief		Désenregistre un modèle de scattering.
		 *\param[in]	name	Le nom du modèle.
		 */
		C3D_API void unregisterScatteringModel( castor::String const & name );
		/**
		 *\~english
		 *\param[in]	baseName	The lighting model base name.
		 *\param[in]	descNames	The lighting model desciption names.
		 *\return		The lighting model ID.
		 *\~french
		 *\param[in]	baseName	Le nom de base du modèle d'éclairage.
		 *\param[in]	descNames	Les noms de la description du modèle.
		 *\return		L'ID du modèle d'éclairage.
		 */
		C3D_API Id getLightingModelId( castor::String const & baseName
			, shader::LightingModelNames descNames = {} );
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
		 *\param[in]	id	The lighting model ID.
		 *\return		The scattering model.
		 *\~french
		 *\param[in]	id	L'ID du modèle d'éclairage.
		 *\return		Le nom du modèle de scattering.
		 */
		C3D_API castor::String getScatteringModelName( Id const & id )const;
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
		 *\param[in]	params				The creation parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet à partir d'un ID.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
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
				, it->desc
				, castor::forward< Parameters >( params )... );
		}

	private:
		void registerType( LightingModel const & model
			, shader::LightingModelDesc desc );
		void unregisterType( castor::String const & baseName
			, shader::LightingModelNames const & descNames );

	private:
		ObjCont m_registered;
		LightingModelCont m_models;
		shader::DiffuseBrdfArray m_diffuseBrdfs;
		shader::SpecularBrdfArray m_specularBrdfs;
		shader::SheenBrdfArray m_sheenBrdfs;
		shader::ClearcoatBrdfArray m_clearcoatBrdfs;
		shader::ScatteringModelArray m_scatteringModels;
	};
}

#endif
