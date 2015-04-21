
/*
 *   Ce fichier fait partie d'un projet de programmation donné en Licence 3 
 *   à l'Université de Bordeaux.
 *
 *   Copyright (C) 2015 Giuliana Bianchi, Adrien Boussicault, Thomas Place, Marc Zeitoun
 *
 *    This Library is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This Library is distributed in the hope that it will be useful,
 *    but WITHUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rationnel.h"
#include "ensemble.h"
#include "automate.h"
#include "parse.h"
#include "scan.h"
#include "outils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int yyparse(Rationnel **rationnel, yyscan_t scanner);

Rationnel *rationnel(Noeud etiquette, char lettre, int position_min, int position_max, void *data, Rationnel *gauche, Rationnel *droit, Rationnel *pere)
{
   Rationnel *rat;
   rat = (Rationnel *) malloc(sizeof(Rationnel));

   rat->etiquette = etiquette;
   rat->lettre = lettre;
   rat->position_min = position_min;
   rat->position_max = position_max;
   rat->data = data;
   rat->gauche = gauche;
   rat->droit = droit;
   rat->pere = pere;
   return rat;
}

Rationnel *Epsilon()
{
   return rationnel(EPSILON, 0, 0, 0, NULL, NULL, NULL, NULL);
}

Rationnel *Lettre(char l)
{
   return rationnel(LETTRE, l, 0, 0, NULL, NULL, NULL, NULL);
}

Rationnel *Union(Rationnel* rat1, Rationnel* rat2)
{
   // Cas particulier où rat1 est vide
   if (!rat1)
      return rat2;

   // Cas particulier où rat2 est vide
   if (!rat2)
      return rat1;
   
   return rationnel(UNION, 0, 0, 0, NULL, rat1, rat2, NULL);
}

Rationnel *Concat(Rationnel* rat1, Rationnel* rat2)
{
   if (!rat1 || !rat2)
      return NULL;

   if (get_etiquette(rat1) == EPSILON)
      return rat2;

   if (get_etiquette(rat2) == EPSILON)
      return rat1;
   
   return rationnel(CONCAT, 0, 0, 0, NULL, rat1, rat2, NULL);
}

Rationnel *Star(Rationnel* rat)
{
   return rationnel(STAR, 0, 0, 0, NULL, rat, NULL, NULL);
}

bool est_racine(Rationnel* rat)
{
   return (rat->pere == NULL);
}

Noeud get_etiquette(Rationnel* rat)
{
   return rat->etiquette;
}

char get_lettre(Rationnel* rat)
{
   return rat->lettre;
}

int get_position_min(Rationnel* rat)
{
   return rat->position_min;
}

int get_position_max(Rationnel* rat)
{
   return rat->position_max;
}

void set_position_min(Rationnel* rat, int valeur)
{
   rat->position_min = valeur;
   return;
}

void set_position_max(Rationnel* rat, int valeur)
{
   rat->position_max = valeur;
   return;
}

Rationnel *fils_gauche(Rationnel* rat)
{
  //pourquoi cette assertion ? 
  //assert((get_etiquette(rat) == CONCAT) || (get_etiquette(rat) == UNION));
   return rat->gauche;
}

Rationnel *fils_droit(Rationnel* rat)
{
  //assert((get_etiquette(rat) == CONCAT) || (get_etiquette(rat) == UNION));
   return rat->droit;
}

Rationnel *fils(Rationnel* rat)
{
   assert(get_etiquette(rat) == STAR);
   return rat->gauche;
}

Rationnel *pere(Rationnel* rat)
{
   assert(!est_racine(rat));
   return rat->pere;
}

void print_rationnel(Rationnel* rat)
{
   if (rat == NULL)
   {
      printf("∅");
      return;
   }
   
   switch(get_etiquette(rat))
   {
      case EPSILON:
         printf("ε");         
         break;
         
      case LETTRE:
         printf("%c", get_lettre(rat));
         break;

      case UNION:
         printf("(");
         print_rationnel(fils_gauche(rat));
         printf(" + ");
         print_rationnel(fils_droit(rat));
         printf(")");         
         break;

      case CONCAT:
         printf("[");
         print_rationnel(fils_gauche(rat));
         printf(" . ");
         print_rationnel(fils_droit(rat));
         printf("]");         
         break;

      case STAR:
         printf("{");
         print_rationnel(fils(rat));
         printf("}*");         
         break;

      default:
         assert(false);
         break;
   }
}

Rationnel *expression_to_rationnel(const char *expr)
{
    Rationnel *rat;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    // Initialisation du scanner
    if (yylex_init(&scanner))
        return NULL;
 
    state = yy_scan_string(expr, scanner);

    // Test si parsing ok.
    if (yyparse(&rat, scanner)) 
        return NULL;
    
    // Libération mémoire
    yy_delete_buffer(state, scanner);
 
    yylex_destroy(scanner);
 
    return rat;
}

void rationnel_to_dot(Rationnel *rat, char* nom_fichier)
{
   FILE *fp = fopen(nom_fichier, "w+");
   rationnel_to_dot_aux(rat, fp, -1, 1);
}

int rationnel_to_dot_aux(Rationnel *rat, FILE *output, int pere, int noeud_courant)
{   
   int saved_pere = noeud_courant;

   if (pere >= 1)
      fprintf(output, "\tnode%d -> node%d;\n", pere, noeud_courant);
   else
      fprintf(output, "digraph G{\n");
   
   switch(get_etiquette(rat))
   {
      case LETTRE:
         fprintf(output, "\tnode%d [label = \"%c-%d\"];\n", noeud_courant, get_lettre(rat), rat->position_min);
         noeud_courant++;
         break;

      case EPSILON:
         fprintf(output, "\tnode%d [label = \"ε-%d\"];\n", noeud_courant, rat->position_min);
         noeud_courant++;
         break;

      case UNION:
         fprintf(output, "\tnode%d [label = \"+ (%d/%d)\"];\n", noeud_courant, rat->position_min, rat->position_max);
         noeud_courant = rationnel_to_dot_aux(fils_gauche(rat), output, noeud_courant, noeud_courant+1);
         noeud_courant = rationnel_to_dot_aux(fils_droit(rat), output, saved_pere, noeud_courant+1);
         break;

      case CONCAT:
         fprintf(output, "\tnode%d [label = \". (%d/%d)\"];\n", noeud_courant, rat->position_min, rat->position_max);
         noeud_courant = rationnel_to_dot_aux(fils_gauche(rat), output, noeud_courant, noeud_courant+1);
         noeud_courant = rationnel_to_dot_aux(fils_droit(rat), output, saved_pere, noeud_courant+1);
         break;

      case STAR:
         fprintf(output, "\tnode%d [label = \"* (%d/%d)\"];\n", noeud_courant, rat->position_min, rat->position_max);
         noeud_courant = rationnel_to_dot_aux(fils(rat), output, noeud_courant, noeud_courant+1);
         break;
         
      default:
         assert(false);
         break;
   }
   if (pere < 0)
      fprintf(output, "}\n");
   return noeud_courant;
}


/*    PARTIE A FAIRE     */

