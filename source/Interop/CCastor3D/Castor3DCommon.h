/*
See LICENSE file in root folder
*/
#ifndef ___CC3D_Common_H___
#define ___CC3D_Common_H___

#include "CCastor3D/Castor3DEnums.h"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Material/Texture/TextureModule.hpp>
#include <Castor3D/Overlay/OverlayModule.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Scene/SceneModule.hpp>

#include <Castor3D/Model/Mesh/Mesh.hpp>

#include <CastorUtils/Design/DesignModule.hpp>
#include <CastorUtils/Graphics/GraphicsModule.hpp>
#include <CastorUtils/Log/Logger.hpp>

#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#define C3D_CatchCommonExceptions() \
	catch ( castor::Exception & exc ) \
	{ \
		return cc3d::reportError( C3D_FAILURE, exc.what() ); \
	} \
	catch ( std::exception & exc ) \
	{ \
		return cc3d::reportError( C3D_FAILURE, exc.what() ); \
	} \
	catch ( ... ) \
	{ \
		return cc3d::reportError( C3D_FAILURE, cuT( "Unknown exception" ) ); \
	}

#define C3D_SafeAlloc( Result, TypeName ) \
	( Result ) = new (std::nothrow) TypeName; \
	if ( !( Result ) ) \
		return C3D_OUTOFMEMORY

namespace cc3d
{
	inline C3D_RESULT reportError( C3D_RESULT error, castor::xchar const * const text )
	{
		castor::Logger::logError( text );
		return error;
	}

	inline void reportWarning( castor::xchar const * const text )
	{
		castor::Logger::logWarning( text );
	}

	inline void copyStringN( castor::String const & value, C3DChar * result, size_t resultSize )
	{
		result[0] = 0;
		strncpy( result, value.data()
			, resultSize <= value.size() ? resultSize : value.size() );
	}

	inline C3D_RESULT copyString( castor::String const & value, C3DString * result )
	{
		size_t byteSize = sizeof( C3DChar ) * ( value.size() + 1U );
		auto res = static_cast< C3DChar * >( malloc( byteSize ) );
		if ( !res )
			return C3D_OUTOFMEMORY;

		copyStringN( value, res, byteSize );
		*result = res;

		return C3D_OK;
	}

	inline C3DRgbColour convert( castor::RgbColour const & value )
	{
		return { value.red(), value.green(), value.blue() };
	}

	inline C3DHdrRgbColour convert( castor::HdrRgbColour const & value )
	{
		return { value.red(), value.green(), value.blue() };
	}

	inline C3DRgbaColour convert( castor::RgbaColour const & value )
	{
		return { value.red(), value.green(), value.blue(), value.alpha() };
	}

	inline C3DHdrRgbaColour convert( castor::HdrRgbaColour const & value )
	{
		return { value.red(), value.green(), value.blue(), value.alpha() };
	}

	inline C3DQuat convert( castor::Quaternion const & value )
	{
		return C3DQuat{ value->x, value->y, value->z, value->w };
	}

	inline C3DPosition convert( castor::Position const & value )
	{
		return C3DPosition{ value.x(), value.y() };
	}

	inline C3DSize convert( castor::Size const & value )
	{
		return C3DSize{ value.getWidth(), value.getHeight() };
	}

	inline C3DVec2 convert( castor::Point2f const & value )
	{
		return C3DVec2{ value->x, value->y };
	}

	inline C3DVec3 convert( castor::Point3f const & value )
	{
		return C3DVec3{ value->x, value->y, value->z };
	}

	inline C3DMat4 convert( castor::Matrix4x4f const & value )
	{
		return { value[0][0], value[0][1], value[0][2], value[0][3]
			, value[1][0], value[1][1], value[1][2], value[1][3]
			, value[2][0], value[2][1], value[2][2], value[2][3]
			, value[3][0], value[3][1], value[3][2], value[3][3] };
	}

	inline castor::RgbColour convert( C3DRgbColour const & value )
	{
		return castor::RgbColour::fromComponents( value.r, value.g, value.b );
	}

	inline castor::HdrRgbColour convert( C3DHdrRgbColour const & value )
	{
		return castor::HdrRgbColour::fromComponents( value.r, value.g, value.b );
	}

	inline castor::RgbaColour convert( C3DRgbaColour const & value )
	{
		return castor::RgbaColour::fromComponents( value.r, value.g, value.b, value.a );
	}

	inline castor::HdrRgbaColour convert( C3DHdrRgbaColour const & value )
	{
		return castor::HdrRgbaColour::fromComponents( value.r, value.g, value.b, value.a );
	}

	inline castor::Quaternion convert( C3DQuat const & value )
	{
		return castor::Quaternion::fromComponents( value.x, value.y, value.z, value.w );
	}

	inline castor::Point2f convert( C3DVec2 const & value )
	{
		return castor::Point2f{ value.x, value.y };
	}

	inline castor::Point3f convert( C3DVec3 const & value )
	{
		return castor::Point3f{ value.x, value.y, value.z };
	}

	inline castor::Position convert( C3DPosition const & value )
	{
		return castor::Position{ value.x, value.y };
	}

	inline castor::Size convert( C3DSize const & value )
	{
		return castor::Size{ value.width, value.height };
	}

