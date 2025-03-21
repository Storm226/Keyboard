#version 460 core

out vec4 FragColor;

void main(){
    
       //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);

       float depth = (gl_FragCoord.z - 0.995) * 200;
        // FragColor = vec4(1.0, 0, 0, 1); //you might first set it to a constant color just for debugging
        FragColor = vec4(depth,depth,depth,1);
}