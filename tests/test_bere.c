#include <automate.h>
#include <rationnel.h>
#include <ensemble.h>
#include <outils.h>
#include <parse.h>
#include <scan.h>
#include <assert.h>

Automate * complementaire2 (Automate * automate){
  Automate * comp = copier_automate (automate);

  Ensemble * nouveau_final = copier_ensemble(get_etats(comp));
  retirer_elements(nouveau_final,get_finaux(automate));

  comp->finaux = nouveau_final;
  return comp;
}

bool meme_langage_rationnel (Rationnel * rat1, Rationnel * rat2) {
  Automate * aut1 = Glushkov(rat1); Automate * aut2 = Glushkov(rat2);

Automate * reverse_aut1 = complementaire2(aut1);
 Automate * aut_inter12 = creer_intersection_des_automates(aut2, reverse_aut1);
 Automate * automate_acc12 = creer_automate_minimal((aut_inter12));

bool undansdeux = (taille_ensemble(get_finaux(automate_acc12))==0);

Automate * reverse_aut2 = complementaire2(aut2);
 Automate * aut_inter21 = creer_intersection_des_automates(aut1, reverse_aut2);
 Automate * automate_acc21 = creer_automate_minimal((aut_inter21));
 print_automate(automate_acc12);
 print_automate(automate_acc21);
 bool deuxdansun = (taille_ensemble(get_finaux(automate_acc21))==0); printf("\n\n") ;
 //print_automate(automate_acc12);
 printf("\n\n") ;
 return ( undansdeux && deuxdansun);
}


Rationnel **resoudre_variable_arden2(Rationnel **ligne, int numero_variable, int n) {
  if (ligne[numero_variable] == NULL)
    return ligne;
  if (contient_mot_vide(ligne[numero_variable]))
    return ligne;
  Rationnel * tmp = ligne[numero_variable];
  ligne[numero_variable] = NULL;
  for (int i = 0; i < n+1; i++){
    ligne[i] = Concat(Star(tmp), ligne[i]);
  }
  return ligne;
}

Rationnel **substituer_variable2(Rationnel **ligne, int numero_variable, Rationnel **valeur_variable, int n) {
  Rationnel * tmp = ligne[numero_variable];
  ligne[numero_variable] = NULL;
  for (int i = 0; i < n+1; i++){
    ligne[i] = Union( ligne[i], Concat(tmp, valeur_variable[i]));
  }
  return ligne;
}

bool systeme_reduit_au_max(Systeme systeme, int n){
  for (int j = 0; j < n; j++){
    for(int i = 0; i < n; i++){
      if (systeme[i][j] != NULL)
	return false; } }
  return true; }

Systeme resoudre_systeme2(Systeme systeme, int n) {
  while(!(systeme_reduit_au_max(systeme, n))){
    for (int i = 0; i < n; i++){
      if(systeme[i][i] != NULL){ systeme[i] = resoudre_variable_arden2(systeme[i], i, n); }
      for(int j = 0; j < n; j++){
	if ((j != i) && (systeme[j][i] != NULL)){ systeme[j] = substituer_variable2(systeme[j], i, systeme[i], n); } } } }
  return systeme; }

Rationnel *Arden2(Automate *automate) {
  int nb_etat = taille_ensemble(get_etats(automate));
  Systeme sys = systeme(automate);
  sys = resoudre_systeme2(sys, nb_etat);
  Rationnel * res = Epsilon();
  for (int i = 0; i <= nb_etat ; ++i){
    numeroter_rationnel(res);
    if(est_un_etat_initial_de_l_automate(automate, i)){
      if(get_position_max(res)==0){
	res = sys[i][nb_etat]; }
      else{ res = Union(res, sys[i][nb_etat]); } } }
  return res; }



void print_rationnel2(Rationnel* rat)
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
   printf("\nmin : %d et max : %d\n",rat->position_min,rat->position_max);
}

void destroy_rationnel(Rationnel * rat){
  if(rat == NULL){
    return;
  }
  destroy_rationnel(fils_gauche(rat));
  destroy_rationnel(fils_droit(rat));
  free(rat);
}