/*
  parcours de l'arbre affectant les valeurs à position_min et position_max pour chaque noeud.
*/
int parcours_numeroter_rationnel(Rationnel *rat, int nb){
  
  if(rat->etiquette == LETTRE){
    rat->position_min = nb;
    rat->position_max = nb;
    return nb;
  }

  rat->position_min = nb;
  //dans les noeuds internes(union, star ou concat) il y a forcément un fils gauche.
  //prend la valeur de la dernière lettre parcourue dans le fils gauche (donc la plus grande du fils gauche)
  int tmp = parcours_numeroter_rationnel(rat->gauche, nb);

  //union/concat ont un fils droit, pas star
  if(rat->droit != NULL){
    rat->position_max = parcours_numeroter_rationnel(rat->droit, tmp+1);
  }
  else{
    rat->position_max = tmp;
  }
  return rat->position_max;
}

/*
  ajoute la numérotation des éléments de la fonction rationnelle.
*/
void numeroter_rationnel(Rationnel *rat)
{
  parcours_numeroter_rationnel(rat, 1);
}



/*
**
** @date 
** @details Retourne si le rationnel peut être remplacé par le mot vide.
*/
bool contient_mot_vide(Rationnel *rat)
{
  switch(get_etiquette(rat))
    {
    case EPSILON:

    case STAR:
      return true;
      
      //pour exp = exp1 + exp 2 : si l'un ou l'autre est remplacable par epsilon, exp l'est aussi.
    case UNION:
      return contient_mot_vide(fils_gauche(rat)) || contient_mot_vide(fils_droit(rat));

      //pour exp = exp1.exp2, il faut que les deux soient remplacables par epsilon pour que exp le soit.
    case CONCAT:
      return contient_mot_vide(fils_gauche(rat)) && contient_mot_vide(fils_droit(rat));

      //une lettre n'est pas remplaçable par epsilon
    case LETTRE:
      return false;

    default:
      assert(false);
      break;
    }
    
}

