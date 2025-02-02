#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "config.h"
#include "level.h"
#include "anim.h"
#include "type_sound.h"
#include "tower.h"


/*****************************************************************
*****************************************************************/
t_tower*   create_Tower( int x, int y, t_animation *ANIM, t_animation *ANIM_SELECTED) {

    int a;

	t_tower *s = (t_tower*)malloc(sizeof(t_tower));

    s->x            = x;
    s->y            = y;
    s->HG_x         = x - ANIM->tx/2;
    s->HG_y         = y - ANIM->ty/2;
    s->BD_x         = x + ANIM->tx/2;
    s->BD_y         = y + ANIM->ty/2;

    s->cible_x      = 0;
    s->cible_y      = 0;

    s->selected     = false;

	s->img_current  = 2;
	s->angle        = 0;
	s->compte_tour  = 0;
	s->nb_tour      = ANIM->nb_tour;
	s->anim         = ANIM;
	s->anim_selected = ANIM_SELECTED;
	s->visible      = 254;
	s->actif        = true;

    // cr�ation des missiles
    for (a = 0; a < TOWER_NB_MISSILE_MAX; a++) {

        s->missile[a].actif = false;
    }

	return s;


}
/*****************************************************************
*****************************************************************/
void        anime_tower             (t_tower *s) {

    if (s->cible_x == 0 && s->cible_y == 0) {

        s->angle = s->angle + 3;
        if (s->angle > 360 ) { s->angle = 0; }

    } else {

        // TODO TIR AUTO
    }

}
/*****************************************************************
*****************************************************************/
void        calcul_angle_tower      (t_tower *s) {

    int a;
    int angle;
    int xx = s->x - s->cible_x;
    int yy = s->y - s->cible_y;


    angle = abs( atan((float)xx/(float)yy) * 180 / M_PI );

    if          ( xx > 0 && yy >= 0 ) {          angle = 360 - angle;
    } else if   ( xx <= 0 && yy < 0 ) {          angle = 180 - angle;
    } else if   ( xx > 0 && yy < 0 ) {          angle = 180 + angle;
    }

    //SDL_log("%d  %d   angle = %d\n", xx, yy, angle);
    s->angle = angle;

    // mise � jour de l'angle des missiles
    float angle_rad, dx, dy;

        // Calcul des distances de d�placement celon l'angle
    if      ( angle >= 0 && angle <= 90 ) {

                angle_rad = angle * (M_PI/180);
                dx = MISSILE_SPEED * sinf(angle_rad);        // le cot� oppos�
                dy = - MISSILE_SPEED * cosf(angle_rad);      // le cot� adjacent

    } else if ( angle > 90 && angle < 180 ) {

                angle_rad = (angle-90) * (M_PI/180);
                dy = MISSILE_SPEED * sinf(angle_rad);      // le cot� oppos�
                dx = MISSILE_SPEED * cosf(angle_rad);      // le cot� adjacent

    } else if ( angle >= 180 && angle <= 270 ) {

                angle_rad = (angle-180) * (M_PI/180);
                dx = - MISSILE_SPEED * sinf(angle_rad);      // le cot� oppos�
                dy = MISSILE_SPEED * cosf(angle_rad);      // le cot� adjacent

    } else if ( angle > 270 ) {

                angle_rad = (angle-270) * (M_PI/180);
                dy = - MISSILE_SPEED * sinf(angle_rad);      // le cot� oppos�
                dx = - MISSILE_SPEED * cosf(angle_rad);      // le cot� adjacent
    }

    for (a = 0; a < TOWER_NB_MISSILE_MAX; a++) {

        s->missile[a].angle = angle;
        s->missile[a].actif = false;
        s->missile[a].dx = dx;
        s->missile[a].dy = dy;
    }

}
/*****************************************************************
*****************************************************************/
void        affiche_tower(SDL_Renderer *r, t_tower *s){

    if (s->actif) {

        SDL_Rect Src;
        SDL_Rect Dst;

        Src.x = s->img_current * s->anim->tx;
        Src.y = 0;
        Src.w = s->anim->tx;
        Src.h = s->anim->ty;

        Dst.x = s->x - s->anim->tx/2;     // permet de center le sprite sur les coordonn�es
        Dst.y = s->y - s->anim->ty/2;
        Dst.w = s->anim->tx;
        Dst.h = s->anim->ty;

        SDL_SetTextureAlphaMod (s->anim->texture, s->visible);

        // Affichage
        if (s->selected) {
            SDL_SetRenderDrawColor (r, 254, 0, 0, 50);
            SDL_RenderDrawRect(r, &Dst);
            SDL_SetRenderDrawColor (r, 0, 0, 0, 50);

            affiche_tower_selected( r, s);
        }
        SDL_RenderCopyEx(r, s->anim->texture, &Src, &Dst, s->angle, NULL, 0);
    }

}
/*****************************************************************
*****************************************************************/
void        affiche_tower_selected(SDL_Renderer *r, t_tower *s){

    if (s->actif) {

        SDL_Rect Src;
        SDL_Rect Dst;

        // Affichage de l'aide au positionnement
        Src.x = 0;
        Src.y = 0;
        Src.w = s->anim_selected->tx;
        Src.h = s->anim_selected->ty;

        Dst.x = s->x - s->anim_selected->tx/2;
        Dst.y = s->y - s->anim_selected->ty/2;
        Dst.w = s->anim_selected->tx;
        Dst.h = s->anim_selected->ty;
        SDL_RenderCopy ( r, s->anim_selected->texture , &Src, &Dst);


    }
}

