# Format de fichier CSCN

Les fichiers CSCN sont au format texte et sont donc modifiables assez facilement (pour peu que l’on comprenne quelque chose à  la syntaxe :P)

Les types de données qui apparaissent dans les fichiers de scène sont les suivants :

- *booléen* : un booléen (*true* ou *false*).
- *entier* : un entier.
- *masque_1* : un entier hexadécimal représentant un masque pour une composante de couleur (*0x000000FF* ou autre).
- *masque_3* : un entier hexadécimal représentant un masque pour trois composantes de couleur (*0x00FFFFFF* ou autre).
- *réel* : un nombre réel, le séparateur des décimales est le point ( . ).
- *2, 3, 4 entiers* : 2, 3 ou 4 entiers, séparés par des virgules ( , ) ou des espaces ( ).
- *2, 3, 4 réels* : 2, 3 ou 4 réels, séparés par des virgules ( , ) ou des espaces ( ).
- *taille* : 2 entiers supérieurs ou égaux à 0.
- *matrice de 2x2, 3x3, 4x4 réels* : 2, 3 ou 4 groupes séparés par des points virgules ( ; ) de 2, 3 ou 4 réels séparés par des virgules ( *,* ) ou des espaces ( )
- *couleur_rvb* : les composantes RVB d’une couleur, exprimées en réels compris entre 0.0 et 1.0.
- *couleur_rvba* : les composantes RVBA d’une couleur, exprimées en réels compris entre 0.0 et 1.0.
- *couleur_hdr_rvb* : les composantes RVB d’une couleur, exprimées en réels supérieurs ou égaux à 0.0.
- *couleur_hdr_rvba* : les composantes RVBA d’une couleur, exprimées en réels supérieurs ou égaux à 0.0.
- *valeur* : chaîne de caractère représentant une valeur prédéfinie.
- *nom* : chaîne de caractères, entourée de guillemets ( &quot; ).
- *fichier* : chaîne de caractères représentant un chemin d’accès à un fichier, entourée de guillemets ( &quot; ).
- *dossier* : chaîne de caractères représentant un chemin d’accès à un dossier, entourée de guillemets ( &quot; ).


## Description

Le fichier est décomposé en sections décrites de la manière suivante:

    [section-type] "[section-name]"
    {
        // Section description
    }

Exemple:

    light "Light0"
    {
        type directional
        colour 1.0 1.0 1.0
        intensity 0.8 1.0
    }

Certaines sections peuvent avoir des sous-sections :

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

## Section root

- **debug_overlays** : *booléen*  
  Active ou désactive les incrustation de débogage.
- **enable_full_loading** : *booléen*  
  Active ou désactive le chargement des passes qui ne sont pas utilisées dans la scène.
- **materials** : *value*  
  Permet de définir le type des matériaux utilisés dans le fichier. Les valeurs possibles sont :
  - *phong* : Matériaux Blinn-Phong.
  - *pbr* : Matériaux PBR.
- **default_unit** : *value*  
  Définit l'untié de longueur par défaut du moteur. Les valeurs possibles sont :
  - *mm* : Millimètres.
  - *cm* : Centimètres.
  - *m* : Mètres.
  - *km* : Kilomètres.
  - *yd* : Yards.
  - *ft* : Pieds.
  - *in* : Pouces.
- **max_image_size**: *entier*
  Permet de limiter les dimensions des images chargées (tout en conservant leurs proportions).
- **debug_max_image_size**: *entier*
  Permet de limiter les dimensions des images chargées (tout en conservant leurs proportions), pour les versions debug.
- **lpv_grid_size**: *entier*
  Permet de personnaliser les dimensions de la grille utilisée pour les Light Propagation Volumes (32 par défaut).
- **sampler** : *section*  
  Permet de définir un objet d’échantillonnage de texture.
- **material** : *section*  
  Permet la définition d’un matériau.
- **loading_screen** : *section*  
  Permet la redéfinition de l'écran de chargement.
- **font** : *section*  
  Permet la définition d’une police utilisée dans les incrustations texte.
- **window** : *section*  
  Permet la définition d’une fenêtre de rendu.
- **panel_overlay** : *section*  
  Permet de définir une incrustation globale de type panneau simple.
- **border_panel_overlay** : *section*  
  Permet de définir une incrustation globale de type panneau avec bordure.
- **text_overlay** : *section*  
  Permet de définir une incrustation globale de type panneau avec texte.
- **scene** : *section*  
  Permet de définir une scène.
- **gui** : *section*  
  Permet de définir une GUI globale.
- **theme** : *name* *section*  
  Permet de définir un thème de GUI.
- **box_layout** : *section*  
  Permet de définir un box layout de GUI global.
- **button** : *name* *section*  
  Permet de définir un bouton.
- **static** : *name* *section*  
  Permet de définir un texte statique.
- **slider** : *name* *section*  
  Permet de définir un slider.
- **combobox** : *name* *section*  
  Permet de définir une liste déroulante.
- **listbox** : *name* *section*  
  Permet de définir une liste.
- **edit** : *name* *section*  
  Permet de définir un texte éditable.
- **panel** : *name* *section*  
  Permet de définir un panneau.
- **progress** : *name* *section*  
  Permet de définir une barre de progression.
- **expandable_panel** : *name* *section*  
  Permet de définir un panneau extensible.
- **frame** : *name* *section*  
  Permet de définir une fenêtre.
- **default_font** : *name*  
  Permet de définir la police de GUI globale.
- **button_style** : *name* *section*  
  Permet de définir un style de bouton.
- **static_style** : *name* *section*  
  Permet de définir un style de texte statique.
- **slider_style** : *name* *section*  
  Permet de définir un style de slider.
- **combobox_style** : *name* *section*  
  Permet de définir un style de liste déroulante.
- **listbox_style** : *name* *section*  
  Permet de définir un style de liste.
- **edit_style** : *name* *section*  
  Permet de définir un style de texte éditable.
- **panel_style** : *name* *section*  
  Permet de définir un style de panneau.
- **progress_style** : *name* *section*  
  Permet de définir un style de barre de proogression.
- **expandable_panel_style** : *name* *section*  
  Permet de définir un style de panneau extensible.
- **frame_style** : *name* *section*  
  Permet de définir un style de fenêtre.
- **scrollbar_style** : *name* *section*  
  Permet de définir un style de barre de progression.

## Section sampler

- **min_filter** : *valeur*  
  Valeur pour la fonction de minification. Les valeurs possibles sont :
  - *linear* : interpolation linéaire.
  - *nearest* : aucune interpolation.
- **mag_filter** : *valeur*  
  Valeur pour la fonction de magnification. Les valeurs possibles sont :
  - *linear* : interpolation linéaire.
  - *nearest* : aucune interpolation.
- **mip_filter** : *valeur*  
  Valeur pour la fonction de mipmapping. Les valeurs possibles sont :
  - *linear* : interpolation linéaire.
  - *nearest* : aucune interpolation.
- **min_lod** : *réel*  
  Définit la valeur minimale du niveau de détail.
- **max_lod** : *réel*  
  Définit la valeur maximale du niveau de détail.
- **lod_bias** : *réel*  
  Définit le MIP-Level minimal utilisé.
- **u_wrap_mode** : *valeur*  
  Définit le paramètre d’enveloppement de la texture en U. Peut prendre les valeurs suivantes :
  - *repeat* : La texture est répétée.
  - *mirrored_repeat* : La texture est répétée, une instance sur 2 en miroir de la précédente.
  - *clamp_to_border* : La texture est étirée, la couleur des arêtes au bord de la texture est celle du bord de la texture.
  - *clamp_to_edge* : La texture est étirée, la couleur des arêtes au bord de la texture est un mélange de celle du bord de la texture et de celle du bord.
- **v_wrap_mode** : *valeur*  
  Définit le paramètre d’enveloppement de la texture en V. Peut prendre les valeurs suivantes :
  - *repeat* : La texture est répétée.
  - *mirrored_repeat* : La texture est répétée, une instance sur 2 en miroir de la précédente.
  - *clamp_to_border* : La texture est étirée, la couleur des arêtes au bord de la texture est celle du bord de la texture.
  - *clamp_to_edge* : La texture est étirée, la couleur des arêtes au bord de la texture est un mélange de celle du bord de la texture et de celle du bord.
- **w_wrap_mode** : *valeur*  
  Définit le paramètre d’enveloppement de la texture en W. Peut prendre les valeurs suivantes :
  - *repeat* : La texture est répétée.
  - *mirrored_repeat* : La texture est répétée, une instance sur 2 en miroir de la précédente.
  - *clamp_to_border* : La texture est étirée, la couleur des arêtes au bord de la texture est celle du bord de la texture.
  - *clamp_to_edge* : La texture est étirée, la couleur des arêtes au bord de la texture est un mélange de celle du bord de la texture et de celle du bord.
- **border_colour** : *valeur*  
  Définit la couleur des bords non texturés.
  - *float_transparent_black* : Noir transparent.
  - *int_transparent_black* : Noir transparent.
  - *float_opaque_black* : Noir opaque.
  - *int_opaque_black* : Noir opaque.
  - *float_opaque_white* : Blanc opaque.
  - *int_opaque_white* : Blanc opaque.
- **anisotropic_filtering** : *booléen*  
  Dit si le filtrage anisotropique est activé (si supporté).
- **max_anisotropy** : *réel*  
  Définit le degré maximal d’anisotropie.
- **comparison_mode** : *value*  
  Définit le mode de comparaison de l'échantillonneur. Peut prendre les valeurs suivantes :
  - *none* : Echantillonneur traditionnel.
  - *ref_to_texture* : Echantillonneur d'ombres.
- **comparison_func** : *value*  
  Définit la fonction de comparaison de l'échantillonneur. Peut prendre les valeurs suivantes :
  - *always* : la couleur de l’échantillon est toujours appliquée.
  - *less* : La couleur de l’échantillon est appliquée si sa valeur est inférieure à la valeur de référence.
  - *less_or_equal* : La couleur de l’échantillon est appliquée si sa valeur est inférieure ou égal à la valeur de référence.
  - *equal* : La couleur de l’échantillon est appliquée si sa valeur est égale à la valeur de référence.
  - *not_equal* : La couleur de l’échantillon est appliquée si sa valeur est différente de la valeur de référence.
  - *greater_or_equal* : La couleur de l’échantillon est appliquée si sa valeur est supérieure ou égal à la valeur de référence.
  - *greater* : La couleur de l’échantillon est appliquée si sa valeur est supérieure à la valeur de référence.
  - *never* : La couleur de l’échantillon n’est jamais appliquée.

