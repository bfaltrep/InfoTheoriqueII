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

  //test Systeme

  printf("\n --- test Systeme --- \n\n");
  
  Systeme s1 = systeme(a1);
  TEST(
	 1
	 && get_lettre (s1[0][0]) == 'b'
	 && get_lettre (s1[0][1]) == 'b'
	 && contient_mot_vide (s1[0][2])
	 && get_lettre (s1[1][0]) == 'a'
	 && get_lettre (s1[1][1]) == 'a'
	 && s1[1][2] == NULL
	 , resultat);
  
  //Systeme s2 = systeme(a2);

  Systeme s3 = systeme(a3);
  //print_systeme(s3,taille_ensemble(get_etats (creer_automate_minimal(a3))));
   TEST(
	 1
	 && s1[0][0] == NULL
	 && s1[0][1] == NULL
	 && s1[0][2] == NULL
	 && s1[0][3] == NULL
	 && s1[0][4] == NULL

	 && get_lettre (s3[1][0]) == 'a'
	 && s1[1][1] == NULL
	 && s1[1][2] == NULL
	 && s1[1][3] == NULL
	 && s1[1][4] == NULL

	 && get_lettre (s3[2][0]) == 'b'
	 && get_lettre (s3[2][0]) == 'a'

	 //&& contient_mot_vide (s3[0][2])


	 , resultat);
  
  Rationnel * ras1 = Arden (a1);
  print_rationnel(ras1);
  Rationnel * ras3 = Arden (a3);
  print_rationnel(ras3);
  /*
  fprintf(stderr,"\ns2 \n\n");
  print_automate(creer_automate_minimal(a2));
  print_systeme (s2,taille_ensemble(get_etats (creer_automate_minimal(a2))));
  fprintf(stderr,"\n\n\n\n\n");
  Rationnel * ras2 = Arden (a2);
  print_rationnel (ras2);
  fprintf(stderr,"\n\n\n\n\n");
  
  fprintf(stderr,"\ns3 \n\n");
  print_automate(a3);
  fprintf(stderr,"\n\n");
  print_automate(creer_automate_minimal(a3));
  fprintf(stderr,"\n\n");
  print_systeme (s3,taille_ensemble(get_etats (creer_automate_minimal(a3))));
  fprintf(stderr,"\n\n\n\n\n");
  Rationnel * ras3 = Arden (a3);
  print_rationnel (ras3);
  */
  //nettoyage final

  destroy_rationnel(ras1);
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

