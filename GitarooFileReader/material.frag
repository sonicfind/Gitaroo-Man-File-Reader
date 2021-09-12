#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoord[2];
	vec4 color;
} vs_in;

struct Material
{
	int blendingType;
	int shadingType;
	sampler2D tex;
	vec4 diffuse;
	vec3 specular;
	float shininess;
	int alphaType;
	int alphaMultiplier;
};
uniform Material materials[2];
uniform int doMulti;

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

vec4 doStuff(const Material material, vec2 texCoord);
vec4 getBlendColor(const int blend, const float multiplier, const vec4 color);
vec4 applyShading(const Material material, vec4 baseColor);
vec3 applySpecular(const Material material, vec3 lightDir, float attenuation);

void main()
{
	//if (gl_FrontFacing)
	//	FragColor = vec4(0, 1, 0, 1);
	//else
	//	FragColor = vec4(1, 0, 0, 1);
	//return;
	vec4 result = doStuff(materials[0], vs_in.texCoord[0]);
	if (result.a < 0.01)
		discard;

	if (doMulti == 1)
	{
		vec4 col2 = doStuff(materials[1], vs_in.texCoord[1]);
		FragColor = vec4(result.rgb * (1 - col2.a) + col2.rgb * col2.a, result.a + col2.a - result.a * col2.a);
	}
	else
		FragColor = result;
}

vec4 doStuff(const Material material, vec2 texCoord)
{
	vec4 result = vec4(1.0f);
	if (material.shadingType < 3 || 5 <= material.shadingType)
	{
		vec4 texColor = texture(material.tex, texCoord);
		if (material.shadingType != 1)
			result = getBlendColor(material.blendingType, material.diffuse.a, texColor);
		else
			result = getBlendColor(material.blendingType, 1, texColor);

		if (material.alphaType == 1)
			result.a *= 2 * texColor.a;
	}
	
	if (material.shadingType >= 3)
		result *= getBlendColor(material.blendingType, material.diffuse.a, vs_in.color);

	switch (material.shadingType)
	{
	//case 1:
	//case 2:
	//case 4:
	//case 6:
	//	return applyShading(material, result);
	default:
		return result;
	}
};

vec4 getBlendColor(const int blend, const float multiplier, const vec4 color)
{
	switch (blend)
	{
	case 0:
		return vec4(color.rgb, 1);
	case 1:
		return vec4(color.rgb, multiplier * (color.r + color.g + color.b) / 3);
	case 2:
		return vec4(color.rgb, color.r * color.g * color.b);
	case 3:
		vec3 sub = vec3(1.0f) - color.rgb;
		return vec4(sub, 1 - min(multiplier * sub.r * sub.g * sub.b, 1));
	case 4:
		return vec4(color.rgb * .5, 1);
	case 5:
		return vec4(color.rgb, multiplier);
	}
}

vec4 applyShading(const Material material, vec4 baseColor)
{
	float distance = length(lightPosition - vs_in.fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = attenuation * light.ambient * vec3(baseColor);
	vec3 lightDir = normalize(lightPosition - vs_in.fragPos);
	float diff = max(dot(vs_in.normal, lightDir), 0.0);
	vec3 diffuse = attenuation * light.diffuse * diff * vec3(baseColor);

	if (material.shadingType != 1)
	{
		vec3 specular = applySpecular(material, lightDir, attenuation);
		return vec4((ambient + diffuse + specular) * baseColor.rgb, baseColor.a);
	}
	else
		return vec4((ambient + diffuse) * baseColor.rgb, baseColor.a);
};

vec3 applySpecular(const Material material, vec3 lightDir, float attenuation)
{
	vec3 viewDir = normalize(viewPos - vs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, vs_in.normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(vs_in.normal, halfwayDir), 0.0), material.shininess);
	if (material.shadingType == 1)
		spec *= material.diffuse.a;
	return attenuation * material.specular * spec * light.specular;
};
