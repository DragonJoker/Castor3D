/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Uncharted2_ToneMapping_H___
#define ___C3D_Uncharted2_ToneMapping_H___

#include <HDR/ToneMapping.hpp>

namespace Uncharted2
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Implements Reinhard tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton de Reinhard
	*/
	class ToneMapping
		: public castor3d::ToneMapping
	{
	public:
		/**
		 *\~english
		 * Specified constructor
		 *\param[in]	engine		The engine.
		 *\param[in]	p_parameters	The implementation specific parameters.
		 *\~french
		 * Constructeur spécifié
		 *\param[in]	engine		Le moteur.
		 *\param[in]	p_parameters	Les paramètres spécifiques à l'implémentation.
		 */
		ToneMapping( castor3d::Engine & engine, castor3d::Parameters const & p_parameters );
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
		 *\param[in]	p_parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Fonction de création, pour la fabrique.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	p_parameters	Les paramètres spécifiques à l'implémentation.
		 */
		static castor3d::ToneMappingSPtr create( castor3d::Engine & engine, castor3d::Parameters const & p_parameters );

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
		/**
		 *\copydoc		castor3d::ToneMapping::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & p_file )override;

	public:
		static castor::String Name;
	};
}

#endif

