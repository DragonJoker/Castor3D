# CSCN File Format

CSCN files are easily editable text files (since the syntax is understood).

The data types used in scene files are these ones :
- *boolean* : a boolean (*true* or *false*).
- *int* : a simple integer.
- *mask_1* : an hexadecimal integer defining a mask for one component from a colour (*0x000000FF*, for instance).
- *mask_3* : an hexadecimal integer defining a mask for three components from a colour  (*0x00FFFFFF*, for instance).
- *real* : a floating point number, using dot ( . ) as the decimals separator.
- *2, 3, 4 ints* : 2, 3 or 4 integers, separated by commas ( , ) or spaces ( ).
- *2, 3, 4 reals* : 2, 3 or 4 floating point numbers, separated by commas ( , ) or spaces ( ).
- *size* : 2 integers greater than or equal to 0.
- *2x2, 3x3, 4x4 reals matrix* : 2, 3 or 4 groups separated by semi colons ( ; ) of 2, 3 or 4 floating point numbers separated by commas ( *,* ) or spaces ( ).
- *rgb_colour* : the RGB components of colour, expressed in floating point numbers between 0.0 and 1.0.
- *rgba_colour* : the RGBA components of colour, expressed in floating point numbers between 0.0 and 1.0.
- *rgb_hdr_colour* : the RGB components of colour, expressed in floating point numbers greater than or equal to 0.0.
- *rgba_hdr_colour* : the RGBA components of colour, expressed in floating point numbers greater than or equal to 0.0.
- *value* : a string corresponding to a predefined value.
- *name* : a string, wrapped into double quotes ( &quot; ).
- *file* : a string, wrapped into double quotes ( &quot; ), containing a file name and path.
- *folder* : a string, wrapped into double quotes ( &quot; ), containing a folder path.

## Description

The file is split into sections, defined as follows :

    [section-type] "[section-name]"
    {
        // Section description
    }

Example:

    light "Light0"
    {
        type directional
        colour 1.0 1.0 1.0
        intensity 0.8 1.0
    }

Some sections can have child subsections :

    material "Bronze"
    {
        pass
        {
            ambient 0.2125 0.1275 0.054
            diffuse 0.714 0.4284 0.12144
            emissive 0.0
            specular 0.393548 0.271906 0.166721
            shininess 25.6
        }
    }

## Root section

- **debug_overlays** : *boolean*  
  Enables or disables debug overlays.
- **enable_full_loading** : *boolean*  
  Enables or disables loading of passes that aren't used by the scene.
- **materials** : *value*  
  Defines the material type used in the whole file. The possible values are :
  - *phong* : Blinn-Phong materials.
  - *pbr* : PBR materials.
- **default_unit** : *value*  
  Defines the unit length used by the engine. The possible values are :
  - *mm* : Millimetres.
  - *cm* : Centimetres.
  - *m* : Metres.
  - *km* : Kilometres.
  - *yd* : Yards.
  - *ft* : Feet.
  - *in* : Inches.
- **max_image_size**: *int*
  Allows limitation of the loaded images (keeping their aspect ratio).
- **debug_max_image_size**: *int*
  Allows limitation of the loaded images (keeping their aspect ratio), for debug builds.
- **lpv_grid_size**: *int*
  Allows customisation of the grid used by Light Propagation Volumes (default: 32).
- **sampler** : *section*  
  Defines a texture sampler object.
- **material** : *section*  
  Defines a material.
- **loading_screen** : *section*  
  Redefines the loading screen.
- **font** : *section*  
  Defines a font used in text overlays.
- **window** : *section*  
  Defines a render window.
- **panel_overlay** : *section*  
  Defines a simple panel overlay.
- **border_panel_overlay** : *section*  
  Defines a panel overlay with a border.
- **text_overlay** : *section*  
  Defines a panel overlay with a text.
- **scene** : *section*  
  Defines a whole scene.
- **gui** : *section*  
  Defines a global GUI.
- **theme** : *name* *section*  
  Defines a GUI theme.
- **box_layout** : *section*  
  Defines a GUI global box layout.
- **button** : *name* *section*  
  Defines a button GUI control.
- **static** : *name* *section*  
  Defines a static text GUI control.
- **slider** : *name* *section*  
  Defines a slider GUI control.
- **combobox** : *name* *section*  
  Defines a combo box GUI control.
- **listbox** : *name* *section*  
  Defines a list box GUI control.
- **edit** : *name* *section*  
  Defines a text edit GUI control.
- **panel** : *name* *section*  
  Defines a panel GUI control.
- **progress** : *name* *section*  
  Defines a progress bar GUI control.
- **expandable_panel** : *name* *section*  
  Defines an expandable panel GUI control.
- **frame** : *name* *section*  
  Defines a frame GUI control.
- **default_font** : *name*  
  Defines the global default font.
- **button_style** : *name* *section*  
  Defines a button GUI style.
- **static_style** : *name* *section*  
  Defines a static text GUI style.
- **slider_style** : *name* *section*  
  Defines a slider GUI style.
- **combobox_style** : *name* *section*  
  Defines a combo box GUI style.
- **listbox_style** : *name* *section*  
  Defines a list box GUI style.
- **edit_style** : *name* *section*  
  Defines a text edit GUI style.
- **panel_style** : *name* *section*  
  Defines a panel GUI style.
- **progress_style** : *name* *section*  
  Defines a progress bar GUI style.
- **expandable_panel_style** : *name* *section*  
  Defines an expandable panel GUI style.
- **frame_style** : *name* *section*  
  Defines a frame GUI style.
- **scrollbar_style** : *name* *section*  
  Defines a scrollbar GUI style.

## sampler section

- **min_filter** : *value*  
  Value used for minification function. The possible values are :
  - *linear* : linear interpolation.
  - *nearest* : no interpolation.
- **mag_filter** : *value*  
  Value used for magnification function. The possible values are :
  - *linear* : linear interpolation.
  - *nearest* : no interpolation.
- **mip_filter** : *value*  
  Value used for mipmapping function. The possible values are :
  - *linear* : linear interpolation.
  - *nearest* : no interpolation.
- **min_lod** : *real*  
  Defines minimum level of detail value.
- **max_lod** : *real*  
  Defines maximum level of detail value.
- **lod_bias** : *real*  
  Defines the base MIP-Level.
- **u_wrap_mode** : *value*  
  Defines the wrapping mode of the texture, for the U component. The possible values are :
  - *repeat* : The texture is repeated.
  - *mirrored_repeat* : The texture is repeated, each instance of 2 being the mirror of the other one.
  - *clamp_to_border* : The texture is stretched, the object edge colour is defined as the texture edge colour.
  - *clamp_to_edge* : The texture is stretched, the object edge colour is defined as the average of the texture edge colour and the border colour.
- **v_wrap_mode** : *value*  
  Defines the wrapping mode of the texture, for the V component. The possible values are :
  - *repeat* : The texture is repeated.
  - *mirrored_repeat* : The texture is repeated, each instance of 2 being the mirror of the other one.
  - *clamp_to_border* : The texture is stretched, the object edge colour is defined as the texture edge colour.
  - *clamp_to_edge* : The texture is stretched, the object edge colour is defined as the average of the texture edge colour and the border colour.
- **w_wrap_mode** : *value*  
  Defines the wrapping mode of the texture, for the W component. The possible values are :
  - *repeat* : The texture is repeated.
  - *mirrored_repeat* : The texture is repeated, each instance of 2 being the mirror of the other one.
  - *clamp_to_border* : The texture is stretched, the object edge colour is defined as the texture edge colour.
  - *clamp_to_edge* : The texture is stretched, the object edge colour is defined as the average of the texture edge colour and the border colour.
- **border_colour** : *rgba_colour*  
  Defines the non textured border colour.
  - *float_transparent_black* : Transparent black.
  - *int_transparent_black* : Transparent black.
  - *float_opaque_black* : Opaque black.
  - *int_opaque_black* : Opaque black.
  - *float_opaque_white* : Opaque white.
  - *int_opaque_white* : Opaque white.
- **anisotropic_filtering** : *boolean*  
  Defines if anisotropic filtering is enabled (if supported).
- **max_anisotropy** : *real*  
  Defines the maximum degree of anisotropy.
- **comparison_mode** : *value*  
  Defines the sampler comparison mode. The possible values are :
  - *none* : Traditional sampler.
  - *ref_to_texture* : Shadow sampler.
