#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord;
	vec4 color;
} vs_out;

struct Material
{
	vec4 color;
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;
uniform int blendingType;
uniform int shadingType;
uniform int alphaType;
uniform int alphaMultiplier;

struct Light
{
	vec3 ambient; // 0
	vec3 diffuse; // 16
	vec3 specular; // 32

	float constant; // 44 ?
	float linear; // 48 ?
	float quadratic; // 52 ?
};

layout (std140) uniform Lights
{
	Light light;
};

uniform vec3 viewPos;
uniform vec3 lightPosition;


void blendTexture(vec4 baseColor);
void blendColor();
void applyShading();
vec3 applySpecular(vec3 lightDir, float attenuation);

void main()
{
	bool shade;
	if (shadingType < 3)
	{
		blendTexture(texture(material.diffuse, vs_out.texCoord));
		if (alphaType == 1)
			FragColor.a *= alphaMultiplier;
		shade = false;//shadingType != 0;
	}
	else if (shadingType < 5)
	{
		blendColor();
		shade = false;//shadingType == 4;
	}
	else
	{
		vec4 texColor = texture(material.diffuse, vs_out.texCoord);
		blendTexture(texColor * vs_out.color);
		if (alphaType == 1)
			FragColor.a *= alphaMultiplier;
		shade = false;//shadingType == 6;
	}

	if (FragColor.a < 0.01)
		discard;
	
	if (shade)
		applyShading();
};

void blendTexture(vec4 baseColor)
{
	vec4 combo = baseColor * material.color;
	switch (blendingType)
	{
	case 0:
		FragColor = vec4(baseColor.rgb, 1);
		break;
	case 1:
		FragColor = baseColor;
		break;
	case 2:
		FragColor = baseColor;
		break;
	case 3:
		FragColor = baseColor;
		break;
	case 4:
		FragColor = vec4(baseColor.rgb * material.color.rgb, 1);
		break;
	case 5:
		FragColor = vec4(combo.rgb, 1 - material.color.a);
	}
};

void blendColor()
{
	vec4 combo = vs_out.color * material.color;
	switch (blendingType)
	{
	case 0:
		FragColor = vec4(vs_out.color.rgb, 1);
		break;
	case 1:
		FragColor = vec4(vs_out.color.rgb, (vs_out.color.r + vs_out.color.g + vs_out.color.b) / 3);
		break;
	case 2:
		FragColor = vec4(vs_out.color.rgb, (combo.r * combo.g * combo.b));
		break;
	case 3:
		FragColor = vs_out.color;
		break;
	case 4:
		FragColor = vec4(vs_out.color.rgb * material.color.rgb, 1);
		break;
	case 5:
		FragColor = vec4(combo.rgb, 1 - material.color.a);
	}
};

vec3 applySpecular(vec3 lightDir, float attenuation)
{
	vec3 viewDir = normalize(viewPos - vs_out.fragPos);
	vec3 reflectDir = reflect(-lightDir, vs_out.normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec =  material.color.a * pow(max(dot(vs_out.normal, halfwayDir), 0.0), material.shininess);
	return attenuation * material.specular * spec * light.specular;
};

void applyShading()
{
	float distance = length(lightPosition - vs_out.fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = attenuation * light.ambient * vec3(FragColor);
	vec3 lightDir = normalize(lightPosition - vs_out.fragPos);
	float diff = max(dot(vs_out.normal, lightDir), 0.0);
	vec3 diffuse = attenuation * light.diffuse * diff * vec3(FragColor);

	if (shadingType != 1)
	{
		vec3 specular = applySpecular(lightDir, attenuation);
		FragColor = vec4((ambient + diffuse + specular) * FragColor.rgb, FragColor.a);
	}
	else
		FragColor = vec4((ambient + diffuse) * FragColor.rgb, FragColor.a);
};