#ifdef GL_ES
precision mediump float;
#endif

//our texture samplers
uniform sampler2D u_TextureUnit;   //diffuse map
uniform sampler2D u_normals;   //normal map

//attributes from vertex shader
varying vec4 v_Color;
varying vec2 v_TexCoord;

//values used for shading algorithm...
uniform vec2 Resolution;      //resolution of screen
uniform vec3 LightPos;        //light position, normalized
uniform vec4 LightColor;      //light RGBA -- alpha is intensity
uniform vec4 AmbientColor;    //ambient RGBA -- alpha is intensity
uniform vec3 Falloff;         //attenuation coefficients

void main() {
    //RGBA of our diffuse color
    vec4 DiffuseColor = texture2D(u_TextureUnit, v_TexCoord);
    
    //RGB of our normal map
    vec3 NormalMap = texture2D(u_normals, v_TexCoord).rgb;
    
    //The delta position of light
    vec3 LightDir = vec3(LightPos.xy - (gl_FragCoord.xy / Resolution.xy), LightPos.z);
    
    //Correct for aspect ratio
    LightDir.x *= Resolution.x / Resolution.y;
    
    //Determine distance (used for attenuation) BEFORE we normalize our LightDir
    float D = length(LightDir);
    
    //normalize our vectors
    vec3 N = normalize(NormalMap * 2.0 - 1.0);
    vec3 L = normalize(LightDir);
    
    //Pre-multiply light color with intensity
    //Then perform "N dot L" to determine our diffuse term
    vec3 Diffuse = (LightColor.rgb * LightColor.a) * max(dot(N, L), 0.0);
    
    //pre-multiply ambient color with intensity
    vec3 Ambient = AmbientColor.rgb * AmbientColor.a;
    
    //calculate attenuation
    float Attenuation = 1.0 / ( Falloff.x + (Falloff.y*D) + (Falloff.z*D*D) );
    
    //the calculation which brings it all together
    vec3 Intensity = Ambient + Diffuse * Attenuation;
    vec3 FinalColor = DiffuseColor.rgb * Intensity;
    gl_FragColor = v_Color * vec4(FinalColor, DiffuseColor.a);
}
