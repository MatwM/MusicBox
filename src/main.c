#include <stdio.h>
#include <math.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"
#include "systick.h"
#include "adc.h"
#include "led.h"
#include "button.h"

volatile uint32_t flag = 0, flag_v = 0, compteur = 0, j=0, jtaille=8, i = 0, mode = 0, compteur2=10000;
volatile float tempo = 0;


#define DO 261
#define SID 261
#define DOD 277
#define REB 277
#define RE 293
#define RED 311
#define MIB 311
#define MI 329
#define FAB 329
#define FA 349
#define MID 349
#define FAD 367
#define SOLB 367
#define SOL 392
#define SOLD 415
#define LAB 415
#define LA 440
#define LAD 466
#define SIB 466
#define SI 493
#define INAUDIBLE 20000

#define VITESSE 300

/*-------------------------------------------------- Procédures d'initialisation -----------------------------------------------------------------*/

void init_timer(){
	TIM2.EGR |= 0x1; /* Autoriser l'accès à tous les registres */
	TIM2.CR1 |= (1<<7); /* ARPE = 1 Activer rechargement automatique */
	TIM2.PSC = 100; /* Comment choisir cette valeur ? */
	TIM2.ARR = 10; /* Initialisation période */
	TIM2.CCER |= (1<<4); /* Timer2 actif pour le signal de sortie */
	TIM2.CCMR1 |= (0b1111<<11); /* Configuer timer 2 en PWR*/
}

void init_buzzer(){
	GPIOB.MODER |= (0b10<<18); /* Alternate fonction de PB9 */
	GPIOB.AFRH |= (1<<4); /* Mode AF1 */
	
}

void configuration_potentiometre(){
	uint32_t SMPR =0x7;
	GPIOB.MODER |= 0x3;
	enable_ADC1();
	ADC_set_prediv(0x1);
	ADC_set_sampling_time(&ADC1, 8,SMPR);
	ADC_init_single_channel(&ADC1,8);
}


/*---------------------------------------------- Interractions avec les composants --------------------------------------------------------*/

void frequence(uint32_t newFreq){ /* Hauteur de la note */
	uint32_t tempFreq = newFreq;
	if (newFreq == 0) tempFreq = 1;
	uint32_t tempNewValue = (uint32_t) get_APB1TIMCLK() / TIM2.PSC / tempFreq; /* Calcul de la valeur pour le timer */
	TIM2.CR1 = 0; /* Désactivation du timer 2 */
	/* Mise à jour du timer ...*/
	TIM2.ARR = tempNewValue;
	TIM2.CCR4 = tempNewValue/2;
	TIM2.CNT=0;
	TIM2.CR1 |= 0x81; /* Réactivation du timer */
}

/*Mesure de la valeur du potentiomètre*/
void mesure_potentiometre(uint32_t *val){
	
	*val=ADC_convert_single_channel( &ADC1);
}

/*---------------------------------------------------------------- Interruptions -------------------------------------------------------------------- */

/* Durée d'une note */ 
void __attribute__((interrupt)) SysTick_Handler() {
  compteur ++;
/*Eteind l'appareil après un temps inscrit dans la variable COMPTEUR2*/
  compteur2 --; /* Quel compteur on choisit ? */
  if (compteur2<=0)
	flag = 3;


  if ((compteur > VITESSE*tempo - 30)&&(compteur<VITESSE*tempo - 10)){
	flag = 2;
}
  if (compteur > VITESSE*tempo){
    flag = 1; /* Prêt à jouer une note */
    compteur = 0; /* On recommence à compter */
  }
}

/* Bouton carte fille pour changer de mélodie */
void __attribute__ ((interrupt)) EXTI9_5_Handler(){
	EXTI.PR |= (1<<8);
	j=j+2;
	i=0;
	if (jtaille<j+2)
		j=0;
	flag_v = 1;		
}

/* Bouton bleu pour pour changer le mode de lecture des musiques*/
void __attribute__((interrupt)) EXTI15_10_Handler(){
	EXTI.PR |= (1<<13);
	mode = 1 - mode;
	
}
/*-------------------------------------------------------------- Procédure de veille -------------------------------------------------------------*/