## Section material

Les matériaux pouvant être multi-passes, il est possible de définir plusieurs sous-sections de passe.
- **pass** : *section*  
  Commence un nouvelle section décrivant les propriétés de la passe.
- **render_pass** : *name*  
  Définit le nom de la render pass utilisée pour dessiner les objets utilisant ce matériau.

### Section pass

- **two_sided** : *booléen*  
  Définit si la passe est double face (défaut à false).
- **lighting** : *booléen*  
  Active l'éclairage (défaut à true).  
- **lighting_model_** : *value*  
  Permet de définir le type d'éclairage de la passe. Les valeurs possibles sont :
  - *phong* : Blinn-Phong.
  - *pbr* : PBR.
- **pickable** : *booléen*  
  Active le picking (défaut à true).  
- **texture_unit** : *section*  
  Définit une nouvelle section concernant une texture.
- **reflections** : *booléen*  
  Active les  réflexions (défaut à false).  
- **refraction_ratio** : *réel*  
  Définit l'indice de réfraction. 
- **has_refraction** : *booléen*  
  Dit si la passe utilise la réfraction. Notez que même s’il n’y a pas de refraction map, la réfraction sera appliquée tout de même, en utilisant seulement la skybox.
- **alpha_blend_mode** : *valeur*  
  Nom du mode de mélange alpha, au choix parmi :
  - *none* : Pas de mélange alpha.
  - *additive* : Les alphas de la source et de la destination s’additionnent.
  - *multiplicative* : Les alphas de la source et de la destination se multiplient.
- **colour_blend_mode** : *valeur*  
  Nom du mode de mélange couleur, au choix parmi :
  - *none* : Pas de mélange couleur.
  - *additive* : Les couleurs de la source et de la destination s’additionnent.
  - *multiplicative* : Les couleurs de la source et de la destination se multiplient.
- **colour_srgb** : *couleur_rvb*  
  Définit la couleur.
- **diffuse** : *couleur_rvb*  
  Définit la couleur.
- **colour_hdr** : *couleur_hdr_rvb*  
  Defines la couleur.
- **albedo** : *couleur_hdr_rvb*  
  Defines la couleur.
- **specular_colour** : *couleur_rvb*  
  Définit la couleur de réflexion spéculaire.
- **specular_factor_** : *réel*  
  Définit l'intensité de la réflexion spéculaire.
- **metallic** : *réel (entre 0 et 1)*  
  Definit la metalness.
- **shininess** : *réel (entre 0 et 128)*  
  Définit la façon dont l'exposant spéculaire.
- **glossiness** : *réel (entre 0 et 1)*  
  Définit l’éclat de la surface.
- **roughness** : *réel (entre 0 et 1)*  
  Définit la rugosité de la surface.
- **emissive_colour** : *couleur_rvb*  
  Définit la couleur émise.
- **emissive_factor** : *réel (entre 0 et 1)*  
  Définit le facteur d'émission.
- **attenuation_colour** : *rgb_colour*  
  Définit la couleur d'atténuation.
- **attenuation_distance** : *réel*  
  Définit la distance d'atténuation.
- **thickness_factor** : *réel*  
  Définit le facteur d'épaisseur.
- **transmission** : *réel*  
  Définit le facteur de transmission.
- **clearcoat_factor** : *réel*  
  Définit le facteur de clearcoat.
- **clearcoat_roughness_factor** : *réel*  
  Définit la rugosité de clearcoat.
- **iridescence_factor** : *réel*  
  Définit le facteur d'iridescence.
- **iridescence_ior** : *réel*  
  Définit l'indice de réfraction de la couche d'iridescence.
- **iridescence_min_thickness** : *réel*  
  Définit l'épaisseur maximale de la couche d'iridescence.
- **iridescence_max_thickness** : *réel*  
  Définit l'épaisseur minimale de la couche d'iridescence.
- **sheen_colour** : *réel*  
  Définit la couleur de la couche de sheen.
- **sheen_roughness** : *réel*  
  Définit la rugosité de la couche de sheen.
- **opacity** : *réel (entre 0 et 1)*  
  Définit l'opacité.
- **bw_accumulation** : *entier compris entre 0 et 5*  
  Définit la fonction d'accumulation, pour le blended weighted rendering.
- **alpha_func** : func : *valeur* ref-val : *réel*  
  Définit la fonction de gestion de l’alpha rejection pour la passe. Le second paramètre est la valeur de référence pour les calculs de transparence. Les valeurs possibles pour le premier paramètre sont :
  - *always* : la couleur de l’échantillon est toujours appliquée.
  - *less* : La couleur de l’échantillon est appliquée si sa transparence est inférieure au 2ème paramètre.
  - *less_or_equal* : La couleur de l’échantillon est appliquée si sa transparence est inférieure ou égal au 2ème paramètre.
  - *equal* : La couleur de l’échantillon est appliquée si sa transparence est égale au 2ème paramètre.
  - *not_equal* : La couleur de l’échantillon est appliquée si sa transparence est différente du 2ème paramètre.
  - *greater_or_equal* : La couleur de l’échantillon est appliquée si sa transparence est supérieure ou égal au 2ème paramètre.
  - *greater* : La couleur de l’échantillon est appliquée si sa transparence est supérieure au 2ème paramètre.
  - *never* : La couleur de l’échantillon n’est jamais appliquée.
- **blend_alpha_func** : func : *valeur* ref-val : *réel*  
  Définit la fonction de gestion de l’alpha rejection pour la passe, lorsque le blending est actif. Le second paramètre est la valeur de référence pour les calculs de transparence. Les valeurs possibles pour le premier paramètre sont :
  - *always* : la couleur de l’échantillon est toujours appliquée.
  - *less* : La couleur de l’échantillon est appliquée si sa transparence est inférieure au 2ème paramètre.
  - *less_or_equal* : La couleur de l’échantillon est appliquée si sa transparence est inférieure ou égal au 2ème paramètre.
  - *equal* : La couleur de l’échantillon est appliquée si sa transparence est égale au 2ème paramètre.
  - *not_equal* : La couleur de l’échantillon est appliquée si sa transparence est différente du 2ème paramètre.
  - *greater_or_equal* : La couleur de l’échantillon est appliquée si sa transparence est supérieure ou égal au 2ème paramètre.
  - *greater* : La couleur de l’échantillon est appliquée si sa transparence est supérieure au 2ème paramètre.
  - *never* : La couleur de l’échantillon n’est jamais appliquée.
- **mixed_interpolation** : *section*  
  Helper définissant **alpha_blend_mode** à *interpolative*, **blend_alpha_func** à *less_or_equal* et **alpha_func** à *greater* avec une valeur de référence à 0.95.
- **fractal** : *booléen*  
  Active le fractal UV mapping (défaut à false).  
- **untile** : *booléen*  
  Active l'untiled UV mapping (défaut à false).  
- **parallax_occlusion** : *valeur*  
  Active ou désactive le parallax occlusion mapping (nécessite une normal map et une height map), les valeurs possibles sont :
  - *none* : Désactivé.
  - *one* : Pas de tiling.
  - *repeat* : Tiling répété.
- **subsurface_scattering** : *section*  
  Définit une nouvelle section décrivant le subsurface scattering.
- **transmittance** : *réel*  
  Facteur de transmittance (pour le subsurface scattering).

#### Section texture_unit

- **image** : *fichier*  
  Définit le chemin où trouver l’image.
- **render_target** : *section*  
  Permet de définir la texture en cible de rendu et de configurer cette cible.
- **channel** : *value {| value}*  
  Les canaux auxquels la texture peut être associée. Peut prendre les valeurs suivantes :
  - *diffuse* : Couleur d'éclairage diffus (Phong uniquement).
  - *albedo* : Couleur de base (PBR uniquement).
  - *specular* : Couleur spéculaire (Phong et PBR Specular/Glossiness).
  - *specular_factor* : Facteur spéculaire (Phong et PBR Specular/Glossiness).
  - *metallic* : Facteur métallique (PBR Metallic/Roughness uniquement).
  - *shininess* : Exposant spéculaire (Phong uniquement).
  - *glossiness* : Facteur de glossiness (PBR Specular/Glossiness uniquement).
  - *roughness* : Facteur de roughness (PBR Metallic/Roughness uniquement).
  - *opacity* : Facteur d'opacité.
  - *emissive* : Couleur émissive.
  - *transmission* : Facteur de transmission (pour le subsurface scattering).
  - *transmittance* : Facteur de transmittance (pour le subsurface scattering).
  - *occlusion* : Facteur d'occlusion.
  - *normal* : Normales.
  - *height* : Hauteur.
  - *iridescence* : Facteur d'iridescence.
  - *iridescence_thickness* : Facteur de profondeur de la couche d'iridescence.
  - *clearcoat* : Facteur de clearcoat.
  - *clearcoat_normal* : Normal map de la couche de clearcoat.
  - *clearcoat_roughness* : Facteur de rugosité de la couche de clearcoat.
  - *thickness* : Facteur d'épaisseur.
- **sampler** : *nom*
  Définit l’échantillonneur pour la texture.
- **level_count** : *entier*
  Définit le nombre maximal de mip levels.
- **diffuse_mask** : *masque_3*  
  Définit les composantes de la texture utilisées pour la couleur.
- **albedo_mask** : *masque_3*  
  Définit les composantes de la texture utilisées pour la couleur.
- **specular_mask** : *masque_3*  
  Définit les composantes de la texture utilisées pour la couleur de réflexion spéculaire.
- **metalness_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur métallique.
- **shininess_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour l'exposant spéculaire.
- **glossiness_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur de glossiness.
- **roughness_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur de roughness.
- **opacity_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur d'opacité.
- **emissive_mask** : *masque_3*  
  Définit les composantes de la texture utilisées pour la couleur émissive.