- **comparison_func** : *value*  
  Defines the sampler comparison function. The possible values are :
  - *always* : The sample colour is always applied.
  - *less* : The sample colour is applied if its value is less than the reference value.
  - *less_or_equal* : The sample colour is applied if its value is less than or equal to the reference value.
  - *equal* : The sample colour is applied if its value is equal to the reference value.
  - *not_equal* : The sample colour is applied if its value is different from the reference value.
  - *greater_or_equal* : The sample colour is applied if its value is greater than or equal to the reference value.
  - *greater* : The sample colour is applied if its value is greater than the reference value.
  - *never* : The sample colour is never applied.

## material section

Materials can be multi-pass, so you can declare more than one pass subsection.
- **pass** : *section*  
  Defines a new section describing a texture.
- **render_pass** : *name*  
  Defines the name of the render pass used to render objects using this material.

### pass section
- **two_sided** : *boolean*  
  Tells the pass is two sided (default is false).
- **lighting** : *boolean*  
  Enables lighting (default is true).  
- **lighting_model** : *value*
  Defines the lighting model type. The possible values are :
  - *phong* : Blinn-Phong.
  - *pbr* : PBR.
- **pickable** : *boolean*  
  Enables picking (default is true).  
- **texture_unit** : *section*  
  Defines a new section describing a texture unit.
- **reflections** : *boolean*  
  Enables reflections (default is false).
- **refraction_ratio** : *real*  
  Defines the refraction ratio.
- **has_refraction** : *boolean*  
  Tells if the pass uses refraction. Note that if there is no refraction map, the refraction is still applied, using only the skybox.
- **alpha_blend_mode** : *value*  
  Alpha blending mode name, can be one of:
  - *none* : No alpha blending.
  - *additive* : Source and destination alpha values are added.
  - *multiplicative* : Source and destination alpha values are multiplied.
  - *interpolative* : Source and destination alpha values are interpolated.
- **colour_blend_mode** : *value*  
  Colour blending mode name, can be one of:
  - *none* : No colour blending.
  - *additive* : Source and destination colour values are added.
  - *multiplicative* : Source and destination colour values are multiplied.
  - *interpolative* : Source and destination colour values are interpolated.
- **colour_srgb** : *rgb_colour*  
  Defines the pass colour.
- **diffuse** : *rgb_colour*  
  Defines the pass colour.
- **colour_hdr** : *rgb_hdr_colour*  
  Defines the pass colour.
- **albedo** : *rgb_hdr_colour*  
  Defines the pass colour.
- **specular_colour** : *rgb_colour*  
  Defines the colour of the specular reflections.
- **specular_factor_** : *real*  
  Defines the intensity of the specular reflections.
- **metalness** : *real (between 0 and 1)*  
  Defines the metalness.
- **glossiness** : *real (between 0 and 1)*  
  Defines the glossiness.
- **roughness** : *real (between 0 and 1)*  
  Defines the roughness.
- **shininess** : *real (between 0 and 128)*  
  Defines the Phong specular exponent.
- **emissive_colour** : *rgb_colour*  
  Defines emissive colour.
- **emissive_factor** : *real*  
  Defines emissive factor.
- **ambient_colour** : *rgb_colour*  
  Defines ambient colour.
- **ambient_factor** : *real*  
  Defines ambient factor.
- **attenuation_colour** : *rgb_colour*  
  Defines the attenuation colour.
- **attenuation_distance** : *real*  
  Defines the attenuation distance.
- **thickness_factor** : *real*  
  Defines the thickness factor.
- **transmission** : *real*  
  Defines the transmission factor.
- **clearcoat_factor** : *real*  
  Defines the clearcoat factor.
- **clearcoat_roughness_factor** : *real*  
  Defines the clearcoat roughness.
- **iridescence_factor** : *real*  
  Defines the iridescence factor.
- **iridescence_ior** : *real*  
  Defines the iridescence layer index of refraction.
- **iridescence_min_thickness** : *real*  
  Defines the iridescence layer minimum thickness.
- **iridescence_max_thickness** : *real*  
  Defines the iridescence layer minimum thickness.
- **sheen_colour** : *real*  
  Defines the sheen layer colour.
- **sheen_roughness** : *real*  
  Defines the sheen layer roughness.
- **opacity** : *real (between 0 and 1)*  
  Defines the opacity factor.
- **bw_accumulation** : *int between 0 and 5*  
  Defines the accumulation function, for blended weighted rendering.
- **alpha_func** : func : *value* ref-val: *real*  
  Defines the way alpha rejection is applied to the pass. The second parameter is the reference value used in alpha rejection function. The first parameter values can be :
  - *always* : The sample colour is always applied.
  - *less* : The sample colour is applied if its alpha component is less than the second parameter.
  - *less_or_equal* : The sample colour is applied if its alpha component is less than or equal to the second parameter.
  - *equal* : The sample colour is applied if its alpha component is equal to the second parameter.
  - *not_equal* : The sample colour is applied if its alpha component is different from the second parameter.
  - *greater_or_equal* : The sample colour is applied if its alpha component is greater than or equal to the second parameter.
  - *greater* : The sample colour is applied if its alpha component is greater than the second parameter.
  - *never* : The sample colour is never applied.
- **blend_alpha_func** : func : *value* ref-val: *real*  
  Defines the way alpha rejection is applied to the pass, when blending is active. The second parameter is the reference value used in alpha rejection function. The first parameter values can be :
  - *always* : The sample colour is always applied.
  - *less* : The sample colour is applied if its alpha component is less than the second parameter.
  - *less_or_equal* : The sample colour is applied if its alpha component is less than or equal to the second parameter.
  - *equal* : The sample colour is applied if its alpha component is equal to the second parameter.
  - *not_equal* : The sample colour is applied if its alpha component is different from the second parameter.
  - *greater_or_equal* : The sample colour is applied if its alpha component is greater than or equal to the second parameter.
  - *greater* : The sample colour is applied if its alpha component is greater than the second parameter.
  - *never* : The sample colour is never applied.
- **mixed_interpolation** : *section*  
  Helper that sets **alpha_blend_mode** to *interpolative*, **blend_alpha_func** to *less_or_equal* and **alpha_func** to *greater* with a ref. value to 0.95.
- **fractal** : *boolean*  
  Enables fractal UV mapping (default is false).  
- **untile** : *boolean*  
  Enables untiled UV mapping (default is false).
- **parallax_occlusion** : *value*  
  Enables or disables parallax occlusion mapping (needs a normal map and a height map), can be one of:
  - *none* : Disabled.
  - *one* : No tiling.
  - *repeat* : Repeated tiling.
- **subsurface_scattering** : *section*  
  Defines a new section describing subsurface scattering for the pass.
- **transmittance** : *real*  
  Transmittance factor (for subsurface scattering).
  

#### texture_unit section

- **image** : *file*  
  Defines the image file name.
- **render_target** : *section*  
  Defines a new section describing a render target.
- **channel** : *value {| value}*  
  The channels to which the texture is bound. Can be one of the following :
  - *diffuse* : Diffuse lighting colour.
  - *albedo* : Base colour.
  - *specular* : Specular colour.
  - *specular_factor* : Specular factor.
  - *metallic* : Metallic factor.
  - *shininess* : Specular exponent.
  - *glossiness* : Glossiness factor.
  - *roughness* : Roughness factor.
  - *opacity* : Opacity factor.
  - *emissive* : Emissive colour.
  - *transmission* : Transmission factor.
  - *transmittance* : Transmittance factor (for subsurface scattering).
  - *occlusion* : Occlusion factor.
  - *normal* : Normals.
  - *height* : Height.
  - *iridescence* : Iridescence layer factor.
  - *iridescence_thickness* : Iridescence layer thickness factor.
  - *clearcoat* : Clearcoat factor.
  - *clearcoat_normal* : Clearcoat layer normal map.
  - *clearcoat_roughness* : Clearcoat layer roughness.
  - *thickness* : Thickness factor.
- **sampler** : *name*  
  Defines the sampler object used by the texture.
- **level_count** : *int*
  Defines the maximum mip levels count.
- **diffuse_mask** : *mask_3*  
  Defines the components from the texture used for the colour.
- **albedo_mask** : *mask_3*  
  Defines the components from the texture used for the colour.
- **colour_mask** : *mask_3*  
  Defines the components from the texture used for the colour.
- **specular_mask** : *mask_3*  
  Defines the components from the texture used for the specular colour.
- **specular_factor_mask** : *mask_1*  
  Defines the components from the texture used for the specular factor.
- **metalness_mask** : *mask_1*  
  Defines the component from the texture used for the metallic factor
- **shininess_mask** : *mask_1*  
  Defines the component from the texture used for the specular exponent.
- **glossiness_mask** : *mask_1*  
  Defines the component from the texture used for the glossiness factor.
- **roughness_mask** : *mask_1*  
  Defines the component from the texture used for the roughness factor.
- **opacity_mask** : *mask_1*  
  Defines the component from the texture used for the opacity factor.
