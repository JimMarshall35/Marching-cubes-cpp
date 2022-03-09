// Fragment shader:
// ================
#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 FragWorldPos;
in vec3 Normal;
in vec3 LightPos;   // extra in variable, since we need the light position in view space we calculate this in the vertex shader
in vec3 WorldSpaceNormal;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;
layout(binding=0) uniform sampler2D RockTexture;
layout(binding=1) uniform sampler2D MossTexture;

const float GRASS_ROCK_BOUNDARY = -0.9;
const float ROCK_GRASS_FADE = 0.2;
const float TRIPLANAR_BLEND_SHARPNESS = 4.0;

vec3 LookupTextureTriplanar(in sampler2D tex){
	float textureScale = 5.0;
	vec2 yUV = FragWorldPos.xz / textureScale;
	vec2 xUV = FragWorldPos.zy / textureScale;
	vec2 zUV = FragWorldPos.xy / textureScale;
	
	vec3 yDiff = vec3(texture(tex, yUV));
	vec3 xDiff = vec3(texture(tex, xUV));
	vec3 zDiff = vec3(texture(tex, zUV));
	
	vec3 blendWeights = vec3(
		pow(abs(WorldSpaceNormal.x),TRIPLANAR_BLEND_SHARPNESS),
		pow(abs(WorldSpaceNormal.y),TRIPLANAR_BLEND_SHARPNESS),
		pow(abs(WorldSpaceNormal.z),TRIPLANAR_BLEND_SHARPNESS)
	);
	blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z);
	
	return xDiff * blendWeights.x + yDiff * blendWeights.y + zDiff * blendWeights.z;
}

void main()
{
    // ambient
    //float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;   
	vec3 grassPixel = LookupTextureTriplanar(MossTexture);
	vec3 rockPixel = LookupTextureTriplanar(RockTexture);
	vec3 pixel_colour = (FragWorldPos.y < GRASS_ROCK_BOUNDARY)? grassPixel : mix(grassPixel,rockPixel, clamp(((FragWorldPos.y -GRASS_ROCK_BOUNDARY)/ROCK_GRASS_FADE),0.0,1.0));
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    //float specularStrength = 0.2;
    vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor; 
    
    vec3 result = (ambient + diffuse + specular) * pixel_colour;
    FragColor = vec4(result, 1.0);
}