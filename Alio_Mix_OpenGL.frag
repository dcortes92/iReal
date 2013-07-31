uniform sampler2D Sampler;
varying vec3 texCoord;

void main(void)
{

	vec3 multipliers;
	multipliers.x = 1.0;
	multipliers.y = 0.125;
	multipliers.z = 1.0;

	vec3 offsets0;
	offsets0.x = 0.0;
	offsets0.y = 0.125;
	offsets0.z = 0.0;
	
	vec3 offsets1;
	offsets1.x = 0.0;
	offsets1.y = 0.25;
	offsets1.z = 0.0;

	vec3 offset;
	vec3 coord;
	coord = texCoord;
	coord.z -= fract(coord.z);
	coord.y += (coord.z * multipliers.y);
   
	//gl_FragColor = texture2D( Sampler, fract(coord));
	//gl_FragColor.y = texture2D( Sampler, fract(coord.xy + offsets0)).y;
	//gl_FragColor.z = texture2D( Sampler, fract(coord.xy + offsets1)).z;

	gl_FragColor = texture2D( Sampler, fract(coord.xy));
	gl_FragColor.y = texture2D( Sampler, fract(coord.xy + offsets0.xy)).y;
	gl_FragColor.z = texture2D( Sampler, fract(coord.xy + offsets1.xy)).z;

}