- **emissive_mask** : *mask_3*  
  Defines the components from the texture used for the emissive colour.
- **transmittance_mask** : *mask_1*  
  Defines the component from the texture used for the transmittance factor (for subsurface scattering).
- **occlusion_mask** : *mask_1*  
  Defines the component from the texture used for the occlusion factor.
- **normal_mask** : *mask_3*  
  Defines the components from the texture used for the normals.
- **normal_directx** : *boolean*  
  Tells if the texture normals are expressed for DirectX (green component will then be inverted).
- **normal_factor** : *real*  
  Defines the normal factor.
- **normal_2channels** : *boolean*  
  Tells if the texture normals are expressed as a 2 channels texture (Z will be reconstructed).
- **height_mask** : *mask_1*  
  Defines the component from the texture used for the height.
- **height_factor** : *real*  
  The height factor multiplier.
- **thickness_mask** : *mask_1*  
  Defines the component from the texture used for the thickness factor.
- **transmission_mask** : *mask_1*  
  Defines the component from the texture used for the transmission factor.
- **clearcoat_normal_mask** : *mask_3*  
  Defines the component from the texture used for the clearcoat layer normal map.
- **clearcoat_mask** : *mask_1*  
  Defines the component from the texture used for the clearcoat factor.
- **clearcoat_roughness_mask** : *mask_1*  
  Defines the component from the texture used for the clearcoat layer roughness factor.
- **iridescence_mask** : *mask_1*  
  Defines the component from the texture used for the iridescence factor.
- **iridescence_thickness_mask** : *mask_1*  
  Defines the component from the texture used for the iridescence layer thickness factor.
- **sheen_mask** : *mask_3*  
  Defines the component from the texture used for the sheen layer colour.
- **sheen_roughness_mask** : *mask_1*  
  Defines the component from the texture used for the sheen layer roughness factor.
- **invert_y** : *boolean*  
  Defines if the image must be inverted along Y axis.
- **transform** : *section*  
  Defines the texture base transformation.
- **tileset** : *2 ints*  
  Defines the tile set dimensions of the texture.
- **tiles** : *int*  
  Defines the tile count of the texture.
- **animation** : *section*  
  Defines the texture transformation animation.
- **texcoord_set** : *section*  
  Defines the target texture coordinates set this texture will be using.

##### transform section

- **rotate** : *real*  
  Defines the texture rotation.
- **translate** : *2 reals*  
  Defines the texture translation.
- **scale** : *2 reals*  
  Defines the texture scaling.
- **tile** : *2 ints*  
  Defines the selected tile.

##### animation section

- **rotate** : *real*  
  Defines the texture rotation animation speed.
- **translate** : *2 reals*  
  Defines the texture translation animation speed.
- **scale** : *2 reals*  
  Defines the texture scaling animation speed.
- **tile** : *2 ints*  
  Defines if the tiles are animated.

#### shader_program section

- **compute_program** : *section*  
  Defines a new section describing the compute program.

#### compute_program section

- **file** : *file*  
  Shader file name
- **group_sizes** : *3 ints*  
  Defines the X, Y and Z dispatch counts.

#### subsurface_scattering section

- **strength** : *real*  
  Defines the strength of the effect.
- **gaussian_width** : *real*  
  Defines the width of the Gaussian blur.
- **thickness_scale** : *real*  
  Defines the thickness scaling.
- **transmittance_profile** : *section*  
  Defines a new section describing the transmittance profile.

#### transmittance_profile section

- **factor** : *vec4f*  
  Defines the three RGB components of the colour, and the fourth component is used for the exponent of that colour.

## font section

- **file** : *file*  
  Defines the file holding the font.
- **height** : *int*  
  Defines the height (precision) of the font.

## scene section

- **ambient_light** : *colour*  
  Defines the ambient lighting colour. For PBR materials, defines the influence of the IBL on the scene.
- **background_colour** : *colour*  
  Defines the background colour.
- **background_image** : *file*  
  Defines the background image.
- **import** : *section*  
  Allows scene import from a CSCN file or another file format supported by Castor3D importer plug-ins.
- **include** : *file*  
  Includes a scene file, allowing you to split your scene in multiple files.
- **fog_type** : *value*  
  Defines the fog type for the scene. Possible values are:
  - *linear*: Fog intensity increases linearly with distance to camera.
  - *exponential*: Fog intensity increases exponentially with distance to camera.
  - *squared_exponential*: Fog intensity increases even more with distance to camera.
- **fog_density** : *real*  
  Defines the fog density, which is multiplied by the distance, according to chosen fog type.
- **directional_shadow_cascades** : *int*  
  Defines the number of cascades for directional light sources.
- **scene_node** : *section*  
  Defines a new section describing a scene node for objects, lights or billboards.
- **camera_node** : *section*  
  Defines a new section describing a scene node for cameras.
- **light** : *section*  
  Defines a new section describing a light source.
- **object** : *section*  
  Defines a new section describing an object.
- **billboard** : *section*  
  Defines a new section describing a billboards list.
- **camera** : *section*  
  Defines a new section describing a camera.
- **panel_overlay** : *section*  
  Defines a new section describing a simple panel overlay.
- **border_panel_overlay** : *section*  
  Defines a new section describing a simple panel overlay with a border.
- **text_overlay** : *section*  
  Defines a new section describing a simple panel overlay with text.
- **animated_object_group** : *section*  
  Defines a new section describing an animated object group, with common animations.
- **skeleton** : *section*  
  Defines a new section describing a skeleton, that can be used with one or more meshes.
- **mesh** : *section*  
  Defines a new section describing a mesh, that can be used with one or more objects.
- **particle_system** : *section*  
  Defines a new section describing a particle system.
- **skybox** : *section*  
  Defines a new section describing the skybox.
- **sampler** : *section*  
  Defines a new section describing a sampler.
- **voxel_cone_tracing** : *section*  
  Defines a new section describing the voxel cone tracing configuration.
- **gui** : *section*  
  Defines a GUI for the scene.
- **theme** : *name* *section*  
  Defines a GUI theme.
- **box_layout** : *section*  
  Defines a box layout for the scene's GUI.
- **button** : *name* *section*  
  Defines a button GUI control.
- **static** : *name* *section*  
  Defines a static text GUI control.
- **slider** : *name* *section*  
  Defines a slider GUI control.
- **combobox** : *name* *section*  
  Defines a combo box GUI control.
- **listbox** : *name* *section*  
  Defines a list box GUI control.
- **edit** : *name* *section*  
  Defines a text edit GUI control.
- **panel** : *name* *section*  
  Defines a panel GUI control.
- **progress** : *name* *section*  
  Defines a progress bar GUI control.
- **expandable_panel** : *name* *section*  
  Defines an expandable panel GUI control.
- **frame** : *name* *section*  
  Defines a frame GUI control.
- **default_font** : *name*  
  Defines the default font for the scene's GUI.
- **button_style** : *name* *section*  
  Defines a button GUI style.
- **static_style** : *name* *section*  
  Defines a static text GUI style.
- **slider_style** : *name* *section*  
  Defines a slider GUI style.
- **combobox_style** : *name* *section*  
  Defines a combo box GUI style.
- **listbox_style** : *name* *section*  
  Defines a list box GUI style.
- **edit_style** : *name* *section*  
  Defines a text edit GUI style.
- **panel_style** : *name* *section*  
  Defines a panel GUI style.
- **progress_style** : *name* *section*  
  Defines a progress bar GUI style.
- **expandable_panel_style** : *name* *section*  
  Defines an expandable panel GUI style.
- **frame_style** : *name* *section*  
  Defines a frame GUI style.
- **scrollbar_style** : *name* *section*  
  Defines a scrollbar GUI style.

### Section skybox

- **visible** : *boolean*  
  Defines the skybox visibility (if not visible, the skybox will still be used for IBL).
- **equirectangular** : *file* *int*  
  Defines the skybox from an HDRi image. The second parameter defines the skybox dimensions.
- **cross** : *file*  
  Defines the skybox from a cross image.
- **left** : *file*  
  Defines the left image of the skybox.
- **right** : *file*  
  Defines the right image of the skybox.
- **top** : *file*  
  Defines the top image of the skybox.
- **bottom** : *file*  
  Defines the bottom image of the skybox.
- **front** : *file*  
  Defines the front image of the skybox.
- **back** : *file*  
  Defines the back image of the skybox.

### voxel_cone_tracing section

- **enabled** : *boolean*  
  Defines the VCT activation status.
- **conservative_rasterization** : *boolean*  
  Defines the conservative rasterization activation status.
- **temporal_smoothing** : *boolean*  
  Defines the temporal smoothing activation status.
- **secondary_bounce** : *boolean*  
  Defines the second bounce activation status.
