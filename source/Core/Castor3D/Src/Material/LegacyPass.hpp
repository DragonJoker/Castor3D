/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LegacyPass_H___
#define ___C3D_LegacyPass_H___

#include "Material/Pass.hpp"

namespace castor3d
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
			: public castor::TextWriter< LegacyPass >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
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
			C3D_API bool operator()( LegacyPass const & pass, castor::TextFile & file )override;
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
		C3D_API explicit LegacyPass( Material & parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LegacyPass();
		/**
		 *\copydoc		castor3d::Pass::Accept
		 */
		C3D_API void accept( PassBuffer & buffer )const override;
		/**
		 *\~english
		 *\remarks	Passes are aligned on float[4], so the size of a pass
		 *			is the number of float[4] needed to contain it.
		 *\~french
		 *\remarks	Les passes sont alignées sur 4 flottants, donc la taille d'une passe
		 *			correspond aux nombres de float[4] qu'il faut pour la contenir.
		 */
		inline uint32_t getPassSize()const override
		{
			return 4u;
		}
		/**
		 *\~english
		 *\brief		sets the diffuse colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur diffuse.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setDiffuse( castor::Colour const & value )
		{
			m_diffuse = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		sets the specular colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur spéculaire.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSpecular( castor::Colour const & value )
		{
			m_specular = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		sets the ambient factor.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le facteur d'ambiante.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setAmbient( float const & value )
		{
			m_ambient = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		sets the shininess.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'exposant.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setShininess( float value )
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
		inline float getShininess()const
		{
			return m_shininess;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		inline castor::Colour const & getDiffuse()const
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		inline castor::Colour const & getSpecular()const
		{
			return m_specular;
		}
		/**
		 *\~english
		 *\return		The ambient factor.
		 *\~french
		 *\return		Le facteur d'ambiante.
		 */
		inline float getAmbient()const
		{
			return m_ambient;
		}
		/**
		 *\~english
		 *\return		The diffuse colour.
		 *\~french
		 *\return		La couleur diffuse.
		 */
		inline castor::Colour & getDiffuse()
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The specular colour.
		 *\~french
		 *\return		La couleur spéculaire.
		 */
		inline castor::Colour & getSpecular()
		{
			return m_specular;
		}

	private:
		/**
		 *\copydoc		castor3d::Pass::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::Pass::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::Pass::doSetOpacity
		 */
		void doSetOpacity( float value )override;

	private:
		//!\~english	Diffuse material colour.
		//!\~french		La couleur diffuse
		castor::Colour m_diffuse;
		//!\~english	Specular material colour.
		//!\~french		La couleur spéculaire.
		castor::Colour m_specular;
		//!\~english	The ambient contribution factor.
		//!\~french		Le facteur de contribution a l'ambiente.
		float m_ambient{ 0.0f };
		//!\~english	The shininess value.
		//!\~french		La valeur d'exposant.
		float m_shininess{ 50.0f };
	};
}

#endif
