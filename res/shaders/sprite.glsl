@vertex
#version 330 core

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexCoord;

layout (location = 2) in vec2 iPos;
layout (location = 3) in vec2 iScale;
layout (location = 4) in float iZ;
layout (location = 5) in float iAngle; 
layout (location = 6) in vec4 iColor;
layout (location = 7) in vec4 iUVs;

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec4 color;

void main() {
	float c = cos(iAngle), s = sin(iAngle);
	vec2 pos = vPos * iScale;
	vec2 rotated = 
		vec2(
			pos.x * c - pos.y * s, 
			pos.x * s + pos.y * c);

	texCoord = mix(iUVs.xy, iUVs.zw, vTexCoord);
	color = iColor;

	gl_Position = projection * view * vec4(rotated + iPos, iZ, 1);
}

@fragment
#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec4 color;

uniform sampler2D tex;

void main() {
	vec4 outColor = texture(tex, texCoord) * color;
	if (outColor.a < 0.1)
		discard ;

	fragColor = outColor;
}