void parcours_prem_dern(Rationnel *rat, Ensemble * ensemble, int prem_dern){
  //arrêter le parcours
  if (rat == NULL)
    {
      return;
    }
   
  switch(get_etiquette(rat))
    {
      //on ne numerote ni ne considère les epsilons
    case EPSILON:
      break;
    case LETTRE:
      //get_position_min(rat) == get_position_max(rat), on prend indifferemment la valeur
      ajouter_element(ensemble,get_position_min(rat));
      break;

    case UNION:
      if(prem_dern)
	{
	  parcours_prem_dern(fils_gauche(rat), ensemble, prem_dern);
	  parcours_prem_dern(fils_droit(rat), ensemble, prem_dern);
	}
      else
	{
	  parcours_prem_dern(fils_droit(rat), ensemble, prem_dern);
	  parcours_prem_dern(fils_gauche(rat), ensemble, prem_dern);
	}
      break;


    case CONCAT:
      if(prem_dern)
	{
	  parcours_prem_dern(fils_gauche(rat), ensemble, prem_dern);
	  //si fils gauche remplacable par ε on peut intégrer des éléments du fils droit à premier
	  if(contient_mot_vide(fils_gauche(rat))){
	    parcours_prem_dern(fils_droit(rat), ensemble, prem_dern);
	  }
	}
      else{
	//lors d'une concaténation, on prend le fils droit.
	parcours_prem_dern(fils_droit(rat), ensemble, prem_dern);
	//si fils droit contient ε on autorise le fils gauche
	if(contient_mot_vide(fils_droit(rat))){
	  parcours_prem_dern(fils_gauche(rat), ensemble, prem_dern);
	}
      }
      break;

    case STAR:
      parcours_prem_dern(fils(rat), ensemble, prem_dern);
      break;

    default:
      assert(false);
      break;
    }
}

Ensemble *premier(Rationnel *rat)
{
  Ensemble * e = creer_ensemble(NULL,NULL,NULL);
  parcours_prem_dern(rat,e,1);
  return e;
}
 
Ensemble *dernier(Rationnel *rat)
{
  Ensemble * e = creer_ensemble(NULL,NULL,NULL);
  parcours_prem_dern(rat,e,0);
  return e;
}

Rationnel * get_rationnel(Rationnel * r, int pos){
    if(r == NULL || r->position_min > pos){
        printf("[ERREUR] durant get_rationnel : %d > %d\n", r->position_min, pos);
    return NULL;
  }
  if(r->etiquette == LETTRE && pos == r->position_min){
    return r;
  }
  if (pos > r->position_max){
    return get_rationnel(r->droit, pos);
  }else{
    return get_rationnel(r->gauche, pos);
  }
}

