void main(void)
{
    float depth = gl_FragCoord.z;
    //gl_FragColor = vec4(1, 0, 0, 1);

    //colour based on proximity to viewport
    if(depth <= 0.5)
    {
        gl_FragColor.rgb = vec3(1, 0, depth);
    }
    else
    {
        gl_FragColor.rgb = vec3(0, 1, depth);
    }
}


    //circle pattern
    /*
    vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
    float dist_squared = dot(pos, pos);
    gl_FragColor = (dist_squared < 400.0) ? vec4(.90, .90, .90, 1.0): vec4(.20, .20, .40, 1.0);
    */