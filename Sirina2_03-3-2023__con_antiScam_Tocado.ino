//TOCADO X ALE
//Reduci el tiempo de deteccion  de sensores cableados de 1000 a 500mg


#include <RCSwitch.h>
#include <EEPROM.h>
#include <avr/wdt.h> // LIBRERIA WATCHDOG //

#define entPulsadorTX 1 // 7 por 1 (solo para probar Serial.Print de mi Nano)
#define entPulsadorZ1 3
#define entPulsadorZ2 4
#define entSensorZ24 13 // 12 por 13 (porque se me quemó la pata 13 de mi Nano)
#define entSensorZ1 A2
#define entSensorZ2 A1
#define entPGMpulso 10
#define salLedStrobo 6
#define salLedTX A3
#define salLedZ1 A4
#define salLedZ2 A5
#define salLedEstadPGM 9
#define salSirena 8
#define salBuzzer 5

const int INTERVALO = 10000;
int tiempoSTROBO = 1200;
int TiempoZcableadaZinalambrica;
int cuentoCiclos;
int cuentoCiclosBis;
int AntiScam;
int anuloZ24;
int ciclosZ24;
int ciclosAct;
int anuloParpadeoZ1;
int anuloParpadeoZ2;
int repitoTeclaC;
int actPulso;
int memoriaDeDisparo;
int reArme;

unsigned long tiempoMEMdisparo;
unsigned long tiempoAntiScam;
unsigned long tiempoAnuloZ24;
unsigned long tiempoPanicoSon;
unsigned long tiempoPanicoSonz24z1z2;
unsigned long tiempoRearme;
unsigned long antiREb;
unsigned long previoMillisTX ;
unsigned long previoMillisZ1 ;
unsigned long previoMillisZ2 ;
unsigned long previoMillisLedStrobo;
unsigned long previoMillisLedStrobo2;
unsigned long previoMillisLedElegido;
unsigned long previoMillisLedElegido2;
unsigned long previoMillisLedz24;
unsigned long previoMillisLedz242;
unsigned long buscarCodigo1 ;
unsigned long buscarCodigo2 ;
int tiempoCumplidoTX ;
int tiempoCumplidoZ1 ;
int tiempoCumplidoZ2 ;
int LedElegido ;
int pulsador1 ;
int pulsador2 ;
int pulsador3 ;
int alarmaActivada;
int disparadaAlarma;

unsigned long codigoRecibido ;
unsigned long codigoAlmacenado ;
unsigned long dato;
int punteroEEprom = EEPROM.get(1000, punteroEEprom);
int punteroEEpromZ1 = EEPROM.get(1004, punteroEEpromZ1);
int punteroEEpromZ2 = EEPROM.get(1008, punteroEEpromZ2);
float decimalTecla1;
float decimalTecla2;

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  
  ////////////////// PERRO GUARDIAN ESPERANDO SEÑAL DE VIDA ANTES DE 8 SEGUNDOS //////////
  wdt_enable(WDTO_8S); 
  ////////////////////////////////////////////////////////////////////////////////////////
  
  mySwitch.enableReceive(0);  // Recibe en la interrupcion 0 => es el Pin #2
  
  pinMode(entPulsadorTX, INPUT_PULLUP);
  pinMode(entPulsadorZ1, INPUT_PULLUP);
  pinMode(entPulsadorZ2, INPUT_PULLUP);
  pinMode(entSensorZ24, INPUT_PULLUP);
  pinMode(entSensorZ1, INPUT_PULLUP);
  pinMode(entSensorZ2, INPUT_PULLUP);
  //pinMode(entPGMpulso, INPUT_PULLUP); //"LOGICA INVERTIDA"
  pinMode(entPGMpulso, INPUT);
  
  pinMode(salLedTX, OUTPUT);
  pinMode(salLedZ1, OUTPUT);
  pinMode(salLedZ2, OUTPUT);
  pinMode(salLedEstadPGM, OUTPUT);
  pinMode(salSirena, OUTPUT);
  pinMode(salBuzzer, OUTPUT);
  pinMode(salLedStrobo, OUTPUT);
}

