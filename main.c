#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <unistd.h>
#include <math.h>

#define MAX_THREADS 5
size_t reglagesPriorite[MAX_THREADS] = {19, 15, 0, -15, -19};

typedef struct {
    int numero ;
    int threadId ;
    int priorite ;
    int erreur ;
    double resultat ;
} Parametres_t;

void *faireQuelqueChose(void* vargp) ;

int main() {
    pthread_t threads[MAX_THREADS];
    Parametres_t params[MAX_THREADS] ;

    /* Créer des paramètres appropriés pour chaque thread, dont la priorité */

    for (int i = 0; i < MAX_THREADS; i++) {
        params[i].numero = i ;
        params[i].threadId = 0 ;
        params[i].priorite = reglagesPriorite[i] ;
        params[i].erreur = 0 ;
    }

    /* Créer chaque thread et afficher le message d'erreur (0 si OK) */

    for (int i = 0; i < MAX_THREADS; ++i) {
        int errorStatus ;
        if ((errorStatus = pthread_create(&(threads[i]), NULL, faireQuelqueChose, (void *) &(params[i])))) {
            printf("Erreur dans la création du thread numéro %d\n", i) ;
            exit(errorStatus) ;
        } else
            printf("Thread numéro %d créé.  Status erreur: %d\n", params[i].numero, errorStatus) ;
    }

    /* Synchroniser les threads avant de quitter */

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL) ;
    }

}

/**
 * Fonction de thread.  Effectue un appel système afin de demander une priorité au thread.  Affiche ensuite les
 * informations résultantes. Puis fait un long calcul inutile en double précision avant de terminer.
 * @param vargp Adresse d'un struct contenant des infos sur le thread et ses résultats.
 * @return Rien
 */
void *faireQuelqueChose(void* vargp) {
    Parametres_t* parametres = (Parametres_t *) vargp ;
    parametres->threadId = (int) syscall(SYS_gettid) ;
    int ret = setpriority(PRIO_PROCESS, parametres->threadId, parametres->priorite) ;
    parametres->erreur = errno ;

    printf("Début du thread numéro %d avec identifiant %d et priorité %d\n", parametres->numero, parametres->threadId, parametres->priorite) ;
    printf("setpriority(%d) pour le thread %d a retourné: %d\n", parametres->priorite, parametres->numero, ret) ;
    printf("La macro errno pour le thread %d est %d\n", parametres->numero, parametres->erreur) ;

    double bidon = 0.0 ;
    for (unsigned long i = 0; i < 20000000; i++) {
        bidon += cos((double) i) ;
    }
    parametres->resultat = bidon ;
    printf("Fin du thread numéro %d\n", parametres->numero) ;
    pthread_exit(NULL) ;
}