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
		struct Entry
		{
			Key name{};
			Creator create{};
			LightingModelID lightingModelId{};
			BackgroundModelID backgroundModelId{};
		};
		using ObjCont = std::vector< Entry >;

	public:
		/**
		 *\~english
		 *\brief		Registers an object type.
		 *\param[in]	key					The object type.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	create				The object creation function.
		 *\~french
		 *\brief		Enregistre un type d'objet.
		 *\param[in]	key					Le type d'objet.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\param[in]	create				La fonction de création d'objet.
		 */
		C3D_API LightingModelID registerType( Key const & key
			, BackgroundModelID backgroundModelId
			, Creator create );
		/**
		 *\~english
		 *\brief		Unregisters an object type.
		 *\param[in]	key					The object type.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\~french
		 *\brief		Désenregistre un type d'objet.
		 *\param[in]	key					Le type d'objet.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 */
		C3D_API void unregisterType( Key const & key
			, BackgroundModelID backgroundModelId );
		/**
		 *\~english
		 *\brief		Unregisters an object type.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\~french
		 *\brief		Désenregistre un type d'objet.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 */
		C3D_API void unregisterType( LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId );
		/**
		 *\~english
		 *\param[in]	key	The object type.
		 *\return		The object type ID.
		 *\~french
		 *\param[in]	key	Le type d'objet.
		 *\return		L'ID du type d'objet.
		 */
		C3D_API Id getNameId( Key const & key )const;
		/**
		 *\~english
		 *\param[in]	id	The object type ID.
		 *\return		The object type.
		 *\~french
		 *\param[in]	id	L'ID du type d'objet.
		 *\return		Le type d'objet.
		 */
		C3D_API Key getIdName( Id const & id )const;
		/**
		 *\~english
		 *\return		The unique lighting models IDs.
		 *\~french
		 *\return		Les ID uniques des modèles d'éclairage.
		 */
		C3D_API std::vector< LightingModelID > getLightingModelsID()const;
		/**
		 *\~english
		 *\param[in]	backgroundModelId	The background model ID.
		 *\return		The lighting models for given background model.
		 *\~french
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\return		Les modèles d'éclairage du mod_le de fond donné.
		 */
		C3D_API std::vector< LightingModelID > getLightingModelsID( BackgroundModelID backgroundModelId )const;
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
			, BackgroundModelID backgroundModelId
			, Parameters && ... params )const
		{
			if ( lightingModelId == 0u )
			{
				return nullptr;
			}

			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [lightingModelId, backgroundModelId]( Entry const & lookup )
				{
					return lightingModelId == lookup.lightingModelId
						&& backgroundModelId == lookup.backgroundModelId;
				} );

			if ( it == m_registered.end() )
			{
				CU_Exception( castor::ERROR_UNKNOWN_OBJECT );
			}

			return it->create( lightingModelId
				, std::forward< Parameters >( params )... );
		}

	private:
		LightingModelID m_currentId{};
		ObjCont m_registered;
	};
}

#endif
