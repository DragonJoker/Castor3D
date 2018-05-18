/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HBD_TONE_MAPPING_H___
#define ___C3D_HBD_TONE_MAPPING_H___

#include <HDR/ToneMapping.hpp>

namespace HejlBurgessDawson
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Implements Jim Hejl and Richard Burgess-Dawson tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton de Jim Hejl et Richard Burgess-Dawson.
	*/
	class ToneMapping
		: public castor3d::ToneMapping
	{
	public:
		/**
		 *\~english
		 * Specified constructor
		 *\param[in]	engine		The engine.
		 *\param[in]	parameters	The implementation specific parameters.
		 *\~french
		 * Constructeur spécifié
		 *\param[in]	engine		Le moteur.
		 *\param[in]	parameters	Les paramètres spécifiques à l'implémentation.
		 */
		ToneMapping( castor3d::Engine & engine
			, castor3d::HdrConfig & hdrConfig
			, castor3d::Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ToneMapping();
		/**
		 *\~english
		 *\brief		Creator function, for the factory.
		 *\param[in]	engine		The engine.
		 *\param[in]	parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Fonction de création, pour la fabrique.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	parameters	Les paramètres spécifiques à l'implémentation.
		 */
		static castor3d::ToneMappingSPtr create( castor3d::Engine & engine
			, castor3d::HdrConfig & hdrConfig
			, castor3d::Parameters const & parameters );

	private:
		/**
		 *\copydoc		castor3d::ToneMapping::doCreate
		 */
		glsl::Shader doCreate()override;
		/**
		 *\copydoc		castor3d::ToneMapping::doDestroy
		 */
		void doDestroy()override;
		/**
		 *\copydoc		castor3d::ToneMapping::doUpdate
		 */
		void doUpdate()override;

	public:
		static castor::String Type;
		static castor::String Name;
	};
}

#endif