- **transmittance_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur de transmittance (pour le subsurface scattering).
- **occlusion_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour le facteur d'occlusion.
- **normal_mask** : *masque_3*  
  Définit les composantes de la texture utilisées pour les normales.
- **normal_directx** : *booléen*  
  Dit si les normales de la texture sont exprimées pour DirectX (la composante verte sera alors inversée).
- **normal_factor** : *réel*  
  Définit le facteur de normal mapping.
- **normal_2channels** : *booléen*  
  Dit si la texture de normales utilise 2 canaux (Z sera reconstruit).
- **height_mask** : *masque_1*  
  Définit la composante de la texture utilisée pour la hauteur.
- **height_factor** : *réel*  
  Définit le facteur de hauteur.
- **thickness_mask** : *mask_1*  
  Définit la composante de la texture utilisée pour le facteur d'épaisseur.
- **transmission_mask** : *mask_1*  
  Définit la composante de la texture utilisée pour le facteur de transmission.
- **clearcoat_normal_mask** : *mask_3*  
  Définit la composante de la texture utilisée pour la normal map de la couche de clearcoat.
- **clearcoat_mask** : *mask_1*  
  Définit la composante de la texture utilisée pour le facteur de clearcoat.
- **clearcoat_roughness_mask** : *mask_1*  
  Définit la composante de la texture utilisée pour le facteur de rugosité de la couche de clearcoat.
- **iridescence_mask** : *mask_1*  
  Définit la composante de la texture utilisée pour le facteur d'iridescence.
- **iridescence_thickness_mask** : *mask_1*  
  Définit la composante de la texture utilisée pour le facteur d'épaisseur de la couche d'iridescence.
- **sheen_mask** : *mask_3*  
  Définit la composante de la texture utilisée pour la couleur de la couche de sheen.
- **sheen_roughness_mask** : *mask_1*  
  Définit la composante de la texture utilisée pour la rugosité de la couche de sheen.
- **invert_y** : *booléen*  
  Définit si l'image doit être inversée sur l'axe Y.
- **transform** : *section*  
  Définit la transformation de base de la texture.
- **tileset** : *2 entiers*  
  Définit les dimensions du tile set de la texture.
- **tiles** : *entier*  
  Définit le nombre de tiles de la texture.
- **animation** : *section*  
  Définit l'animation de transformation de la texture.
- **texcoord_set** : *section*  
  Définit le set de coordonnées de textures que cette texture va utiliser.

##### Section transform

- **rotate** : *réel*  
  Définit la rotation de la texture.
- **translate** : *2 réels*  
  Définit la translation de la texture.
- **scale** : *2 réels*  
  Définit la mise à l'échelle de la texture.
- **tile** : *2 entiers*  
  Définit la tile sélectionnée.

##### Section animation

- **rotate** : *réel*  
  Définit la vitesse d'animation de la rotation de la texture.
- **translate** : *2 réels*  
  Définit la vitesse d'animation de la translation de la texture.
- **scale** : *2 réels*  
  Définit la vitesse d'animation de la mise à l'échelle de la texture.
- **tile** : *2 entiers*  
  Définit si les tiles sont animées.

#### Section shader_program

- **compute_program** : *section*  
  Définit une nouvelle section concernant le compute program.

#### Section compute_program

- **file** : *fichier*  
  Nom du fichier où se trouve le programme.
- **group_sizes** : *3 ints*  
  Définit les dispatch counts en X, Y et Z.

#### Section subsurface_scattering

- **strength** : *réel*  
  Définit la force de l’effet.
- **gaussian_width** : *réel*  
  Définit la largeur du flou Gaussien.
- **thickness_scale** : *réel*  
  Définit l'échelle de profondeur.
- **transmittance_profile** : *new section*  
  Définit une nouvelle section décrivant le profil de transmission.

#### Section transmittance_profile

- **factor** : *vec4f*  
  Définit les trois composantes RVB de la couleur, la quatrième composante définissant le facteur de multiplication de cette couleur.

## Section font

- **file** : *fichier*  
  Définit le fichier contenant la police.
- **height** : *entier*  
  Définit la hauteur des caractères (la précision).

## Section scene

- **ambient_light** : *couleur*  
  Définit la couleur de l’éclairage ambiant.
- **background_colour** : *couleur*  
  Définit la couleur de fond de la scène.
- **background_image** : *fichier*  
  Définit l’image de fond de la scène.
- **import** : *fichier*  
  Permet l’import d’une scène à partir d’un fichier de scène CSCN ou autre, supporté par les plug-ins d’import Castor3D.
- **include** : *fichier*  
  Inclut un fichier de scène, permettant de découper une scène en de multiples fichiers.
- **fog_type** : *valeur*  
  Définit le type de brouillard pour la scène. Les valeurs possibles sont :
  - *linear* : L’intensité du brouillard augmente linéairement, avec la distance à la caméra.
  - *exponential* : L’intensité du brouillard augmente exponentiellement, avec la distance à la caméra.
  - *squared_exponential* : L’intensité du brouillard augmente encore plus, avec la distance à la caméra.
- **fog_density** : *réel*  
  Définit la densité du brouillard, qui est multipliée par la distance, en fonction du type de brouillard.
- **directional_shadow_cascades** : *entier*  
  Définit le nombre de cascades des sources lumineuses directionnelles.
- **scene_node** : *section*  
  Définit un noeud de scène.
- **camera_node** : *section*  
  Définit un noeud de scène spécifique aux caméras.
- **light** : *section*  
  Définit une source lumineuse.
- **object** : *section*  
  Définit un objet.
- **billboard** : *section*  
  Définit des billboards.
- **camera** : *section*  
  Définit une caméra.
- **panel_overlay** : *section*  
  Définit une incrustation de type panneau simple.
- **border_panel_overlay** : *section*  
  Définit une incrustation de type panneau avec bordure.
- **text_overlay** : *section*  
  Définit une incrustation de type panneau avec texte.
- **animated_object_group** : *section*  
  Définit un groupe d’objets animés avec des animations communes.
- **mesh** : *section*  
  Définit une nouvelle section décrivant un maillage, pouvant être utilisé pou un ou plusieurs objets.
- **skeleton** : *section*  
  Définit une nouvelle section décrivant un squelette, pouvant être utilisé pou un ou plusieurs maillages.
- **particle_system** : *section*  
  Définit une nouvelle section décrivant un système de particules.
- **skybox** : *section*  
  Définit une nouvelle section décrivant la skybox.
- **sampler** : *section*  
  Définit une nouvelle section décrivant un échantillonneur.
- **voxel_cone_tracing** : *section*  
  Définit une nouvelle section décrivant la configuration du voxel cone tracing.
- **gui** : *section*  
  Permet de définir une GUI pour la scène.
- **theme** : *name* *section*  
  Permet de définir un thème de GUI.
- **box_layout** : *section*  
  Permet de définir un box layout de GUI pour la scène.
- **button** : *name* *section*  
  Permet de définir un bouton.
- **static** : *name* *section*  
  Permet de définir un texte statique.
- **slider** : *name* *section*  
  Permet de définir un slider.
- **combobox** : *name* *section*  
  Permet de définir une liste déroulante.
- **listbox** : *name* *section*  
  Permet de définir une liste.
- **edit** : *name* *section*  
  Permet de définir un texte éditable.
- **panel** : *name* *section*  
  Permet de définir un panneau.
- **progress** : *name* *section*  
  Permet de définir une barre de progression.
- **expandable_panel** : *name* *section*  
  Permet de définir un panneau extensible.
- **frame** : *name* *section*  
  Permet de définir une fenêtre.
- **default_font** : *name*  
  Permet de définir la police de GUI pour la scène.
- **button_style** : *name* *section*  
  Permet de définir un style de bouton.
- **static_style** : *name* *section*  
  Permet de définir un style de texte statique.
- **slider_style** : *name* *section*  
  Permet de définir un style de slider.
- **combobox_style** : *name* *section*  
  Permet de définir un style de liste déroulante.
- **listbox_style** : *name* *section*  
  Permet de définir un style de liste.
- **edit_style** : *name* *section*  
  Permet de définir un style de texte éditable.
- **panel_style** : *name* *section*  
  Permet de définir un style de panneau.
- **progress_style** : *name* *section*  
  Permet de définir un style de barre de proogression.
- **expandable_panel_style** : *name* *section*  
  Permet de définir un style de panneau extensible.
- **frame_style** : *name* *section*  
  Permet de définir un style de fenêtre.
- **scrollbar_style** : *name* *section*  
  Permet de définir un style de barre de progression.

### Section skybox

