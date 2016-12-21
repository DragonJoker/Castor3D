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
#ifndef ___C3D_LegacyPass_H___
#define ___C3D_LegacyPass_H___

#include "Material/Pass.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Definition of a traditional (pre-PBR) pass.
	\remark		A traditional pass is composed of : base colours (ambient, diffuse, specular, emissive), and shininess.
	\~french
	\brief		Définition d'une passe traditionnelle (pré-PBR).
	\remark		Une passe traditionnelle est composée de : couleurs (ambiante, diffuse, spéculaire, émissive), et d'exposant.
	*/
	class LegacyPass
		: public Pass
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief Pass loader.
		\~french
		\brief Loader de Pass.
		*/
		class TextWriter
			: public Castor::TextWriter< LegacyPass >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief			Writes a LegacyPass into a text file.
			 *\param[in]		p_pass	The LegacyPass to write.
			 *\param[in,out]	p_file	The file where to write the LegacyPass.
			 *\~french
			 *\brief			Ecrit une LegacyPass dans un fichier texte.
			 *\param[in]		p_pass	La LegacyPass à écrire.
			 *\param[in,out]	p_file	Le file où écrire la LegacyPass.
			 */
			C3D_API bool operator()( LegacyPass const & p_pass, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Used by Material, don't use it.
		 *\param[in]	p_parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		A ne pas utiliser autrement que via la classe Material.
		 *\param[in]	p_parent	Le matériau parent.
		 */
		C3D_API LegacyPass( Material & p_parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LegacyPass();
		/**
		 *\~english
		 *\brief		Sets the diffuse colour.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la couleur diffuse.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetDiffuse( Castor::Colour const & p_value)
		{
			m_diffuse = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the ambient colour.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la couleur ambiante.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetAmbient( Castor::Colour const & p_value)
		{
			m_ambient = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la couleur spéculaire.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetSpecular( Castor::Colour const & p_value)
		{
			m_specular = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the emissive colour.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la couleur émissive.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetEmissive( Castor::HdrColour const & p_value)
		{
			m_emissive = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the shininess.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit l'exposant.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetShininess( float p_value )
		{
			m_shininess = p_value;
		}
		/**
		 *\~english
		 *\return		The shininess value.
		 *\~french
		 *\return		La valeur d'exposant.
		 */
		inline float GetShininess()const
		{
			return m_shininess;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		inline Castor::Colour const & GetDiffuse()const
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The ambient colour.
		 *\~french
		 *\return		La couleur ambiante.
		 */
		inline Castor::Colour const & GetAmbient()const
		{
			return m_ambient;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		inline Castor::Colour const & GetSpecular()const
		{
			return m_specular;
		}
		/**
		 *\~english
		 *\return		The emissive colour.
		 *\~french
		 *\return		La couleur émissive.
		 */
		inline Castor::HdrColour const & GetEmissive()const
		{
			return m_emissive;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		inline Castor::Colour & GetDiffuse()
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The ambient colour.
		 *\~french
		 *\return		La couleur ambiante.
		 */
		inline Castor::Colour & GetAmbient()
		{
			return m_ambient;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		inline Castor::Colour & GetSpecular()
		{
			return m_specular;
		}
		/**
		 *\~english
		 *\return		The emissive colour.
		 *\~french
		 *\return		La couleur émissive.
		 */
		inline Castor::HdrColour & GetEmissive()
		{
			return m_emissive;
		}

	private:
		/**
		 *\copydoc		Castor3D::Pass::DoInitialise
		 */
		void DoInitialise()override;
		/**
		 *\copydoc		Castor3D::Pass::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::Pass::DoUpdateRenderNode
		 */
		void DoUpdateRenderNode( PassRenderNodeUniforms & p_node )const override;
		/**
		 *\copydoc		Castor3D::Pass::DoSetOpacity
		 */
		void DoSetOpacity( float p_value )override;

	private:
		//!\~english	Diffuse material colour.
		//!\~french		La couleur diffuse
		Castor::Colour m_diffuse;
		//!\~english	Ambient material colour.
		//!\~french		La couleur ambiante.
		Castor::Colour m_ambient;
		//!\~english	Specular material colour.
		//!\~french		La couleur spéculaire.
		Castor::Colour m_specular;
		//!\~english	Emissive material colour.
		//!\~french		La couleur émissive.
		Castor::HdrColour m_emissive;
		//!\~english	The shininess value.
		//!\~french		La valeur d'exposant.
		float m_shininess{ 50.0f };
	};
}

#endif
