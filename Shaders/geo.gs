
#version 460 core

layout (triangles) in;
layout ( line_strip, max_vertices = 4 ) out;

in vec3 fragment_position_eval[];
in vec3 light_pos_out_eval[];



void main(){
	
     // Emit first edge
    gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.0f, -5.0f, 0.0f);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(0.0f, 0.0f, -5.0f, 0.0f);
    EmitVertex();

    // Emit second edge
    gl_Position = gl_in[2].gl_Position + vec4(0.0f, 0.0f, -5.0f, 0.0f);
    EmitVertex();

    // Emit third edge
    gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.0f, -5.0f, 0.0f);
    EmitVertex();
    EndPrimitive();


}