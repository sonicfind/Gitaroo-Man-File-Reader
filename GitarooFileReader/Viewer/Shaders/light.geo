R"(#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 8) out;

const int MAX_LIGHTS = 4;
struct Light
{
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
	float min;
	float coefficient;
	float max;
};

layout (std140) uniform Lights
{
	int doLights;
	int numLights;
	float globalCoefficient;
	int useGlobal;

	vec3 globalVertexColor;
	vec3 sceneAmbience;
	Light lights[MAX_LIGHTS];
};

layout (std140) uniform View
{
	mat4 view;
};

layout (std140) uniform Projection
{
	mat4 projection;
};

void generateLightVector(int index)
{
	mat4 combo = projection * view;
	gl_Position = combo * gl_in[0].gl_Position;
	EmitVertex();
	
	gl_Position = combo * (gl_in[0].gl_Position + vec4(100 * lights[index].direction, 0));
	EmitVertex();
	
	EndPrimitive();
}

void main()
{	
	for (int i = 0; i < numLights; ++i)
		generateLightVector(i);
}
)"