- **visible** : *booléen*  
  Définit la visibilité du fond (s'il n'est pas visible, il entre quand même en compte lors du calcul de l'IBL).
- **equirectangular** : *fichier* *entier*  
  Définit la skybox depuis une image HDRi. Le second paramètre détermine les dimensions de la skybox.
- **cross** : *fichier*  
  Définit la skybox depuis une image en croix.
- **left** : *fichier*  
  Définit l'image de gauche de la skybox.
- **right** : *fichier*  
  Définit l'image de droite de la skybox.
- **top** : *fichier*  
  Définit l'image du haut de la skybox.
- **bottom** : *fichier*  
  Définit l'image du bas de la skybox.
- **front** : *fichier*  
  Définit l'image de l'avant de la skybox.
- **back** : *fichier*  
  Définit l'image de l'arrière de la skybox.

### Section voxel_cone_tracing

- **enabled** : *booléen*  
  Définit le statut d’activation du VCT.
- **conservative_rasterization** : *booléen*  
  Définit le statut d’activation de la rastérization conservatrice.
- **temporal_smoothing** : *booléen*  
  Définit le statut d’activation du lissage temporel.
- **secondary_bounce** : *booléen*  
  Définit le statut d’activation du second rebond.
- **grid_size** : *réel*  
  Définit les dimensions de la texture 3D contenant les voxels.
- **num_cones** : *réel*  
  Définit le nombre de cônes utilisés (maximum 16).
- **max_distance** : *réel*  
  Définit la distance maximale que les rayons peuvent atteindre.
- **ray_step_size** : *entier*  
  Définit la taille d'un pas de rayon.
- **voxel_size** : *réel*  
  Définit la taille représentée par un voxel.

### Section import

- **file** : *path*  
  Importe la scène depuis le chemin donné.
- **file_anim** : *path*  
  Importe les animations depuis le chemin donné.
- **prefix** : *text*  
  Définit un préfixe qui sera attribué aux noms des objets, pour éviter les conflits de noms.
- **rescale** : *réel*  
  Met les objects importés à l’échelle, sur les trois axes.
- **pitch** : *réel*  
  Tourne les objects importés de l'angle donné (en degrés) autour de l'axe X.
- **yaw** : *réel*  
  Tourne les objects importés de l'angle donné (en degrés) autour de l'axe Y.
- **roll** : *réel*  
  Tourne les objects importés de l'angle donné (en degrés) autour de l'axe Z.
- **no_optimisations** : *booléen*  
  Désactive les optimisations lors de l'import.
- **emissive_mult** : *réel*  
  Applique un facteur de multiplication de l'émissive aux valeurs d'émissive importées.
- **recenter_camera** : *nom*  
  Centre sur le résultat de l'import la caméra désignée par le nom donné.
- **preferred_importer** : *nom*  
  Permet de définir un importeur qui sera utilisé plutôt qu'un autre, si plusieurs importeurs supportent le format de fichier.
- **ignore_vertex_colour** : *booléen*  
  Permet d'ignorer la composante de couleur de sommets lors de l'import d'un mesh.

### Sections scene_node et camera_node

- **static** : *name*  
  Dit que le noeud est statique, et ne peut donc pas être déplacé/tourné//mis à l'échelle après sa définition.
- **parent** : *nom*  
  Définit le Node parent de celui-ci. Par défaut le parent est le RootNode. Le fait de transformer (tranlate, rotate, scale) un node parent transforme ses enfants.
- **position** : *3 réels*  
  La position du node par rapport à son parent.
- **orientation** : *4 réels*  
  Quaternion représentant l’orientation du node par rapport à son parent.
- **rotate** : *4 reals*  
  Accumule l'orientation du noeud avec la rotation donnée.
- **direction** : *3 réels*  
  Définit la direction du node, par rapport à son parent.
- **scale** : *3 réels*  
  Echelle du node par rapport à son parent.

### Section light

- **type** : *valeur*  
  3 types de sources lumineuses existent dans Castor3D :
  - *directional* : lumière directionnelle (telle le soleil).
  - *point_light* : une source située à un endroit et émettant dans toutes les directions.
  - *spot_light* : une source située à un endroit et émettant dans un cône orienté dans une direction.
- **colour** : *3 réels*  
  Définit la couleur de la source lumineuse, au format RGB.
- **intensity** : *2 réels*  
  Définit les intensités diffuse et spéculaire de la source lumineuse.
- **attenuation** : *3 réels*  
  Définit les 3 composantes d’atténuation de la source lumineuse en fonction de l’éloignement de la source : constante, linéaire et quadratique.  
- **range** : *réel*  
  Définit la distance maximale d'éclairage de la source lumineuse.  
  spot_light et point_light uniquement.
- **inner_cut_off** : *réel*  
  Angle d'ouverture intérieur du cône du spot.  
  spot_light uniquement.
- **outer_cut_off** : *réel*  
  Angle d'ouverture extérieur du cône du spot.  
  spot_light uniquement.
- **exponent** : *réel*  
  Attenuation fonction de la distance entre le point éclairé et le centre du cône lumineux.  
  spot_light uniquement.
- **parent** : *nom*  
  Définit le nom du SceneNode auquel la source lumineuse est attachée.
- **shadows** : *section*  
  Définit une nouvelle section décrivant les ombres produites par la source lumineuse.

#### Section shadows

- **producer** : *booléen*  
  Dit si la source lumineuse produit des ombres (*true*) ou pas (*false*).
- **filter** : *valeur*  
  Définit le type d'ombres à utiliser. Peut prendre les valeurs suivantes :
  - *raw* : Aucun filtre.
  - *pcf* : Filtre PCF.
  - *variance* : Variance Shadow Map.
- **global_illumination** : *value*  
  Définit le type d'éclairage indirect. Peut prendre les valeurs suivantes :
  - *none* : Aucune.
  - *lpv* : Light Propagation Volumes.
  - *lpv_geometry* : Light Propagation Volumes avec injection des Géometries.
  - *layered_lpv* : Layered Light Propagation Volumes (uniquement pour les sources lumineuses directionnelles).
  - *layered_lpv_geometry* : Layered Light Propagation Volumes avec injection des Géometries (uniquement pour les sources lumineuses directionnelles).
- **volumetric_steps** : *entier*  
  Le nombre d'étapes effectuées par le rayon pour calculer le volumetric light scattering.
- **volumetric_scattering** : *réel*  
  Définit le facteur de volumetric light scattering.
- **raw_config** : *section*  
  Définit une nouvelle section décrivant la configuration des ombres RAW.
- **pcf_config** : *section*  
  Définit une nouvelle section décrivant la configuration des ombres PCF.
- **vsm_config** : *section*  
  Définit une nouvelle section décrivant la configuration des ombres VSM.

##### raw_config section

- **min_offset** : *réel*  
  Définit l'offset minimal à appliquer à la valeur récupérée depuis la texture d'ombres.
- **max_slope_offset** : *réel*  
  Définit l'offset maximal, relatif à la pente, à appliquer à la valeur récupérée depuis la texture d'ombres.

##### pcf_config section

- **min_offset** : *réel*  
  Définit l'offset minimal à appliquer à la valeur récupérée depuis la texture d'ombres.
- **max_slope_offset** : *réel*  
  Définit l'offset maximal, relatif à la pente, à appliquer à la valeur récupérée depuis la texture d'ombres.
- **filter_size** : *réel*  
  Définit la taille du filtre PCF.
- **sample_count** : *réel*  
  Définit le nombre d'échantillons du filtre PCF.

##### vsm_config section

- **min_variance** : *réel*  
  Définit le facteur maximal pour la variance, à appliquer à la valeur récupérée depuis la texture d'ombres.
- **light_bleeding_reduction** : *réel*  
  Définit le facteur de réduction du light bleeding.

### Section object

- **parent** : *nom*  
  Nom du SceneNode auquel la géométrie est attachée.
- **mesh** : *nom*  
  Définit le maillage utilisé par cet objet.
- **material** : *nom*  
  Nom d’un materiau défini dans un fichier .cmtl ou dans ce fichier. Applique le materiau à tous les sous-maillages.
- **materials** : *section*  
  Permet de définir le matériau pour chaque sous-maillage.
- **cast_shadows** : *booléen*  
  Définit si l’objet projette des ombres (*true*, valeur par défaut) ou pas (*false*).
- **receive_shadows** : *booléen*  
  Définit si l’objet reçoit des ombres (*true*, valeur par défaut) ou pas (*false*).
- **cullable** : *booléen*  
  Définit si l’objet est cullable (*true*, valeur par défaut) ou pas (*false*).

#### Section materials

- **material** : *entier*, *nom*  
  Index du sous-maillage et nom du matériau à utiliser.

### Section billboard

Permet de définir des billboards partageant le même matériau et faisant les mêmes dimensions.

- **parent** : *nom*  
  Définit le SceneNode auquel ces billboards seront attachés.
- **type** : *valeur*  
  Définit le type de billboards. Les valeurs possibles sont :
  - *cylindrical*: Les billboards font face à la caméra, sauf pour leur axe Y, qui reste fixe.
  - *spherical*: Les billboards font face à la caméra sur tous les axes.
- **positions** : *section*  
  Permet de définir les positions relatives des différentes instances des billboards.
- **material** : *nom*  
  Définit le matériau utilisé pour l’affichage des billboards.
- **dimensions** : *taille*  
  Définit la taille des billboards.
- **size** : *valeur*  
  Définit le redimensionnement des billboards. Les valeurs possibles sont :
  - *dynamic*: La taille varie, en fonction de la distance de la caméra.
  - *fixed*: La taille est fixe, quelle que soit la distance à la caméra.

#### Section positions

- **pos** : *3 réels*  
  Définit la position relative d’un billboard.

### Section particle_system

- **parent** : *name*  
  Définit le SceneNode auquel ce partcle system sera attaché.
- **particles_count** : *uint*  
  Définit le nombre maximal supporté de particules.
- **material** : *name*  
  Définit le matériau utilisé par les particules.
- **dimensions** : *size*  
  Définit la taille des particules.
- **particle** : *section*  
  Nouvelle section décrivant le contenu d'une particule.
- **cs_shader_program** : *section*  
  Nouvelle section décrivant le programme shader utilisé pour gérer les particules.

#### Section particle

- **type** : *nom*  
  Définit le nom du type de particule.
- **variable** : *nom* *valeur* *valeur par défaut*   
  Définit une variable membre d'une particule.
  Le premier paramètre est le nom de la variable.
  Le second pparamètre est le type de la variable, peut valoir :
  - *int* : 1 entier signé.
  - *vec2i* : 2 entiers signés.
  - *vec3i* : 3 entiers signés.
  - *vec4i* : 4 entiers signés.
  - *uint* : 1 entier non signé.
  - *vec2ui* : 2 entiers non signés.
  - *vec3ui* : 3 entiers non signés.
  - *vec4ui* : 4 entiers non signés.
  - *float* : 1 nombre flottant simple précision.
  - *vec2f* : 2 nombres flottants simple précision.
  - *vec3f* : 3 nombres flottants simple précision.
  - *vec4f* : 4 nombres flottants simple précision.
  - *mat2x2f* : Matrice 2x2 de nombres flottants simple précision.
  - *mat3x3f* : Matrice 3x3 de nombres flottants simple précision.
  - *mat4x4f* : Matrice 4x4 de nombres flottants simple précision.
  Le troisième paramètre est la valeur par défaut de la variable.

### Section camera

- **parent** : *nom*  
  Définit le CameraNode auquel la caméra est attachée.
- **primitive** : *valeur*  
  Définit le type d’affichage de la caméra, peut prendre les valeurs suivantes :
  - *points* : Des points.
  - *lines* : Des lignes disjointes.
  - *line_loop* : Une boucle formée de lignes jointes.
  - *line_strip* : Des lignes jointes.
  - *triangles* : Des triangles disjoints.
  - *triangle_strip* : Des triangles joints.
  - *triangle_fan* : Des triangles joints par le premier point.
  - *quads* : Des quadrilatères disjoints.
  - *quad_strip* : Des quadrilatères joints.
  - *polygon* : Des polygones.
- **viewport** : *section*
  Définit la fenêtre d’affichage de la caméra.
- **hdr_config** : *section*  
  Définit une nouvelle section, décrivant la configuration HDR.

#### Section viewport

- **type** : *valeur*  
  Type de viewport. Peut prendre les valeurs suivantes :
  - *ortho* : Viewport orthographique.
  - *perspective* : Viewport perspective.
  - *frustum* : Viewport frustum.
- **left** : *réel*  
  Définit la coordonnée X minimale affichée.
- **right** : *réel*  
  Définit la coordonnée X maximale affichée.
- **top** : *réel*  
  Définit la coordonnée Y minimale affichée.
- **bottom** : *réel*  
  Définit la coordonnée Y maximale affichée.
- **near** : *réel*  
  Définit la coordonnée Z minimale affichée.
- **far** : *réel*  
  Définit la coordonnée Z maximale affichée.
- **size** : *taille*  
  Définit la taille de la fenêtre d’affichage (en pixels).
- **fov_y** : *réel*  
  Définit l’angle d’ouverture vertical, en radians.
- **aspect_ratio** : *réel*  
  Définit l’aspect global de la fenêtre (1.33333 pour 4/3, 1.77777 pour 16/9 … ).

#### Section hdr_config

- **exposure** : *réel*  
  Définit l’exposition de la scène.
- **gamma** : *réel*  
  Définit la correction gamma de la scène.

### Section animated_object_group

- **animated_object** : *nom*  
  Définit le nom d’un objet à ajouter dans le groupe.
- **animated_mesh** : *nom*  
  Définit le nom d’un objet dont le maillage est à ajouter dans le groupe.
- **animated_skeleton** : *nom*  
  Définit le nom d’un objet dont le squelette est à ajouter dans le groupe.
- **animated_node** : *nom*  
  Définit le nom d’un node à ajouter dans le groupe.
- **animation** : *nom*  
  Ajoute l’animation dont le nom est donné à la liste d’animations communes.
- **start_animation** : *nom*  
  Démarre l’animation donnée.
- **pause_animation** : *nom*  
  Met l’animation donnée en pause (elle doit avoir été démarrée au préalable).

#### Section animation

- **looped** : *booléen*  
  Définit si l’animation est bouclée (*true*) ou pas (*false*, valeur par défaut).
- **scale** : *réel*  
  Définit la vitesse de l’animation (peut être négative, l’animation sera alors jouée à l’envers).
- **start_at** : *réel*  
  Définit l'index de départ de l'animation.
- **stop_at** : *réel*  
  Définit l'index d'arrêt de l'animation.
- **interpolation** : *value*  
  Définit l'interpolation entre les keyframes, peut valoir :
  - *nearest* : Pas d'interpolation.
  - *linear* : Interpolation linéaire.

## Section skeleton

- **import** : *fichier* &lt;*options*&gt;  
  Permet l’import d’un fichier contenant les données du squelette. Ce fichier peut être au format cmsh ou tout autre format supporté par Castor3D. Uniquement si le type du maillage est **custom**. Cette directive peut de plus prendre plusieurs options parmi les suivantes :
  - *rescale*=*réel* : Met le squelette à l’échelle, sur les trois axes.
  - *preferred_importer*=*nom* : Permet de définir un importeur qui sera utilisé plutôt qu'un autre, si plusieurs importeurs supportent le format de fichier.
- **import_anim** : *fichier* &lt;*options*&gt;  
  Permet l’import d’un fichier contenant des données d'animation de squelette.  
  Cette directive peut de plus prendre plusieurs options parmi les suivantes :
  - *rescale*=*réel* : Met le squelette à l’échelle, sur les trois axes.
  - *preferred_importer*=*nom* : Permet de définir un importeur qui sera utilisé plutôt qu'un autre, si plusieurs importeurs supportent le format de fichier.

## Section mesh

- **type** : *nom*  
  Nom du type de maillage. Peut être :
  - *custom* : maillage défini manuellement ou maillage importé.
  - *cube* : cube, il faut définir ses 3 dimensions par la suite.
  - *cone* : cône, il faut définir son rayon et sa hauteur par la suite.
  - *cylinder* : cylindre, dont il faut entrer ensuite le rayon et la hauteur.
  - *sphere* : sphère à faces “carrées”, il faut définir le nombre de subdivision et le rayon.
  - *icosahedron* : sphère à faces triangulaires, il faut définir le nombre de subdivision et le rayon.
  - *torus* : torre, il est nécessaire de définir le nombre de subdivisions internes, externes et les rayons interne et externe.
  - *plane* : un plan, il est nécessaire de définir le nombre de subdivisions en largeur et en profondeur ainsi que la largeur et la profondeur.
- **skeleton** : *name*  
  Définit le squelette utilisé par le maillage.
- **import** : *fichier* &lt;*options*&gt;  
  Permet l’import d’un fichier contenant les données du maillage. Ce fichier peut être au format cmsh ou tout autre format supporté par Castor3D. Uniquement si le type du maillage est **custom**. Cette directive peut de plus prendre plusieurs options parmi les suivantes :
  - *rescale*=*réel* : Met le maillage à l’échelle, sur les trois axes.
  - *pitch*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe X.
  - *yaw*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe Y.
  - *roll*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe Z.
  - *preferred_importer*=*nom* : Permet de définir un importeur qui sera utilisé plutôt qu'un autre, si plusieurs importeurs supportent le format de fichier.
- **import_morph_target** : *fichier* &lt;*options*&gt;  
  Permet l’import d’un fichier contenant des données de maillage qui seront utilisées en tant que morph targets pour le mesh courant. Ce fichier peut être au format cmsh ou tout autre format supporté par Castor3D. Uniquement si le type du maillage est **custom**. Cette directive peut de plus prendre plusieurs options parmi les suivantes :
  Ce fichier peut être au format cmsh ou tout autre format supporté par Castor3D.  
  Disponible uniquement si le type du maillage est *custom*.  
  Cette directive peut de plus prendre plusieurs options parmi les suivantes :
  - *rescale*=*réel* : Met le maillage à l’échelle, sur les trois axes.
  - *pitch*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe X.
  - *yaw*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe Y.
  - *roll*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe Z.
  - *preferred_importer*=*nom* : Permet de définir un importeur qui sera utilisé plutôt qu'un autre, si plusieurs importeurs supportent le format de fichier.
- **import_anim** : *fichier* &lt;*options*&gt;  
  Permet l’import d’un fichier contenant des données d'animation de maillage.  
  Ce fichier peut être au format cmsh ou tout autre format supporté par Castor3D.  
  Disponible uniquement si le type du maillage est *custom*.  
  Cette directive peut de plus prendre plusieurs options parmi les suivantes :
  - *rescale*=*réel* : Met le maillage à l’échelle, sur les trois axes.
  - *pitch*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe X.
  - *yaw*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe Y.
  - *roll*=*réel* : Tourne le maillage de l'angle donné (en degrés) autour de l'axe Z.
  - *preferred_importer*=*nom* : Permet de définir un importeur qui sera utilisé plutôt qu'un autre, si plusieurs importeurs supportent le format de fichier.
- **division** : *nom* *entier*  
  Permet la subdivision du maillage en utilisant un algorithm défini par le nom donné (support en fonction des plugins). Le second paramètre est le nombre de fois où la division est effectuée (récursivement).
- **submesh** : *section*  
  Définit un sous-maillage, uniquement si le type du maillage est **custom**.
- **morph_animation** : *section*  
  Définit une nouvelle section, décrivant une animation de morphing.
- **default_materials** : *section*  
  Permet de définir le matériau pour chaque sous-maillage.

### Section submesh

- **vertex** : *3 réels*  
  Ajoute le sommet défini par les coordonnées au sous-maillage.
- **uv** : *2 réels*  
  Définit les uv à utiliser pour le sommet déclaré précédemment.
- **uvw** : *3 réels*  
  Définit les uvw à utiliser pour le sommet déclaré précédemment.
- **normal** : *3 réels*  
  Définit la normale à utiliser pour le sommet déclaré précédemment.
- **tangent** : *3 réels*  
  Définit la tangente à utiliser pour le sommet déclaré précédemment.
- **face** : *3 ou 4 entiers*  
  Définit une face qui utilise les 3 ou 4 indices de sommet. S’il y a plus de 3 indices, crée autant de faces triangulaires que nécessaire.
- **face_uv** : *autant d’uv que d’indices de la face*  
  Définit les uv à utiliser pour la face déclarée précédemment.
- **face_uvw** : *autant d’uvw que d’indices de la face*  
  Définit les uvw à utiliser pour la face déclarée précédemment.
- **face_normals** : *autant de groupes de 3 réels que d’indices de la face*  
  Définit les normales à utiliser pour la face déclarée précédemment.
- **face_tangents** : *autant de groupes de 3 réels que d’indices de la face*  
  Définit les tangentes à utiliser pour la face déclarée précédemment.

### Section morph_animation

- **target_weight** : *réel* *entier* *réel*
  Définit le poids pour une morph target. La première valeur est un indice de temps en secondes. La seconde valeur est l'indice de la morph target. La troisième valeur est le poids de la target.

#### Section default_materials

- **material** : *entier*, *nom*  
  Index du sous-maillage et nom du matériau à utiliser.

## Section panel_overlay

- **material** : *nom*  
  Définit le matériau utilisé par le panneau.
- **position** : *2 réels*  
  Définit la position de l’incrustation, par rapport à son parent (ou à l’écran).
- **size** : *2 réels*  
  Définit la taille de l’incrustation, par rapport à son parent (ou à l’écran).
- **pxl_position** : *2 entiers*  
  Définit la position absolue de l’incrustation, en pixels.
- **pxl_size** : *2 entiers*  
  Définit la taille absolue de l’incrustation, en pixels.
- **uv** : *4 réels*  
  Définit les UV pour l’incrustation (gauche, haut, droit, bas).
- **panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau simple.
- **border_panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec bordure.
- **text_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec texte.

## Section border_panel_overlay

- **material** : *nom*  
  Définit le matériau utilisé par le panneau.
- **position** : *2 réels*  
  Définit la position de l’incrustation, par rapport à son parent (ou à l’écran).
- **size** : *2 réels*  
  Définit la taille de l’incrustation, par rapport à son parent (ou à l’écran).
- **pxl_position** : *2 réels*  
  Définit la position absolue de l’incrustation, en pixels.
- **pxl_size** : *2 réels*  
  Définit la taille absolue de l’incrustation, en pixels.
- **center_uv** : *4 réels*  
  Définit les UV pour le centre de l’incrustation (gauche, haut, droit, bas).
- **border_material** : *nom*  
  Définit le matériau utilisé par la bordure du panneau.
- **border_position** : *valeur*  
  Définit la position de la bordure de l’incrustation, parmi les valeurs suivantes :
  - *internal* : La bordure à l’intérieur de l’inscrustation (elle ne dépasse pas de l’incrustation).
  - *middle* : La bordure est à moitié à l’intérieur et à moitié à l’extérieur de l’incrustation (elle dépasse de l’incrustation).
  - *external* : La bordure est à l’extérieur de l’incrustation (elle n’empiète pas sur le contenu de l’incrustation).
- **border_size** : *4 réels*  
  Définit la taille des bords (gauche, droite, haut , bas), par rapport au parent (ou à l’écran).
- **pxl_border_size** : *2 entiers*  
  Définit la taille absolue de l’incrustation, en pixels.
- **border_inner_uv** : *4 réels*  
  Définit les UV pour la bordure de l’incrustation, côté intérieur (gauche, haut, droit, bas).
- **border_outer_uv** : *4 réels*  
  Définit les UV pour la bordure de l’incrustation, côté extérieur (gauche, haut, droit, bas).
- **panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau simple.
- **border_panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec bordure.
- **text_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec texte.

## Section text_overlay

- **material** : *nom*  
  Définit le matériau utilisé par le panneau.
- **position** : *2 réels*  
  Définit la position de l’incrustation, par rapport à son parent (ou à l’écran).
- **size** : *2 réels*  
  Définit la taille de l’incrustation, par rapport à son parent (ou à l’écran).
- **pxl_position** : *2 entiers*  
  Définit la position absolue de l’incrustation, en pixels.
- **pxl_size** : *2 entiers*  
  Définit la taille absolue de l’incrustation, en pixels.
- **font** : *nom*  
  Définit la police utilisée par l’incrustation.
- **text** : *texte*  
  Définit le texte affiché.
- **text_wrapping** : *valeur*  
  Définit la manière dont le texte est découpé dans le cas où une ligne dépasse les dimensions de l’incrustation :
  - *none* : Le texte n’est pas découpé (ce qui dépasse n’est pas affiché).
  - *break* : Le texte est découpé à la lettre (les mots sont coupés).
  - *break_words* : Le texte est découpé au mot (les mots restent entiers).
- **vertical_align** : *valeur*  
  Définit la manière dont le texte est aligné verticalement, dans son incrustation :
  - *top* : Le texte est aligné en haut.
  - *center* : Le texte est centré.
  - *bottom* : Le texte est aligné en bas.
- **horizontal_align** : *valeur*  
  Définit la manière dont le texte est aligné horizontalement, dans son incrustation :
  - *left* : Le texte est aligné à gauche.
  - *center* : Le texte est centré.
  - *right* : Le texte est aligné à droite.
- **texturing_mode** : *valeur*  
  Définit la manière dont la texture est appliquée :
  - *letter* : La texture est appliquée sur chaque lettre.
  - *text* : La texture est appliquée sur tout le texte.
- **line_spacing_mode** : *valeur*  
  Définit la hauteur des lignes :
  - *own_height* : Chaque ligne a pour hauteur sa propre hauteur.
  - *max_lines_height* : Chaque ligne a pour hauteur la hauteur de la plus haute ligne.
  - *max_font_height* : Chaque ligne a pour hauteur la hauteur du plus haut caractère de la police.
- **panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau simple.
- **border_panel_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec bordure.
- **text_overlay** : *nom* *section*  
  Permet de définir une incrustation fille de type panneau avec texte.

## Section window

- **vsync** : *booléen*  
  Permet de dire si on veut activer la synchronisation verticale.
- **fullscreen** : *booléen*  
  Permet d’activer ou non l’affichage plein écran.
- **allow_hdr** : *booléen*  
  Permet d’activer le rendu HDR si la surface le permet.
- **render_target** : *section*  
  Permet de définir une nouvelle section décrivant la cible de rendu.

## Section render_target

- **scene** : *nom*  
  Permet de définir la scène rendue dans cette cible.
- **camera** : *nom*  
  Permet de définir la caméra utilisée pour rendre la scène.
- **size** : *taille*  
  Définit la taille du buffer interne de la cible de rendu.
- **format** : *valeur*  
  Définit le format des pixels du buffer couleur de la cible de rendu. Peut valoir :
  - *l8* : Luminance 8 bits, 1 nombre entier 8 bits.
  - *l16f* : Luminance 16 bits, 1 nombre en virgule flottante 16 bits (half float).
  - *l32f* : Luminance 32 bits, 1 nombre en virgule flottante 32 bits (float).
  - *al16* : Transparence + Luminance, 2 nombres entiers 8 bits.
  - *al16f* : Transparence + Luminance, 2 nombres en virgule flottante 16 bits (half float).
  - *al32f* : Transparence + Luminance, 2 nombres en virgule flottante 32 bits (float).
  - *argb1555* : ARGB 16 bits, 1 bit alpha et chaque composante sur un entier 5 bits.
  - *rgb565* : RGB 16 bits, R sur un entier 5 bits, G sur un entier 6 bits et B sur un entier 5 bits.
  - *argb16* : ARGB 16 bits, chaque composante sur un entier 4 bits.
  - *rgb24* : RGB 24 bits, chaque composante sur un entier 8 bits.
  - *argb32* : ARGB 32 bits, chaque composante sur un entier 8 bits.
  - *argb16f* : ARGB 64 bits, chaque composante sur un flottant 16 bits (half float).
  - *rgb32f* : RGB 96 bits, chaque composante sur un flottant 32 bits (float).
  - *argb32f* : ARGB 128 bits, chaque composante sur un flottant 32 bits (float).
- **hdr_format** : *valeur*  
  Définit le format des pixels du buffer couleur HDR de la cible de rendu, si la surface supporte le rendu HDR. Peut valoir :
  - *l8* : Luminance 8 bits, 1 nombre entier 8 bits.
  - *l16f* : Luminance 16 bits, 1 nombre en virgule flottante 16 bits (half float).
  - *l32f* : Luminance 32 bits, 1 nombre en virgule flottante 32 bits (float).
  - *al16* : Transparence + Luminance, 2 nombres entiers 8 bits.
  - *al16f* : Transparence + Luminance, 2 nombres en virgule flottante 16 bits (half float).
  - *al32f* : Transparence + Luminance, 2 nombres en virgule flottante 32 bits (float).
  - *argb1555* : ARGB 16 bits, 1 bit alpha et chaque composante sur un entier 5 bits.
  - *rgb565* : RGB 16 bits, R sur un entier 5 bits, G sur un entier 6 bits et B sur un entier 5 bits.
  - *argb16* : ARGB 16 bits, chaque composante sur un entier 4 bits.
  - *rgb24* : RGB 24 bits, chaque composante sur un entier 8 bits.
  - *argb32* : ARGB 32 bits, chaque composante sur un entier 8 bits.
  - *argb16f* : ARGB 64 bits, chaque composante sur un flottant 16 bits (half float).
  - *rgb32f* : RGB 96 bits, chaque composante sur un flottant 32 bits (float).
  - *argb32f* : ARGB 128 bits, chaque composante sur un flottant 32 bits (float).
- **srgb_format** : *valeur*  
  Définit le format des pixels du buffer couleur SRGB de la cible de rendu, si la surface ne supporte pas le rendu HDR. Peut valoir :
  - *l8* : Luminance 8 bits, 1 nombre entier 8 bits.
  - *l16f* : Luminance 16 bits, 1 nombre en virgule flottante 16 bits (half float).
  - *l32f* : Luminance 32 bits, 1 nombre en virgule flottante 32 bits (float).
  - *al16* : Transparence + Luminance, 2 nombres entiers 8 bits.
  - *al16f* : Transparence + Luminance, 2 nombres en virgule flottante 16 bits (half float).
  - *al32f* : Transparence + Luminance, 2 nombres en virgule flottante 32 bits (float).
  - *argb1555* : ARGB 16 bits, 1 bit alpha et chaque composante sur un entier 5 bits.
  - *rgb565* : RGB 16 bits, R sur un entier 5 bits, G sur un entier 6 bits et B sur un entier 5 bits.
  - *argb16* : ARGB 16 bits, chaque composante sur un entier 4 bits.
  - *rgb24* : RGB 24 bits, chaque composante sur un entier 8 bits.
  - *argb32* : ARGB 32 bits, chaque composante sur un entier 8 bits.
  - *argb16f* : ARGB 64 bits, chaque composante sur un flottant 16 bits (half float).
  - *rgb32f* : RGB 96 bits, chaque composante sur un flottant 32 bits (float).
  - *argb32f* : ARGB 128 bits, chaque composante sur un flottant 32 bits (float).
- **postfx** : *nom*, *parametres optionnels*  
  Définit un effet post-rendu à utiliser. Les paramètres optionnels dépendent de l’effet choisi.
- **tone_mapping** : *nom*  
  Définit l’opérateur de mappage de ton, à utiliser avec la cible de rendu.
- **ssao** : *section*  
  Définit une nouvelle section décrivant le Screen Space Ambient Occlusion.
- **clusters** : *section*  
  Définit une nouvelle section décrivant la configuration des clusters de sources lumineuses.

### Section ssao

- **enabled** : *booléen*  
  Définit le statut d’activation du SSAO.
- **high_quality** : *booléen*  
  Définit la qalité de l’effet.
- **radius** : *réel*  
  Définit le rayon de l’effet (exprimé en mètres).
- **bias** : *réel*  
  Définit le bias de l’effet.
- **intensity** : *réel*  
  Définit l’intensité de l’effet.
- **num_samples** : *entier*  
  Définit le nombre d’échantillons par pixel.
- **edge_sharpness** : *réel*  
  Définit la netteté des contours lors de la passe de flou.
- **blur_step_size** : *entier*  
  Définit la taille d’une étape, durant la passe de flou.
- **blur_radius** : *entier*  
  Définit le rayon du flou.

### clusters section

- **enabled** : *booléen*
  Définit le statut d'activation du clustered lighting.
- **use_lights_bvh** : *booléen*  
  Dit si le BVH est utilisé.
- **sort_lights** : *booléen*  
  Dit si les sources lumineuses sont triées avant la construction du BVH.
- **limit_clusters_to_lights_aabb** : *booléen*  
  Dit si les clusters de la caméra sont limités aux sources lumineuses visibles.
- **parse_depth_buffer** : *booléen*  
  Dit si le depth buffer est utilisé pour limiter le nombre de cluster de caméra à traiter.
- **use_spot_bounding_cone** : *booléen*  
  Dit si les bounding boxes des spot lights sont des cônes.
- **use_spot_tight_aabb** : *booléen*  
  Dit si les bounding boxes des spot lights sont les plus petites possibles.
- **enable_reduce_warp_optimisation** : *booléen*  
  Dit si la passe de réduction des bounding boxes des sources lumineuses utilise l'optimisation des warps.
- **enable_bvh_warp_optimisation** : *booléen*  
  Dit si la passe de construction du BVH utilise l'optimisation des warps.
- **split_scheme** : *value*  
  Définit le schéma de répartition des clusters le long de la profondeur de scène, peut valoir :
  - *exponential* : L'augmentation de la profondeur de clusters est exponentielle.
  - *exponential_biased* : L'augmentation de la profondeur de clusters est exponentielle, commençant à une taille donnée.
  - *linear* : L'augmentation de la profondeur de clusters est linéaire.
  - *hybrid* : Commence linéaire, finit exponentielle.
- **bias** : *réel*  
  Définit la taille de départ lorsque *split_scheme* vaut *exponential_biased*.

## Section gui

- **theme** : *name* *section*  
  Permet de définir un thème.
- **box_layout** : *section*  
  Permet de définir un box layout.
- **button** : *name* *section*  
  Permet de définir un bouton.
- **static** : *name* *section*  
  Permet de définir un texte statique.
- **slider** : *name* *section*  
  Permet de définir un slider.
- **combobox** : *name* *section*  
  Permet de définir une liste déroulante.
- **listbox** : *name* *section*  
  Permet de définir une liste.
- **edit** : *name* *section*  
  Permet de définir un texte éditable.
- **panel** : *name* *section*  
  Permet de définir un panneau.
- **progress** : *name* *section*  
  Permet de définir une barre de progression.
- **expandable_panel** : *name* *section*  
  Permet de définir un panneau extensible.
- **frame** : *name* *section*  
  Permet de définir une fenêtre.
- **default_font** : *name*  
  Permet de définir la police de texte.
- **button_style** : *name* *section*  
  Permet de définir un style de bouton.
- **static_style** : *name* *section*  
  Permet de définir un style de texte statique.
- **slider_style** : *name* *section*  
  Permet de définir un style de slider.
- **combobox_style** : *name* *section*  
  Permet de définir un style de liste déroulante.
- **listbox_style** : *name* *section*  
  Permet de définir un style de liste.
- **edit_style** : *name* *section*  
  Permet de définir un style de texte éditable.
- **panel_style** : *name* *section*  
  Permet de définir un style de panneau.
- **progress_style** : *name* *section*  
  Permet de définir un style de barre de proogression.
- **expandable_panel_style** : *name* *section*  
  Permet de définir un style de panneau extensible.
- **frame_style** : *name* *section*  
  Permet de définir un style de fenêtre.
- **scrollbar_style** : *name* *section*  
  Permet de définir un style de barre de progression.

### Section theme

- **default_font** : *name*  
  Permet de définir la police de texte.
- **button_style** : *name* *section*  
  Permet de définir un style de bouton.
- **static_style** : *name* *section*  
  Permet de définir un style de texte statique.
- **slider_style** : *name* *section*  
  Permet de définir un style de slider.
- **combobox_style** : *name* *section*  
  Permet de définir un style de liste déroulante.
- **listbox_style** : *name* *section*  
  Permet de définir un style de liste.
- **edit_style** : *name* *section*  
  Permet de définir un style de texte éditable.
- **panel_style** : *name* *section*  
  Permet de définir un style de panneau.
- **progress_style** : *name* *section*  
  Permet de définir un style de barre de proogression.
- **expandable_panel_style** : *name* *section*  
  Permet de définir un style de panneau extensible.
- **frame_style** : *name* *section*  
  Permet de définir un style de fenêtre.
- **scrollbar_style** : *name* *section*  
  Permet de définir un style de barre de progression.

### Section box_layout

- **layout_ctrl** : *name*  
  Ajoute un contrôle au layout.
- **layout_staspace** : *uint*  
  Ajoute un spacer à taille statique au layout.
- **layout_dynspace**  
  Ajoute un spacer à taille dynamique au layout.
- **horizontal** : *boolean*  
  Définit la direction du layout à horizontale.

#### Section layout_ctrl

- **horizontal_align** : *value*
  Définit l'alignement horizontal du contrôle, peut valoir :
  - *left* : Aligné à gauche.
  - *center* : Centré.
  - *right* : Aligné à droite.
- **vertical_align** : *value*  
  Définit l'alignement vertical du contrôle, peut valoir :
  - *top* : Aligné en haut.
  - *center* : Centré.
  - *bottom* : Aligné en bas.
- **stretch** : *boolean*
  Définit si le contrôle remplit le layout, dans la direction opposée à celle du layout.
- **reserve_if_hidden** : *boolean*
  Définit si les dimensions du contrôle sont réservées même si le contrôle est caché.
- **padding** : *4 uints*
  Définit le nombre de pixels de padding autour du contrôle.
- **pad_left** : *uint*
  Définit le nombre de pixels de padding à gauche du contrôle.
- **pad_top** : *uint*
  Définit le nombre de pixels de padding au dessus du contrôle.
- **pad_right** : *uint*
  Définit le nombre de pixels de padding à droite du contrôle.
- **pad_bottom** : *uint*
  Définit le nombre de pixels de padding en dessous du contrôle.

### Section button

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **caption** : *text*  
  Définit le texte du bouton.
- **horizontal_align** : *value*
  Définit l'alignement horizontal du texte du bouton, peut valoir :
  - *left* : Aligné à gauche.
  - *center* : Centré.
  - *right* : Aligné à droite.
- **vertical_align** : *value*  
  Définit l'alignement vertical du texte du bouton, peut valoir :
  - *top* : Aligné en haut.
  - *center* : Centré.
  - *bottom* : Aligné en bas.

### Section combobox

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **item** : *text*  
  Définit un item de la liste déroulante.

### Section edit

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **vertical_scrollbar** : *boolean*  
  Définit si le contrôle a une barre de défilement verticale en cas de besoin.
- **horizontal_scrollbar** : *boolean*  
  Définit si le contrôle a une barre de défilement horizontale en cas de besoin.
- **vertical_scrollbar_style** : *name*  
  Définit le style de la barre de défilement verticale.
- **horizontal_scrollbar_style** : *name*  
  Définit le style de la barre de défilement horizontale.
- **caption** : *text*  
  Définit le texte à éditer.
- **multiline** : *boolean*  
  Définit si le contrôle est multilignes.

### Section expandable_panel

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **expand_caption** : *text*  
  Définit le texte de la zone d'extension lorsque le panneau est étendu.
- **retract_caption** : *text*  
  Définit le texte de la zone d'extension lorsque le panneau est rétracté.
- **header** : *section*  
  Définit la zone d'en-tête du panneau.
- **expand** : *section*  
  Définit la zone d'extension du panneau.
- **content** : *section*
  Définit la zone du contenu du panneau.

#### Section header

- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **box_layout** : *section*  
  Permet de définir un box layout.
- **button** : *name* *section*  
  Permet de définir un bouton.
- **static** : *name* *section*  
  Permet de définir un texte statique.
- **slider** : *name* *section*  
  Permet de définir un slider.
- **combobox** : *name* *section*  
  Permet de définir une menu déroulant.
- **listbox** : *name* *section*  
  Permet de définir une liste.
- **edit** : *name* *section*  
  Permet de définir un texte éditable.
- **panel** : *name* *section*  
  Permet de définir un panneau.
- **progress** : *name* *section*  
  Permet de définir une barre de progression.
- **expandable_panel** : *name* *section*  
  Permet de définir un panneau extensible.
- **frame** : *name* *section*  
  Permet de définir une fenêtre.

#### Section expand

- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).

