/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_TONE_MAPPING_H___
#define ___C3D_TONE_MAPPING_H___

#include "Castor3DPrerequisites.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Tone mapping base class.
	\~french
	\brief		Classe de base de mappage de ton.
	*/
	class ToneMapping
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	p_type			The tone mapping type.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	p_type			Le type de mappage de tons.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_parameters	Les paramètres spécifiques à l'implémentation.
		 */
		C3D_API ToneMapping( eTONE_MAPPING_TYPE p_type, Engine & p_engine, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ToneMapping();
		/**
		 *\~english
		 *\brief		Initialises tone mapping shader.
		 *\~french
		 *\brief		Initialise le shader de mappage de tons.
		 */
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Applies the tone mapping for given HDR texture.
		 *\param[in]	p_size		The target dimensions.
		 *\param[in]	p_texture	The HDR texture.
		 *\~english
		 *\brief		Applique le mappage de tons pour la texture HDR donnée.
		 *\param[in]	p_size		Les dimensions de la cible.
		 *\param[in]	p_texture	La texture HDR.
		 */
		C3D_API void Apply( Castor::Size const & p_size, Texture const & p_texture );

	protected:
		/**
		 *\~english
		 *\brief		Creates tone mapping shader, and the shader variables.
		 *\return		The pixel shader program.
		 *\~french
		 *\brief		Crée le shader de mappage de tons, ainsi que les variables shader.
		 *\return		Le pixel shader program.
		 */
		C3D_API virtual Castor::String DoCreate() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the tone mapping shader variables.
		 *\~french
		 *\brief		Nettoie les variables shader de mappage de tons.
		 */
		C3D_API virtual void DoDestroy() = 0;
		/**
		 *\~english
		 *\brief		Updates the tone mapping shader variables.
		 *\~english
		 *\brief		Met à jour les variables shader du mappage de tons.
		 */
		C3D_API virtual void DoUpdate() = 0;

	protected:
		//!\~english The tone mapping type.	\~french Le type de mappage de tons.
		eTONE_MAPPING_TYPE m_type;
		//!\~english The Reinhard tone mapping shader program.	\~french Le shader de mappage de ton de Reinhard
		ShaderProgramSPtr m_program;
		//!\~english The exposure value.	\~french La valeur d'exposition.
		float m_exposure;
		//!\~english The exposure shader variable.	\~french La variable shader pour l'exposition.
		OneFloatFrameVariableSPtr m_exposureVar;
		//!\~english The configuration constants buffer name.	\~french Le nom du tampon de constantes de configuration.
		static Castor::String const HdrConfig;
		//!\~english The exposure constant name.	\~french Le nom de la constante d'exposition.
		static Castor::String const Exposure;
	};
}

#endif

