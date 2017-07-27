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
			C3D_API explicit TextWriter( Castor::String const & tabs );
			/**
			 *\~english
			 *\brief			Writes a LegacyPass into a text file.
			 *\param[in]		pass	The LegacyPass to write.
			 *\param[in,out]	file	The file where to write the LegacyPass.
			 *\~french
			 *\brief			Ecrit une LegacyPass dans un fichier texte.
			 *\param[in]		pass	La LegacyPass à écrire.
			 *\param[in,out]	file	Le file où écrire la LegacyPass.
			 */
			C3D_API bool operator()( LegacyPass const & pass, Castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Used by Material, don't use it.
		 *\param[in]	parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		A ne pas utiliser autrement que via la classe Material.
		 *\param[in]	parent	Le matériau parent.
		 */
		C3D_API LegacyPass( Material & parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LegacyPass();
		/**
		 *\copydoc		Castor3D::Pass::Accept
		 */
		C3D_API void Accept( PassBuffer & buffer )const override;
		/**
		 *\~english
		 *\remarks	Passes are aligned on float[4], so the size of a pass
		 *			is the number of float[4] needed to contain it.
		 *\~french
		 *\remarks	Les passes sont alignées sur 4 flottants, donc la taille d'une passe
		 *			correspond aux nombres de float[4] qu'il faut pour la contenir.
		 */
		inline uint32_t GetPassSize()const override
		{
			return 4u;
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur diffuse.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void SetDiffuse( Castor::Colour const & value )
		{
			m_diffuse = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur spéculaire.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void SetSpecular( Castor::Colour const & value )
		{
			m_specular = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the ambient factor.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le facteur d'ambiante.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void SetAmbient( float const & value )
		{
			m_ambient = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the shininess.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'exposant.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void SetShininess( float value )
		{
			m_shininess = value;
			onChanged( *this );
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
		 *\return		The ambient factor.
		 *\~french
		 *\return		Le facteur d'ambiante.
		 */
		inline float GetAmbient()const
		{
			return m_ambient;
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
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		inline Castor::Colour & GetSpecular()
		{
			return m_specular;
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
		 *\copydoc		Castor3D::Pass::DoSetOpacity
		 */
		void DoSetOpacity( float value )override;

	private:
		//!\~english	Diffuse material colour.
		//!\~french		La couleur diffuse
		Castor::Colour m_diffuse;
		//!\~english	Specular material colour.
		//!\~french		La couleur spéculaire.
		Castor::Colour m_specular;
		//!\~english	The ambient contribution factor.
		//!\~french		Le facteur de contribution a l'ambiente.
		float m_ambient{ 0.0f };
		//!\~english	The shininess value.
		//!\~french		La valeur d'exposant.
		float m_shininess{ 50.0f };
	};
}

#endif