#### Section content

- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **box_layout** : *section*  
  Permet de définir un box layout.
- **button** : *name* *section*  
  Permet de définir un bouton.
- **static** : *name* *section*  
  Permet de définir un texte statique.
- **slider** : *name* *section*  
  Permet de définir un slider.
- **combobox** : *name* *section*  
  Permet de définir une menu déroulant.
- **listbox** : *name* *section*  
  Permet de définir une liste.
- **edit** : *name* *section*  
  Permet de définir un texte éditable.
- **panel** : *name* *section*  
  Permet de définir un panneau.
- **progress** : *name* *section*  
  Permet de définir une barre de progression.
- **expandable_panel** : *name* *section*  
  Permet de définir un panneau extensible.
- **frame** : *name* *section*  
  Permet de définir une fenêtre.
- **vertical_scrollbar** : *boolean*  
  Définit si le contrôle a une barre de défilement verticale en cas de besoin.
- **horizontal_scrollbar** : *boolean*  
  Définit si le contrôle a une barre de défilement horizontale en cas de besoin.
- **vertical_scrollbar_style** : *name*  
  Définit le style de la barre de défilement verticale.
- **horizontal_scrollbar_style** : *name*  
  Définit le style de la barre de défilement horizontale.

### Section frame

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **horizontal_align** : *value*
  Définit l'alignement horizontal du texte du header, peut valoir :
  - *left* : Aligné à gauche.
  - *center* : Centré.
  - *right* : Aligné à droite.
