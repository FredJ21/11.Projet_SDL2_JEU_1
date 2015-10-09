#ifndef TOWER_H_INCLUDED
#define TOWER_H_INCLUDED


// sprite de type tower
typedef struct {
    int     x, y;               // position
    int     HG_x, HG_y;         // position du coin en haut � gauche
    int     BD_x, BD_y;         // position du coin en bas � droite

    int     img_current;        // image current
    int     angle;

    int     cible_x, cible_y;   // position de la cible
    bool    selected;           // la tourelle est selectionn�e

	int     compte_tour;        // compte tours
	int     nb_tour;            // nombre de tours

	t_animation *anim;          // les images de l'animations

	int     visible;            // effet de transparence  255->visible  0->invisible

} t_tower;


t_tower*    create_Tower            ( int x, int y, t_animation *ANIM);
void        anime_tower             (t_tower *s);
void        calcul_angle_tower      (t_tower *s);

void        affiche_tower           (SDL_Renderer *r, t_tower *s);
bool        is_tower_new_valid_position (t_tower *s, t_level *pLevel, t_tower *t[], int current_nb_tower);  // *s est pointeur sur une tourelle temporaire, veridie si elle poet etre positionn�
int         is_tower_position       (int x, int y, t_tower *s[], int current_nb_tower );                    // verifie si il y a d�ja une tourelle en x et y

void        destroy_tower           (t_tower **s);


#endif // TOWER_H_INCLUDED
