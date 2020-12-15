/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * GUI text boxes.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "m_cps.h"
#include "m_char.h"

struct gui_txt *G_TxtBox_Imp(struct gui_state *g, u32 id, struct gui_arg_txt const *a) {
   struct gui_txt *st = a->st;

   G_Auto(g, id, a->x, a->y, 260, 10);

   bool hot = g->hot == id;

   if(hot) a->p->grabInput = true;

   if(*a->p->txtbuf) ACS_LocalAmbientSound(ss_player_cbi_keypress, 30);

   for(char *c = a->p->txtbuf; *c; c++)
   {
      switch(*c)
      {
      case '\b':
         if(st->tbptr - 1 >= 0)
            st->tbptr--;
         break;
      case '\r':
         *c = '\n';
      default:
         if(st->tbptr + 1 < Cps_CountOf(st->txtbuf) && (IsPrint(*c) || IsSpace(*c)))
         {
            Cps_SetC(st->txtbuf, st->tbptr, *c);
            st->tbptr++;
         }
         break;
      }
   }

   Cps_SetC(st->txtbuf, st->tbptr, '\0');

   PrintSprite(sp_UI_TextBoxBack, a->x-3 + g->ox,1, a->y-3 + g->oy,1);

   SetClipW(a->x + g->ox, a->y + g->oy, 260, 200, 260);
   if(st->tbptr)
      PrintTextFmt("%s%s", Cps_Expand(st->txtbuf, 0, st->tbptr),
                           hot ? Ticker("|", "") : "");
   else
      PrintTextFmt("\C%c%s", hot ? 'c' : 'm', LC(LANG "GUI_TEXTBOX"));
   PrintText(sf_smallfnt, g->defcr, a->x + g->ox,1, a->y + g->oy,1);
   ClearClip();

   fastmemset(a->p->txtbuf, 0, sizeof a->p->txtbuf);
   a->p->tbptr = 0;

   return st;
}

/* EOF */
