#version 150 core
#extension all : warn

in GS_OUT
{
    flat vec4 color[4];
    vec2 uv;
    float fog;
    
} fs_in;

out vec4 color;
//out float fog;

void main(void)
{
    // NOTE: I wasn't seeing any color after normalizing the verts
    // so this is a hard-coded value.
    // color = vec4(1.0,0.0,0.0,1.0);
    // return;
    

	// I need to tweak this
//	if( fs_in.uv.x < .02 || fs_in.uv.x > .98 || fs_in.uv.y < .02 || fs_in.uv.y > .98) {
//		color = vec4(1.0);
//	}
//	else {
		vec4 c1 = mix(fs_in.color[0], fs_in.color[1], fs_in.uv.x);
		vec4 c2 = mix(fs_in.color[2], fs_in.color[3], fs_in.uv.x);
		
		color = mix(c1, c2, fs_in.uv.y);
//	}
	
    
}