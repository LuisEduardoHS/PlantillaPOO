#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform float time; 

void main()
{
    // [FIX] TILING (Repeticion)
    // Multiplicamos las coordenadas:
    // x * 6.0 -> La textura se repite 6 veces a lo ancho
    // y * 20.0 -> La textura se repite 20 veces a lo largo
    vec2 TiledCoords = TexCoord * vec2(6.0, 20.0);

    // 1. EFECTO DE FLUJO
    vec2 movingUV = TiledCoords;
    
    // Mover en Y (usamos la coordenada ya repetida)
    movingUV.x += time * 0.05; 
    
    // Distorsion
    movingUV.y += sin(movingUV.y * 5.0 + time) * 0.02; 

    // 2. Obtener color
    vec4 texColor = texture(texture_diffuse1, movingUV);
    
    // 3. EFECTO RADIOACTIVO
    vec3 colorRadioactivo = vec3(0.1, 1.0, 0.2); 
    vec3 finalColor = mix(texColor.rgb, colorRadioactivo, 0.6);
    
    // 4. BRILLO
    float brillo = (sin(time * 0.5) + 1.0) * 0.15; 
    finalColor += brillo; 
    
    FragColor = vec4(finalColor, 0.85);
}