void loop() {
  int potencia;
  int bitLeido;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  wdt_reset(); // ENVIO CONSTANTEMENTE SEÑAL DE VIDA AL WATCHDOG siempre que el programa esté funcionando correctamente
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //////////////////////////////// LEER ENTRADA CABLEADA Z1 /////////////////////////
  if ((digitalRead(entSensorZ1) == HIGH) && (memoriaDeDisparo == 0) && (alarmaActivada != 8) && (alarmaActivada != 10)) {
    delay(500);//delay tocado x ale
    if ((digitalRead(entSensorZ1) == HIGH) && (memoriaDeDisparo == 0)) {  
        digitalWrite(salLedZ1,HIGH);
        anuloParpadeoZ1 = 1;
        if ((alarmaActivada == 1) || (alarmaActivada == 2) || (alarmaActivada == 4) || (alarmaActivada == 5)){
          //Serial.println( "ALARMA DISPARADA !");
          disparadaAlarma = 1;
          digitalWrite(salSirena, HIGH);
          tiempoSTROBO = 200;
          memoriaDeDisparo = 1;
          TiempoZcableadaZinalambrica = 1500;
        }
     }else{
           anuloParpadeoZ1 = 0;
           if ((anuloZ24 == 0) && (alarmaActivada != 8) && (alarmaActivada != 10)){
             digitalWrite(salLedZ1,LOW);
           }
          }
  }        
  //////////////////////////////// LEER ENTRADA CABLEADA Z2 /////////////////////////        
  if ((digitalRead(entSensorZ2) == HIGH) && (memoriaDeDisparo == 0) && (alarmaActivada != 2) && (alarmaActivada != 5)) {
    delay(500);//delay tocado x ale
    if ((digitalRead(entSensorZ2) == HIGH) && (memoriaDeDisparo == 0)) {
       digitalWrite(salLedZ2,HIGH);
       anuloParpadeoZ2 = 1;
       if ((alarmaActivada == 1) || (alarmaActivada == 4) || (alarmaActivada == 8) || (alarmaActivada == 10)){
         //Serial.println( "ALARMA DISPARADA !");
         disparadaAlarma = 1;
         digitalWrite(salSirena, HIGH);
         tiempoSTROBO = 200;
         memoriaDeDisparo = 2;
         TiempoZcableadaZinalambrica = 1500;
       }
     }else{
           anuloParpadeoZ2 = 0;
           if ((anuloZ24 == 0) && (alarmaActivada != 2) && (alarmaActivada != 5)){
             digitalWrite(salLedZ2,LOW);
           }
          }
  }     
  //////////////////////////////// LEER ENTRADA CABLEADA Z24 /////////////////////////////////////////////////////////////////////////////////////////////////////////////              

  if  (anuloZ24 == 0){ 
      if (digitalRead(entSensorZ24) == LOW) cuentoCiclos = 0;
      if ((digitalRead(entSensorZ24) == HIGH) && (cuentoCiclos == 0)){
      delay(1000);
      if (digitalRead(entSensorZ24) == HIGH){ /////// ANTIREBOTE 1000 msEG. ////////
        cuentoCiclos = 1;
        alarmaActivada = 1;
        disparadaAlarma = 1;
        tiempoSTROBO = 200;
        memoriaDeDisparo = 3;
        TiempoZcableadaZinalambrica = 3000;
        digitalWrite(salSirena, HIGH);
      }
    }
  }

  
  //////////////////////////////// LEER SENSORES INHALAMBRICOS Y CONTROLES REMOTOS /////////////////////////////////////////////////////////////////////////////////////                   
  if (mySwitch.available()) {
    //Serial.print("Received ");
    //Serial.print( mySwitch.getReceivedValue() );
    //Serial.println(" ");
    //Serial.print(" / ");
    //Serial.print( mySwitch.getReceivedBitlength() );
    //Serial.print("bit ");
    //Serial.print("Protocol: ");
    //Serial.println( mySwitch.getReceivedProtocol() );
    //Serial.println(" "); 
    
    codigoRecibido = mySwitch.getReceivedValue();
 
    //////////////////////////////// ALGORITMO DE CONVERSION DONDE TRANSFORMO CUALQUIER TECLA PRESIONADA AL CODIGO DE LA TECLA "A" EN AMBOS PATRONES DE CONTROLES REMOTOS //////
    //////////////////////////////// número código a buscar en memoria para identificarlo del patrón 1 /////////////////////////
    for (int i = 0; i < 4; i++) {
     bitLeido = bitRead(codigoRecibido, (3-i));
     potencia = (3-i);
     decimalTecla1  =  decimalTecla1 + pow(2,(potencia)) * bitLeido;
    }
    buscarCodigo1 = codigoRecibido - decimalTecla1 + 4;
    
    //////////////////////////////// número código a buscar en memoria para identificarlo del patrón 2 /////////////////////////
    for (int i = 0; i < 8; i++) {
     bitLeido = bitRead(codigoRecibido, (7-i));
     potencia = (7-i);
     decimalTecla2  =  decimalTecla2 + pow(2,(potencia)) * bitLeido;
    }
    buscarCodigo2 = codigoRecibido - decimalTecla2 + 3;

    ////////////////////////////////////////////////////// LUEGO DE ESA TRANSFORMACION ENVIO A BUSCAR EL CODIGO A EEPROM /////////////////////////////////////////////////
    VerificarCodigos();
    buscarCodigo1 =0;
    buscarCodigo2 =0;
    decimalTecla1 = 0;
    decimalTecla2 = 0;
    mySwitch.resetAvailable();
 }

  /////////////////////////////////// PULSADOR TX //////////////////////////////////////////////////////////
  if ((digitalRead(entPulsadorTX) == LOW) && ((alarmaActivada == 0) || (alarmaActivada == 6)))  { // cuando se complete el pulso recien entro a programación
    digitalWrite(salLedEstadPGM, LOW);
    pulsador1 = 1;
  }
  if (digitalRead(entPulsadorTX) == HIGH) { // cuando se complete el pulso recien entro a programación

    if (tiempoCumplidoTX == 1){ 
      pulsador1 = 0;
      tiempoCumplidoTX = 0;
      borrarTX();
    }
    previoMillisTX = millis();
    if (pulsador1 == 1) {
      delay (500); // antirebote
      LedElegido = salLedTX;
      digitalWrite(salBuzzer, HIGH);
      delay (250);
      digitalWrite(salBuzzer, LOW);
      digitalWrite(salLedZ1,LOW);
      digitalWrite(salLedZ2,LOW);
      digitalWrite(salLedTX, LOW);
      almacenarTX();
    }
  }

  /////////////////////////////////// PULSADOR Z1 //////////////////////////////////////////////////////////
  if ((digitalRead(entPulsadorZ1) == LOW) && ((alarmaActivada == 0) || (alarmaActivada == 6))) { // cuando se complete el pulso recien entro a programación
    digitalWrite(salLedEstadPGM, LOW);
    pulsador2 = 1;
  }
  
  if (digitalRead(entPulsadorZ1) == HIGH) { // cuando se complete el pulso recien entro a programación
    if (tiempoCumplidoZ1 == 1){ 
      pulsador2 = 0;
      tiempoCumplidoZ1 = 0;
      borrarZ1();
    }
    previoMillisZ1 = millis();
    if (pulsador2 == 1) {
      delay (500); // antirebote
      LedElegido = salLedZ1;
      digitalWrite(salBuzzer, HIGH);
      delay (250);
      digitalWrite(salBuzzer, LOW);
      digitalWrite(salLedZ1,LOW);
      digitalWrite(salLedZ2,LOW);
      digitalWrite(salLedTX, LOW);
      almacenarZ1();
    }
  }

  /////////////////////////////////// PULSADOR Z2 //////////////////////////////////////////////////////////
  if ((digitalRead(entPulsadorZ2) == LOW) && ((alarmaActivada == 0) || (alarmaActivada == 6))) { // cuando se complete el pulso recien entro a programación
    digitalWrite(salLedEstadPGM, LOW);
    pulsador3 = 1;
  }

  if (digitalRead(entPulsadorZ2) == HIGH) { // cuando se complete el pulso recien entro a programación
    if (tiempoCumplidoZ2 == 1){ 
      pulsador3 = 0;
      tiempoCumplidoZ2 = 0;
      borrarZ2();
    }
    previoMillisZ2 = millis();
    if (pulsador3 == 1) {
      delay (500); // antirebote
      LedElegido = salLedZ2;
      digitalWrite(salBuzzer, HIGH);
      delay (250);
      digitalWrite(salBuzzer, LOW);
      digitalWrite(salLedZ1,LOW);
      digitalWrite(salLedZ2,LOW);
      digitalWrite(salLedTX, LOW);
      almacenarZ2();
    }
  }


////////////////////////////////////////// ACTIVACION POR PULSO ent. PGM /////////////////////////////////////////////////////////////////
  if (digitalRead(entPGMpulso) == HIGH) {
    delay (1000);
    if (digitalRead(entPGMpulso) == HIGH) {
      if ((alarmaActivada == 6) || (alarmaActivada == 0)){
        alarmaActivada = 4;
        actPulso = 1;
        memoriaDeDisparo = 0;
      }else{
            alarmaActivada = 6;
            actPulso = 0;
           }
      ActivarDesactivar();
    } 
  }

///////////////////////////////////////// ACTIVACION POR REARME AL CUMPLIRSE 5 MINUTOS DE ESTAR DISPARADA //////////////////////////////
 if (disparadaAlarma == 1) {
   if (cuentoCiclosBis == 0 ) tiempoRearme = millis(); //RESETEO EL TIEMPO DE REARME !!
   cuentoCiclosBis = cuentoCiclosBis + 1;
   if (cuentoCiclosBis >= 1) cuentoCiclosBis = 2;
   
   if (millis() - tiempoRearme >= 300000){  //si ha transcurrido el periodo programado
     //Serial.println ("tiempo Cumplido 5 minutos");
     alarmaActivada = 0;
     ActivarDesactivar();
     delay(2000);
     alarmaActivada = 1;
     actPulso = 1;
     reArme = 1;
     ActivarDesactivar();
   }
 }
   
//////////////////////////////////////////////////// destello de Z1 y Z2 cuando deshabilito la z24 ///////////////////////////////////////
 if ((anuloZ24 == 1) && (anuloParpadeoZ1 == 0) && (anuloParpadeoZ2 == 0) && (memoriaDeDisparo == 0) && ((alarmaActivada == 0) || (alarmaActivada == 6))){
   if (millis() - previoMillisLedz24 >= 100){ // parpadeo del led de Z1 y Z2 cuando deshabilito la Z24 para que el técnico pueda Instalar la Alarma 
     previoMillisLedz24 = millis();
     digitalWrite(salLedZ1,LOW);
     digitalWrite(salLedZ2,LOW);
     if (millis() - previoMillisLedz242 >= 5000){
       previoMillisLedz242 = millis();
       digitalWrite(salLedZ1,HIGH);
       digitalWrite(salLedZ2,HIGH);
     }
   }
 }

//////////////////////////////////////////////////// destello de Z1 cuando deshabilito la z1 (tecla C del control de anulacion de zona) ///////////////////////////////////////
 if ((alarmaActivada == 8) || (alarmaActivada == 10)){
   if (millis() - previoMillisLedz24 >= 100){ // parpadeo del led Z1 cuando presiono la tecla C del control (anulacion de zona)  
     previoMillisLedz24 = millis();
     digitalWrite(salLedZ1,LOW);
     if (millis() - previoMillisLedz242 >= 3000){
       previoMillisLedz242 = millis();
       digitalWrite(salLedZ1,HIGH);
     }
   }
 }
 
//////////////////////////////////////////////////// destello de Z2 cuando deshabilito la z2 (tecla C del control de anulacion de zona) ///////////////////////////////////////
 if ((alarmaActivada == 2) || (alarmaActivada == 5)){
   if (millis() - previoMillisLedz24 >= 100){ // parpadeo del led Z2 cuando presiono la tecla C del control (anulacion de zona)  
     previoMillisLedz24 = millis();
     digitalWrite(salLedZ2,LOW);
     if (millis() - previoMillisLedz242 >= 3000){
       previoMillisLedz242 = millis();
       digitalWrite(salLedZ2,HIGH);
     }
   }
 }
 
///////////////////////////////////// Anulo o Habilito Z24 luego de precionar la tecla D durante 5 seg. ///////////////////////////////////
 if (alarmaActivada == 0){
   if (millis() - tiempoAnuloZ24 >= 5000){
     tiempoAnuloZ24 = millis();
     if (ciclosZ24 >= 45) {
       //Serial.println("Anulo o Habilito Z24 luego de precionar la tecla D durante 5 seg.");
       osciladorTresLED();
       if (anuloZ24 == 0){
         anuloZ24 = 1;
       }else{
             anuloZ24 = 0;
            }
     }
     ciclosZ24 = 0; 
   }
 }

///////////////////////////////////// DISPARO POR PANICO SONORO luego de presionar la tecla A durante 5 seg. ///////////////////////////////////
  if (millis() - tiempoPanicoSon >= 5000){
    tiempoPanicoSon = millis();
    if (ciclosAct >= 35) {
      //Serial.println(" DISPARO POR PANICO SONORO luego de presionar la tecla A durante 5 seg.");
      disparadaAlarma = 1;
      digitalWrite(salSirena, HIGH);
      tiempoSTROBO = 200;
      memoriaDeDisparo = 4;
      TiempoZcableadaZinalambrica = 3000;
    }
    ciclosAct = 0;
  }

 ///////////////////////////////// Parpadeo del led de strobo cuando se activa o se dispara la Alarma Y ANTISCAM ///////////////////////////
 if ((alarmaActivada != 0) && (alarmaActivada != 6) && (alarmaActivada != 9)){
   if (millis() - previoMillisLedStrobo >= (tiempoSTROBO/2)){ // parpadeo del led de strobo cuando se activa o se dispara la Alarma 
     previoMillisLedStrobo = millis();
     digitalWrite(salLedStrobo,HIGH);
     if (millis() - previoMillisLedStrobo2 >= tiempoSTROBO){
       previoMillisLedStrobo2 = millis();
       digitalWrite(salLedStrobo,LOW);
     }
   }
   //ANTISCAM -------- proboca un disparo al recibir señales de codigos no reconocidos
   // durante un periodo de 10 segundos mienttras la Alarma permanezca activada.
   if (millis() - tiempoAntiScam >= 10000){ /// AntiiScam deshabilitado TEMPORALMENTE
     tiempoAntiScam = millis();
     if (AntiScam >= 50) {
       //Serial.println(" Scam Scam Scam Scam !!! ");
       //Serial.println( "ALARMA DISPARADA !");
       //Serial.println( AntiScam);
       disparadaAlarma = 1;
       digitalWrite(salSirena, HIGH);
       tiempoSTROBO = 200;
       memoriaDeDisparo = 5;
     }
     AntiScam = 0; 
   }
 }
 else{
       digitalWrite(salLedStrobo,LOW);
      }

 ////////////////////////////////// MEMORIAS DE DISPARO ////////////////////////////////////////////////////////////////////////////////////
    if ((disparadaAlarma == 0 ) && ( reArme == 0)) {
      if (millis() - tiempoMEMdisparo >= TiempoZcableadaZinalambrica) {
        tiempoMEMdisparo = millis();
        //Serial.println(memoriaDeDisparo);
        if (memoriaDeDisparo == 1){
          digitalWrite(salLedZ1,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedZ1,LOW);
          digitalWrite(salBuzzer,LOW);
        }
        if (memoriaDeDisparo == 2){
          digitalWrite(salLedZ2,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedZ2,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedZ2,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedZ2,LOW);
          digitalWrite(salBuzzer,LOW);
        }
        if (memoriaDeDisparo == 3){
          digitalWrite(salLedZ1,HIGH);
          digitalWrite(salLedZ2,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedZ1,LOW);
          digitalWrite(salLedZ2,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedZ1,HIGH);
          digitalWrite(salLedZ2,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedZ1,LOW);
          digitalWrite(salLedZ2,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedZ1,HIGH);
          digitalWrite(salLedZ2,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedZ1,LOW);
          digitalWrite(salLedZ2,LOW);
          digitalWrite(salBuzzer,LOW);
        }                        
        if (memoriaDeDisparo == 4){
          digitalWrite(salLedTX,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedTX,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedTX,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedTX,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedTX,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedTX,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedTX,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (150);
          digitalWrite(salLedTX,LOW);
          digitalWrite(salBuzzer,LOW);
        }
        
        if (memoriaDeDisparo == 5){
          digitalWrite(salLedTX,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (50);
          digitalWrite(salLedTX,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedZ1,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (50);
          digitalWrite(salLedZ1,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedZ2,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (50);
          digitalWrite(salLedZ2,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedZ1,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (50);
          digitalWrite(salLedZ1,LOW);
          digitalWrite(salBuzzer,LOW);
          delay (50);
          digitalWrite(salLedTX,HIGH);
          digitalWrite(salBuzzer,HIGH);
          delay (50);
          digitalWrite(salLedTX,LOW);
          digitalWrite(salBuzzer,LOW);
        }
                                                                
      }
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// SUB RUTINAS /////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////******************* MODO USUARIO ************************////////////////////////////////////////////////////////////////////

void VerificarCodigos(){
   int verificado;
   int patronTX;
  
   //Serial.println("VERIFICANDO...");
   verificado = 0;

   for (int i = 20; i < 40; i++) {
     EEPROM.get(i * 4, codigoAlmacenado);
     if (codigoAlmacenado == codigoRecibido) verificado = 1;
   }
   if (verificado == 1) {
     //Serial.println( "Codigo sensor Z1 DETECTADO ! ");
     digitalWrite(salLedZ2,LOW);
     digitalWrite(salLedZ1,HIGH); ////////////////////////// enciendo Led Z1 por 500 mSeg. ///////////////////////////
     delay (500);
     digitalWrite(salLedZ1,LOW);
     verificado = 0;
     if ((alarmaActivada == 1) || (alarmaActivada == 2) || (alarmaActivada == 4) || (alarmaActivada == 5)){
       //Serial.println( "ALARMA DISPARADA !");
       disparadaAlarma = 1;
       memoriaDeDisparo = 1;
       TiempoZcableadaZinalambrica = 3000;
       digitalWrite(salSirena, HIGH);
       tiempoSTROBO = 200;
     }
   }else {
          //Serial.println( "Codigo sensor Z1 No RECONOCIDO ! ");
         }
    
   for (int i = 40; i < 60; i++) {
     EEPROM.get(i * 4, codigoAlmacenado);
     if (codigoAlmacenado == codigoRecibido) verificado = 1;
   }
   if (verificado == 1) {
     //Serial.println( "Codigo sensor Z2 DETECTADO ! ");
     digitalWrite(salLedZ1,LOW);
     digitalWrite(salLedZ2,HIGH); ////////////////////////// enciendo Led Z2 por 500 mSeg. ///////////////////////////
     delay (500);
     digitalWrite(salLedZ2,LOW);
     verificado = 0;
     if ((alarmaActivada == 1) || (alarmaActivada == 4)){
       //Serial.println( "ALARMA DISPARADA !");
       disparadaAlarma = 1;
       memoriaDeDisparo = 2;
       TiempoZcableadaZinalambrica = 3000;
       digitalWrite(salSirena, HIGH);
       tiempoSTROBO = 200;
     }
   }else {
          //Serial.println( "Codigo sensor Z2 No RECONOCIDO ! ");
          //Serial.println(" "); 
         }
   
   for (int i = 0; i < 20; i++) {
     EEPROM.get(i * 4, codigoAlmacenado);
     if (codigoAlmacenado == buscarCodigo1) {
      patronTX = 1;
      verificado = 1;
     }
     if (codigoAlmacenado == buscarCodigo2) {
      patronTX = 2;
      verificado = 1;
     }
   }
   if (verificado == 1) {
    //Serial.println("Codigo control remoto TX DETECTADO ! ");
    //Serial.println(" ");    
    verificado = 0;
    identificarTeclas(patronTX);
    }else {
            //Serial.println( "Codigo control remoto TX No RECONOCIDO ! ");
            //Serial.println(" ");
            //Serial.println(AntiScam);
            if (alarmaActivada != 0) AntiScam = AntiScam + 1;
          }    
}

void identificarTeclas(int patronTX){
///////////////////////////////////////////// IDENTIFICO TECLAS PRECIONADAS Y DEFINO EL EVENTO A EJECUTAR /////////////////////////////////////
  if (patronTX == 1) {
    if (round(decimalTecla1) == 4){
      //Serial.println("PRESIONASTE LA TECLA A DEL PATRON 1");
      if (alarmaActivada == 0) tiempoPanicoSonz24z1z2 = millis();
      actPulso = 0;
      repitoTeclaC = 0;
      ciclosAct = ciclosAct + 1;
      if (alarmaActivada != 1){ //si presiono dos veces la tecla A (activado) salgo de esta rutina, asi solo activará 
                                //despues de solo haber hecho previamente cualquier tipo de desActivacion.
        memoriaDeDisparo = 0;
        alarmaActivada = 1;
      }else{
             return;        
           }
    }      
    if (round(decimalTecla1) == 1){
      //Serial.println("PRESIONASTE LA TECLA C DEL PATRON 1");
      alarmaActivada = 2;
      repitoTeclaC = 1;
      memoriaDeDisparo = 0;
    }
    if (round(decimalTecla1) == 8){
      //Serial.println("PRESIONASTE LA TECLA D DEL PATRON 1");
      ciclosZ24 = ciclosZ24 +1;
      if (alarmaActivada != 0){ //si presiono dos veces la tecla D (desactivado) salgo de esta rutina, asi solo desactivará 
                                //despues de solo haber hecho previamente cualquier tipo de Activacion.
        alarmaActivada = 0;
      }else{
             return;        
           }
    }
    if (round(decimalTecla1) == 2){
      //Serial.println("PRESIONASTE LA TECLA B DEL PATRON 1");
      //alarmaActivada = 3;
      return;
    }
    if (round(decimalTecla1) == 6){
      //Serial.println("PRESIONASTE LAS TECLAS B+A DEL PATRON 1");
      actPulso = 0;
      alarmaActivada = 4;
      memoriaDeDisparo = 0;
    }
    if (round(decimalTecla1) == 3){
      //Serial.println("PRESIONASTE LAS TECLAS B+C DEL PATRON 1");
      alarmaActivada = 5;
      repitoTeclaC = 1;
      memoriaDeDisparo = 0;
    } 
    if (round(decimalTecla1) == 10){
      //Serial.println("PRESIONASTE LAS TECLAS B+D DEL PATRON 1");
      alarmaActivada = 6;
    }
    if (round(decimalTecla1) == 9){
      //Serial.println("PRESIONASTE LAS TECLAS C+D DEL PATRON 1");
      return;
    }                        
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (patronTX == 2) {
    if (round(decimalTecla2) == 3){
      //Serial.println("PRESIONASTE LA TECLA A DEL PATRON 2");
      if (alarmaActivada == 0) tiempoPanicoSonz24z1z2 = millis();
      ciclosAct = ciclosAct + 1;
      actPulso = 0;
      repitoTeclaC = 0;
      if (alarmaActivada != 1){ //si presiono dos veces la tecla A (activado) salgo de esta rutina, asi solo activará 
                                //despues de solo haber hecho previamente cualquier tipo de desActivacion.
        memoriaDeDisparo = 0;                                
        alarmaActivada = 1;
      }else{
             return;        
           }
    }
    if (round(decimalTecla2) == 192){
      //Serial.println("PRESIONASTE LA TECLA C DEL PATRON 2");
      alarmaActivada = 2;
      repitoTeclaC = 1;
      memoriaDeDisparo = 0;
    }
    if (round(decimalTecla2) == 48){
      //Serial.println("PRESIONASTE LA TECLA D DEL PATRON 2");
      ciclosZ24 = ciclosZ24 +1;
      if (alarmaActivada != 0){ //si presiono dos veces la tecla D (desactivado) salgo de esta rutina, asi solo desactivará 
                                //despues de solo haber hecho previamente una cualquier tipo de Activacion.
        alarmaActivada = 0;
      }else{
             return;        
           }
    }
    if (round(decimalTecla2) == 12){
      //Serial.println("PRESIONASTE LA TECLA B DEL PATRON 2");
      //alarmaActivada = 3;
      return;
    }
    if (round(decimalTecla2) == 15){
      //Serial.println("PRESIONASTE LAS TECLAS B+A DEL PATRON 2");
      actPulso = 0;
      alarmaActivada = 4;
      memoriaDeDisparo = 0;
    }
    if (round(decimalTecla2) == 204){
      //Serial.println("PRESIONASTE LAS TECLAS B+C DEL PATRON 2");
      alarmaActivada = 5;
      repitoTeclaC = 1;
      memoriaDeDisparo = 0;
    } 
    if (round(decimalTecla2) == 60){
      //Serial.println("PRESIONASTE LAS TECLAS B+D DEL PATRON 2");
      alarmaActivada = 6;
   }
    if (round(decimalTecla2) == 240){
      //Serial.println("PRESIONASTE LAS TECLAS C+D DEL PATRON 2");
      return;
    }                         
  }

  ActivarDesactivar();
}


////////////////////////////////////// Rutina que ejecuta distintos tipos de Activaciones o Desactivaciones segun lo indicado por 
////////////////////////////////////// las distintas maneras de operar el sistema.  
void ActivarDesactivar(){
  if (alarmaActivada == 1){
    if (disparadaAlarma == 0) {
      if (((digitalRead(entSensorZ1) == HIGH) && (digitalRead(entSensorZ2) == HIGH)) || (digitalRead(entSensorZ24) == HIGH && (anuloZ24 == 0) && (actPulso == 0))) {
        //Serial.println("BLOQUEO TODA ACTIVACION al detectar Z1 Y Z2 O Z24 abiertas !");
        alarmaActivada = 9;
      }else{
            if (digitalRead(entSensorZ2) == HIGH) {
              //Serial.println("Cambio de Activacion TOTAL a PARCIAL al detectar Z2 abierta!");
              alarmaActivada = 2;
            }else{
                  if (digitalRead(entSensorZ1) == HIGH) {
                    //Serial.println("Cambio de Activacion TOTAL a PARCIAL al detectar Z1 abierta!");
                    alarmaActivada = 8;
                  }else{
                        tiempoSTROBO = 1200;
                        digitalWrite(salLedEstadPGM, HIGH);
                        //Serial.println("ALARMA ACTIVADA TOTAL! EN MODO SONORO POR SIRENA");
                        digitalWrite(salSirena, HIGH);
                        delay (400);
                        digitalWrite(salSirena, LOW);
                      }
                 }
           }
    }
  }

  if (alarmaActivada == 4){
    if (disparadaAlarma == 0) {
      if (((digitalRead(entSensorZ1) == HIGH) && (digitalRead(entSensorZ2) == HIGH)) || (digitalRead(entSensorZ24) == HIGH && (anuloZ24 == 0) && (actPulso == 0))) {
          //Serial.println("BLOQUEO TODA ACTIVACION al detectar Z1 Y Z2 O Z24 abiertas !");
          alarmaActivada = 9;
      }else{
            if (digitalRead(entSensorZ2) == HIGH) {
              //Serial.println("Cambio de Activacion TOTAL a PARCIAL al detectar Z2 abierta! (MODO SILENCIOSO)");
              alarmaActivada = 5;
            }else{
                  if (digitalRead(entSensorZ1) == HIGH) {
                    //Serial.println("Cambio de Activacion TOTAL a PARCIAL al detectar Z1 abierta!  (MODO SILENCIOSO)");
                    alarmaActivada = 10;
                  }else{
                        tiempoSTROBO = 1200;
                        digitalWrite(salLedEstadPGM, HIGH);
                        digitalWrite(salBuzzer, HIGH);
                        delay (400);
                        digitalWrite(salBuzzer, LOW);
                       }
                 }
           }
    }
  }

  if (alarmaActivada == 2) {
    if (disparadaAlarma == 0) {
      if ((digitalRead(entSensorZ1) == HIGH) || (digitalRead(entSensorZ24) == HIGH && (repitoTeclaC == 1) && (anuloZ24 == 0))) {
      //if (((digitalRead(entSensorZ1) == HIGH) && (digitalRead(entSensorZ2) == HIGH)) || (digitalRead(entSensorZ24) == HIGH && (anuloZ24 == 0))) {
      //Serial.println("AL INTENTAR ACTIVACION PARCIAL (anular Z2) BLOQUEO TODA ACTIVACION al detectar Z1!");
        alarmaActivada = 9;
      }else{
            //Serial.println("ALARMA ACTIVADA PARCIAL (Z2 ANULADA)!");
            tiempoSTROBO = 1200;
            digitalWrite(salLedEstadPGM, HIGH);
            digitalWrite(salSirena, HIGH);
            delay (300);
            digitalWrite(salSirena, LOW);
            delay (600); 
            digitalWrite(salSirena, HIGH);
            delay (800);         
            digitalWrite(salSirena, LOW);
            if (repitoTeclaC == 0) ciclosAct = ciclosAct + 20;
           }
     }
  }  

  if (alarmaActivada == 8) {
      if (disparadaAlarma == 0) {
        //Serial.println("ALARMA ACTIVADA PARCIAL (Z1 ANULADA)!");
        tiempoSTROBO = 1200;
        digitalWrite(salLedEstadPGM, HIGH);
        digitalWrite(salSirena, HIGH);
        delay (300);
        digitalWrite(salSirena, LOW);
        delay (600);
        digitalWrite(salSirena, HIGH);
        delay (800);
        digitalWrite(salSirena, LOW);
        if (repitoTeclaC == 0) ciclosAct = ciclosAct + 20;
      }
  }

  if (alarmaActivada == 5) {
    if (disparadaAlarma == 0) {
      if ((digitalRead(entSensorZ1) == HIGH) || (digitalRead(entSensorZ24) == HIGH && (repitoTeclaC == 1) && (anuloZ24 == 0))) {
        //Serial.println("AL INTENTAR ACTIVACION PARCIAL (anular Z2) BLOQUEO TODA ACTIVACION al detectar Z1 abierta!");
         alarmaActivada = 9;
      }else{
            //Serial.println("ALARMA ACTIVADA PARCIAL (Z2 ANULADA)! MODO SILECIOSO, LA SEÑALIZACION SALE POR EL BUZZER EN VEZ DE LA SIRENA ");
            tiempoSTROBO = 1200;
            digitalWrite(salLedEstadPGM, HIGH);
            digitalWrite(salBuzzer, HIGH);
            delay (300);
            digitalWrite(salBuzzer, LOW);
            delay (600);
            digitalWrite(salBuzzer, HIGH);
            delay (800);
            digitalWrite(salBuzzer, LOW);
           }
    }
  }

  if (alarmaActivada == 10) {
    if (disparadaAlarma == 0) {
      //Serial.println("ALARMA ACTIVADA PARCIAL (Z1 ANULADA)! MODO SILECIOSO, LA SEÑALIZACION SALE POR EL BUZZER EN VEZ DE LA SIRENA ");
      tiempoSTROBO = 1200;
      digitalWrite(salLedEstadPGM, HIGH);
      digitalWrite(salBuzzer, HIGH);
      delay (300);
      digitalWrite(salBuzzer, LOW);
      delay (600);
      digitalWrite(salBuzzer, HIGH);
      delay (800);
      digitalWrite(salBuzzer, LOW);
    }
  }

  if (alarmaActivada == 0){         
    //Serial.println("ALARMA DESACTIVADA !");
    digitalWrite(salSirena, LOW);
    delay (250);   
    digitalWrite(salSirena, HIGH);
    delay (350);
    digitalWrite(salSirena, LOW);
    delay (450);
    digitalWrite(salSirena, HIGH);
    delay (350);
    digitalWrite(salSirena, LOW);
    digitalWrite(salLedStrobo,LOW);
    digitalWrite(salLedEstadPGM, LOW);
    digitalWrite(salLedZ1,LOW);
    digitalWrite(salLedZ2,LOW);
    ciclosAct = 0;
    cuentoCiclosBis = 0;
    repitoTeclaC = 0;
    actPulso = 0;
    disparadaAlarma = 0;
    reArme = 0;
    anuloParpadeoZ1 = 0;
    anuloParpadeoZ2 = 0;
    tiempoMEMdisparo = millis();
    tiempoAntiScam = millis(); // reseteo el tiempo de antiscam cuando la desactivo
    AntiScam = 0;
  }

  if (alarmaActivada == 6){         
    //Serial.println("ALARMA DESACTIVADA ! MODO SILECIOSO, LA SEÑALIZACION SALE POR EL BUZZER EN VEZ DE LA SIRENA ");
    digitalWrite(salSirena, LOW);
    delay (250);   
    digitalWrite(salBuzzer, HIGH);
    delay (250);
    digitalWrite(salBuzzer, LOW);
    delay (400);
    digitalWrite(salBuzzer, HIGH);
    delay (250);
    digitalWrite(salBuzzer, LOW);
    digitalWrite(salLedStrobo,LOW);
    digitalWrite(salLedEstadPGM, LOW);
    digitalWrite(salLedZ1,LOW);
    digitalWrite(salLedZ2,LOW);
    ciclosAct = 0;
    cuentoCiclosBis = 0;
    repitoTeclaC = 0;
    actPulso = 0;
    disparadaAlarma = 0;
    anuloParpadeoZ1 = 0;
    anuloParpadeoZ2 = 0;
    reArme = 0;
    tiempoMEMdisparo = millis();
    tiempoAntiScam = millis(); // reseteo el tiempo de antiscam cuando la desactivo
    AntiScam = 0;
  }

  if (alarmaActivada == 9) {
    if (disparadaAlarma == 0) {
      //Serial.println("ACTIVACION BLOQUEADA POR ESTAR LAS DOS ZONAS Z1 Y Z2 ABIERTAS !!!"); 
      digitalWrite(salSirena, LOW);
      digitalWrite(salLedZ1, LOW);
      digitalWrite(salLedZ2, LOW);
      delay (250);   
      digitalWrite(salSirena, HIGH);
      digitalWrite(salLedZ1, HIGH);
      digitalWrite(salLedZ2, HIGH);
      delay (250);
      digitalWrite(salSirena, LOW);
      digitalWrite(salLedZ1, LOW);
      digitalWrite(salLedZ2, LOW);
      delay (400);
      digitalWrite(salSirena, HIGH);
      digitalWrite(salLedZ1, HIGH);
      digitalWrite(salLedZ2, HIGH);
      delay (250);
      digitalWrite(salSirena, LOW);
      digitalWrite(salLedZ1, LOW);
      digitalWrite(salLedZ2, LOW);
      delay (400);
      digitalWrite(salSirena, HIGH);
      digitalWrite(salLedZ1, HIGH);
      digitalWrite(salLedZ2, HIGH);
      delay (250);
      digitalWrite(salSirena, LOW);
      digitalWrite(salLedZ1, LOW);
      digitalWrite(salLedZ2, LOW);
      delay (400);
      digitalWrite(salSirena, HIGH);
      digitalWrite(salLedZ1, HIGH);
      digitalWrite(salLedZ2, HIGH);
      delay (250);
      digitalWrite(salSirena, LOW);
      digitalWrite(salLedZ1, LOW);
      digitalWrite(salLedZ2, LOW);
      if (repitoTeclaC == 0) ciclosAct = ciclosAct + 20;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////******************* MODO PROGRAMACION *******************////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// ALMACENAR CONTROLES REMOTOS ///////////////////////////////////////////////////////////////////////////////////////////////////////
void almacenarTX(){
  int bitPatron1Letra_A[] = {0, 1, 0, 0};
  int bitPatron2Letra_A[] = {0, 0, 0, 0, 0, 0, 1, 1};
  int coincidencias1 ;
  int coincidencias2 ;
  
  while( pulsador1 != 0 ) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    wdt_reset(); // ENVIO CONSTANTEMENTE SEÑAL DE VIDA AL WATCHDOG siempre que el programa esté funcionando correctamente
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    destelloLedElegido();
    if (mySwitch.getReceivedValue()) {
        codigoRecibido = mySwitch.getReceivedValue();
        if ( codigoRecibido <= 10000)  codigoRecibido = 0;
        //Serial.println(" ");
        //Serial.print( "codigoRecibido: ");
        //Serial.println( codigoRecibido );

        if ( punteroEEprom  > 100) punteroEEprom = 0;
      
       ////////////////////////////////////// identifico tecla A del patron 2 //////////////////////////////////
        for (int i = 0; i < 8; i++) {
         if (bitPatron2Letra_A[i] == bitRead(codigoRecibido, (7-i))) coincidencias1 = coincidencias1 + 1;
        }
        if (coincidencias1 == 8) {
          //Serial.println("PRESIONASTE LA TECLA A patron 2");
          comparaTXmem();
         }else {
               ////////////////////////////////////// identifico tecla A del patron 1 //////////////////////////////////
               for (int i = 0; i < 4; i++) {
               if (bitPatron1Letra_A[i] == bitRead(codigoRecibido, (3-i))) coincidencias2 = coincidencias2 + 1;
               }
               if (coincidencias2 == 4) {
               //Serial.println("PRESIONASTE LA TECLA A patron 1");
               comparaTXmem();
               }else {
                      digitalWrite(salLedZ1,HIGH);
                      digitalWrite(salLedZ2,HIGH);
                      digitalWrite(salBuzzer, HIGH);
                      delay (1000);
                      digitalWrite(salLedZ1,LOW);
                      digitalWrite(salLedZ2,LOW);
                      digitalWrite(salBuzzer, LOW);      
                     }   
              }
        coincidencias1 = 0;
        coincidencias2 = 0;
        mySwitch.resetAvailable();
    }
    
    if (digitalRead(entPulsadorTX) == LOW) {  // cuando se complete el pulso recien salgo a programación
        pulsador1 = 2;
        timerTX();
    } 
    if (digitalRead(entPulsadorTX) == HIGH) { // cuando se complete el pulso recien salgo a programación
      if (tiempoCumplidoTX == 1){ 
        pulsador1 = 1;
        tiempoCumplidoTX = 0;
      }
      previoMillisTX = millis();
      if (pulsador1 == 2) {
        delay (500); // antirebote 
        digitalWrite(salLedTX,LOW);
        digitalWrite(salLedZ1,LOW);
        digitalWrite(salLedZ2,LOW);
        digitalWrite(salBuzzer, HIGH);
        delay (150);
        digitalWrite(salBuzzer, LOW);
        delay (150);
        digitalWrite(salBuzzer, HIGH);
        delay (150);
        digitalWrite(salBuzzer, LOW);
        pulsador1 = 0;
      }
    }  
  }
}

/////////////////////////////////// COMPARAR TX RECIBIDO CON LOS CODIGOS DE LA MEMORIA  ////////////////////////////////////////////////////////////////////////////////////////////////
void comparaTXmem(){
  int repetido ;
  
  if (punteroEEprom == 20){
    digitalWrite(salLedZ1,HIGH);
    digitalWrite(salLedZ2,HIGH);
    digitalWrite(salBuzzer, HIGH);
    delay (1000);
    digitalWrite(salLedZ1,LOW);
    digitalWrite(salLedZ2,LOW);
    digitalWrite(salBuzzer, LOW);
  } 
  if (punteroEEprom <= 19){
    repetido = 0;
    for (int i = 0; i < 20; i++) {
      EEPROM.get(i * 4, codigoAlmacenado);
      if (codigoAlmacenado == codigoRecibido){
        //Serial.println( "Codigo TX YA ALMACENADO: ");
        digitalWrite(salLedZ1,HIGH);
        digitalWrite(salLedZ2,HIGH);
        digitalWrite(salBuzzer, HIGH);
        delay (1000);
        digitalWrite(salLedZ1,LOW);
        digitalWrite(salLedZ2,LOW);
        digitalWrite(salBuzzer, LOW);
        repetido = 1;
      }
    }
    if (repetido == 0){
      EEPROM.put(punteroEEprom * 4, codigoRecibido);
      //Serial.print( "Almacenado en Puntero Numero: ");
      //Serial.println (punteroEEprom);
      osciladorLED();
      punteroEEprom = punteroEEprom + 1;
      EEPROM.put(1000, punteroEEprom);
    }
    /*for (int i = 0; i < 20; i++) {
      Serial.print (i*4);
      Serial.print ("   ");
      Serial.println (EEPROM.get(i*4, codigoAlmacenado));
    }
    */
  }
}

////////////////////////////////////////////////// ALMACENAR SENSORES DE LA ZONA1 /////////////////////////////////////////////////////////////////////////////////////////////////////////
void almacenarZ1(){
  while( pulsador2 != 0 ) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    wdt_reset(); // ENVIO CONSTANTEMENTE SEÑAL DE VIDA AL WATCHDOG siempre que el programa esté funcionando correctamente
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    destelloLedElegido();
    if (mySwitch.getReceivedValue()) {
      codigoRecibido = mySwitch.getReceivedValue();
      if ( codigoRecibido <= 10000)  codigoRecibido = 0;
      //Serial.println(" ");
      //Serial.print( "codigoRecibido: ");
      //Serial.println( codigoRecibido );

      if ( punteroEEpromZ1  > 100) punteroEEpromZ1 = 20;
      comparaZ1mem();
      mySwitch.resetAvailable();
    }
 
    if (digitalRead(entPulsadorZ1) == LOW) {  // cuando se complete el pulso recien salgo a programación
      pulsador2 = 2;
      timerZ1();
    } 
    if (digitalRead(entPulsadorZ1) == HIGH) { // cuando se complete el pulso recien salgo a programación
      if (tiempoCumplidoZ1 == 1){ 
        pulsador2 = 1;
        tiempoCumplidoZ1 = 0;
      }
      previoMillisZ1 = millis();
      if (pulsador2 == 2) {
        delay (500);
        digitalWrite(salLedTX,LOW);
        digitalWrite(salLedZ1,LOW);
        digitalWrite(salLedZ2,LOW);
        digitalWrite(salBuzzer, HIGH);
        delay (150);
        digitalWrite(salBuzzer, LOW);
        delay (150);
        digitalWrite(salBuzzer, HIGH);
        delay (150);
        digitalWrite(salBuzzer, LOW);
        pulsador2 = 0;
      }
    }  
  }
}

/////////////////////////////////// COMPARAR Z1 RECIBIDO CON LOS CODIGOS DE LA MEMORIA  ////////////////////////////////////////////////////////////////////////////////////////////////
void comparaZ1mem(){
  int repetido ;
  
  if (punteroEEpromZ1 == 40) {
    digitalWrite(salLedTX,HIGH);
    digitalWrite(salLedZ2,HIGH);
    digitalWrite(salBuzzer, HIGH);
    delay (1000);
    digitalWrite(salLedTX,LOW);
    digitalWrite(salLedZ2,LOW);
    digitalWrite(salBuzzer, LOW);
  } 
  if (punteroEEpromZ1 <= 39){
    repetido = 0;
    for (int i = 20; i < 40; i++) {
      EEPROM.get(i * 4, codigoAlmacenado);
      if (codigoAlmacenado == codigoRecibido){
        //Serial.println( "Codigo YA Z1 ALMACENADO: ");
        digitalWrite(salLedTX,HIGH);
        digitalWrite(salLedZ2,HIGH);
        digitalWrite(salBuzzer, HIGH);
        delay (1000);
        digitalWrite(salLedTX,LOW);
        digitalWrite(salLedZ2,LOW);
        digitalWrite(salBuzzer, LOW);
        repetido = 1;
      }
    }
    if (repetido == 0){
      EEPROM.put(punteroEEpromZ1 * 4, codigoRecibido);
      //Serial.print( "Almacenado en Puntero Numero Z1: ");
      //Serial.println (punteroEEpromZ1);
      osciladorLED();
      punteroEEpromZ1 = punteroEEpromZ1 + 1;
      EEPROM.put(1004, punteroEEpromZ1);
    }
    /*for (int i = 0; i < 256; i++) {
      Serial.print (i*4);
      Serial.print ("   ");
      Serial.println (EEPROM.get(i*4, codigoAlmacenado));
    }*/
  }
}

////////////////////////////////////////////////// ALMACENAR SENSORES DE LA ZONA2 ////////////////////////////////////////////////////////////////////////////////////////////////////
void almacenarZ2(){
  while( pulsador3 != 0 ) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    wdt_reset(); // ENVIO CONSTANTEMENTE SEÑAL DE VIDA AL WATCHDOG siempre que el programa esté funcionando correctamente
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    destelloLedElegido();
    if (mySwitch.getReceivedValue()) {
      codigoRecibido = mySwitch.getReceivedValue();
      if ( codigoRecibido <= 10000)  codigoRecibido = 0;
      //Serial.println(" ");
      //Serial.print( "codigoRecibido: ");
      //Serial.println( codigoRecibido );

      if ( punteroEEpromZ2  > 100) punteroEEpromZ2 = 40;
      comparaZ2mem();
      mySwitch.resetAvailable();
    }
 
    if (digitalRead(entPulsadorZ2) == LOW) {  // cuando se complete el pulso recien salgo a programación
      pulsador3 = 2;
      timerZ2();
    } 
    if (digitalRead(entPulsadorZ2) == HIGH) { // cuando se complete el pulso recien salgo a programación
      if (tiempoCumplidoZ2 == 1){ 
        pulsador3 = 1;
        tiempoCumplidoZ2 = 0;
      }
      previoMillisZ2 = millis();
      if (pulsador3 == 2) {
        delay (500);
        digitalWrite(salLedTX,LOW);
        digitalWrite(salLedZ1,LOW);
        digitalWrite(salLedZ2,LOW);
        digitalWrite(salBuzzer, HIGH);
        delay (150);
        digitalWrite(salBuzzer, LOW);
        delay (150);
        digitalWrite(salBuzzer, HIGH);
        delay (150);
        digitalWrite(salBuzzer, LOW);
        pulsador3 = 0;
      }
    }  
  }
}

/////////////////////////////////// COMPARAR Z2 RECIBIDO CON LOS CODIGOS DE LA MEMORIA  ////////////////////////////////////////////////////////////////////////////////////////////////
void comparaZ2mem(){
  int repetido ;
  
  if (punteroEEpromZ2 == 60) {
    digitalWrite(salLedZ1,HIGH);
    digitalWrite(salLedTX,HIGH);
    digitalWrite(salBuzzer, HIGH);
    delay (1000);
    digitalWrite(salLedZ1,LOW);
    digitalWrite(salLedTX,LOW);
    digitalWrite(salBuzzer, LOW);
  } 
  if (punteroEEpromZ2 <= 59){
    repetido = 0;
    for (int i = 40; i < 60; i++) {
      EEPROM.get(i * 4, codigoAlmacenado);
      if (codigoAlmacenado == codigoRecibido){
        //Serial.println( "Codigo YA Z2 ALMACENADO: ");
        digitalWrite(salLedZ1,HIGH);
        digitalWrite(salLedTX,HIGH);
        digitalWrite(salBuzzer, HIGH);
        delay (1000);
        digitalWrite(salLedZ1,LOW);
        digitalWrite(salLedTX,LOW);
        digitalWrite(salBuzzer, LOW);
        repetido = 1;
      }
    }
    if (repetido == 0){
      EEPROM.put(punteroEEpromZ2 * 4, codigoRecibido);
      //Serial.print( "Almacenado en Puntero Numero Z2: ");
      //Serial.println (punteroEEpromZ2);
      osciladorLED();
      punteroEEpromZ2 = punteroEEpromZ2 + 1;
      EEPROM.put(1008, punteroEEpromZ2);
    }
    /*for (int i = 0; i < 256; i++) {
      Serial.print (i*4);
      Serial.print ("   ");
      Serial.println (EEPROM.get(i*4, codigoAlmacenado));
    }*/
  }
}


////////////////////////////////////////////////// OSCILARDOR DE LED ///// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
void osciladorLED(){
            digitalWrite(LedElegido,HIGH);
            digitalWrite(salBuzzer,HIGH);
            delay (50);
            digitalWrite(LedElegido,LOW);
            digitalWrite(salBuzzer,LOW);
            delay (50);
            digitalWrite(LedElegido,HIGH);
            digitalWrite(salBuzzer,HIGH);
            delay (50);
            digitalWrite(LedElegido,LOW);
            digitalWrite(salBuzzer,LOW);
            delay (50);
            digitalWrite(LedElegido,HIGH);
            digitalWrite(salBuzzer,HIGH);
            delay (50);
            digitalWrite(LedElegido,LOW);
            digitalWrite(salBuzzer,LOW);
            delay (50);
            digitalWrite(LedElegido,HIGH);
            digitalWrite(salBuzzer,HIGH);
            delay (50);
            digitalWrite(LedElegido,LOW);
            digitalWrite(salBuzzer,LOW);
            delay (50);
            digitalWrite(LedElegido,HIGH);
            delay(3500);
}

////////////////////////////////////////////////// OSCILARDOR DE TRES LED ///// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
void osciladorTresLED(){
            digitalWrite(salLedTX,HIGH); 
            digitalWrite(salLedZ1,HIGH);
            digitalWrite(salLedZ2,HIGH);
            digitalWrite(salBuzzer,HIGH);
            delay (50);
            digitalWrite(salLedTX,LOW); 
            digitalWrite(salLedZ1,LOW);
            digitalWrite(salLedZ2,LOW);
            digitalWrite(salBuzzer,LOW);
            delay (50);
            digitalWrite(salLedTX,HIGH); 
            digitalWrite(salLedZ1,HIGH);
            digitalWrite(salLedZ2,HIGH);
            digitalWrite(salBuzzer,HIGH);
            delay (50);
            digitalWrite(salLedTX,LOW); 
            digitalWrite(salLedZ1,LOW);
            digitalWrite(salLedZ2,LOW);
            digitalWrite(salBuzzer,LOW);
            delay (50);
            digitalWrite(salLedTX,HIGH); 
            digitalWrite(salLedZ1,HIGH);
            digitalWrite(salLedZ2,HIGH);
            digitalWrite(salBuzzer,HIGH);
            delay (50);
            digitalWrite(salLedTX,LOW); 
            digitalWrite(salLedZ1,LOW);
            digitalWrite(salLedZ2,LOW);
            digitalWrite(salBuzzer,LOW);
            delay (50);
            digitalWrite(salLedTX,HIGH); 
            digitalWrite(salLedZ1,HIGH);
            digitalWrite(salLedZ2,HIGH);
            digitalWrite(salBuzzer,HIGH);
            delay (50);
            digitalWrite(salLedTX,LOW); 
            digitalWrite(salLedZ1,LOW);
            digitalWrite(salLedZ2,LOW);
            digitalWrite(salBuzzer,LOW);
            delay (50);
            if ((pulsador1 == 2) || (pulsador2 == 2) || (pulsador3 == 2)) digitalWrite(LedElegido,HIGH);
}

////////////////////////////////////////////////// TIMERS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void destelloLedElegido(){
  if (millis() - previoMillisLedElegido >= (600)){ // parpadeo del led de strobo cuando se activa o se dispara la Alarma 
    previoMillisLedElegido = millis();
    digitalWrite(LedElegido,LOW);
    if (millis() - previoMillisLedElegido2 >= 1200){
      previoMillisLedElegido2 = millis();
      digitalWrite(LedElegido,HIGH);
    }
  }
}

void timerTX(){
  if (disparadaAlarma == 0) { 
    if (millis() - previoMillisTX >= INTERVALO) {
      previoMillisTX = millis();
      //Serial.println( "TIEMPO CUMPLIDO .... BORRAR REGISTROS" );
      tiempoCumplidoTX = 1;
      osciladorTresLED();
      if (pulsador1 == 2) borrarTX();
    }
  }
}

void timerZ1(){
  if (millis() - previoMillisZ1 >= INTERVALO)
   {
    previoMillisZ1 = millis();
    //Serial.println( "TIEMPO CUMPLIDO .... BORRAR REGISTROS" );
    tiempoCumplidoZ1 = 1;
    osciladorTresLED();
    if (pulsador2 == 2) borrarZ1();
   }
}

void timerZ2(){
  if (millis() - previoMillisZ2 >= INTERVALO)
   {
    previoMillisZ2 = millis();
    //Serial.println( "TIEMPO CUMPLIDO .... BORRAR REGISTROS" );
    tiempoCumplidoZ2 = 1;
    osciladorTresLED();
    if (pulsador3 == 2) borrarZ2();
   }
}

/////////////////////////////////////////////////////////////////
void borrarTX(){
   dato = 101;
   for (int i = 0; i < 20; i++) { 
      EEPROM.put(i*4, dato);
   }
   punteroEEprom = 0; 
   EEPROM.put(1000, punteroEEprom);
 
   //Serial.println (" ");
   //Serial.println (" ");
   //Serial.println( "TODOS LOS REGISTROS DE LOS CONTROLES  TX REMOTOS HAN SIDO BORRADOS" );
   /*
   for (int i = 0; i < 256; i++) {
      Serial.print (i*4);
      Serial.print ("   ");
      Serial.println (EEPROM.get(i*4, dato));
   }
   */
}

/////////////////////////////////////////////////////////////////
void borrarZ1(){
   dato = 101;
   for (int i = 20; i < 40; i++) { 
      EEPROM.put(i*4, dato);
   }
   punteroEEpromZ1 = 20; 
   EEPROM.put(1004, punteroEEpromZ1);
 
   //Serial.println (" ");
   //Serial.println (" ");
   //Serial.println( "TODOS LOS REGISTROS DE LOS SENSORES Z1 REMOTOS HAN SIDO BORRADOS" );
   /*for (int i = 0; i < 256; i++) {
      Serial.print (i*4);
      Serial.print ("   ");
      Serial.println (EEPROM.get(i*4, dato));
   }*/
}

/////////////////////////////////////////////////////////////////
void borrarZ2(){
   dato = 101;
   for (int i = 40; i < 60; i++) { 
      EEPROM.put(i*4, dato);
   }
   punteroEEpromZ2 = 40; 
   EEPROM.put(1008, punteroEEpromZ2);
 
   //Serial.println (" ");
   //Serial.println (" ");
   //Serial.println( "TODOS LOS REGISTROS DE LOS SENSORES Z2 REMOTOS HAN SIDO BORRADOS" );
   /*for (int i = 0; i < 256; i++) {
      Serial.print (i*4);
      Serial.print ("   ");
      Serial.println (EEPROM.get(i*4, dato));
   }*/
}

///////////////////////////////////////////////////////////////// RUTINAS EN PAPELERA //////////////////////////////////////////////////////////////////
/*
void osciladorLEDz1(){
  if (millis() - previoMillis >= INTERVALO)
   {
    previoMillis = millis();
    digitalWrite(salLedZ1,HIGH);
    if (millis() - previoMillis2 >= INTERVALO2)
     {
      previoMillis2 = millis();
      digitalWrite(salLedZ1,LOW);
     }
   }
}
*/
///////////////////////////////////////////////////////////////////