/*****************************************************************
*****************************************************************/
bool        is_tower_new_valid_position(t_tower *s, t_level *pLevel, t_tower *tower[], int current_nb_tower, int z_button_tower_x, int z_button_tower_y) {

    int a;

    t_pos HD;      // postion en haut � droite
    t_pos HG;      // postion en haut � gauche
    t_pos BD;      // postion en bas � droite
    t_pos BG;      // postion en bas � gauche


    // les ccordonn�es du sprite correspondent � son centre
    // calcul des coordonn�es des quatres coins
    HD.x = s->x + s->anim->tx/2 ;
    BD.x = HD.x;
    HD.tileX = HD.x / TILE_TAILLE_X;
    BD.tileX = HD.tileX;

    HG.x = s->x - s->anim->tx/2 ;
    BG.x = HG.x;
    HG.tileX = HG.x / TILE_TAILLE_X;
    BG.tileX = HG.tileX;

    HD.y = s->y - s->anim->ty/2 ;
    HG.y = HD.y;
    HD.tileY = HD.y / TILE_TAILLE_Y;
    HG.tileY = HD.tileY;

    BD.y = s->y + s->anim->ty/2 ;
    BG.y = BD.y;
    BD.tileY = BD.y / TILE_TAILLE_Y;
    BG.tileY = BD.tileY;

    // test si la tourelle n'est pas sous le bouton TOWER
    if ( BG.x < z_button_tower_x && BG.y > z_button_tower_y ) {

        return false;
    }

    // test si la position n'est pas sur l'eau (LIBRE)
    if ( pLevel->map_Info[HD.tileX][HD.tileY] == LIBRE ||
         pLevel->map_Info[HG.tileX][HG.tileY] == LIBRE ||
         pLevel->map_Info[BD.tileX][BD.tileY] == LIBRE ||
         pLevel->map_Info[BG.tileX][BG.tileY] == LIBRE ) {

         return false;
    }


    // test, pour chaque tourelle d�j� en place, si la nouvelle n'est pas dessus
    for (a = 0; a < current_nb_tower; a++) {

        if (tower[a]->actif) {

            // pour la position centale du nouveau sprite
            if ( s->x >= tower[a]->HG_x && s->x <= tower[a]->BD_x && s->y >= tower[a]->HG_y && s->y <= tower[a]->BD_y ) {

                return false;
            }
            // pour la position HG
            if ( HG.x >= tower[a]->HG_x && HG.x <= tower[a]->BD_x && HG.y >= tower[a]->HG_y && HG.y <= tower[a]->BD_y ) {

                return false;
            }
            // pour la position HD
            if ( HD.x >= tower[a]->HG_x && HD.x <= tower[a]->BD_x && HD.y >= tower[a]->HG_y && HD.y <= tower[a]->BD_y ) {

                return false;
            }
            // pour la position BD
            if ( BD.x >= tower[a]->HG_x && BD.x <= tower[a]->BD_x && BD.y >= tower[a]->HG_y && BD.y <= tower[a]->BD_y ) {

                return false;
            }
            // pour la position BG
            if ( BG.x >= tower[a]->HG_x && BG.x <= tower[a]->BD_x && BG.y >= tower[a]->HG_y && BG.y <= tower[a]->BD_y ) {

                return false;
            }
        }
    }


    return true;
}
/*****************************************************************
*****************************************************************/
int         is_tower_position       (int x, int y, t_tower *s[], int current_nb_tower ){


    int a;
    int result = TOWER_MAX;


    for (a = 0; a < current_nb_tower; a++) {

        if (s[a]->actif) {

            if (x >= s[a]->HG_x && x <= s[a]->BD_x && y >= s[a]->HG_y && y <= s[a]->BD_y ) {

                result = a;
            }
        }
    }
    return result;

}
/*****************************************************************
*****************************************************************/
void        destroy_tower           (t_tower **s) {


    free(*s);
    *s = NULL;
}
/*****************************************************************
*****************************************************************/
void        avance_missile          (t_missile *m, t_system *my_system) {

    if (m->actif) {
       m->x += m->dx;
       m->y += m->dy;

       if (m->x < 0 || m->y < 0 || m->x > my_system->map_taille_x || m->y > my_system->map_taille_y) {

            m->actif = false;
       }
    }

}
/*****************************************************************
*****************************************************************/
void        affiche_missile         (SDL_Renderer *r, t_missile *m, t_animation *ANIM) {

    if (m->actif) {

    SDL_Rect Src;
    SDL_Rect Dst;

    Src.x = 0;
    Src.y = 0;
    Src.w = ANIM->tx;
    Src.h = ANIM->ty;

    Dst.x = m->x - ANIM->tx/2;
    Dst.y = m->y - ANIM->ty/2;
    Dst.w = ANIM->tx;
    Dst.h = ANIM->ty;


    // Affichage
    SDL_RenderCopy ( r, ANIM->texture , &Src, &Dst);

    }
}
/*****************************************************************
*****************************************************************/
void        tir_tower   (t_tower *t, int current_nb_tower, int *score, t_sound *sound) {

    int a;

    if ( t->actif && t->cible_x + t->cible_y > 0 ) {

        for (a = 0; a < TOWER_NB_MISSILE_MAX; a++) {

            if ( ! t->missile[a].actif ) {

                t->missile[a].x = t->x;
                t->missile[a].y = t->y;

                t->missile[a].actif = true;

                Mix_PlayChannel( -1, sound->Tir, 0 );

                *score -= SCORE_TIR;

                break;
            }
        }
    }
}

