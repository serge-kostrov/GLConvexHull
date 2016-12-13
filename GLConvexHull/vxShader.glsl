in vec3 in_Position;
in vec4 in_Color;

uniform mat4 un_MVP;

out vec4 vx_Color;

void main()
{
	vx_Color = in_Color;
	gl_Position = un_MVP*vec4(in_Position, 1.0);
}