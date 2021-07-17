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
		C3D_API explicit SpecularGlossinessPbrPass( Material & parent
			, PassFlags initialFlags = PassFlag::eNone );
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
		C3D_API SpecularGlossinessPbrPass( Material & parent
			, PassTypeID typeID
			, PassFlags initialFlags = PassFlag::eNone );
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
		 *\copydoc		castor3d::Pass::getPassSectionID
		 */
		C3D_API uint32_t getPassSectionID()const override;
		/**
		 *\copydoc		castor3d::Pass::getTextureSectionID
		 */
		C3D_API uint32_t getTextureSectionID()const override;
		/**
		 *\copydoc		castor3d::Pass::writeText
		 */
		C3D_API bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		/**
		 *\copydoc		castor3d::Pass::setColour
		 */
		void setColour( castor::RgbColour const & value ) override
		{
			setDiffuse( value );
		}
		/**
		 *\copydoc		castor3d::Pass::getColour
		 */
		castor::RgbColour const & getColour()const override
		{
			return getDiffuse();
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
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		castor::RgbColour const & getDiffuse()const
		{
			return *m_diffuse;
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
			return *m_specular;
		}

	public:
		C3D_API static castor::String const Type;
		C3D_API static castor::String const LightingModel;

	protected:
		C3D_API void doAccept( PassVisitorBase & vis )override;
		C3D_API void doAccept( TextureConfiguration & config
			, PassVisitorBase & vis )override;

	private:
		C3D_API void doInitialise()override;
		C3D_API void doCleanup()override;
		C3D_API void doSetOpacity( float value )override;
		C3D_API void doPrepareTextures( TextureUnitPtrArray & result )override;
		C3D_API void doJoinSpcGls( TextureUnitPtrArray & result );

	private:
		//!\~english	The diffuse colour.
		//!\~french		La couleur diffuse.
		castor::GroupChangeTracked< castor::RgbColour > m_diffuse;
		//!\~english	The specular colour.
		//!\~french		La couleur spéculaire.
		castor::GroupChangeTracked< castor::RgbColour > m_specular;
		//!\~english	The reflectance.
		//!\~french		La réflectivité.
		castor::GroupChangeTracked< float > m_glossiness;
	};
}

#endif
