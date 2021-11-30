/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialTextureAnimationModule_H___
#define ___C3D_MaterialTextureAnimationModule_H___

#include "Castor3D/Material/Texture/TextureModule.hpp"

#include <CastorUtils/Math/MathModule.hpp>

namespace castor3d
{
	/**@name Material */
	//@{
	/**@name Texture */
	//@{
	/**@name Animation */
	//@{

	struct TextureAnimationData
	{
		castor::Point4f translate;
		castor::Point4f rotate;
		castor::Point4f scale;
		castor::Point4f tileSet;
	};

	class TextureAnimation;
	class TextureAnimationKeyFrame;

	CU_DeclareSmartPtr( TextureAnimation );
	CU_DeclareSmartPtr( TextureAnimationKeyFrame );

	using TextureRotateSpeed = castor::SpeedT< castor::Angle, castor::Seconds >;
	using TextureTranslateSpeed = castor::SpeedT< castor::Point2f, castor::Seconds >;
	using TextureScaleSpeed = castor::SpeedT< castor::Point2f, castor::Seconds >;

	//@}
	//@}
	//@}
}

#endif