- **vertical_align** : *value*  
  Définit l'alignement vertical du texte du header, peut valoir :
  - *top* : Aligné en haut.
  - *center* : Centré.
  - *bottom* : Aligné en bas.
- **header_caption** : *text*  
  Définit le texte de l'en-tête.
- **min_size** : *2 uints*  
  Définit les dimensions minimales de la fenêtre.
- **content** : *section*  
  Définit le contenu de la fenêtre.

#### Section content

- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **box_layout** : *section*  
  Permet de définir un box layout.
- **button** : *name* *section*  
  Permet de définir un bouton.
- **static** : *name* *section*  
  Permet de définir un texte statique.
- **slider** : *name* *section*  
  Permet de définir un slider.
- **combobox** : *name* *section*  
  Permet de définir une menu déroulant.
- **listbox** : *name* *section*  
  Permet de définir une liste.
- **edit** : *name* *section*  
  Permet de définir un texte éditable.
- **panel** : *name* *section*  
  Permet de définir un panneau.
- **progress** : *name* *section*  
  Permet de définir une barre de progression.
- **expandable_panel** : *name* *section*  
  Permet de définir un panneau extensible.
- **frame** : *name* *section*  
  Permet de définir une fenêtre.
- **vertical_scrollbar** : *boolean*  
  Définit si le contrôle a une barre de défilement verticale en cas de besoin.
