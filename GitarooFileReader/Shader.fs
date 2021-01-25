#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec4 ourColor;
in vec2 texCoord;

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
uniform int useTexAlpha;


    struct Light
    {
        vec3 position;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };
    uniform Light light;
    uniform vec3 viewPos;


void blend(vec4 baseColor);
void applyShading();
vec3 applySpecular(vec3 lightDir, float attenuation);

void main()
{
    
    if (shadingType < 3)
    {
	vec4 texColor = texture(material.diffuse, texCoord);
	blend(texColor);
	if (false) //(shadingType != 0)
	    applyShading();
	if (useTexAlpha == 1)
	    FragColor.a = texColor.a;
    }
    else if (shadingType < 5)
    {
	blend(ourColor);
	if (false) //(shadingType == 4)
	    applyShading();
    }
    else
    {
	vec4 texColor = texture(material.diffuse, texCoord);
	blend(texColor * ourColor);
	if (false) //(shadingType == 6)
	    applyShading();
	if (useTexAlpha == 1)
	    FragColor.a = texColor.a;
    }
};

void blend(vec4 baseColor)
{
    vec4 combo = baseColor * material.color;
    switch (blendingType)
    {
    case 0:
	FragColor = vec4(baseColor.rgb, 1);
	break;
    case 1:
	FragColor = vec4(baseColor.rgb, (combo.r + combo.g + combo.b) / 3);
	break;
    case 2:
	FragColor = vec4(baseColor.rgb, (combo.r * combo.g * combo.b));
	break;
    case 3:
	FragColor = vec4(baseColor.rgb, (combo.r * combo.g * combo.b));
	break;
    case 4:
	FragColor = combo;
	break;
    case 5:
	FragColor = combo;
    }
};

vec3 applySpecular(vec3 lightDir, float attenuation)
{
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    return attenuation * material.specular * spec * light.specular;
};

void applyShading()
{
    float distance = length(light.position - fragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float attenuation = 1;

    vec3 ambient = attenuation * light.ambient * vec3(FragColor);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = attenuation * light.diffuse * diff * vec3(FragColor);

    if (shadingType != 1)
    {
	vec3 specular = applySpecular(lightDir, attenuation);
	FragColor = vec4((ambient + diffuse + specular) * FragColor.rgb, FragColor.a);
    }
    else
	FragColor = vec4((ambient + diffuse) * FragColor.rgb, FragColor.a);
};