- **grid_size** : *real*  
  Defines the dimensions of the 3D texture holding the voxels.
- **num_cones** : *real*  
  Defines the number of used cones.
- **max_distance** : *real*  
  Defines the maximum distance a ray can reach.
- **ray_step_size** : *int*  
  Defines the size of a ray step.
- **voxel_size** : *real*  
  Defines the size a voxel represents.

### import section

- **file** : *path*  
  Imports the scene from the given file name.
- **file_anim** : *path*  
  Imports animations from the given file name.
- **prefix** : *text*  
  Defines the imported objects' name prefix, used to prevent names clash.
- **rescale** : *real*  
  Rescales the imported objects by given factor, on three axes.
- **pitch** : *real*  
  Rotates the imported objects by given angle (in degrees) along X axis.
- **yaw** : *real*  
  Rotates the imported objects by given angle (in degrees) along Y axis.
- **roll** : *real*  
  Rotates the imported objects by given angle (in degrees) along Z axis.
- **no_optimisations** : *boolean*  
  Disables all optimisations during import.
- **emissive_mult** : *real*  
  Applies a multiplier on all importer emissive factors.
- **recenter_camera** : *nom*  
  Centers on the imported result the camera with the given name.
- **preferred_importer** : *nom*  
  Allows use of an importer instead of another one, when more than one importer supports the imported file type.
- **ignore_vertex_colour** : *boolean*  
  Allows to ignore the vertex colour component when importing a mesh.

### scene_node and camera_node sections

- **static** : *name*  
  Tells the node is static, hence can't be moved/rotated/scaled after its definition.
- **parent** : *name*  
  Defines this node’s parent. The default parent node is the root node.
- **position** : *3 reals*  
  Node position relative to its parent.
- **orientation** : *4 reals*  
  A quaternion holding node orientation relative to its parent.
- **rotate** : *4 reals*  
  Accumulates the node's orientation with given quaternion.
- **direction** : *3 reals*  
  Holds node direction relative to its parent.
- **scale** : *3 reals*  
  Node scale relative to its parent.

### light section

- **type** : *value*  
  Three light source types exist in Castor3D :
  - *directional* : directional light (like the sun).
  - *point_light* : a positioned source which emits in all directions.
  - *spot_light* : a positioned source which emits in an oriented cone.
- **colour** : *rgb_colour*  
  Defines the colour for this source.
- **intensity** : *2 reals*  
  Defines the diffuse and specular intensities for this source.
- **attenuation** : *3 reals*  
  Defines the three attenuation components : constant, linear and quadratic. This attenuation is computed from the distance to the light source.  
  Only for spot_light and point_light.
- **range** : *real*  
  Defines the light source range.  
  Only for spot_light and point_light.
- **inner_cut_off** : *real*  
  Defines the inner angle of the emission cone.  
  Only for spot_light.
- **outer_cut_off** : *real*  
  Defines the outer angle of the emission cone.  
  Only for spot_light.
- **exponent** : *real*  
  Defines the attenuation computed with the distance from the emission cone centre.  
  Only for spot_light.
- **parent** : *name*  
  Defines the node which this light source is attached to.
- **shadows** : *section*  
  Defines a new section describing the shadows for the light source.

#### shadows section

- **producer** : *boolean*  
  Tells if the light source produces shadows (*true*) or not (*false*).
- **filter** : *valeur*  
  Defines the type of shadows to use. Can be one of the following:
  - *raw* : No filter.
  - *pcf* : PCF filter.
  - *variance* : Variance Shadow Map.
- **global_illumination** : *value*  
  Defines the global illumination type. Can be one of the following:
  - *none* : None applied.
  - *lpv* : Light Propagation Volumes.
  - *lpv_geometry* : Light Propagation Volumes with Geometry injection.
  - *layered_lpv* : Layered Light Propagation Volumes (directional light sources only).
  - *layered_lpv_geometry* : Layered Light Propagation Volumes with Geometry injection (directional light sources only).
- **volumetric_steps** : *int*  
  Defines the number of steps performed by the ray to compute the volumetric light scattering.
- **volumetric_scattering** : *real*  
  Defines the volumetric light scattering factor.
- **raw_config** : *section*  
  Defines a new section describing the configuration for RAW shadows.
- **pcf_config** : *section*  
  Defines a new section describing the configuration for PCF shadows.
- **vsm_config** : *section*  
  Defines a new section describing the configuration for VSM shadows.

##### raw_config section

- **min_offset** : *real*  
  Defines the minimal offset to apply to the shadow value.
- **max_slope_offset** : *real*  
  Defines the mawimal offset, slope wise, to apply to the shadow value.

##### pcf_config section

- **min_offset** : *real*  
  Defines the minimal offset to apply to the shadow value.
- **max_slope_offset** : *real*  
  Defines the mawimal offset, slope wise, to apply to the shadow value.
- **filter_size** : *real*  
  Defines the PCF filter size.
- **sample_count** : *real*  
  Defines the PCF filter sample count.

##### vsm_config section

- - **min_variance** : *real*  
  Defines the minimal variance factor to apply to the shadow value.
- **light_bleeding_reduction** : *real*  
  Defines the light bleeding reduction factor.

### object section

- **parent** : *name*  
  Defines the node which this object is attached to.
- **mesh** : *name*  
  Defines the mesh this object uses.
- **material** : *name*  
  Name of a material, defined in a .cmtl file or in this file. Applies this material too all the submeshes.
- **materials** : *section*  
  New section used to specify each submesh’s material.
- **cast_shadows** : *boolean*  
  Defines if the object casts shadows (*true*, default value) or not (*false*).
- **receive_shadows** : *boolean*  
  Defines if the object receives shadows (*true*, default value) or not (*false*).
- **cullable** : *boolean*  
  Defines if the object is cullable (*true*, default value) or not (*false*).

#### materials section

- **material** : *int*, *name*  
  Submesh index, and material name for this submesh.

### billboard section

Allows the definition of billboards that share the same material and dimensions.

- **parent** : *name*  
  Defines the parent scene node.
- **type** : *value*  
  Defines the type of billboard. Possible values are:
  - *cylindrical*: The billboard faces the camera, except for their Y axis, which remains still.
  - *spherical*: The billboard faces the camera on all axes.
- **positions** : *section*  
  Defines a new section describing each billboard position.
- **material** : *name*  
  Defines the material used by every billboard of this list.
- **dimensions** : *size*  
  Defines billboards dimensions.
- **size** : *value*  
  Defines the billboards sizing. Possible values are:
  - *dynamic*: The size varies, depending on the distance to camera.
  - *fixed*: The size is fixed, where the camera is.

#### positions section

- **pos** : *3 reals*  
  Defines the relative position of a billboard.

### particle_system section

- **parent** : *name*  
  Defines the node which this particle system is attached to.
- **particles_count** : *uint*  
  Defines the maximum supported number of particles.
- **material** : *name*  
  Defines the material used by the particles.
- **dimensions** : *size*  
  Defines particles dimensions.
- **particle** : *section*  
  New section to describe a particle.
- **cs_shader_program** : *section*  
  New section defining the program used to handle the particles.

#### particle section

- **type** : *name*  
  Defines the particle type name.
- **variable** : *name* *value* *default_value*  
  Defines a particle member variable.
  First parameter is the member name
  Second parameter is the member type, can be:
  - *int* : 1 signed integer.
  - *vec2i* : 2 signed integers.
  - *vec3i* : 3 signed integers.
  - *vec4i* : 4 signed integers.
  - *uint* : 1 unsigned integer.
  - *vec2ui* : 2 unsigned integers.
  - *vec3ui* : 3 unsigned integers.
  - *vec4ui* : 4 unsigned integers.
  - *float* : 1 simple precision floating point number.
  - *vec2f* : 2 simple precision floating point numbers.
  - *vec3f* : 3 simple precision floating point numbers.
  - *vec4f* : 4 simple precision floating point numbers.
  - *mat2x2f* : 2x2 simple precision floating point numbers matrix.
  - *mat3x3f* : 3x3 simple precision floating point numbers matrix.
  - *mat4x4f* : 4x4 simple precision floating point numbers matrix.
  Third parameter is the default value.

### camera section

- **parent** : *name*  
  Defines the parent CameraNode.
- **primitive** : *value*  
  Defines the display type. Can be one of :
  - *points* : Points.
  - *lines* : Disjointed lines.
  - *line_loop* : Jointed lines loop.
  - *line_strip* : Joined lines.
  - *triangles* : Disjointed triangles.
  - *triangle_strip* : Jointed triangles.
  - *triangle_fan* : Triangles jointed using the first point.
  - *quads* : Disjointed quads.
  - *quad_strip* : Jointed quads.
  - *polygon* : Polygons.
