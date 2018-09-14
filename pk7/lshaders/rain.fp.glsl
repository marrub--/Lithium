// stolen from:
// "The Drive Home" by Martijn Steinrucken aka BigWings - 2017
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// thank you!
#define S(x, y, z) smoothstep(x, y, z)
vec3 N31(float p) {
   vec3 p3 = fract(vec3(p) * vec3(.1031,.11369,.13787));
   p3 += dot(p3, p3.yzx + 19.19);
   return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}
float SawTooth(float t) {return cos(t+cos(0.2*t))+sin(t)*.2+sin(t)*.02;}
float DeltaSawTooth(float t) {return .4*cos(2.*t)+.08*cos(4.*t) - (1.-sin(t))*sin(t+cos(t));}
vec2 GetDrops(vec2 uv, float seed) {
   float t = lith_t*2.;
   vec2 o = vec2(0.);
   uv.y += t*.05;
   uv *= vec2(10., 2.5)*1.5;
   vec2 id = floor(uv);
   vec3 n = N31(id.x + (id.y+seed)*546.3524);
   vec2 bd = fract(uv);
   vec2 uv2 = bd;
   bd -= .5;
   bd.y*=4.;
   bd.x += (n.x-.5)*.6;
   t += n.z * 6.28;
   float slide = SawTooth(t) / 3.;
   float ts = 1.5;
   vec2 trailPos = vec2(bd.x*ts, (fract(bd.y*ts*2.-t*2.)-.5)*.5);
   bd.y += slide;
   float dropShape = bd.x*bd.x;
   dropShape *= DeltaSawTooth(t);
   bd.y += dropShape;
   float d = length(bd)*1.5;
   float trailMask = S(-.2, .2, bd.y);
   trailMask *= bd.y;
   float td = length(trailPos*max(.5, trailMask));
   float mainDrop = S(.2, .1, d);
   float dropTrail = S(.1, .02, td);
   dropTrail *= trailMask;
   o = mix(bd*mainDrop, trailPos, dropTrail);
   return o;
}
float N1(float t) {return fract(sin(t*100.0)*1000.0);}
float N2(vec2 p) {return N1(p.x + N1(p.y));}
float raindot(vec2 uv, vec2 id, float t) {
   vec2 p = .1 + .8 * vec2(N2(id), N2(id + vec2(1., .0)));
   float r = clamp(.5 - mod(t + N2(id), 1.), 0., 1.);
   return smoothstep(.6 * r, .0, length(p - uv));
}
void main() {
   vec2 uv = TexCoord.xy;
   vec2 uv1 = vec2(uv.x * 20., uv.y * 1.3 + N1(floor(uv.x * 20.)));
   vec2 uvi = floor(vec2(uv1.x, uv1.y));
   vec2 uvf = uv1 - uvi;
   float v = raindot(fract(uv * 20. + vec2(0, .1 * lith_t)), floor(uv * 20. + vec2(0, .1 * lith_t)), lith_t/5.)*lith_m;
   vec2 offs;
   offs = GetDrops(uv, 1.);
   offs += GetDrops(uv * 1.4, 10.);
   float ripple = sin(lith_t+uv.y*3.1415*30.+uv.x*124.)*.5+.5;
   ripple *= .007;
   offs += vec2(ripple*ripple, ripple);
   uv += offs*lith_m;
   FragColor = texture(InputTexture, uv + vec2(dFdx(v), dFdy(v)), 3. / (v + 1.));
}
// EOF
