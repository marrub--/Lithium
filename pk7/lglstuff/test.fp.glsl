// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

vec4 Process(vec4 color)
{
   float pi = 3.1415926535897932384626433832795;
   vec2 wh = vec2(256.0, 224.0);
   float t = timer * 35.0;
   float tbx = sin(t / 8.0);
   float tby = cos(t / 8.0);
   float tcx = sin(t / 3.0);
   float tcy = cos(t / 2.0);
   float tsx = sin(t / 48.0);
   vec2 uv = gl_TexCoord[0].st * wh;
   float x = uv.x;
   float y = uv.y;
   float bx;
   float by;
   float v;
   bx = x + 0.5 * tbx;
   by = y + 0.5 * tby;
   v = sin((-bx + t) / 32.0);
   v += cos((by + t) / 32.0);
   v += sin((-bx + by + t) / 32.0);
   v += sin((sqrt(pow(bx + tcx, 2.0) + pow(by + tcy, 2.0) + 1.0) + t) / 128.0);
   vec4 fc;
   fc.r = cos(v * pi) * 0.5 + 0.5;
   fc.g = sin(v * pi) * 0.5 + 0.5;
   fc.g *= 0.6;
   fc.b = abs(tsx * 0.25);
   fc.a = 0.0;

   return (getTexel(gl_TexCoord[0].st) + fc) * color;
}

/* EOF */
