// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │                              adapted from:                               │
// │            Parallax Scrolling Star Field by AxleMike - 2015              │
// │                            no known license                              │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

float Hash(in vec2 p) {
   float h = dot(p, vec2(12.9898, 78.233));
   return -1.0 + 2.0 * fract(sin(h) * 43758.5453);
}

vec2 Hash2D(in vec2 p) {
   float h  = dot(p, vec2(12.9898, 78.233));
   float h2 = dot(p, vec2(37.271, 377.632));
   return -1.0 + 2.0 * vec2(fract(sin(h) * 43758.5453), fract(sin(h2) * 43758.5453));
}

float Noise(in vec2 p) {
   vec2 n = floor(p);
   vec2 f = fract(p);
   vec2 u = f * f * (3.0 - 2.0 * f);
   return mix(mix(Hash(n), Hash(n + vec2(1.0, 0.0)), u.x),
              mix(Hash(n + vec2(0.0, 1.0)), Hash(n + vec2(1.0)), u.x), u.y);
}

vec3 Voronoi(in vec2 p) {
   vec2  n = floor(p);
   vec2  f = fract(p);
   vec2  mg, mr;
   float md = 8.0;
   for(int j = -1; j <= 1; ++j) {
      for(int i = -1; i <= 1; ++i) {
         vec2  g = vec2(float(i), float(j));
         vec2  o = Hash2D(n + g);
         vec2  r = g + o - f;
         float d = dot(r, r);
         if(d < md) {
            md = d;
            mr = r;
            mg = g;
         }
      }
   }
   return vec3(md, mr);
}

float AddStarField(vec2 samplePosition, float threshold) {
   vec3  star  = Voronoi(samplePosition);
   float power = 1.0 - (star.x / threshold);
   return min(max(power * power * power, 0.0), 1.0);
}

vec4 Process(vec4 color) {
   vec2  uv   = gl_TexCoord[0].st;
   float star = AddStarField(uv * vec2(20.0, 10.0), 0.0007);
   vec3  sky  = texture(lith_skyColorTex, vec2(0)).rgb;
   return vec4(min(sky * uv.y + star, 1.0), 1.0) * color;
}

/* EOF */