- **viewport** : *section*  
  Defines the camera view port.
- **hdr_config** : *section*  
  Defines a new section describing the HDR configuration.

#### viewport section

- **type** : *value*  
  Viewport type. Can be one of :
  - *ortho* : Orthographic viewport.
  - *perspective* : Perspective viewport.
  - *frustum* : Frustum viewport.
- **left** : *real*  
  Defines the minimum displayed X coordinate.
- **right** : *real*  
  Defines the maximum displayed X coordinate.
- **top** : *real*  
  Defines the minimum displayed Y coordinate.
- **bottom** : *real*  
  Defines the maximum displayed Y coordinate.
- **near** : *real*  
  Defines the minimum displayed Z coordinate.
- **far** : *real*  
  Defines the maximum displayed Z coordinate.
- **size** : *size*  
  Defines the window display size (in pixels).
- **fov_y** : *real*  
  Defines the vertical field of view angle, in radians.
- **aspect_ratio** : *real*  
  Defines the global window aspect ratio (1.33333 for 4/3, 1.77777 for 16/9 … ).

#### hdr_config section

- **exposure** : *real*  
  Defines the scene’s exposure.
- **gamma** : *real*  
  Defines the gamma correction.

### animated_object_group section

- **animated_object** : *name*  
  Adds the object with the given name to the group.
- **animated_mesh** : *name*  
  Adds the mesh from the object with the given name to the group.
- **animated_skeleton** : *name*  
  Adds the skeleton from the object with the given name to the group.
- **animated_node** : *name*  
  Adds the node with the given name to the group.
- **animation** : *name*  
  Adds the animation with the given name to the group’s common animations list.
- **start_animation** : *name*  
  Starts the given animation.
- **pause_animation** : *name*  
  Pauses the given animation (which must have been started first).

#### animation section

- **looped** : *boolean*  
  Defines if the animation is looped (*true*) or not (*false*, default value).
- **scale** : *real*  
  Defines the time scale of the animation (can be negative, the animation will then be played backwards).
- **start_at** : *real*  
  Defines start index of the animation.
- **stop_at** : *real*  
  Defines stop index of the animation.
- **interpolation** : *value*  
  Defines interpolation between keyframes, one of :
  - *nearest* : No interpolation.
  - *linear* : Linear interpolation.

## skeleton section

- **import** : *file* *&lt;options&gt;*  
  Allows import of skeleton data from a file, in CMSH file format or any format supported by Castor3D import plug-ins. Only if the mesh type is custom. This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting skeleton by given factor, on three axes.
  - *preferred_importer*=*name* : Allows use of an importer instead of another one, when more than one importer supports the imported file type.
- **import_anim** : *file* *&lt;options*&gt;  
  Allows import of skeleton animations from a file.  
  This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting mesh by given factor, on three axes.
  - *preferred_importer*=*name* : Allows use of an importer instead of another one, when more than one importer supports the imported file type.

## mesh section

- **type** : *name*  
  Mesh type name, one of :
  - *custom* : manually defined mesh or imported mesh.
  - *cube* : a cube, user must then define its three dimensions.
  - *cone* : a cone, user must then define its radius and height.
  - *cylinder* : a cylinder, user must then define its radius and height.
  - *sphere* : a sphere with quad faces, user must then define the subdivisions count and the radius.
  - *icosahedron* : a sphere with triangular faces, user must then define the subdivisions count and the radius.
  - *torus* : a torus, user must then define the internal and external subdivisions count and the internal and external radius.
  - *plane* : a plane, user must then define the width and depth subdivisions count and the width and depth.
- **skeleton** : *name*  
  Defines the skeleton used by the mesh.
- **import** : *file* *&lt;options&gt;*  
  Allows import of mesh data from a file, in CMSH file format or any format supported by Castor3D import plug-ins. Only if the mesh type is custom. This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting mesh by given factor, on three axes.
  - *pitch*=*real* : Rotates the resulting mesh by given angle (in degrees) along X axis.
  - *yaw*=*real* : Rotates the resulting mesh by given angle (in degrees) along Y axis.
  - *roll*=*real* : Rotates the resulting mesh by given angle (in degrees) along Z axis.
  - *preferred_importer*=*name* : Allows use of an importer instead of another one, when more than one importer supports the imported file type.
- **import_morph_target** : *file* *&lt;options&gt;*  
  Allows import of morph target data from a file, in CMSH file format or any format supported by Castor3D import plug-ins. Only if the mesh type is custom. This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting mesh by given factor, on three axes.
  - *pitch*=*real* : Rotates the resulting mesh by given angle (in degrees) along X axis.
  - *yaw*=*real* : Rotates the resulting mesh by given angle (in degrees) along Y axis.
  - *roll*=*real* : Rotates the resulting mesh by given angle (in degrees) along Z axis.
  - *preferred_importer*=*name* : Allows use of an importer instead of another one, when more than one importer supports the imported file type.
- **import_anim** : *file* *&lt;options*&gt;  
  Allows import of mesh animations from a file.  
  This directive must happen after a first import directive.  
  Available only if the mesh type is *custom*.  
  This directive can accept few optional parameters :
  - *rescale*=*real* : Rescales the resulting mesh by given factor, on three axes.
  - *pitch*=*real* : Rotates the resulting mesh by given angle (in degrees) along X axis.
  - *yaw*=*real* : Rotates the resulting mesh by given angle (in degrees) along Y axis.
  - *roll*=*real* : Rotates the resulting mesh by given angle (in degrees) along Z axis.
  - *preferred_importer*=*name* : Allows use of an importer instead of another one, when more than one importer supports the imported file type.
- **division** : *name* *int*  
  Allows the mesh subdivision, using a supported Castor3D divider plug-in algorithm. The second parameter is the application count of the algorithm (its applied recursively).
- **submesh** : *section*  
  Defines a new section describing a submesh. Only if the mesh type is custom.
- **morph_animation** : *section*  
  Defines a new section describing a morphing animation.
- **default_materials** : *section*  
  New section used to specify each submesh’s material.

### submesh section

- **vertex** : *3 reals*  
  Defines a vertex position.
- **uv** : *2 reals*  
  Defines the UV texture coordinates for the previously declared vertex.
- **uvw** : *3 reals*  
  Defines the UVW texture coordinates for the previously declared vertex.
- **normal** : *3 reals*  
  Defines the normal coordinates for the previously declared vertex.
- **tangent** : *3 reals*  
  Defines the tangent coordinates for the previously declared vertex.
- **face** : *3 or 4 integers*  
  Defines a face using the three or four vertices whose indices are given. If more than three indices are given, creates the appropriate count of triangular faces.
- **face_uv** : *as much uv as the face indices*  
  Defines the UV coordinates for each vertex of the previously declared face.
- **face_uvw** : *as much uvw as the face indices*  
  Defines the UVW coordinates for each vertex of the previously declared face.
- **face_normals** : *as much 3 reals groups as the face indices*  
  Defines the normals coordinates for each vertex of the previously declared face.
- **face_tangents** : *as much 3 reals groups as the face indices*  
  Defines the tangents coordinates for each vertex of the previously declared face.
  
### morph_animation section

- **target_weight** : *real* *int* *real*  
  Defines the weight for a morph target. First value is a time index, in seconds. Second value is the morph target index. Third value is the target weight.

### default_materials section

- **material** : *int*, *name*  
  Submesh index, and material name for this submesh.

## panel_overlay section

- **material** : *name*  
  Defines the material used by the panel.
- **position** : *2 reals*  
  Defines the overlay position, relative to its parent (or to screen, if no parent).
- **size** : *2 reals*  
  Defines the overlay size, relative to its parent (or to screen, if no parent).
- **pxl_position** : *2 ints*  
  Defines the absolute position for the overlay, in pixels.
- **pxl_size** : *2 ints*  
  Defines the absolute size for the overlay, in pixels.
- **uv** : *4 reals*  
  Defines the UV for the overlay (left, top, right, bottom).
- **panel_overlay** : *name* *section*  
  Defines a new section describing a simple panel children overlay.
- **border_panel_overlay** : *name* *section*  
  Defines a new section describing a border panel children overlay.
- **text_overlay** : *name* *section*  
  Defines a new section describing a text panel children overlay.

## border_panel_overlay section

- **material** : *name*  
  Defines the material used by the panel.
- **position** : *2 reals*  
  Defines the overlay position, relative to its parent (or to screen, if no parent).
- **size** : *2 reals*  
  Defines the overlay size, relative to its parent (or to screen, if no parent).
- **pxl_position** : *2 ints*  
  Defines the absolute position for the overlay, in pixels.
- **pxl_size** : *2 ints*  
  Defines the absolute size for the overlay, in pixels.
