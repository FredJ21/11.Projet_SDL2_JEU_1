#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "level.h"
#include "anim.h"

/*****************************************************************
*****************************************************************/
void init_animation(t_animation *a, SDL_Renderer *r)  {


    SDL_Surface *pSurface_tmp = SDL_LoadBMP ( a->file );
            if(!pSurface_tmp) { printf( "LOAD BMP ERROR : %s\n", SDL_GetError() ); exit(1);}

    SDL_SetColorKey(pSurface_tmp, SDL_TRUE, SDL_MapRGB(pSurface_tmp->format, 255, 255, 255));

    a->texture = SDL_CreateTextureFromSurface(r, pSurface_tmp);
        if(!a->texture) { printf( "SDL_Texture ERREUR! SDL_GetError: %s\n", SDL_GetError() ); exit(1);}

    SDL_FreeSurface(pSurface_tmp);

}

/*****************************************************************
*****************************************************************/
t_sprite *init_sprite(int posx, int posy,
	int dx, int dy,
	int nbtour, int dir,
	t_animation *a,
	bool actif,
	int time_before_ativiation)
{
	t_sprite *s = (t_sprite*)malloc(sizeof(t_sprite));
	s->x = posx;
	s->y = posy;
	s->dx = dx;
	s->dy = dy;
	s->img_current = 0;
	s->compte_tour = 0;
	s->nb_tour = nbtour;
	s->direction = dir;
	s->anim = a;
	s->actif = actif;
	s->time_before_ativiation = time_before_ativiation;

	return s;
}
/*****************************************************************
*****************************************************************/
void avance_sprite(t_sprite *s, t_level *pLevel)
{

    int tile_x;
    int tile_y;
    int new_x;
    int new_y;

    int decalage;
    t_pos HD;      // postion en haut � droite
    t_pos HG;      // postion en haut � gauche
    t_pos BD;      // postion en bas � droite
    t_pos BG;      // postion en bas � gauche
    bool In_Tile = false;
    bool Chemin_trouve = false;

    // gestion du retardement au d�marrage
    if (s->time_before_ativiation > 1 ) {
        s->time_before_ativiation--;
    }
    if (s->time_before_ativiation == 1 ) {
        s->time_before_ativiation = 0;
        s->actif = true;
    }


    if (s->actif == true ) {

        // les ccordonn�es du sprite correspondent � sont centre
        // d�calage par rapport au centre :
        decalage = ( TILE_TAILLE_X + TILE_TAILLE_Y ) / 10;
        // calcul de 4 points de test
        HD.x = (s->x + decalage) / TILE_TAILLE_X ;
        BD.x = HD.x;
        HG.x = (s->x - decalage) / TILE_TAILLE_X;
        BG.x = HG.x;
        HD.y = (s->y - decalage) / TILE_TAILLE_Y ;
        HG.y = HD.y;
        BD.y = (s->y + decalage) / TILE_TAILLE_Y;
        BG.y = BD.y;

       // printf ("X = %d %d %d %d Y = %d %d %d %d\n",HD.x,HG.x,BD.x,BG.x,HD.y,HG.y,BD.y,BG.y);

        if ( HD.x == HG.x && HD.x == BD.x && HD.x == BG.x && HD.y == HG.y && HD.y == BD.y && HD.x == BG.y ) {

            In_Tile = true;
        }

        if ( pLevel->map_Direction[HD.x][HD.y] != INCONNU ) {

            s->direction = pLevel->map_Direction[HD.x][HD.y] - 1;

            switch (s->direction) {
                case UP :
                                s->y -= s->dy;
                                break;
                case RIGHT :
                                s->x += s->dx;
                                break;
                case DOWN :
                                s->y += s->dy;
                                break;
                case LEFT :
                                s->x -= s->dx;
                                break;
            }
        } else {

        switch (s->direction) {
                case UP :
                                new_x = s->x;
                                new_y = s->y - s->dy;
                                tile_x = new_x / TILE_TAILLE_X;
                                tile_y = (new_y - s->anim->ty/2) / TILE_TAILLE_Y;

                                // detection des bords
                                if ( new_y - s->anim->ty/2 <= 0 ) {
                                        s->direction = DOWN;
                                        printf ("1 Detection bord\n");
                                // detection des obstacles
                                } else if ( pLevel->map_Info[tile_x][tile_y] == OBSTACLE ) {
                                        s->direction = RIGHT + ( rand()%2 * 2 );
                                        printf ("1 Detection d'un obstacle -> new direct : %d\n", s->direction);
                                } else {
                                    s->y = new_y;
                                }

                                break;
                case RIGHT :
                                new_x = s->x + s->dx;
                                new_y = s->y;
                                tile_x = (new_x + s->anim->tx/2) / TILE_TAILLE_X;
                                tile_y = new_y / TILE_TAILLE_Y;

                                // detection des bords
                                if ( new_x + s->anim->tx/2 >= MAP_TAILLE_X ) {
                                        s->direction = LEFT;
                                         printf ("2 Detection bord\n");
                               // detection des obstacles
                                } else if ( pLevel->map_Info[tile_x][tile_y] == OBSTACLE ) {
                                        s->direction = UP + ( rand()%2 * 2 );
                                       printf ("2 Detection d'un obstacle -> new direct : %d\n", s->direction);
                                } else {
                                    s->x = new_x;
                                }

                                break;
                case DOWN :
                                new_x = s->x;
                                new_y = s->y + s->dy;
                                tile_x = new_x / TILE_TAILLE_X;
                                tile_y = (new_y + s->anim->ty/2) / TILE_TAILLE_Y;

                                // detection des bords
                                if ( new_y + s->anim->ty/2 >= MAP_TAILLE_Y ) {
                                        s->direction = UP;
                                         printf ("3 Detection bord\n");
                               // detection des obstacles
                                } else if ( pLevel->map_Info[tile_x][tile_y] == OBSTACLE ) {
                                        s->direction = RIGHT + ( rand()%2 * 2 );
                                        printf ("3 Detection d'un obstacle -> new direct : %d\n", s->direction);
                                } else {
                                    s->y = new_y;
                                }

                                break;
                case LEFT :
                                new_x = s->x - s->dx;
                                new_y = s->y;
                                tile_x = (new_x - s->anim->tx/2) / TILE_TAILLE_X;
                                tile_y = new_y / TILE_TAILLE_Y;

                                // detection des bords
                                if ( new_x - s->anim->tx/2 <= 0 ) {
                                        s->direction = RIGHT;
                                          printf ("4 Detection bord\n");
                              // detection des obstacles
                                } else if ( pLevel->map_Info[tile_x][tile_y] == OBSTACLE ) {
                                        s->direction = UP + ( rand()%2 * 2 );
                                       printf ("4 Detection d'un obstacle -> new direct : %d\n", s->direction);
                                } else {
                                    s->x = new_x;
                                }
        }
    }

    //printf("%d %d - %d %d\n", s->x, (int)pLevel->cibleX, s->y, (int)pLevel->cibleY );

    if (s->x /TILE_TAILLE_X == pLevel->cibleX && s->y /TILE_TAILLE_Y == pLevel->cibleY ) {

        printf ("Je suis arrive !!!\n");
        s->actif = false;
    }

    }
}
/*****************************************************************
*****************************************************************/
void anime_sprite(t_sprite*s)
{
	if (s->actif == true ) {
      if (++s->compte_tour > s->nb_tour) {

        s->img_current = (s->img_current + 1)%s->anim->nb_img_by_dir;

		s->compte_tour = 0;
	  }
	}
}
/*****************************************************************
*****************************************************************/
void affiche_sprite(SDL_Renderer *r, t_sprite *s)
{
    SDL_Rect Src;
    SDL_Rect Dst;

    Src.x = s->img_current * s->anim->tx;
    Src.y = s->direction * s->anim->ty;
    Src.w = s->anim->tx;
    Src.h = s->anim->ty;

    Dst.x = s->x - s->anim->tx/2;     // permet de center le sprite sur les coordon�es
    Dst.y = s->y - s->anim->ty/2;
    Dst.w = s->anim->tx;
    Dst.h = s->anim->ty;

    SDL_RenderCopy ( r, s->anim->texture , &Src, &Dst);

}
/*****************************************************************
*****************************************************************/
void destroy_sprite(t_sprite **s)
{

    free(*s);
    *s = NULL;
}
/*****************************************************************
*****************************************************************/
/*****************************************************************
*****************************************************************/
void place_sprite (t_sprite *p, int x, int y) {

    p->x = TILE_TAILLE_X * x;
    p->y = TILE_TAILLE_Y * y;
}


