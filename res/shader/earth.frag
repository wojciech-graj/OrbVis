#version 410 core

out vec4 frag_color;

in vec2 uv;
in vec3 pos;

uniform sampler2D texture_day;
uniform sampler2D texture_night;
uniform sampler2D texture_specular;
uniform sampler2D texture_clouds;
uniform sampler1D texture_gradient;
uniform vec3 sun_dir;
uniform vec3 look_pos;
uniform bool clouds;
uniform bool lighting;

const float PI = 3.14159;
const float TRANSITION_WIDTH = 0.1;
const float FRENSEL_EXPONENT = 5.0;
const float SHININESS = 12.0;

void main()
{
	if (lighting) {
		float angle_incidence = acos(dot(pos, sun_dir)) / PI;
		float shade_factor = 0.1 * (1 - angle_incidence) + 0.9 * (1 - (clamp(angle_incidence, 0.5, 0.5 + TRANSITION_WIDTH) - 0.5) * (1.0 / TRANSITION_WIDTH));

		vec3 rel_cam_dir = normalize(pos - look_pos);
		float angle_viewer = sin(acos(dot(pos, rel_cam_dir)));
		float persp_factor = 0.05 + 0.4 * pow(angle_viewer, FRENSEL_EXPONENT) + 0.55 * pow(angle_viewer, FRENSEL_EXPONENT * 20.0);
		float col_atmosphere = clamp(persp_factor * shade_factor * 1.4, 0.0, 1.0);

		vec3 color_specular = texture(texture_specular, uv).rrr * max(0.0, dot(sun_dir, pos)) * pow(max(0.0, dot(reflect(sun_dir, pos), rel_cam_dir)), SHININESS);
		vec4 color_atmosphere = texture(texture_gradient, angle_incidence) * col_atmosphere;
		vec3 color_surface = mix(texture(texture_night, uv), texture(texture_day, uv), shade_factor).rgb;
		vec3 color_comb_surface = color_surface + color_specular;
		if (clouds) {
			vec4 cloud_color = texture(texture_clouds, uv).rrrr * shade_factor;
			color_comb_surface = cloud_color.rgb + (1.0 - cloud_color.a) * (color_comb_surface);
		}
		frag_color = vec4(color_atmosphere.rgb + vec3(1.0 - color_atmosphere.a) * color_comb_surface, 1.0);
	} else {
		vec3 color_comb_surface = texture(texture_day, uv).rgb;
		if (clouds) {
			vec4 cloud_color = texture(texture_clouds, uv).rrrr;
			color_comb_surface = cloud_color.rgb + (1.0 - cloud_color.a) * (color_comb_surface);
		}
		frag_color = vec4(color_comb_surface, 1.0);
	}
}
