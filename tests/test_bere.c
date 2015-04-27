#include <automate.h>
#include <rationnel.h>
#include <ensemble.h>
#include <outils.h>
#include <parse.h>
#include <scan.h>
#include <assert.h>

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

  /*
  Automate * a4 = creer_automate();
  ajouter_transition (a4,0,'b',1);
  ajouter_transition (a4,0,'a',3);
  ajouter_transition (a4,1,'b',0);
  ajouter_transition (a4,3,'a',0);
  ajouter_transition (a4,3,'b',2);
  ajouter_transition (a4,1,'a',2);
  ajouter_transition (a4,2,'a',3);
  ajouter_transition (a4,2,'b',1);
  ajouter_etat_initial (a4,0);
  ajouter_etat_final (a4,2);
  */
  
  //test Systeme

  printf("\n --- test Systeme --- \n\n");
  
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

  /*
  Rationnel * ras1 = Arden (a1);
  printf("\n\n rationnel final :\n");
  print_rationnel(ras1);
  
  Rationnel * ras3 = Arden (a3);
  printf("\n\n rationnel final :\n");
  print_rationnel(ras3);
  printf("\n\n");
  */

  Rationnel * r4 = expression_to_rationnel("(a+b)*");
  Automate * a4 = Glushkov(r4);
  Rationnel * ras4 = Arden (creer_automate_minimal(a4));
  printf("\n\n rationnel final :\n");
  print_rationnel(ras4);
  printf("\n\n");
  
  
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

