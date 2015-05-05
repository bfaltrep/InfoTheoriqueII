
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

char* rationnel_to_expression_rec(Rationnel* rat,char* Expression)
{
   if (rat == NULL)
    {
      sprintf(Expression,"%s%s",Expression,"∅");         

      return Expression;
    }
   
  switch(get_etiquette(rat))
    {
    case EPSILON:
      sprintf(Expression,"%s%s",Expression,"ε");         
      break;
         
    case LETTRE:
      sprintf(Expression,"%s%c",Expression,get_lettre(rat));  
      break;

    case UNION:
      sprintf(Expression,"%s%s",Expression,"(");
      rationnel_to_expression_rec(fils_gauche(rat),Expression);

      sprintf(Expression,"%s%s",Expression,"+");
      rationnel_to_expression_rec(fils_droit(rat),Expression);

      sprintf(Expression,"%s%s",Expression,")");   
      break;

    case CONCAT:
      sprintf(Expression,"%s%s",Expression,"(");

      rationnel_to_expression_rec(fils_gauche(rat),Expression);
	 
      sprintf(Expression,"%s%s",Expression,".");
	 
      rationnel_to_expression_rec(fils_droit(rat),Expression);
	 
      sprintf(Expression,"%s%s",Expression,")");

      break;

    case STAR:
	
      sprintf(Expression,"%s%s",Expression,"(");

      rationnel_to_expression_rec(fils(rat),Expression);
      sprintf(Expression,"%s%s",Expression,")*");

         break;

      default:
         assert(false);
         break;
   }
  return Expression;
}