- **horizontal_scrollbar** : *boolean*  
  Définit si le contrôle a une barre de défilement horizontale en cas de besoin.
- **vertical_scrollbar_style** : *name*  
  Définit le style de la barre de défilement verticale.
- **horizontal_scrollbar_style** : *name*  
  Définit le style de la barre de défilement horizontale.

### Section listbox

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **item** : *text*  
  Définit un item de la liste.

### Section panel

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **box_layout** : *section*  
  Permet de définir un box layout.
- **button** : *name* *section*  
  Permet de définir un bouton.
- **static** : *name* *section*  
  Permet de définir un texte statique.
- **slider** : *name* *section*  
  Permet de définir un slider.
- **combobox** : *name* *section*  
  Permet de définir une menu déroulant.
- **listbox** : *name* *section*  
  Permet de définir une liste.
- **edit** : *name* *section*  
  Permet de définir un texte éditable.
- **panel** : *name* *section*  
  Permet de définir un panneau.
- **progress** : *name* *section*  
  Permet de définir une barre de progression.
- **expandable_panel** : *name* *section*  
  Permet de définir un panneau extensible.
- **frame** : *name* *section*  
  Permet de définir une fenêtre.
- **vertical_scrollbar** : *boolean*  
  Définit si le contrôle a une barre de défilement verticale en cas de besoin.