void destroy_systeme(Systeme s, int size){
  for(int i  = size-1  ; i >= 0 ; i--){
    for(int j = size ; j >= 0 ; j--){
      destroy_rationnel(s[i][j]);
    }
    free(s[i]);
  }
  free(s);
}

int test_bere (void){
  int resultat = 1;
  
  Rationnel * r1 = expression_to_rationnel("(a+b)*.a");
  Rationnel * r2 = expression_to_rationnel("a.(a+b)*");
  
  //test automates

  printf("\n --- test Glushkov --- \n\n");
  
  Automate * a1 = Glushkov(r1);
  TEST(
       1
       && ! le_mot_est_reconnu(a1, "ab")
       && ! le_mot_est_reconnu(a1, "")
       && ! le_mot_est_reconnu(a1, "b")
       && le_mot_est_reconnu(a1, "a")
       && le_mot_est_reconnu(a1, "ba")
       && le_mot_est_reconnu(a1, "ababa")
       && le_mot_est_reconnu(a1, "aba")
       && le_mot_est_reconnu(a1, "ba")
       , resultat);

  Automate * a2 = Glushkov(r2);
  TEST(
       1
       && ! le_mot_est_reconnu(a2, "ba")
       && ! le_mot_est_reconnu(a2, "")
       && ! le_mot_est_reconnu(a2, "b")
       && le_mot_est_reconnu(a2, "a")
       && le_mot_est_reconnu(a2, "aa")
       && le_mot_est_reconnu(a2, "abb")
       && le_mot_est_reconnu(a2, "ababab")
       && le_mot_est_reconnu(a2, "aba")
       , resultat);

  //feuille 1 Figure 1.
  Automate * a3 = creer_automate();
  ajouter_transition (a3,1,'a',2);
  ajouter_transition (a3,1,'b',4);
  ajouter_transition (a3,4,'a',4);
  ajouter_transition (a3,2,'a',4);
  ajouter_transition (a3,2,'b',3);
  ajouter_transition (a3,3,'a',3);
  ajouter_transition (a3,3,'b',3);
  ajouter_etat_initial (a3,1);
  ajouter_etat_final (a3,3);
  ajouter_etat_final (a3,4);
  
  //test Systeme

  printf("\n --- test Systeme --- \n\n");
  /*
  Systeme s1 = systeme(a1);
  TEST(
	 1
	 && get_lettre (s1[0][0]) == 'b'
	 && get_lettre (s1[0][1]) == 'b'
	 && get_etiquette (s1[0][2]) == EPSILON
	 && get_lettre (s1[1][0]) == 'a'
	 && get_lettre (s1[1][1]) == 'a'
	 && s1[1][2] == NULL
	 , resultat);
  
  Systeme s3 = systeme(a3);
  TEST(
       1
       && s3[0][0] == NULL
       && s3[0][1] == NULL
       && s3[0][2] == NULL
       && s3[0][3] == NULL
       && s3[0][4] == NULL
       && get_etiquette (s3[0][5]) == EPSILON
	 
       && get_lettre (s3[1][0]) == 'a'
       && s3[1][1] == NULL
       && s3[1][2] == NULL
       && s3[1][3] == NULL
       && s3[1][4] == NULL
       && s3[1][5] == NULL
	 
       && get_lettre (s3[2][0]) == 'b'
       && get_lettre (s3[2][1]) == 'a'
       && get_lettre (s3[2][2]) == 'a'
       && s3[2][3] == NULL
       && s3[2][4] == NULL
       && s3[2][5] == NULL
	 
       && s3[3][0] == NULL
       && s3[3][1] == NULL
       && get_lettre (s3[3][2]) == 'b'
       && get_etiquette (s3[3][3]) == UNION
       && ((get_lettre (fils_gauche (s3[3][3])) == 'a'
	    && get_lettre (fils_droit (s3[3][3])) == 'b')
	   || (get_lettre (fils_droit (s3[3][3])) == 'a'
	       && get_lettre (fils_gauche (s3[3][3])) == 'b' ))
       && s3[3][4] == NULL
       && s3[3][5] == NULL

       && s3[4][0] == NULL
       && get_lettre (s3[4][1]) == 'b'
       && s3[4][2] == NULL
       && s3[4][3] == NULL
       && ((get_lettre (fils_gauche (s3[4][4])) == 'a'
	    && get_lettre (fils_droit (s3[4][4])) == 'b')
	   || (get_lettre (fils_droit (s3[4][4])) == 'a'
	       && get_lettre (fils_gauche (s3[4][4])) == 'b' ))
       && s3[4][5] == NULL
       , resultat);
  */
  /*
  Rationnel * ras1 = Arden (a1);
  printf("\n\n rationnel final :\n");
  print_rationnel(ras1);
  
  Rationnel * ras3 = Arden (a3);
  printf("\n\n rationnel final :\n");
  print_rationnel(ras3);
  printf("\n\n");
  */
  /*
  Rationnel * r4 = expression_to_rationnel("(a+b)*");
  Automate * a4 = Glushkov(r4);
  Rationnel * ras4 = Arden (a4);
  printf("\n rationnel final :\n");
  print_rationnel(ras4);
  printf("\n\n");
  */
  
  Automate * a5 = creer_automate();
  ajouter_transition (a5,0,'a',0);
  ajouter_transition (a5,0,'b',1);
  ajouter_transition (a5,1,'b',0);
  ajouter_transition (a5,1,'a',2);
  ajouter_transition (a5,2,'a',1);
  ajouter_transition (a5,2,'b',2);
  ajouter_etat_initial(a5,0);
  ajouter_etat_final (a5,0);
  
  
  Rationnel * ras5 = Arden (a5);
  printf("\n rationnel final :\n");
  print_rationnel(ras5);
  printf("\n\n");
  
  
  Automate * a6 = creer_automate();
  ajouter_transition (a6,0,'a',0);
  ajouter_transition (a6,0,'a',1);
  ajouter_transition (a6,0,'b',2);
  ajouter_transition (a6,2,'a',2);
  ajouter_etat_final (a6,1);
  ajouter_etat_final (a6,2);
  ajouter_etat_initial(a6,0);


  
  Rationnel * ras6 = Arden (a6);
  printf("\n rationnel final :\n");
  print_rationnel(ras6);
  printf("\n\n");
  
  printf("DANS TA FACE !!!! --------------------------------------- \n");
  
  Systeme s = systeme(a6);
  for (int j=0 ;j<3; j++){
      for (int i=0; i<= 3; i++){
	print_rationnel(s[j][i]);
	printf("  -  ");
      }
      printf("\n");
  }
  /**
  Rationnel ** tmp = resoudre_variable_arden(s[0],0,3);
  Rationnel ** tmp2 = resoudre_variable_arden(s[1],1,3);  
  Rationnel ** tmp3 = resoudre_variable_arden(s[2],2,3);
  **/
  s=resoudre_systeme(s,3);
  printf("\nresoudre variable 1: \n");
  for (int i=0; i<= 3; i++){
    print_rationnel(s[0][i]);
    printf("  -  ");
  }
  printf("\nresoudre variable 2: \n");
  for (int i=0; i<= 3; i++){
    print_rationnel(s[1][i]);
    printf("  -  ");
  }
  printf("\nresoudre variable 3: \n");
  for (int i=0; i<= 3; i++){
    print_rationnel(s[2][i]);
    printf("  -  ");
  }
  printf("\n");
  

  //nettoyage final

  //destroy_rationnel(ras1);
  //destroy_rationnel(ras2);
  //destroy_rationnel(ras3);
  
  /*
  destroy_systeme(s1,taille_ensemble(get_etats (creer_automate_minimal(a1))));
  destroy_systeme(s2,taille_ensemble(get_etats (creer_automate_minimal(a2))));
  destroy_systeme(s2,taille_ensemble(get_etats (creer_automate_minimal(a3))));
  */
  liberer_automate (a1);
  liberer_automate (a2);
  liberer_automate (a3);
  
  destroy_rationnel(r1);
  destroy_rationnel(r2);
  
  return resultat;

}





int main (int argc, char ** argv){
  if( ! test_bere() ){
    return 1;
  }
  return 0;
}