char* rationnel_to_expression(Rationnel* rat){
  return rationnel_to_expression_rec(rat,"");
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
  if(rat == NULL){
    printf("error");
    fflush(stdout);
    return 0;
  }
  
  if(rat->etiquette == LETTRE || rat->etiquette == EPSILON){
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
  if(rat != NULL){
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
  return false;
}

void parcours_prem_dern(Rationnel *rat, Ensemble * ensemble, int prem_dern){
  //arrêter le parcours
  if (rat == NULL)
    {
      return;
    }
   
  switch(get_etiquette(rat))
    {
      //on ne ne considère pas les epsilons
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
  for (int i = 0; i < n; i++)
    {
      printf("X%d\t= ", i);
      print_ligne(systeme[i], n);
    }
}

// ---------- fonctions locales pour systeme ----------

Systeme creer_systeme(Automate * automate, int size){
  Systeme s = malloc(sizeof(Rationnel **) * size);
  
  for(int i = 0 ; i < size ; i++){
    //toutes les cases de la matrice sont initialisées à ∅
    s[i] = calloc(size+1,sizeof(Rationnel *));
  }
  return s;
}

void ajoute_dans_systeme_sortante (int origine, char lettre, int fin, void *data){
  //la fonction d'union gère le cas ou un des deux paramètre est NULL
  ((Systeme)data)[origine][fin] = Union(((Systeme)data)[origine][fin],Lettre(lettre));
}

void ajoute_dans_systeme_entrante (int origine, char lettre, int fin, void *data){
  ((Systeme)data)[fin][origine] = Union(((Systeme)data)[fin][origine],Lettre(lettre));
}

// ------------------------------

//Systeme des transition sortante
Systeme systeme(Automate *minimal)
{
  int size = taille_ensemble(get_etats(minimal));
  Systeme s = creer_systeme(minimal, size);
  
  //remplissage de la matrice en fonction des transition : les n premières colonnes.
  pour_toute_transition (minimal, ajoute_dans_systeme_sortante, (void *)s);
  
  //remplissage de la dernière colonne : ε ou NULL, à la création du systeme.
  Ensemble_iterateur ens_i = premier_iterateur_ensemble(get_finaux(minimal));
  while (!iterateur_ensemble_est_vide(ens_i)){
    s[((int)get_element(ens_i))][size] = Epsilon();
    ens_i = iterateur_suivant_ensemble(ens_i);
  }
  return s;
}

 //Systeme des transition entrantes
Systeme systeme2(Automate *automate)
{
  Automate * minimal = creer_automate_minimal(automate);
  int size = taille_ensemble(get_etats(minimal));
  Systeme s = creer_systeme(minimal, size);
  
  //remplissage de la matrice en fonction des transition : les n premières colonnes.
  pour_toute_transition (minimal, ajoute_dans_systeme_entrante, (void *)s);
  
  //remplissage de la dernière colonne : ε ou NULL, à la création du systeme.
  Ensemble_iterateur ens_i = premier_iterateur_ensemble(get_initiaux(minimal));
  while (!iterateur_ensemble_est_vide(ens_i)){
    s[((int)get_element(ens_i))][size] = Epsilon();
    ens_i = iterateur_suivant_ensemble(ens_i);
  }
  return s;
}

/*     PARTIE 2     */

Rationnel **resoudre_variable_arden(Rationnel **ligne, int numero_variable, int n)
{
  /*
  //A RETIRER, QUE PR LES TESTS
  printf("\narden V1 %d: \n",numero_variable);
  for (int i=0; i<= n; i++){
    print_rationnel(ligne[i]);
    printf("  -  ");
  }
  printf("\n");
  */
  
  Rationnel* tmp = Star(ligne[numero_variable]);
  
  for (int i = 0; i <= n; i++){
    //on distribue le U* de U*.V pour conserver la forme de la ligne (union des cellules)
    if (i != numero_variable){
      if(ligne[i] != NULL){
        ligne[i] = Concat(ligne[i],tmp);
      }
    }
    //on a déja traité la valeur de ligne[numero_variable], on peut donc la supprimer.
    else
      ligne[i] = NULL;
  }
  /*
  //A RETIRER, QUE PR LES TESTS
  printf("\narden V2: \n");
  for (int i=0; i<= n; i++){
    print_rationnel(ligne[i]);
    printf("  -  ");
  }
  printf("\n");
  */
  return ligne;
			 
}
  
Rationnel **substituer_variable(Rationnel **ligne, int numero_variable, Rationnel **valeur_variable, int n)
{
  int i = 0;
  for(;i < n ;i++){
    if(valeur_variable[i] != NULL){
      ligne[i] = Concat(ligne[numero_variable],valeur_variable[i]);
    }
  }
  if(valeur_variable[n] != NULL){
    ligne[n] = Union(ligne[numero_variable],valeur_variable[i]);
    }
  
  return ligne;
}

Systeme resoudre_systeme(Systeme systeme, int n)
{

  for(int i = 0; i < n ; i++){
    //on n'a pas a parcourir n puisqu'il s'agit du rationnel non lié à un état et que le but ici est de se débarasser des variables d'états
    for(int j = 0 ; j < n ; j++){
    
      //Si systeme[i][j] est null, aucune raison d'appliquer un changement
      //Et si U contient epsilon, on ne peut appliquer arden ou résoudre => négation.
      if(systeme[i][j] != NULL && ((i == j && !contient_mot_vide(systeme[i][j])) || i != j)){
	//
	printf("\ni %d j %d\n",i,j);
	printf("\n\n V2 : \n");
	print_systeme(systeme,n);
	printf("\n");
	//
	
	if(i == j){
	  systeme[i] = resoudre_variable_arden(systeme[i],i,n);
	}else{
	  systeme[i] = substituer_variable(systeme[i], j, systeme[j], n);
	}
      }
    }
  }
  return systeme;
}

Rationnel *Arden(Automate *minimal)
{
  //on minimise pour s'assurer que le systeme sera le plus simple possible
  //Automate * minimal = creer_automate_minimal(automate);
  int size = taille_ensemble(get_etats(minimal));
  
  //on créer le système puis on le résout
  Systeme s = systeme(minimal);

  print_systeme(s,size);
  //TMP
  //printf("\n\n V1 : \n");
  //print_systeme(s,size);
  
  s = resoudre_systeme(s,size);

  //TMP
  /*
  printf("\n\n V2 : \n");
  print_systeme(s,size);
  printf("\n");
  */
  
  Rationnel * res =NULL;
  
  //on peut alors faire l'union des finaux
  Ensemble_iterateur ens_i = premier_iterateur_ensemble(get_finaux(minimal));   
  while (!iterateur_ensemble_est_vide(ens_i)){
    if (s[((int)get_element(ens_i))][size] != NULL){
      res = Union(res,s[((int)get_element(ens_i))][size]);
    }
    ens_i = iterateur_suivant_ensemble(ens_i);
  }
  return res;
}
