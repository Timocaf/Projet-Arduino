//bluetooth
#include <SoftwareSerial.h> 
#define RX 34
#define TX 32
SoftwareSerial BlueT(RX,TX);
char Data;

//servo
#include <Servo.h>
int angleInitial=40;
int angleFinal=120;
Servo servo10;
Servo servo20;
Servo servo50;
Servo servo100;
Servo servo200;
int servo10Pos=angleInitial;
int servo20Pos=angleInitial;
int servo50Pos=angleInitial;
int servo100Pos=angleInitial;
int servo200Pos=angleInitial;

//capteurs
int cap10=21;
int cap20=19;
int cap50=17;
int cap100=15;
int cap200=13;

// Moteur central 
int capteurMoteur = 23;
int ENB=2;
int IN3=3;
int IN4=4;

//LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//boutons
const int bouton10=9;
const int bouton20=10;
const int bouton50=11;
const int bouton100=12;
const int bouton200=13;

//
int val[]={200,100,50,20,10};
int nb[]={10,0,0,0,10};    //nombre de pieces presentes dans la machine
int aRendre[]={0,0,0,0,0};                   //nombre de pieces à rendre
int argent=0;                                 //argent que demande le client





void setup() {
  Serial.begin(9600);  //sert pour les tests
  BlueT.begin(9600);

//les capteurs
  pinMode(cap10,INPUT);
  pinMode(cap20,INPUT);
  pinMode(cap50,INPUT);
  pinMode(cap100,INPUT);
  pinMode(cap200,INPUT);

//les servos
  servo10.attach(22);
  servo20.attach(24);
  servo50.attach(26);
  servo100.attach(28);
  servo200.attach(30);
  servo10.write(servo10Pos);
  servo20.write(servo20Pos);
  servo50.write(servo50Pos);
  servo100.write(servo100Pos);
  servo200.write(servo200Pos);
  servo10.detach();
  servo20.detach();
  servo50.detach();
  servo100.detach();
  servo200.detach();

// MOTEUR central --
  pinMode(capteurMoteur,INPUT);
  pinMode(ENB,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);

  //LCD
  lcd.init();
  lcd.backlight();

  //boutons
  pinMode(bouton10,INPUT);
  pinMode(bouton20,INPUT);
  pinMode(bouton50,INPUT);
  pinMode(bouton100,INPUT);
  pinMode(bouton200,INPUT);

  delay(1000); 
}





int tot(int list[]){            //fonction qui permet de calculer l'argent total a partir d'une liste
  int nb=0;
  for (int i=0;i<5;i++){nb+=list[i]*val[i];}
  return nb;
}

void piecesARendre(int euro){
  if (euro<=tot(nb)){
    for(int i=0;i<5;i++){
      while (nb[i]>0 && tot(aRendre)+val[i]<=euro){
        aRendre[i]+=1;
        nb[i]-=1;}}}
}

int distrib(Servo servo,int pos, int nbPiece){
  for (int j=0; j<nbPiece; j++){

    if (pos==angleInitial){
      for (int i = angleInitial; i >= angleFinal; i++) {
        servo.write(i);
        delay(5);}
      pos=angleFinal;}
    else{

      for (int i = angleFinal; i >= angleInitial; i--) {
        servo.write(i);
        delay(5);}
      pos=angleInitial;}
    delay(500);}

  return pos;
}






void loop(){

//LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Piece de 10c : " + String(nb[0]));
  lcd.setCursor(0, 1);
  lcd.print("Piece de 20c : " + String(nb[1]));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Piece de 50c : " + String(nb[2]));
  lcd.setCursor(0, 1);
  lcd.print("Piece de 1€ : " + String(nb[3]));
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Piece de 2€ : " + String(nb[4]));

//le disque tourne
  if(digitalRead(capteurMoteur)==LOW){
      analogWrite(ENB,1000);}
      //On continue de tourner pour vider tous les emplacements du premier plateau, cette valeur devra être calculer plus précisément pour éviter que le capteur se bloque dans une boucle
    else{ //Il n'y a plus de pièces à trier
      analogWrite(ENB,0);
    }



//pour compter les pieces
  if(digitalRead(cap10)==LOW){nb[0]+=1;delay(100);}
  if(digitalRead(cap20)==LOW){nb[1]+=1;delay(100);}
  if(digitalRead(cap50)==LOW){nb[2]+=1;delay(100);}
  if(digitalRead(cap100)==LOW){nb[3]+=1;delay(100);}
  if(digitalRead(cap200)==LOW){nb[4]+=1;delay(100);}



//distribution boutons
if(digitalRead(bouton10)==HIGH && nb[4]>0){servo10.attach(22); servo10Pos=distrib(servo10,servo10Pos,1); servo10.detach(); nb[4]-=1;}
if(digitalRead(bouton20)==HIGH && nb[3]>0){servo20.attach(24); servo20Pos=distrib(servo20,servo20Pos,1); servo20.detach(); nb[3]-=1;}
if(digitalRead(bouton50)==HIGH && nb[2]>0){servo50.attach(26); servo50Pos=distrib(servo50,servo50Pos,1); servo50.detach(); nb[2]-=1;}
if(digitalRead(bouton100)==HIGH && nb[1]>0){servo100.attach(28); servo100Pos=distrib(servo100,servo100Pos,1); servo100.detach(); nb[1]-=1;}
if(digitalRead(bouton200)==HIGH && nb[0]>0){servo200.attach(30); servo200Pos=distrib(servo200,servo200Pos,1); servo200.detach(); nb[0]-=1;}



//distribution bluetooth
  if (BlueT.available()){
    Data=BlueT.read();
    if (Data=='R'){BlueT.println("*BVous avez " + String(float(tot(nb))/100) +" euros");}
    if (Data=='A') {
      argent=BlueT.parseFloat()*100;
      piecesARendre(argent);              // ici mettre l'argent que demande le client

      if (tot(aRendre)==0){BlueT.println("*BCela est impossible");}
      else {
        BlueT.println("*BVoici "+String(float(tot(aRendre))/100)+" euros");

        //on distribue les pieces
        servo10.attach(22);
        servo20.attach(24);
        servo50.attach(26);
        servo100.attach(28);
        servo200.attach(30);
        servo10Pos=distrib(servo10,servo10Pos,aRendre[4]);
        servo20Pos=distrib(servo20,servo20Pos,aRendre[3]);
        servo50Pos=distrib(servo50,servo50Pos,aRendre[2]);
        servo100Pos=distrib(servo100,servo100Pos,aRendre[1]);
        servo200Pos=distrib(servo200,servo200Pos,aRendre[0]);
        servo10.detach();
        servo20.detach();
        servo50.detach();
        servo100.detach();
        servo200.detach();

        /*serie de print pour les tests :
        Serial.print(argent);
        Serial.print("  ");
        Serial.print(aRendre[0]);
        Serial.print("  ");
        Serial.print(aRendre[1]);
        Serial.print("  ");
        Serial.print(aRendre[2]);
        Serial.print("  ");
        Serial.print(aRendre[3]);
        Serial.print("  ");
        Serial.print(aRendre[4]);
        Serial.print("        ");
        Serial.println(tot(aRendre));
        */

        //on reinitialise tout
        argent=0;
        for(int i=0;i<5;i++){aRendre[i]=0;}
      }
    }
  }
  else {BlueT.println("*B ");}

  delay(1000);
}
