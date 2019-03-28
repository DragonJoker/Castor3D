/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SpecularGlossinessPbrPass_H___
#define ___C3D_SpecularGlossinessPbrPass_H___

#include "Castor3D/Material/Pass.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		13/02/2017
	\~english
	\brief		Definition of a PBR Metallic/Roughness pass.
	\remark		A PBR pass is composed of : colour, roughness, reflectance.
	\~french
	\brief		Définition d'une passe PBR Metallic/Roughness.
	\remark		Une passe PBR est composée de : couleur, rugosité, réflectivité.
	*/
	class SpecularGlossinessPbrPass
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
			: public castor::TextWriter< SpecularGlossinessPbrPass >
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
			 *\brief			Writes a PhongPass into a text file.
			 *\param[in]		pass	The PhongPass to write.
			 *\param[in,out]	file	The file where to write the PhongPass.
			 *\~french
			 *\brief			Ecrit une PhongPass dans un fichier texte.
			 *\param[in]		pass	La PhongPass à écrire.
			 *\param[in,out]	file	Le file où écrire la PhongPass.
			 */
			C3D_API bool operator()( SpecularGlossinessPbrPass const & pass
				, castor::TextFile & file )override;
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
		C3D_API explicit SpecularGlossinessPbrPass( Material & parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SpecularGlossinessPbrPass();
		/**
		 *\copydoc		castor3d::Pass::accept
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
		 *\brief		Sets the albedo colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur d'albédo.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setDiffuse( castor::RgbColour const & value )
		{
			m_diffuse = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the glossiness.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la brillance.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setGlossiness( float value )
		{
			m_glossiness = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the specular.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la spécularité.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSpecular( castor::RgbColour const & value )
		{
			m_specular = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline castor::RgbColour const & getDiffuse()const
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The glossiness.
		 *\~french
		 *\return		La brillance.
		 */
		inline float getGlossiness()const
		{
			return m_glossiness;
		}
		/**
		 *\~english
		 *\return		The specular.
		 *\~french
		 *\return		La spécularité.
		 */
		inline castor::RgbColour const & getSpecular()const
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
		//!\~english	The diffuse colour.
		//!\~french		La couleur diffuse.
		castor::RgbColour m_diffuse;
		//!\~english	The specular colour.
		//!\~french		La couleur spéculaire.
		castor::RgbColour m_specular;
		//!\~english	The reflectance.
		//!\~french		La réflectivité.
		float m_glossiness{ 0.0 };
	};
}

#endif
