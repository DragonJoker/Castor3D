/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SpecularGlossinessPbrPass_H___
#define ___C3D_SpecularGlossinessPbrPass_H___

#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Shader/PassBuffer/PassBufferModule.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	class SpecularGlossinessPbrPass
		: public Pass
	{
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

		C3D_API static PassSPtr create( Material & parent );
		C3D_API static castor::AttributeParsersBySection createParsers();
		C3D_API static castor::StrUInt32Map createSections();
		/**
		 *\copydoc		castor3d::Pass::accept
		 */
		C3D_API void accept( PassBuffer & buffer )const override;
		/**
		 *\copydoc		castor3d::Pass::getSectionID
		 */
		C3D_API uint32_t getSectionID()const override;
		/**
		 *\copydoc		castor3d::Pass::writeText
		 */
		C3D_API bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		/**
		 *\~english
		 *\remarks	Passes are aligned on float[4], so the size of a pass
		 *			is the number of float[4] needed to contain it.
		 *\~french
		 *\remarks	Les passes sont alignées sur 4 flottants, donc la taille d'une passe
		 *			correspond aux nombres de float[4] qu'il faut pour la contenir.
		 */
		uint32_t getPassSize()const override
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
		void setDiffuse( castor::RgbColour const & value )
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
		void setGlossiness( float value )
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
		void setSpecular( castor::RgbColour const & value )
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
		castor::RgbColour const & getDiffuse()const
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The glossiness.
		 *\~french
		 *\return		La brillance.
		 */
		float getGlossiness()const
		{
			return m_glossiness;
		}
		/**
		 *\~english
		 *\return		The specular.
		 *\~french
		 *\return		La spécularité.
		 */
		castor::RgbColour const & getSpecular()const
		{
			return m_specular;
		}

	public:
		static castor::String const Type;

	private:
		void doInitialise()override;
		void doCleanup()override;
		void doSetOpacity( float value )override;
		void doPrepareTextures( TextureUnitPtrArray & result )override;
		void doJoinSpcGls( TextureUnitPtrArray & result );

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
