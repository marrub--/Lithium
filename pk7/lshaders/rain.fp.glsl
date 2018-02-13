// stolen from:
// "The Drive Home" by Martijn Steinrucken aka BigWings - 2017
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// thank you!
#define S(x, y, z) smoothstep(x, y, z)

vec3 N31(float p)
{
    //  3 out, 1 in... DAVE HOSKINS
   vec3 p3 = fract(vec3(p) * vec3(.1031,.11369,.13787));
   p3 += dot(p3, p3.yzx + 19.19);
   return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float SawTooth(float t)
   {return cos(t+cos(t))+sin(2.*t)*.2+sin(4.*t)*.02;}

float DeltaSawTooth(float t)
   {return 0.4*cos(2.*t)+0.08*cos(4.*t) - (1.-sin(t))*sin(t+cos(t));}

vec2 GetDrops(vec2 uv, float seed)
{
    float t = lith_t*2;
    vec2 o = vec2(0.);
    
    uv.y += t*.05;
    
    uv *= vec2(10., 2.5)*2.;
    vec2 id = floor(uv);
    vec3 n = N31(id.x + (id.y+seed)*546.3524);
    vec2 bd = fract(uv);
    
    vec2 uv2 = bd;
    
    bd -= .5;
    
    bd.y*=4.;
    
    bd.x += (n.x-.5)*.6;
    
    t += n.z * 6.28;
    float slide = SawTooth(t) / 2;
    
    float ts = 1.5;
    vec2 trailPos = vec2(bd.x*ts, (fract(bd.y*ts*2.-t*2.)-.5)*.5);
    
    bd.y += slide*2.;								// make drops slide down
    
    float dropShape = bd.x*bd.x;
    dropShape *= DeltaSawTooth(t);
    bd.y += dropShape;								// change shape of drop when it is falling
    
    float d = length(bd);							// distance to main drop
    
    float trailMask = S(-.2, .2, bd.y);				// mask out drops that are below the main
    trailMask *= bd.y;								// fade dropsize
    float td = length(trailPos*max(.5, trailMask));	// distance to trail drops
    
    float mainDrop = S(.2, .1, d);
    float dropTrail = S(.1, .02, td);
    
    dropTrail *= trailMask;
    o = mix(bd*mainDrop, trailPos, dropTrail);		// mix main drop and drop trail
    
    return o;
}

void main()
{
   vec2 uv = TexCoord;
   vec2 offs;

   offs = GetDrops(uv, 1.);
   offs += GetDrops(uv * 1.4, 10.);

    float ripple = sin(lith_t+uv.y*3.1415*30.+uv.x*124.)*.5+.5;
    ripple *= .007;
    offs += vec2(ripple*ripple, ripple);

   uv += offs*lith_m;

   FragColor = texture(InputTexture, uv);
}

// EOF