	inline castor::Matrix4x4f convert( C3DMat4 const & value )
	{
		return castor::Matrix4x4f{ { value.m11, value.m12, value.m13, value.m14
		, value.m21, value.m22, value.m23, value.m24
		, value.m31, value.m32, value.m33, value.m34
		, value.m41, value.m42, value.m43, value.m44 } };
	}
}

#ifdef __cplusplus
extern "C"
{
#endif

	struct C3DLogger_
	{
		castor::LoggerInstance * internal;
	};

	struct C3DGlyph_
	{
		castor::Glyph * internal;
	};

	struct C3DFont_
	{
		castor::ResourceObsT< castor::Font, castor::String > internal;
	};

	struct C3DPixelBuffer_
	{
		castor::PxBufferBaseUPtr internal;
	};

	struct C3DImage_
	{
		castor::ResourceObsT< castor::Image, castor::String > internal;
	};

	struct C3DEngine_
	{
		castor3d::EngineUPtr internal;
	};

	struct C3DRenderTarget_
	{
		castor3d::RenderTarget * internal;
	};

	struct C3DMaterial_
	{
		castor3d::Material * internal;
	};

	struct C3DBorderPanelOverlay_
	{
		castor3d::BorderPanelOverlay * internal;
	};

	struct C3DPanelOverlay_
	{
		castor3d::PanelOverlay * internal;
	};

	struct C3DTextOverlay_
	{
		castor3d::TextOverlay * internal;
	};

	struct C3DRenderWindow_
	{
		castor3d::RenderWindowUPtr internal;
	};

	struct C3DSampler_
	{
		void setInternal( castor3d::SamplerUPtr v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::Sampler * v )noexcept
		{
			internal = v;
		}

		castor3d::SamplerUPtr releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::Sampler * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::SamplerUPtr ownInternal;
		castor3d::Sampler * internal;
	};

	struct C3DScene_
	{
		void setInternal( castor3d::SceneUPtr v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::Scene * v )noexcept
		{
			internal = v;
		}

		castor3d::SceneUPtr releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::Scene * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::SceneUPtr ownInternal;
		castor3d::Scene * internal;
	};

	struct C3DOverlay_
	{
		void setInternal( castor3d::OverlayUPtr v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::Overlay * v )noexcept
		{
			internal = v;
		}

		castor3d::OverlayUPtr releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::Overlay * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::OverlayUPtr ownInternal;
		castor3d::Overlay * internal;
	};

	struct C3DSceneNode_
	{
		void setInternal( castor3d::SceneNodeUPtr v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::SceneNode * v )noexcept
		{
			internal = v;
		}

		castor3d::SceneNodeUPtr releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::SceneNode * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::SceneNodeUPtr ownInternal;
		castor3d::SceneNode * internal;
	};

	struct C3DCamera_
	{
		void setInternal( castor3d::CameraUPtr v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::Camera * v )noexcept
		{
			internal = v;
		}

		castor3d::CameraUPtr releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::Camera * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::CameraUPtr ownInternal;
		castor3d::Camera * internal;
	};

	struct C3DGeometry_
	{
		void setInternal( castor3d::GeometryUPtr v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::Geometry * v )noexcept
		{
			internal = v;
		}

		castor3d::GeometryUPtr releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::Geometry * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::GeometryUPtr ownInternal;
		castor3d::Geometry * internal;
	};

	struct C3DLight_
	{
		void setInternal( castor3d::LightUPtr v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::Light * v )noexcept
		{
			internal = v;
		}

		castor3d::LightUPtr releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::Light * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::LightUPtr ownInternal;
		castor3d::Light * internal;
	};

	struct C3DLightGroup_
	{
		void setInternal( castor3d::LightGroupUPtr v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::LightGroup * v )noexcept
		{
			internal = v;
		}

		castor3d::LightGroupUPtr releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::LightGroup * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::LightGroupUPtr ownInternal;
		castor3d::LightGroup * internal;
	};

	struct C3DMesh_
	{
		void setInternal( castor3d::MeshRes v )noexcept
		{
			ownInternal = castor::move( v );
		}

		void setInternal( castor3d::MeshResPtr v )noexcept
		{
			internal = v;
		}

		castor3d::MeshRes releaseInternal()noexcept
		{
			return castor::move( ownInternal );
		}

		castor3d::MeshResPtr getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		castor3d::MeshRes ownInternal;
		castor3d::MeshResPtr internal;
	};

	struct C3DSubmesh_
	{
		castor3d::Submesh * internal;
	};

	struct C3DDirectionalLight_
	{
		castor3d::DirectionalLight * internal;
	};

	struct C3DPointLight_
	{
		castor3d::PointLight * internal;
	};

	struct C3DSpotLight_
	{
		castor3d::SpotLight * internal;
	};

	struct C3DShadow_
	{
		castor3d::ShadowConfig * internal;
	};

	struct C3DLinesMapping_
	{
		castor3d::LinesMapping * internal;
	};

	struct C3DTriFaceMapping_
	{
		castor3d::TriFaceMapping * internal;
	};

	struct C3DPass_
	{
		castor3d::Pass * internal;
	};

	struct C3DSkybox_
	{
		castor3d::SkyboxBackgroundUPtr internal;
	};

#ifdef __cplusplus
}
#endif

#endif