void trouver_suivant(Rationnel * rat, Ensemble * e, int position){
  if (rat == NULL){
    return;
  }
  
  switch (get_etiquette(rat)){
  case LETTRE:
    break;
  case EPSILON:
    break;
  case UNION :
    trouver_suivant(rat->gauche,e, position);
    trouver_suivant(rat->droit,e, position);
    break;
  case CONCAT:
    if (est_dans_l_ensemble(dernier(rat->gauche),(intptr_t) position)){
      ajouter_elements(e,premier(rat->droit));
    }
    trouver_suivant(rat->gauche,e, position);
    trouver_suivant(rat->droit,e, position);
    break;
  case STAR:
        
    if(est_dans_l_ensemble(dernier(fils(rat)), (intptr_t)position)) 
      {
	ajouter_elements(e,premier(fils(rat)));
      }      
    trouver_suivant(rat->gauche,e, position);
   break;
    
  }
}

Ensemble *suivant(Rationnel *rat, int position)
{
   Ensemble* e = creer_ensemble(NULL, NULL, NULL);
   trouver_suivant(rat,e,position);
   return e;
}

char get_lettre_in_position(Rationnel * rat, int pos) {
  /** on est sur la lettre**/
  if(get_etiquette(rat) == LETTRE && pos == rat->position_min){
    return get_lettre(rat);
  }
  /** Si on arrive sur une concaténation ou une union**/
  else if ((get_etiquette(rat) == CONCAT) || (get_etiquette(rat) == UNION)) {
    if (pos <= fils_gauche(rat)->position_max ) {
      return get_lettre_in_position(fils_gauche(rat), pos);
    }
    else {
      return get_lettre_in_position(fils_droit(rat), pos);
    }
  }
  /** finalement le cas de l'étoile **/
  else if (get_etiquette(rat) == STAR) {
    if (fils(rat) != NULL) {
      return get_lettre_in_position(fils(rat), pos);
    }
  }
  return -1;
}


Automate *Glushkov(Rationnel *rat){
    
    Automate* a = creer_automate();
    Ensemble_iterateur it;
    char c = 0;
    numeroter_rationnel(rat);
    
    int nb_positions = rat -> position_max;
    
    ajouter_etat_initial(a,0);
    
    /** si l'expression rationnelle est effaçable **/ 
    if ( contient_mot_vide(rat))
      ajouter_etat_final(a,0);
    
    //on rajoute les états premiers 
    Ensemble* prems = premier(rat);
    for (it = premier_iterateur_ensemble(prems) ; !iterateur_est_vide(it) ; it = iterateur_suivant_ensemble(it)) {
      ajouter_etat(a,get_element(it));
      c = get_lettre_in_position(rat, get_element(it));
      ajouter_transition(a,0,c,get_element(it));
    }
    liberer_ensemble(prems);
    /** Rajout des suivants **/
    
    for (int i=1; i<= nb_positions; i++){
      Ensemble* suivnt=suivant (rat,i);
      //print_ensemble(suivnt,NULL);
        
      for (it = premier_iterateur_ensemble(suivnt);!iterateur_est_vide(it); it= iterateur_suivant_ensemble(it)) {
	ajouter_etat(a,i);
	c = get_lettre_in_position(rat,get_element(it));
	ajouter_transition(a,i,c,get_element(it));
      }
      liberer_ensemble(suivnt);
    }
    
    /** Rajout des états finaux**/

    Ensemble* lst = dernier(rat);
    for (it = premier_iterateur_ensemble(lst); !iterateur_est_vide(it); it = iterateur_suivant_ensemble(it))
      {
	ajouter_etat_final(a, get_element(it));
      }
    return a;
}
    
/*
  retourne un nouvel automate qui est le complémentaire de l'automate en paramètre.
*/
Automate * complementaire (Automate * automate){
  Automate * comp = copier_automate (automate);

  Ensemble * nouveau_final = copier_ensemble(get_etats(comp));
  retirer_elements(nouveau_final,get_finaux(automate));

  comp->finaux = nouveau_final;
  return comp;
}