void veille(){
	if (flag_v==1){
		compteur2 = 5 * 60 * 1000;
		return;
	}
	veille();
}

/*-------------------------------------------------------------- Fonction principale -------------------------------------------------------------*/

int main() {

	/* Melodies disponibles (dans l'ordre) */
	char nom[4][20]={"Au clair de la lune","La Berceuse","Vive le vent","Harry Potter"};

	/* Variable pour valeur potentiomètre et tableau des fréquences et durées des notes de chaques mélodies */
	uint32_t val, tab[8][100] = {
	
	/*Chanson au clair de la lune*/
	
	{INAUDIBLE,DO,DO,DO,RE,MI,RE,DO,MI,RE,RE,DO,
	DO,DO,DO,RE,MI,RE,DO,MI,RE,RE,DO,
	RE,RE,RE,RE,LA/2,LA/2,RE,DO,SI/2,LA/2,SOL/2,
	DO,DO,DO,RE,MI,RE,DO,MI,RE,RE,DO,
	'\0'},	
	{10,
	1,1,1,1,2,2,1,1,1,1,4,
	1,1,1,1,2,2,1,1,1,1,4,
	1,1,1,1,2,2,1,1,1,1,4,
	1,1,1,1,2,2,1,1,1,1,4
	},
	
	/*Chanson La Berceuse*/	
	{INAUDIBLE,
	MI,MI,SOL,MI,MI,SOL,
	MI,SOL,DO*2,SI,LA,LA,SOL,
	RE,MI,FA,RE,RE,MI,FA,
	RE,FA,SI,LA,SOL,SI,
	DO*2,DO,DO,DO*2,
	LA,FA,SOL,MI,DO,FA,
	SOL,LA,SOL,DO,DO,
	DO*2,LA,FA,SOL,MI,DO,
	FA,MI,RE,DO,
	'\0'},
	{10,
	1,1,3,1,2,4,
	1,1,2,3,1,2,2,
	1,1,2,2,1,1,4,
	1,1,1,1,2,2,
	4,1,1,4,
	1,1,4,1,1,2,
	2,2,4,1,1,
	4,1,1,4,1,1,
	2,2,2,4
	},
	
	/* Vive le vent */
	{INAUDIBLE,
	MI,MI,MI,MI,MI,MI,
	MI,SOL,DO,RE,MI,
	FA,FA,FA,MI,MI,MI,
	RE,RE,RE,MI,RE,SOL,
	MI,MI,MI,MI,MI,MI,
	MI,SOL,DO,RE,MI,
	FA,FA,FA,SOL,FA,MI,
	MI,MI,SOL,SOL,FA,RE,DO,
	'\0'},
	{10,
	1,1,2,1,1,2,
	1,1,1,1,4,
	1,1,2,1,1,2,
	1,1,1,1,2,2,
	1,1,2,1,1,2,
	1,1,1,1,4,
	1,1,1,1,1,1,
	1,1,1,1,1,1,4
	},

	/* Harry Potter */
	{INAUDIBLE,
	MI,LA,DO*2,SI,LA,MI,
	RE,SI,LA,DO*2,SI,SOLD,
	LAD,MI,MI,LA,DO*2,SI,
	LA,MI*2,SOL*2,FAD*2,FA*2,DOD*2,
	FA*2,MI*2,RED*2,RED*2,DO*2,LA,
	DO*2,MI*2,DO*2,MI*2,DO*2,FA*2,
	MI*2,RED*2,SI,DO*2,MI*2,RED*2,
	RED,MI,MI*2,DO*2,MI*2,DO*2,
	MI*2,DO*2,SOL*2,FAD*2,FA*2,DOD*2,
	FA*2,MI*2,RED*2,RED,DO*2,LA,
	'\0'
	},
	{5,
	1,2,1,1,2,1,
	3,3,2,1,1,2,
	1,4,1,2,1,1,
	2,1,2,1,2,1,
	1,1,1,2,1,4,
	1,2,1,2,1,2,
	1,2,1,1,1,1,
	2,1,4,1,2,1,
	2,1,2,1,2,1,
	1,1,1,2,1,4
	}
	}; 

	/* --- Initialisation ---*/
	
	printf("Initialisation...\n\r");
	/* Autorisation horloge */
	enable_TIM2();
	enable_GPIOA();
	enable_GPIOB();
	/* Configuration du Systick */
	SysTick_init(1000); /* Tick toutes les 1 ms */
	/* Configuration du buzzer */
	init_buzzer(); 
	/* Configuration du timer */
	init_timer();
	/* Configuration du potentiometre */
	configuration_potentiometre();
	/* Configuration du bouton  de la carte fille */
	button_irq_init();
	button_init();
	/* Configuration des LEDs */
  	led_init();
  	/* Configuration du bouton bleu */
  	buttonPC13_init();
	buttonPC13_irq_init();
	/* Initialisation du timer */
	TIM2.CNT = 0;
	TIM2.CCR2 = 2; /* Volume initiale*/
	TIM2.CR1 |= 0x1; /* Activer le timer */
	printf("Fin initialisation !\n\r");
	
	/* --- Boucle principale --- */

	while(1)
	{
		/*Eteind l'appareil après un temps inscrit dans la variable COMPTEUR2 en desactivant les différents modules*/

		if (flag == 3){
			TIM2.CCR2 = 0;
			GPIOA.ODR &= ~0x7F0;
			flag_v = 0;
			veille();
		}		

		/* Pause entre chaque note */
		if (flag == 2){
			TIM2.CCR2 = 0;
		}
		
		else {
			/*Change le volume en fonction de la valeur du potentiomètre*/
			TIM2.CCR2 = val;
			/* Passage à la note d'après */
			if (flag == 1)
			{
				mesure_potentiometre(&val);
				
				/*Allumage des 4 leds rouges en fonction du volume*/
				val=2*(val*50)/4096;
				if (val<25){
					GPIOA.ODR &= ~0xF0;
					GPIOA.ODR |= 0x10;
				}
				if ((val<50)&&(val>24)){
					GPIOA.ODR &= ~0xF0;
					GPIOA.ODR |= 0x30;
				}
				if ((val<75)&&(val>49)){
					GPIOA.ODR &= ~0xF0;
					GPIOA.ODR |= 0x70;
				}
				if ((val<100)&&(val>74)){
					GPIOA.ODR &= ~0xF0;
					GPIOA.ODR |= 0xF0;
				}

				/* Modification de la fréquence et de la durée de la note */
				if (2*tab[j][i]==INAUDIBLE){
					frequence(INAUDIBLE);
					tempo = 1/2*tab[j+1][i];
					}
				else{
				frequence(2*tab[j][i]);
				tempo = tab[j+1][i];
				}
				flag = 0;
				
			/*Affichage des différentes valeur pour le débugage, on peut l'enlever*/
				printf(" En cours: %s, note = %lu Hz (n°%lu) en %lu temps, duty_cycle =0.%lu, mode = %lu, time %lu\r\n",nom[j/2],tab[j][i],i,tab[j+1][i],val,mode,compteur2);

			/*Allumage de la led tricolore en fonction de la fréquence de la note, pour un effet funki*/
				if (tab[j][i]<=300){			
					GPIOA.ODR=GPIOA.ODR ^ 0x0200;}/*vert*/
				else {
					if ((tab[j][i]<=500)&&(tab[j][i]>=300)){				
						GPIOA.ODR=GPIOA.ODR ^ 0x0400;}/*bleu*/
					else {
					
						GPIOA.ODR=GPIOA.ODR ^ 0x0100;}/*rouge*/
				}
			/* Passage à la prochaine note du tableau */
				i++;
			}
		}
		/* Detecte la fin de la chanson */
		if (tab[j][i]=='\0'){
			/* Mode musicle en boucle */
			if (mode == 0) i = 0;
			/* Mode passer automatique d'une musique à l'autre */
			if (mode == 1){
				j=j+2;
				i=0;
				if (jtaille<j+2)
					j=0;
			}
		}

	} 

return 0;
}

