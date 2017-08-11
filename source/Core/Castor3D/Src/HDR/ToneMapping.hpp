/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_TONE_MAPPING_H___
#define ___C3D_TONE_MAPPING_H___

#include "Render/RenderInfo.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/MatrixUbo.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
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
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	name		The tone mapping name.
		 *\param[in]	engine		The engine.
		 *\param[in]	parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	name		Le nom du mappage de tons.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	parameters	Les paramètres spécifiques à l'implémentation.
		 */
		C3D_API ToneMapping( castor::String const & name
			, Engine & engine
			, Parameters const & parameters );
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
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Applies the tone mapping for given HDR texture.
		 *\param[in]	size	The target dimensions.
		 *\param[in]	texture	The HDR texture.
		 *\param[in]	info	Receives the render information.
		 *\~english
		 *\brief		Applique le mappage de tons pour la texture HDR donnée.
		 *\param[in]	size	Les dimensions de la cible.
		 *\param[in]	texture	La texture HDR.
		 *\param[in]	info	Reçoit les informations de rendu.
		 */
		C3D_API void apply( castor::Size const & size
			, TextureLayout const & texture
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Writes the tone mapping into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit le mappage de tons dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API bool writeInto( castor::TextFile & file );
		/**
		 *\~english
		 *\brief		sets the HDR config for the tone mapping.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la configuration HDR pour le mappage de tons.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void setConfig( HdrConfig const & config );
		/**
		 *\~english
		 *\return		The HDR configuration.
		 *\~french
		 *\return		La configuration HDR.
		 */
		inline HdrConfig const & getConfig()const
		{
			return m_config;
		}

	private:
		/**
		 *\~english
		 *\brief			Creates tone mapping shader, and the shader variables.
		 *\param[in,out]	The shader program UBO to create variables.
		 *\return			The pixel shader source.
		 *\~french
		 *\brief			Crée le shader de mappage de tons, ainsi que les variables shader.
		 *\param[in,out]	Le tampon de variables shader, pour créer les variables.
		 *\return			Le source du pixel shader.
		 */
		C3D_API virtual glsl::Shader doCreate() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the tone mapping shader variables.
		 *\~french
		 *\brief		Nettoie les variables shader de mappage de tons.
		 */
		C3D_API virtual void doDestroy() = 0;
		/**
		 *\~english
		 *\brief		Updates the tone mapping shader variables.
		 *\~english
		 *\brief		Met à jour les variables shader du mappage de tons.
		 */
		C3D_API virtual void doUpdate() = 0;
		/**
		 *\~english
		 *\brief		Writes the tone mapping into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit le mappage de tons dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API virtual bool doWriteInto( castor::TextFile & file ) = 0;

	protected:
		//!\~english	The HDR configuration.
		//!\~french		La configuration HDR.
		HdrConfig m_config;
		//!\~english	The exposure shader variable.
		//!\~french		La variable shader pour l'exposition.
		Uniform1fSPtr m_exposureVar;
		//!\~english	The gamma correction shader variable.
		//!\~french		La variable shader pour la correction gamma.
		Uniform1fSPtr m_gammaVar;
		//!\~english	The tone mapping shader program.
		//!\~french		Le shader de mappage de ton.
		RenderPipelineUPtr m_pipeline;
		//!\~english	The matrix data UBO.
		//!\~french		L'UBO de données de matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The configuration data UBO.
		//!\~french		L'UBO de données de configuration.
		UniformBuffer m_configUbo;
		//!\~english	The pipeline used to render the tone mapping.
		//!\~french		Le pipeline utilisé pour le rendu du mappage de tons.
		RenderColourToTextureUPtr m_colour;
		//!\~english	The configuration constants buffer name.
		//!\~french		Le nom du tampon de constantes de configuration.
		C3D_API static castor::String const HdrConfigUbo;
		//!\~english	The render pass timer.
		//!\~french		Le timer de passe de rendu.
		RenderPassTimerSPtr m_timer;
	};
}

#endif