- **center_uv** : *4 reals*  
  Defines the UV for the center of the overlay (left, top, right, bottom).
- **border_material** : *name*  
  Defines the material used for the panel’s border.
- **border_position** : *value*  
  Defines the border’s position, can be one of:
  - *internal* : The border is inside the overlay.
  - *middle* : The border is half inside, half outside the overlay.
  - *external* : The border is outside the overlay.
- **border_size** : *4 reals*  
  Defines the borders sizes (left right, top, bottom).
- **pxl_border_size** : *2 ints*  
  Defines the absolute border size, in pixels.
- **border_inner_uv** : *4 reals*  
  Defines the UV for the border of the overlay, inner side (left, top, right, bottom).
- **border_outer_uv** : *4 reals*  
  Defines the UV for the border of the overlay, outer side (left, top, right, bottom).
- **panel_overlay** : *name* *section*  
  Defines a new section describing a simple panel children overlay.
- **border_panel_overlay** : *name* *section*  
  Defines a new section describing a border panel children overlay.
- **text_overlay** : *name* *section*  
  Defines a new section describing a text panel children overlay.

## text_overlay section

- **material** : *name*  
  Defines the material used by the panel.
- **position** : *2 reals*  
  Defines the overlay position, relative to its parent (or to screen, if no parent).
- **size** : *2 reals*  
  Defines the overlay size, relative to its parent (or to screen, if no parent).
- **pxl_position** : *2 ints*  
  Defines the absolute position for the overlay, in pixels.
- **pxl_size** : *2 ints*  
  Defines the absolute size for the overlay, in pixels.
- **font** : *name*  
  Defines the font used to display the text.
- **text** : *texte*  
  Defines the displayed text.
- **text_wrapping** : *value*  
  Defines the way the text is cut when a line overflows the overlay dimensions. Can be one of:
  - *none* : The text is not cut (the part that overflows won’t be displayed, though).
  - *break* : The text is cut at the letter (the words will be cut).
  - *break_words* : The text is cut at the word (the words remain uncut).
- **vertical_align** : *value*  
  Defines the text vertical alignment:
  - *top* : Align on top.
  - *center* : Vertically center.
  - *bottom* : Align on bottom.
- **horizontal_align** : *value*  
  Defines the text horizontal alignment:
  - *left* : Align on left.
  - *center* : Horizontally center.
  - *right* : Align on right.
- **texturing_mode** : *value*  
  Defines the way the texture is applied:
  - *letter* : The texture is applied on each letter.
  - *text* : The texture is applied on the whole text.
- **line_spacing_mode** : *value*  
  Defines the height of the lines:
  - *own_height* : Each line has its own height.
  - *max_lines_height* : Each line has the height of the line with the maximum height.
  - *max_font_height* : Each line has the height of the character with the maximum height in the font.
- **panel_overlay** : *name* *section*  
  Defines a new section describing a simple panel children overlay.
- **border_panel_overlay** : *name* *section*  
  Defines a new section describing a border panel children overlay.
- **text_overlay** : *name* *section*  
  Defines a new section describing a text panel children overlay.

## window section

- **vsync** : *boolean*  
  Defines the activation or deactivation of vertical synchronisation.
- **fullscreen** : *boolean*  
  Defines the activation or deactivation of full-screen display.
- **allow_hdr** : *boolean*  
  Allows HDR rendering, if the surface supports it.
- **render_target** : *section*  
  Defines a new section describing the render target.

## render_target section

- **scene** : *nom*  
  Defines the scene rendered in this target.
- **camera** : *nom*  
  Defines the camera used to render the scene.
- **size** : *size*  
  Defines the internal buffer dimensions.
- **format** : *value*  
  Defines the colour buffer pixel format. Can be one of :
  - *l8* : Luminance 8 bits, one 8 bits integral number.
  - *l16f* : Luminance 16 bits, one 16 bits floating point number (half float).
  - *l32f* : Luminance 32 bits, one 32 bits floating point number(float).
  - *al16* : Alpha + Luminance, two 8 bits integral number.
  - *al16f* : Alpha + Luminance, two 16 bits floating point number (half float).
  - *al32f* : Alpha + Luminance, two 32 bits floating point number (float).
  - *argb1555* : ARGB 16 bits, 1 bit for alpha and each other component on a 5 bits integer.
  - *rgb565* : RGB 16 bits, R and B on a 5 bits integer, G on a 6 bits integer.
  - *argb16* : ARGB 16 bits, each component on a 4 bits integer.
  - *rgb24* : RGB 24 bits, each component on a 8 bits integer.
  - *argb32* : ARGB 32 bits, each component on a 8 bits integer.
  - *argb16f* : ARGB 64 bits, each component on a 16 bits floating point number (half float).
  - *rgb32f* : RGB 96 bits, each component on a 32 bits floating point number (half float).
  - *argb32f* : ARGB 128 bits, each component on a 32 bits floating point number (half float).
- **hdr_format** : *value*  
  Defines the colour buffer HDR pixel format if the surface supports HDR rendering. Can be one of :
  - *l8* : Luminance 8 bits, one 8 bits integral number.
  - *l16f* : Luminance 16 bits, one 16 bits floating point number (half float).
  - *l32f* : Luminance 32 bits, one 32 bits floating point number(float).
  - *al16* : Alpha + Luminance, two 8 bits integral number.
  - *al16f* : Alpha + Luminance, two 16 bits floating point number (half float).
  - *al32f* : Alpha + Luminance, two 32 bits floating point number (float).
  - *argb1555* : ARGB 16 bits, 1 bit for alpha and each other component on a 5 bits integer.
  - *rgb565* : RGB 16 bits, R and B on a 5 bits integer, G on a 6 bits integer.
  - *argb16* : ARGB 16 bits, each component on a 4 bits integer.
  - *rgb24* : RGB 24 bits, each component on a 8 bits integer.
  - *argb32* : ARGB 32 bits, each component on a 8 bits integer.
  - *argb16f* : ARGB 64 bits, each component on a 16 bits floating point number (half float).
  - *rgb32f* : RGB 96 bits, each component on a 32 bits floating point number (half float).
  - *argb32f* : ARGB 128 bits, each component on a 32 bits floating point number (half float).
- **srgb_format** : *value*  
  Defines the colour buffer SRGB pixel format if the surface doesn't support HDR rendering. Can be one of :
  - *l8* : Luminance 8 bits, one 8 bits integral number.
  - *l16f* : Luminance 16 bits, one 16 bits floating point number (half float).
  - *l32f* : Luminance 32 bits, one 32 bits floating point number(float).
  - *al16* : Alpha + Luminance, two 8 bits integral number.
  - *al16f* : Alpha + Luminance, two 16 bits floating point number (half float).
  - *al32f* : Alpha + Luminance, two 32 bits floating point number (float).
  - *argb1555* : ARGB 16 bits, 1 bit for alpha and each other component on a 5 bits integer.
  - *rgb565* : RGB 16 bits, R and B on a 5 bits integer, G on a 6 bits integer.
  - *argb16* : ARGB 16 bits, each component on a 4 bits integer.
  - *rgb24* : RGB 24 bits, each component on a 8 bits integer.
  - *argb32* : ARGB 32 bits, each component on a 8 bits integer.
  - *argb16f* : ARGB 64 bits, each component on a 16 bits floating point number (half float).
  - *rgb32f* : RGB 96 bits, each component on a 32 bits floating point number (half float).
  - *argb32f* : ARGB 128 bits, each component on a 32 bits floating point number (half float).
- **postfx** : *value*  
  Defines a post render effect to use. The parameters depend on the chosen effect.
- **tone_mapping** : *name*  
  Defines the tone mapping operator to use with the render target.
- **ssao** : *section*  
  Defines a new section describing the Screen Space Ambient Occlusion.
- **clusters** : *section*  
  Defines a new section describing the lights clusters configuration.

### ssao section

- **enabled** : *boolean*  
  Defines the activation status of SSAO.
- **high_quality** : *boolean*  
  Defines the quality of the effect.
- **radius** : *real*  
  Defines the radius of the effect (expressesd in meters).
- **bias** : *real*  
  Defines the bias of the effect.
- **intensity** : *real*  
  Defines the intensity of the effect.
- **num_samples** : *int*  
  Defines the number of samples per pixel.
- **edge_sharpness** : *real*  
  Defines the edge sharpness, in the blur pass.
- **blur_step_size** : *int*  
  Defines the size of a step in the blur pass.
- **blur_radius** : *int*  
  Defines the blur radius.

### clusters section

- **enabled** : *boolean*  
  Defines the clustered lighting activation status.
- **use_lights_bvh** : *boolean*  
  Tells if clustered lighting builds and uses the BVH.
