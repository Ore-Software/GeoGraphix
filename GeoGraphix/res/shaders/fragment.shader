#version 460 core

layout(location = 0) out vec4 color;

in vec3 pos;
in vec4 view_pos;
in vec3 view_pos_normal;
in vec4 light1_pos;
in vec4 light2_pos;

void main()
{
	vec3 ka; // ambient constant
	vec3 kd; // diffuse constant
	vec3 ks; // specular constnat
	float p; // phong exponent
	vec3 n = normalize(view_pos_normal);
	vec3 v = normalize(-view_pos.xyz);
	vec3 l1 = normalize(light1_pos.xyz - view_pos.xyz);
	vec3 l2 = normalize(light2_pos.xyz - view_pos.xyz);
	vec3 h1 = normalize(v + l1);
	vec3 h2 = normalize(v + l2);

	if (pos.y > 0.75)
	{
		ka = vec3(0.0, 0.25, 0.0);
		kd = vec3(0.0, 0.5, 0.0);
		ks = vec3(0.5, 0.5, 0.5);
		p = 50.0f;
	}
	else if (pos.y > 0.5)
	{
		ka = vec3(0.0, 0.125, 0.0);
		kd = vec3(0.0, 0.25, 0.0);
		ks = vec3(0.5, 0.5, 0.5);
		p = 50.0f;
	}
	else if (pos.y > 0.25)
	{
		ka = vec3(0, 0, 0.4);
		kd = vec3(0, 0, 0.8);
		ks = vec3(0.8, 0.8, 0.8);
		p = 500.0f;
	}
	else
	{
		ka = vec3(0, 0, 0.25);
		kd = vec3(0, 0, 0.5);
		ks = vec3(0.8, 0.8, 0.8);
		p = 500.0f;
	}

	color = vec4(ka * 0.2 + kd * max(0.0, dot(n, l1)) + kd * max(0.0, dot(n, l2)) + ks * pow(max(0, dot(n, h1)), p) + ks * pow(max(0, dot(n, h2)), p), 1.0);
};