bool meme_langage (const char *expr1, const char* expr2)
{
  //pour chaque expression, on la change en rationnel, qui est alors tranformée en automate que l'on minimise
  Rationnel * r1 = expression_to_rationnel(expr1);
  Rationnel * r2 = expression_to_rationnel(expr2);
  
  Automate * a1= Glushkov(r1);
  Automate * a2= Glushkov(r2);

  //si l'ensemble des alphabets est différent, il ne peut s'agir du meme langage
  if (comparer_ensemble(get_alphabet (a1),get_alphabet (a2)) != 0){
    return false;
  }

  //seul l'automate minimal est canonique, nous minimisons donc nos automates.
  Automate * am1 = creer_automate_minimal(a1);
  Automate * am2 = creer_automate_minimal(a2);

  //on test si inter(complementaire(am1),am2) = ensemble vide soit :  am2 inclus dans am1
  Automate * intersection = creer_intersection_des_automates (complementaire(am1),am2);  
  if(taille_ensemble(creer_intersection_ensemble(etats_accessibles(intersection,0),get_finaux(intersection))) != 0){
    return false;
  }
  
  //on test si inter(complementaire(am2),am1) = ensemble vide soit : am1 inclus dans am2
  intersection = creer_intersection_des_automates (complementaire(am2),am1);
  if(taille_ensemble(creer_intersection_ensemble(etats_accessibles(intersection,0),get_finaux(intersection))) != 0){
    return false;
  }

  //si la double inclusion est correcte, alors il s'agit du meme langage
  return true;
}

// ---------- fonctions locales pour systeme ----------

Systeme creer_systeme(Automate * automate){
  int nbEtats = taille_ensemble(get_etats (automate));
  Systeme s = malloc(sizeof(Rationnel **) * nbEtats);
  for(int i = 0 ; i < nbEtats ; i++){
    //toutes les cases de la matrice sont initialisées à ∅
    s[i] = calloc(nbEtats+1,sizeof(Rationnel *));
  }
  return s;
}

/*
 * \brief Pour les informations d'une transition données en paramètre, ajoute dans le Système, dernier paramètre, le rationnel associé
*/
bvoid ajoute_dans_systeme (int origine, char lettre, int fin, void *data){
  ((Systeme)data)[fin][origine] = rationnel(LETTRE,lettre,1,1,NULL,NULL,NULL,NULL);
}

// ------------------------------

Systeme systeme(Automate *automate)
{
  Automate * minimal = creer_automate_minimal(automate);
  Systeme s =  creer_systeme(a);
  
  //remplissage de la matrice en fonction des transition : les n premières colonnes.
  pour_toute_transition (a, ajoute_dans_systeme, (void *)s);
  
  //remplissage de la dernière colonne correspondant à ε
  int size = taille_ensemble(get_etats(a));
  Ensemble_iterateur ens_i = premier_iterateur_ensemble(get_initiaux(a));
  while (!iterateur_ensemble_est_vide(ens_i)){
    s[get_element(ens_i)][size] = rationnel( EPSILON,0,0,0,NULL,NULL,NULL,NULL);
    ens_i = iterateur_suivant_ensemble(ens_i);
  }
  
  return s;
}

void print_ligne(Rationnel **ligne, int n)
{
  for (int j = 0; j <=n; j++)
    {
      print_rationnel(ligne[j]);
      if (j<n)
	printf("X%d\t+\t", j);
    }
  printf("\n");
}

void print_systeme(Systeme systeme, int n)
{
  for (int i = 0; i <= n-1; i++)
    {
      printf("X%d\t= ", i);
      print_ligne(systeme[i], n);
    }
}

/*     PARTIE 2     */

Rationnel **resoudre_variable_arden(Rationnel **ligne, int numero_variable, int n)
{
   A_FAIRE_RETURN(NULL);
}

Rationnel **substituer_variable(Rationnel **ligne, int numero_variable, Rationnel **valeur_variable, int n)
{
   A_FAIRE_RETURN(NULL);
}

Systeme resoudre_systeme(Systeme systeme, int n)
{
   A_FAIRE_RETURN(NULL);
}

Rationnel *Arden(Automate *automate)
{
   A_FAIRE_RETURN(NULL);
}