- **sort_lights** : *boolean*  
  Tells if clustered lighting sorts the lights before buildng the BVH.
- **limit_clusters_to_lights_aabb** : *boolean*  
  Tells if the camera clusters are limited to the visible lights.
- **parse_depth_buffer** : *boolean*  
  Tells if the clustered lighting parses the depth buffer to reduce the number of camera clusters to process.
- **use_spot_bounding_cone** : *boolean*  
  Tells if the bounding boxes for spot lights are cones.
- **use_spot_tight_aabb** : *boolean*  
  Tells if the bounding boxes for spot lights are tight.
- **enable_reduce_warp_optimisation** : *boolean*  
  Tells if the lights AABB reduce pass uses warp optimisation.
- **enable_bvh_warp_optimisation** : *boolean*  
  Tells if the lights BVH building pass uses warp optimisation.
- **split_scheme** : *value*  
  Defines the clusters repartition scheme along the scene depth, can be:
  - *exponential* : Clusters depth increase is exponential.
  - *exponential_biased* : Clusters depth increase is exponential, starting at a given size.
  - *linear* : Clusters depth increase is linear.
  - *hybrid* : Starting linear, finishing exponential.
- **bias** : *real*  
  Defines the starting size when *split_scheme* is *exponential_biased*.

## gui section

- **theme** : *name* *section*  
  Defines a GUI theme.
- **box_layout** : *section*  
  Defines a box layout for the scene's GUI.
- **button** : *name* *name* *section*  
  Defines a button GUI control.
- **static** : *name* *section*  
  Defines a static text GUI control.
- **slider** : *name* *section*  
  Defines a slider GUI control.
- **combobox** : *name* *section*  
  Defines a combo box GUI control.
- **listbox** : *name* *section*  
  Defines a list box GUI control.
- **edit** : *name* *section*  
  Defines a text edit GUI control.
- **panel** : *name* *section*  
  Defines a panel GUI control.
- **progress** : *name* *section*  
  Defines a progress bar GUI control.
- **expandable_panel** : *name* *section*  
  Defines an expandable panel GUI control.
- **frame** : *name* *section*  
  Defines a frame GUI control.
- **default_font** : *name*  
  Defines the default font for the GUI.
- **button_style** : *name* *section*  
  Defines a button GUI style.
- **static_style** : *name* *section*  
  Defines a static text GUI style.
- **slider_style** : *name* *section*  
  Defines a slider GUI style.
- **combobox_style** : *name* *section*  
  Defines a combo box GUI style.
- **listbox_style** : *name* *section*  
  Defines a list box GUI style.
- **edit_style** : *name* *section*  
  Defines a text edit GUI style.
- **panel_style** : *name* *section*  
  Defines a panel GUI style.
- **progress_style** : *name* *section*  
  Defines a progress bar GUI style.
- **expandable_panel_style** : *name* *section*  
  Defines an expandable panel GUI style.
- **frame_style** : *name* *section*  
  Defines a frame GUI style.
- **scrollbar_style** : *name* *section*  
  Defines a scrollbar GUI style.

### theme section

- **default_font** : *name*  
  Defines the default font for the theme.
- **button_style** : *name* *section*  
  Defines a button style.
- **static_style** : *name* *section*  
  Defines a static text style.
- **slider_style** : *name* *section*  
  Defines a slider style.
- **combobox_style** : *name* *section*  
  Defines a combo box style.
- **listbox_style** : *name* *section*  
  Defines a list box style.
- **edit_style** : *name* *section*  
  Defines a text edit style.
- **panel_style** : *name* *section*  
  Defines a panel style.
- **progress_style** : *name* *section*  
  Defines a progress bar style.
- **expandable_panel_style** : *name* *section*  
  Defines an expandable panel style.
- **frame_style** : *name* *section*  
  Defines a frame style.
- **scrollbar_style** : *name* *section*  
  Defines a scrollbar style.

### box_layout section

- **layout_ctrl** : *name*  
  Adds a control to the layout.
- **layout_staspace** : *uint*  
  Adds a statically sized spacer to the layout.
- **layout_dynspace**  
  Adds a dynamically sized spacer to the layout.
- **horizontal** : *boolean*  
  Sets the layout direction to horizontal.

#### layout_ctrl section

- **horizontal_align** : *value*  
  Defines the control horizontal alignment, can be one of:
  - *left* : Aligned on the left.
  - *center* : Centered.
  - *right* : Aligned on the right.
- **vertical_align** : *value*  
  Defines the control vertical alignment, can be one of:
  - *top* : Aligned on the top.
  - *center* : Centered.
  - *bottom* : Aligned on the bottom.
- **stretch** : *boolean*
  Defines the control to fill the layout size, in opposite direction.
- **reserve_if_hidden** : *boolean*
  Defines the control reserve its size even if it is hidden.
- **padding** : *4 uints*
  Defines padding pixels around the control.
- **pad_left** : *uint*
  Defines padding pixels a the left of the control.
- **pad_top** : *uint*
  Defines padding pixels a the top of the control.
- **pad_right** : *uint*
  Defines padding pixels a the right of the control.
- **pad_bottom** : *uint*
  Defines padding pixels a the bottom of the control.

### button section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **caption** : *text*  
  Defines the button caption.
- **horizontal_align** : *value*  
  Defines the button caption horizontal alignment, can be one of:
  - *left* : Caption is aligned on the left.
  - *center* : Caption is centered.
  - *right* : Caption is aligned on the right.
- **vertical_align** : *value*  
  Defines the button caption vertical alignment, can be one of:
  - *top* : Caption is aligned on the top.
  - *center* : Caption is centered.
  - *bottom* : Caption is aligned on the bottom.

### combobox section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **item** : *text*  
  Defines an item of the combo box.

### edit section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **vertical_scrollbar** : *boolean*  
  Defines if the control has an vertical scrollbar when needed.
- **horizontal_scrollbar** : *boolean*  
  Defines if the control has an horizontal scrollbar when needed.
- **vertical_scrollbar_style** : *name*  
  Defines the vertical scrollbar style.
- **horizontal_scrollbar_style** : *name*  
  Defines the horizontal scrollbar style.
- **caption** : *text*  
  Defines the edit text.
- **multiline** : *boolean*  
  Defines if the edit is multiline.

### expandable_panel section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines the panel content area.
- **expand_caption** : *text*  
  Defines the text of the expand area when the panel is expanded.
- **retract_caption** : *text*  
  Defines the text of the expand area when the panel is retracted.
  Defines if the control can be resized.
- **header** : *section*  
  Defines the panel header area.
- **expand** : *section*  
  Defines the panel expand area.
- **content** : *section* 
  Defines the panel content area. 

#### header section

- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **button** : *name* *name* *section*  
  Defines a button control.
- **static** : *name* *section*  
  Defines a static text control.
- **slider** : *name* *section*  
  Defines a slider control.
- **combobox** : *name* *section*  
  Defines a combo box control.
- **listbox** : *name* *section*  
  Defines a list box control.
- **edit** : *name* *section*  
  Defines a text edit control.
- **panel** : *name* *section*  
  Defines a panel control.
- **progress** : *name* *section*  
  Defines a progress bar control.
- **expandable_panel** : *name* *section*  
  Defines an expandable panel control.
- **frame** : *name* *section*  
  Defines a frame control.
- **box_layout** : *section*  
  Defines a box layout for the control.

#### expand section

- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).

#### content section

- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **button** : *name* *name* *section*  
  Defines a button control.
- **static** : *name* *section*  
  Defines a static text control.
- **slider** : *name* *section*  
  Defines a slider control.
- **combobox** : *name* *section*  
  Defines a combo box control.
- **listbox** : *name* *section*  
  Defines a list box control.
- **edit** : *name* *section*  
  Defines a text edit control.
- **panel** : *name* *section*  
  Defines a panel control.
- **progress** : *name* *section*  
  Defines a progress bar control.
- **expandable_panel** : *name* *section*  
  Defines an expandable panel control.
- **frame** : *name* *section*  
  Defines a frame control.
- **vertical_scrollbar** : *boolean*  
  Defines if the control has an vertical scrollbar when needed.
- **horizontal_scrollbar** : *text*  
  Defines if the control has an horizontal scrollbar when needed.
- **vertical_scrollbar_style** : *boolean*  
  Defines the vertical scrollbar style.
- **horizontal_scrollbar_style** : *text*  
  Defines the horizontal scrollbar style.
- **box_layout** : *section*  
  Defines a box layout for the control.

### frame section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **header_horizontal_align** : *value*  
  Defines the header caption horizontal alignment in the header area, can be one of:
  - *left* : Caption is aligned on the left.
  - *center* : Caption is centered.
  - *right* : Caption is aligned on the right.
