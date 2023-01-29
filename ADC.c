/*
 * ADC.c
 *
 *  Created on: 02/01/2023
 *      Author: AdrianaMtzR
 */
#include "lib/include.h"

extern void Configura_Reg_ADC0(void)
{   
/*
    Configurar el timer 0 y timer 1 en modo de 32bits para hacer la siguiente rutina:
    una función que active  el convertidor analógico digital y muestre dos señales de 
    cierta frecuencia y amplitud definida.
    y las guarde en dos arreglos  de tamaño 1024 y la envié por el protocolo de 
    comunicación asíncrono para ser procesadas y regresadas al microcontrolador en 
    valores binarios las cuales modificaran el ancho de pulso y reconstruirán la señal 
    enviada a un transistor u opam.
    c)10khz, -1 a 1
    */
   
     //Pag 352 INICIALIZA MODULO DE RELOJ RCGCADC
    SYSCTL->RCGCADC = (1<<0); 

    //Pag 340 (RGCGPIO) PUERTOS BASE HABILITACION DE RELOJ
    //                     F     E      D       C      B     A
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|(1<<1);

    //Pag 663 (GPIODIR) HABILITA PINES COMO I/O 
    GPIOE_AHB->DIR = (0<<1) | (0<<2) | (0<<5); //PE5 PE1 y PE2* 

    //(GPIOAFSEL) pag.671 FUNCION ALTERNATIVA, PARA QUE EL MODULO CONTROLE LOS PINES 
    GPIOE_AHB->AFSEL =  (1<<1) | (1<<2) | (1<<5);

    //(GPIODEN) pag.781 DESHABILITA MODO DIGITAL
    GPIOE_AHB->DEN = (0<<1) | (0<<2)| (0<<5);

    //Pag 1351 GPIOPCTL COMBINADO CON GPIOAFSEL TABLA pag 1808 - ESTABLECE MASCARA
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFF0FF00F);

    //(GPIOAMSEL) pag.687 HABILITA ANALOGICO
    GPIOE_AHB->AMSEL = (1<<1) | (1<<2) | (1<<5);

    //Pag 891 (ADCPC) VELOCIDAD DE CONVERSION POR SSEGUNDO
    ADC0->PC = (0<<2)|(1<<1)|(1<<0);//250ksps 

    //Pag 841 (ADCSSPRI) DA PRIORIDAD A LOS SECUENCIADORES
    ADC0->SSPRI = 0x3012; //SEC 2 CON 4 CANALES
     
    //Pag 821 (ADCACTSS) CONTROLA LA ACTIVACION DE LOS SEC
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0);

    //Pag 834 (ADCEMUX) EVENTO ACTIVO DE LA CONVERSION (trigger)
    ADC0->EMUX  = (0x0000); //PROCESADOR

    //Pag 867 (ADCSSMUX2) DEFINE ENTRADAS ANALOG CANAL Y SEC
    ADC0->SSMUX2 = 0x0821; 

    //pag 868 (ADCSSCTL2), CONF BIT CONTROL DE MUESTRO Y SEC 
    ADC0->SSCTL2 = (1<<1) | (1<<2) | (1<<5) | (1<<6) | (1<<10) | (1<<9); // 2 POR CANAL 
    
    // INDICA CUANDO MANDA UNA SEÑAL, MANDA 1 AL SECUENCIADOR (2)
    /* Enable ADC Interrupt */
    ADC0->IM |= (1<<2); /* Unmask ADC0 sequence 2 interrupt pag 825*/

    //Pag 821 (ADCACTSS) ACTIVACION DE SEC
    ADC0->ACTSS = (0<<3) | (1<<2) | (0<<1) | (0<<0);
    ADC0->PSSI |= (1<<2);
}
extern void ADC0_Seq2(uint16_t *Result, uint16_t *duty){ 
        //uint16_t Rojo;
    //ADC Processor Sample Sequence Initiate (ADCPSSI)
       ADC0->PSSI = 0x00000004; //HABILITA PROCESADOR 4 EN HEXA
       while((ADC0->RIS&0x04)==0){}; // ESPERA AL COMVERTIDOR
       Result[2] = ADC0->SSFIFO2&0xFFF; //  LEE Y GUARDA EL RESULTADO 
       // FIFO ALMACENA LAS MUESTRAS - p. 860
       Result[1] = ADC0->SSFIFO2&0xFFF; 
       Result[0] = ADC0->SSFIFO2&0xFFF; 
       Result[2] = ADC0->SSFIFO2&0xFFF;
       // UN REESULTADO POR CANAL - 3 CANALES -
       //Rojo = (uint16_t)readString('%');
       //printChar('A');
       
       duty[0] = 50000 - (Result[0]*5000)/4096;
       duty[1] = 50000 - (Result[1]*5000)/4096;
       duty[2] = 50000 - (Result[2]*5000)/4096;

       ADC0->ISC = 0x0004;  //FINALIZA CONVERSION

}