- **horizontal_scrollbar** : *boolean*  
  Définit si le contrôle a une barre de défilement horizontale en cas de besoin.
- **vertical_scrollbar_style** : *name*  
  Définit le style de la barre de défilement verticale.
- **horizontal_scrollbar_style** : *name*  
  Définit le style de la barre de défilement horizontale.

### Section progress

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **left_to_right** : *boolean*  
  Définit si la progression s'effectue de gauche à droite (défaut).
- **right_to_left** : *boolean*  
  Définit si la progression s'effectue de droite à gauche.
- **top_to_bottom** : *boolean*  
  Définit si la progression s'effectue de de haut en bas.
- **bottom_to_top** : *boolean*  
  Définit si la progression s'effectue de bas en haut.
- **hide_title** : *boolean*
  Définit si le titre est caché.
- **container_border_size** : *4 uints*  
  Définit si les dimensions des bordures du conteneur de la barre de progression.
- **bar_border_size** : *4 uints*  
  Définit si les dimensions des bordures de la barre de progression.
  Defines the progress bar border sizes.

### Section slider

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).

### Section static

- **theme** : *name*  
  Définit le style du contrôle depuis le thème donné.
- **style** : *name*  
  Définit le style du contrôle
- **visible** : *boolean*  
  Définit la visibilité du contrôle
- **pixel_position** : *2 ints*  
  Définit la position du contrôle, en pixels.
- **pixel_size** : *2 uints*  
  Définit les dimensions du contrôle, en pixels.
- **movable** : *boolean*  
  Définit si le contrôle est déplaçable.
- **resizable** : *boolean*  
  Définit si le contrôle est redimensionnable.
- **pixel_border_size** : *4 uints*  
  Définit les dimensions des bordures du contrôle (gauche, droite, haut, bas).
- **border_inner_uv** : *4 floats*  
  Définit les UV internes des bordures du contrôle (gauche, droite, haut, bas).
- **border_outer_uv** : *4 floats*  
  Définit les UV externes des bordures du contrôle (gauche, droite, haut, bas).
- **center_uv** : *4 floats*  
  Définit les UV du centre du contrôle (gauche, droite, haut, bas).
- **caption** : *text*  
  Définit le texte du contrôle.
- **horizontal_align** : *value*
  Définit l'alignement horizontal du texte, peut valoir :
  - *left* : Aligné à gauche.
  - *center* : Centré.
  - *right* : Aligné à droite.
- **vertical_align** : *value*  
  Définit l'alignement vertical du texte, peut valoir :
  - *top* : Aligné en haut.
  - *center* : Centré.
  - *bottom* : Aligné en bas.
  
### Section button_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **text_material** : *name*  
  Définit le matériau du texte du bouton.
- **highlighted_background_material** : *name*  
  Définit le matériau de l'arrière plan du bouton en surbrillance.
- **highlighted_foreground_material** : *name*  
  Définit le matériau du premier plan du bouton en surbrillance.
- **highlighted_text_material** : *name*  
  Définit le matériau du texte du bouton en surbrillance.
- **pushed_background_material** : *name*  
  Définit le matériau de l'arrière plan du bouton appuyé.
- **pushed_foreground_material** : *name*  
  Définit le matériau du premier plan du bouton appuyé.
- **pushed_text_material** : *name*  
  Définit le matériau du texte du bouton appuyé.
- **disabled_background_material** : *name*  
  Définit le matériau de l'arrière plan du bouton désactivé.
- **disabled_foreground_material** : *name*  
  Définit le matériau du premier plan du bouton désactivé.
- **disabled_text_material** : *name*  
  Définit le matériau du texte du bouton désactivé.
- **font** : *name*
  Définit la police du texte du bouton.

### Section combobox_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **expand_style** : *button_style*  
  Définit le style pour le bouton d'extension.
- **elements_style** : *listbox_style*  
  Définit le style pour les items.

### Section edit_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **text_material** : *name*  
  Définit le matériau du texte.
- **selection_material** : *name*  
  Définit le matériau du texte sélectionné.
- **font** : *name*
  Définit la police du texte.
- **scrollbar_style** : *scrollbar_style*
  Définit le style pour les barres de défilement.

### Section expandable_panel_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **header_style** : *panel_style*  
  Définit le style pour la zone d'en-tête du panneau.
- **expand_style** : *button_style*  
  Définit le style pour la zone d'extension du panneau.
- **content_style** : *panel_style*  
  Définit le style pour la zone de contenu du panneau.

### Section frame_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **header_text_material** : *name*  
  Définit le matériau du texte de l'en-tête.
- **header_font** : *name*  
  Définit la police du texte de l'en-tête.

### Section listbox_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **item_style** : *static_style*  
  Définit le style pour les items de la liste.
- **selected_item_style** : *static_style*  
  Définit le style pour les items sélectionnés.
- **highlighted_item_style** : *static_style*  
  Définit le style pour les items en surbrillance.

### Section panel_style

- **default_font** : *name*  
  Permet de définir la police de texte.
- **button_style** : *name* *section*  
  Permet de définir un style de bouton.
- **static_style** : *name* *section*  
  Permet de définir un style de texte statique.
- **slider_style** : *name* *section*  
  Permet de définir un style de slider.
- **combobox_style** : *name* *section*  
  Permet de définir un style de liste déroulante.
- **listbox_style** : *name* *section*  
  Permet de définir un style de liste.
- **edit_style** : *name* *section*  
  Permet de définir un style de texte éditable.
- **panel_style** : *name* *section*  
  Permet de définir un style de panneau.
- **progress_style** : *name* *section*  
  Permet de définir un style de barre de proogression.
- **expandable_panel_style** : *name* *section*  
  Permet de définir un style de panneau extensible.
- **frame_style** : *name* *section*  
  Permet de définir un style de fenêtre.
- **scrollbar_style** : *name* *section*  
  Permet de définir un style de barre de progression.
- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.

### Section progress_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **title_font** : *name*  
  Définit la police du texte du titre.
- **title_material** : *name*  
  Définit le matériau du texte du titre.
- **text_font** : *name*  
  Définit la police du texte de la barre de progression.
- **text_material** : *name*  
  Définit le matériau du texte de la barre de progression.
- **container_style** : *panel_style*  
  Définit le style du conteneur de la barre de progression.
- **bar_style** : *panel_style*  
  Définit le style de la barre de progression.

### Section slider_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **line_style** : *static_style*  
  Définit le style de la barre du slider.
- **tick_style** : *static_style*  
  Définit le style du curseur du slider.

### Section static_style

- **background_invisible** : *boolean*  
  Définit la visibilité de l'arrière plan du contrôle.
- **foreground_invisible** : *boolean*  
  Définit la visibilité du premier plan du contrôle.
- **border_invisible** : *boolean*  
  Définit la visibilité des bordures du contrôle.
- **background_material** : *name*  
  Définit le matériau de l'arrière plan du contrôle.
- **foreground_material** : *name*  
  Définit le matériau du premier plan du contrôle.
- **border_material** : *name*  
  Définit le matériau des bordures du contrôle.
- **text_material** : *name*  
  Définit le matériau du texte.
- **font** : *name*  
  Définit la police du texte.