- **header_vertical_align** : *value*  
  Defines the header caption vertical alignment in the header area, can be one of:
  - *top* : Caption is aligned on the top.
  - *center* : Caption is centered.
  - *bottom* : Caption is aligned on the bottom.
- **header_caption** : *text*  
  Defines the header caption.
- **min_size** : *2 uints*  
  Defines the minimal size of the frame.
- **content** : *section*  
  Defines the frame content area.

#### content section

- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **button** : *name* *name* *section*  
  Defines a button control.
- **static** : *name* *section*  
  Defines a static text control.
- **slider** : *name* *section*  
  Defines a slider control.
- **combobox** : *name* *section*  
  Defines a combo box control.
- **listbox** : *name* *section*  
  Defines a list box control.
- **edit** : *name* *section*  
  Defines a text edit control.
- **panel** : *name* *section*  
  Defines a panel control.
- **progress** : *name* *section*  
  Defines a progress bar control.
- **expandable_panel** : *name* *section*  
  Defines an expandable panel control.
- **frame** : *name* *section*  
  Defines a frame control.
- **vertical_scrollbar** : *boolean*  
  Defines if the control has an vertical scrollbar when needed.
- **horizontal_scrollbar** : *text*  
  Defines if the control has an horizontal scrollbar when needed.
- **vertical_scrollbar_style** : *boolean*  
  Defines the vertical scrollbar style.
- **horizontal_scrollbar_style** : *text*  
  Defines the horizontal scrollbar style.
- **box_layout** : *section*  
  Defines a box layout for the control.

### listbox section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **item** : *text*  
  Defines an item of the list box.

### panel section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **button** : *name* *name* *section*  
  Defines a button control.
- **static** : *name* *section*  
  Defines a static text control.
- **slider** : *name* *section*  
  Defines a slider control.
- **combobox** : *name* *section*  
  Defines a combo box control.
- **listbox** : *name* *section*  
  Defines a list box control.
- **edit** : *name* *section*  
  Defines a text edit control.
- **panel** : *name* *section*  
  Defines a panel control.
- **progress** : *name* *section*  
  Defines a progress bar control.
- **expandable_panel** : *name* *section*  
  Defines an expandable panel control.
- **frame** : *name* *section*  
  Defines a frame control.
- **vertical_scrollbar** : *boolean*  
  Defines if the control has an vertical scrollbar when needed.
- **horizontal_scrollbar** : *text*  
  Defines if the control has an horizontal scrollbar when needed.
- **vertical_scrollbar_style** : *boolean*  
  Defines the vertical scrollbar style.
- **horizontal_scrollbar_style** : *text*  
  Defines the horizontal scrollbar style.
- **box_layout** : *section*  
  Defines a box layout for the control.

### progress section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **left_to_right** : *boolean*  
  Defines if the progres goes from the left to the right (default).
- **right_to_left** : *boolean*  
  Defines if the progres goes from the right to the left.
- **top_to_bottom** : *boolean*  
  Defines if the progres goes from the top to the bottom.
- **bottom_to_top** : *boolean*  
  Defines if the progres goes from the bottom to the top.
- **hide_title** : *boolean*  
  Defines if the title is hidden.
- **container_border_size** : *4 uints*  
  Defines the progress container border sizes.
- **bar_border_size** : *4 uints*  
  Defines the progress bar border sizes.

### slider section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).

### static section

- **theme** : *name*  
  Defines the control's style from given theme name.
- **style** : *name*  
  Defines the control's style.
- **visible** : *boolean*  
  Defines the control's visibility.
- **pixel_position** : *2 ints*  
  Defines the control's position, in pixels.
- **pixel_size** : *2 uints*  
  Defines the control's size, in pixels.
- **movable** : *boolean*  
  Defines if the control can be moved.
- **resizable** : *boolean*  
  Defines if the control can be resized.
- **pixel_border_size** : *4 uints*  
  Defines the control's borders sizes (left, right, top, bottom).
- **border_inner_uv** : *4 floats*  
  Defines the control's borders inner UV (left, right, top, bottom).
- **border_outer_uv** : *4 floats*  
  Defines the control's borders outer UV (left, right, top, bottom).
- **center_uv** : *4 floats*  
  Defines the control's center UV (left, right, top, bottom).
- **caption** : *text*  
  Defines the static text.
- **horizontal_align** : *value*  
  Defines the button caption horizontal alignment, can be one of:
  - *left* : Caption is aligned on the left.
  - *center* : Caption is centered.
  - *right* : Caption is aligned on the right.
- **vertical_align** : *value*  
  Defines the button caption vertical alignment, can be one of:
  - *top* : Caption is aligned on the top.
  - *center* : Caption is centered.
  - *bottom* : Caption is aligned on the bottom.
  
### button_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **text_material** : *name*  
  Defines the material for the button's text.
- **highlighted_background_material** : *name*  
  Defines the material for the button's highlighted background material.
- **highlighted_foreground_material** : *name*  
  Defines the material for the button's highlighted foreground material.
- **highlighted_text_material** : *name*  
  Defines the material for the button's highlighted text material.
- **pushed_background_material** : *name*  
  Defines the material for the button's pushed background material.
- **pushed_foreground_material** : *name*  
  Defines the material for the button's pushed foreground material.
- **pushed_text_material** : *name*  
  Defines the material for the button's pushed text material.
- **disabled_background_material** : *name*  
  Defines the material for the button's disabled background material.
- **disabled_foreground_material** : *name*  
  Defines the material for the button's disabled foreground material.
- **disabled_text_material** : *name*  
  Defines the material for the button's disabled text material.
- **font** : *name*
  Defines the font for the button's text.

### combobox_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **expand_style** : *button_style*  
  Defines the style for the expand button.
- **elements_style** : *listbox_style*  
  Defines the style for the items.

### edit_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **text_material** : *name*  
  Defines the material for the edit's text.
- **selection_material** : *name*  
  Defines the material for the edit's selected text.
- **font** : *name*
  Defines the font for the edit's text.
- **scrollbar_style** : *scrollbar_style*
  Defines the style for the edit's scrollbars.

### expandable_panel_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **header_style** : *panel_style*  
  Defines the style for the panel's header area.
- **expand_style** : *button_style*  
  Defines the style for the panel's expand area.
- **content_style** : *panel_style*  
  Defines the style for the panel's content area.

### frame_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **header_text_material** : *name*  
  Defines the material for the control's header text.
- **header_font** : *name*  
  Defines the font for the control's header text.

### listbox_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **item_style** : *static_style*  
  Defines the style for the list items.
- **selected_item_style** : *static_style*  
  Defines the style for the list selected items.
- **highlighted_item_style** : *static_style*  
  Defines the style for the list highlighted items.

### panel_style section

- **default_font** : *name*  
  Defines the default font for the control.
- **button_style** : *name* *section*  
  Defines a button style.
- **static_style** : *name* *section*  
  Defines a static text style.
- **slider_style** : *name* *section*  
  Defines a slider style.
- **combobox_style** : *name* *section*  
  Defines a combo box style.
- **listbox_style** : *name* *section*  
  Defines a list box style.
- **edit_style** : *name* *section*  
  Defines a text edit style.
- **panel_style** : *name* *section*  
  Defines a panel style.
- **progress_style** : *name* *section*  
  Defines a progress bar style.
- **expandable_panel_style** : *name* *section*  
  Defines an expandable panel style.
- **frame_style** : *name* *section*  
  Defines a frame style.
- **scrollbar_style** : *name* *section*  
  Defines a scrollbar style.
- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.

### progress_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **title_font** : *name*  
  Defines the font for the progress' title text.
- **title_material** : *name*  
  Defines the material for the progress' title text.
- **text_font** : *name*  
  Defines the font for the progress' text.
- **text_material** : *name*  
  Defines the material for the progress' text.
- **container_style** : *panel_style*  
  Defines the style for the progress' container.
- **bar_style** : *panel_style*  
  Defines the style for the progress' bar.

### slider_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **line_style** : *static_style*  
  Defines the style for the slider's line.
- **tick_style** : *static_style*  
  Defines the style for the slider's tick.

### static_style section

- **background_invisible** : *boolean*  
  Defines the control's background visibility.
- **foreground_invisible** : *boolean*  
  Defines the control's foreground visibility.
- **border_invisible** : *boolean*  
  Defines the control's borders visibility.
- **background_material** : *name*  
  Defines the material for the control's background.
- **foreground_material** : *name*  
  Defines the material for the control's foreground.
- **border_material** : *name*  
  Defines the material for the control's borders.
- **text_material** : *name*  
  Defines the material for the static's text.
- **font** : *name*  
  Defines the font for the static's text.
