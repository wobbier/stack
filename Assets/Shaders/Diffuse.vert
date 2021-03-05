$input a_position, a_normal, a_texcoord0, a_tangent, a_bitagent
$output v_color0

#include "../../Engine/Assets/Shaders/Common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_color0 = vec4(a_normal.x,a_normal.y,a_normal.z,1.0);
}
