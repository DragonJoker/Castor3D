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
	catch ( c3d::Exception & exc ) \
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
	inline C3D_RESULT reportError( C3D_RESULT error, c3d::xchar const * const text )
	{
		c3d::Logger::logError( text );
		return error;
	}

	inline void reportWarning( c3d::xchar const * const text )
	{
		c3d::Logger::logWarning( text );
	}

	inline void copyStringN( c3d::String const & value, C3DChar * result, size_t resultSize )
	{
		result[0] = 0;
		strncpy( result, value.data()
			, resultSize <= value.size() ? resultSize : value.size() );
	}

	inline C3D_RESULT copyString( c3d::String const & value, C3DString * result )
	{
		size_t byteSize = sizeof( C3DChar ) * ( value.size() + 1U );
		auto res = static_cast< C3DChar * >( malloc( byteSize ) );
		if ( !res )
			return C3D_OUTOFMEMORY;

		copyStringN( value, res, byteSize );
		*result = res;

		return C3D_OK;
	}

	inline C3DRgbColour convert( c3d::RgbColour const & value )
	{
		return { value.red(), value.green(), value.blue() };
	}

	inline C3DHdrRgbColour convert( c3d::HdrRgbColour const & value )
	{
		return { value.red(), value.green(), value.blue() };
	}

	inline C3DRgbaColour convert( c3d::RgbaColour const & value )
	{
		return { value.red(), value.green(), value.blue(), value.alpha() };
	}

	inline C3DHdrRgbaColour convert( c3d::HdrRgbaColour const & value )
	{
		return { value.red(), value.green(), value.blue(), value.alpha() };
	}

	inline C3DQuat convert( c3d::Quaternion const & value )
	{
		return C3DQuat{ value->x, value->y, value->z, value->w };
	}

	inline C3DPosition convert( c3d::Position const & value )
	{
		return C3DPosition{ value.x(), value.y() };
	}

	inline C3DSize convert( c3d::Size const & value )
	{
		return C3DSize{ value.getWidth(), value.getHeight() };
	}

	inline C3DVec2 convert( c3d::Point2f const & value )
	{
		return C3DVec2{ value->x, value->y };
	}

	inline C3DVec3 convert( c3d::Point3f const & value )
	{
		return C3DVec3{ value->x, value->y, value->z };
	}

	inline C3DMat4 convert( c3d::Matrix4x4f const & value )
	{
		return { value[0][0], value[0][1], value[0][2], value[0][3]
			, value[1][0], value[1][1], value[1][2], value[1][3]
			, value[2][0], value[2][1], value[2][2], value[2][3]
			, value[3][0], value[3][1], value[3][2], value[3][3] };
	}

	inline c3d::RgbColour convert( C3DRgbColour const & value )
	{
		return c3d::RgbColour::fromComponents( value.r, value.g, value.b );
	}

	inline c3d::HdrRgbColour convert( C3DHdrRgbColour const & value )
	{
		return c3d::HdrRgbColour::fromComponents( value.r, value.g, value.b );
	}

	inline c3d::RgbaColour convert( C3DRgbaColour const & value )
	{
		return c3d::RgbaColour::fromComponents( value.r, value.g, value.b, value.a );
	}

	inline c3d::HdrRgbaColour convert( C3DHdrRgbaColour const & value )
	{
		return c3d::HdrRgbaColour::fromComponents( value.r, value.g, value.b, value.a );
	}

	inline c3d::Quaternion convert( C3DQuat const & value )
	{
		return c3d::Quaternion::fromComponents( value.x, value.y, value.z, value.w );
	}

	inline c3d::Point2f convert( C3DVec2 const & value )
	{
		return c3d::Point2f{ value.x, value.y };
	}

	inline c3d::Point3f convert( C3DVec3 const & value )
	{
		return c3d::Point3f{ value.x, value.y, value.z };
	}

	inline c3d::Position convert( C3DPosition const & value )
	{
		return c3d::Position{ value.x, value.y };
	}

	inline c3d::Size convert( C3DSize const & value )
	{
		return c3d::Size{ value.width, value.height };
	}

	inline c3d::Matrix4x4f convert( C3DMat4 const & value )
	{
		return c3d::Matrix4x4f{ { value.m11, value.m12, value.m13, value.m14
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
		c3d::LoggerInstance * internal;
	};

	struct C3DGlyph_
	{
		c3d::Glyph * internal;
	};

	struct C3DFont_
	{
		c3d::ResourceObsT< c3d::Font, c3d::String > internal;
	};

	struct C3DPixelBuffer_
	{
		c3d::PxBufferBaseUPtr internal;
	};

	struct C3DImage_
	{
		c3d::ResourceObsT< c3d::Image, c3d::String > internal;
	};

	struct C3DEngine_
	{
		c3d::EngineUPtr internal;
	};

	struct C3DRenderTarget_
	{
		c3d::RenderTarget * internal;
	};

	struct C3DMaterial_
	{
		c3d::Material * internal;
	};

	struct C3DBorderPanelOverlay_
	{
		c3d::BorderPanelOverlay * internal;
	};

	struct C3DPanelOverlay_
	{
		c3d::PanelOverlay * internal;
	};

	struct C3DTextOverlay_
	{
		c3d::TextOverlay * internal;
	};

	struct C3DRenderWindow_
	{
		c3d::RenderWindowUPtr internal;
	};

	struct C3DSampler_
	{
		void setInternal( c3d::SamplerUPtr v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::Sampler * v )noexcept
		{
			internal = v;
		}

		c3d::SamplerUPtr releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::Sampler * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::SamplerUPtr ownInternal;
		c3d::Sampler * internal;
	};

	struct C3DScene_
	{
		void setInternal( c3d::SceneUPtr v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::Scene * v )noexcept
		{
			internal = v;
		}

		c3d::SceneUPtr releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::Scene * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::SceneUPtr ownInternal;
		c3d::Scene * internal;
	};

	struct C3DOverlay_
	{
		void setInternal( c3d::OverlayUPtr v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::Overlay * v )noexcept
		{
			internal = v;
		}

		c3d::OverlayUPtr releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::Overlay * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::OverlayUPtr ownInternal;
		c3d::Overlay * internal;
	};

	struct C3DSceneNode_
	{
		void setInternal( c3d::SceneNodeUPtr v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::SceneNode * v )noexcept
		{
			internal = v;
		}

		c3d::SceneNodeUPtr releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::SceneNode * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::SceneNodeUPtr ownInternal;
		c3d::SceneNode * internal;
	};

	struct C3DCamera_
	{
		void setInternal( c3d::CameraUPtr v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::Camera * v )noexcept
		{
			internal = v;
		}

		c3d::CameraUPtr releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::Camera * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::CameraUPtr ownInternal;
		c3d::Camera * internal;
	};

	struct C3DGeometry_
	{
		void setInternal( c3d::GeometryUPtr v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::Geometry * v )noexcept
		{
			internal = v;
		}

		c3d::GeometryUPtr releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::Geometry * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::GeometryUPtr ownInternal;
		c3d::Geometry * internal;
	};

	struct C3DLight_
	{
		void setInternal( c3d::LightUPtr v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::Light * v )noexcept
		{
			internal = v;
		}

		c3d::LightUPtr releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::Light * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::LightUPtr ownInternal;
		c3d::Light * internal;
	};

	struct C3DLightGroup_
	{
		void setInternal( c3d::LightGroupUPtr v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::LightGroup * v )noexcept
		{
			internal = v;
		}

		c3d::LightGroupUPtr releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::LightGroup * getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::LightGroupUPtr ownInternal;
		c3d::LightGroup * internal;
	};

	struct C3DMesh_
	{
		void setInternal( c3d::MeshRes v )noexcept
		{
			ownInternal = c3d::move( v );
		}

		void setInternal( c3d::MeshResPtr v )noexcept
		{
			internal = v;
		}

		c3d::MeshRes releaseInternal()noexcept
		{
			return c3d::move( ownInternal );
		}

		c3d::MeshResPtr getInternal()const noexcept
		{
			return ownInternal
				? ownInternal.get()
				: internal;
		}

	private:
		c3d::MeshRes ownInternal;
		c3d::MeshResPtr internal;
	};

	struct C3DSubmesh_
	{
		c3d::Submesh * internal;
	};

	struct C3DDirectionalLight_
	{
		c3d::DirectionalLight * internal;
	};

	struct C3DPointLight_
	{
		c3d::PointLight * internal;
	};

	struct C3DSpotLight_
	{
		c3d::SpotLight * internal;
	};

	struct C3DShadow_
	{
		c3d::ShadowConfig * internal;
	};

	struct C3DLineMapping_
	{
		c3d::LineMapping * internal;
	};

	struct C3DTriFaceMapping_
	{
		c3d::TriFaceMapping * internal;
	};

	struct C3DPass_
	{
		c3d::Pass * internal;
	};

	struct C3DSkybox_
	{
		c3d::SkyboxBackgroundUPtr internal;
	};

#ifdef __cplusplus
}
#endif

#endif
