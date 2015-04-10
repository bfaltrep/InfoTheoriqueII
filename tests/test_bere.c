#include <automate.h>
#include <rationnel.h>
#include <ensemble.h>
#include <outils.h>
#include <parse.h>
#include <scan.h>
#include <assert.h>

void  print_position(Rationnel* rat){
  
  if (rat == NULL)
    {
      printf("∅");
      return;
    }
   
  switch(get_etiquette(rat))
    {
    case EPSILON:
      printf("ε ");
      printf("pos_min %d et pos_max %d.\n",get_position_min(rat),get_position_max(rat));
      break;
         
    case LETTRE:
      printf("%c ", get_lettre(rat));
      printf("pos_min %d et pos_max %d.\n",get_position_min(rat),get_position_max(rat));
      break;

    case UNION:
      printf("\n+ ");
      printf("pos_min %d et pos_max %d.\n",get_position_min(rat),get_position_max(rat));
      printf(" (G ");
	
      print_position(fils_gauche(rat));
      printf("D ");
      print_position(fils_droit(rat));
      printf(" )");         
      break;

    case CONCAT:
      printf("\n. ");
      printf("pos_min %d et pos_max %d.\n",get_position_min(rat),get_position_max(rat));
      printf(" [G ");
      
      print_position(fils_gauche(rat));
      printf(" D ");
      print_position(fils_droit(rat));
      printf(" ] ");         
      break;

    case STAR:
      printf("\n* ");
      printf("pos_min %d et pos_max %d.\n",get_position_min(rat),get_position_max(rat));
      printf(" {G ");
      print_position(fils(rat));
      printf(" } ");         
      break;

    default:
      assert(false);
      break;
    }

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

  Rationnel * tmp = expression_to_rationnel("(a.a)*.(b+c*).a.b*");
  numeroter_rationnel(tmp);
  Ensemble * e = suivant(tmp, 3);
  print_ensemble(e,NULL);
       
  TEST(
       1
       && !est_dans_l_ensemble(e, 1)
       && !est_dans_l_ensemble(e, 2)
       && !est_dans_l_ensemble(e, 3)
       && !est_dans_l_ensemble(e, 4)
       && est_dans_l_ensemble(e, 5)
       && !est_dans_l_ensemble(e, 6)
       , resultat);
  /**
     Rationnel * rat;
  rat = expression_to_rationnel("(a.a)*.(b+c*).a.b*");
  numeroter_rationnel(rat);
  Automate * automate = Glushkov(rat);
  print_automate(automate);
  TEST(
       1
       && le_mot_est_reconnu(automate, "ab")
       && le_mot_est_reconnu(automate, "a")
       && ! le_mot_est_reconnu(automate, "aa")
       && ! le_mot_est_reconnu(automate, "")
       && ! le_mot_est_reconnu(automate, "aaaabcc")
       && ! le_mot_est_reconnu(automate, "aaaabccabbb")
       && le_mot_est_reconnu(automate, "aaaaccabbb")
       && ! le_mot_est_reconnu(automate, "aaaabccaabbb")
       , resultat);


     **/
  
  /*
  //test numeroter
  numeroter_rationnel(r1);
  numeroter_rationnel(r2);
*/
  /*
    printf(" --- test numeroter --- \n\n");
    printf(" r1 : (a+b)*.a\n");
    print_position(r1);
    printf("\n\n\n\n r2 : a.(a+b)*\n");
    print_position(r2);
  
    printf("\n\n\n\n");
  */

  /*
  //test contient mot vide
    TEST(
       !contient_mot_vide(r1)
       && contient_mot_vide(fils_gauche(r1))
       && !contient_mot_vide(fils_droit(r1))
       && contient_mot_vide(expression_to_rationnel("a*.b*"))
       , resultat);
  */
    /*
  //test premier
  Ensemble * prem_r1 = premier(r1);
  Ensemble * prem_r2 = premier(r2);
  
  TEST(
       taille_ensemble(prem_r1) == 3
       && est_dans_l_ensemble(prem_r1,1)
       && est_dans_l_ensemble(prem_r1,2)
       && est_dans_l_ensemble(prem_r1,3)
       && taille_ensemble(prem_r2) == 1
       && est_dans_l_ensemble(prem_r2,1)
       , resultat);
  
  //test dernier
  Ensemble * dern_r1 = dernier(r1);
  Ensemble * dern_r2 = dernier(r2);

  TEST(
       taille_ensemble(dern_r1) == 1
       && est_dans_l_ensemble(dern_r1,3)
       && taille_ensemble(dern_r2) == 3
       && est_dans_l_ensemble(dern_r2,1)
       && est_dans_l_ensemble(dern_r2,2)
       && est_dans_l_ensemble(dern_r2,3)
       , resultat);
    */
  //test automates
  /*
  Automate * a1 = creer_automate();
  ajouter_transition (a1,0,'a',1);
  ajouter_transition (a1,0,'b',2);
  ajouter_transition (a1,1,'a',2);
  ajouter_transition (a1,2,'a',2);
  ajouter_etat_initial (a1,0);
  ajouter_etat_final (a1,1);
  ajouter_etat_final (a1,2);

  Automate * a2 = creer_automate();
  ajouter_transition (a2,0,'b',1);
  ajouter_transition (a2,0,'a',0);
  ajouter_etat_initial (a2,0);
  ajouter_etat_final (a2,1);
  ajouter_etat_final (a2,0);
  */
  /*
    printf("\n --- test complementaire --- \n\n");
  
    printf("\n --- a1 etats, finaux --- \n\n");
    print_ensemble(get_finaux(a1),NULL);
    print_ensemble(get_etats(a1),NULL);
  
    printf("\n --- comp(a1) etats, finaux--- \n\n");

    Automate * ca1 = complementaire (a1);
    print_ensemble(get_finaux(ca1),NULL);
    print_ensemble(get_etats(ca1),NULL);
    printf("\n\n\n");
  */

  //test Systeme

  //Systeme s1 = systeme(a1);
  //Systeme s2 = systeme(a2);

  /*
    fprintf(stderr,"\ns1 \n\n");
    print_systeme (s1,taille_ensemble(get_etats (a1)));
  

    fprintf(stderr,"\ns2 \n\n");
    print_systeme (s2,taille_ensemble(get_etats (a2)));
  */
  
  //nettoyage final
  
  //destroy_systeme(s1,taille_ensemble(get_etats (a1)));
  //destroy_systeme(s2,taille_ensemble(get_etats (a2)));
  
  //liberer_automate (a1);
  //liberer_automate (a2);
  /*
  liberer_ensemble(prem_r1);
  liberer_ensemble(prem_r2);
  liberer_ensemble(dern_r1);
  liberer_ensemble(dern_r2);
  */
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

