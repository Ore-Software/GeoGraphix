#version 460 core

layout(location = 0) in vec3 position;

out vec3 pos;
out vec4 view_pos;
out vec3 view_pos_normal;
out vec4 light1_pos;
out vec4 light2_pos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    pos = position;
    view_pos = u_View * u_Model * vec4(pos, 1.0);
    mat4 normalmatrix = transpose(inverse(u_Model));
    view_pos_normal = (normalmatrix * vec4(pos.xyz, 1.0)).xyz;
    light1_pos = u_Projection * u_View * vec4(0, 80, 10, 1);
    light2_pos = u_Projection * u_View * vec4(0, 20, 10, 1);

    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0f);
};