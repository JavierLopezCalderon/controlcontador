//ARDUINO
//v1.4
//MOTOR A PASOS Y DRIVER CONTROLADOR
//BLOQUEOS
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define dirPin        2
#define stepPin       3
#define Botonreset    4 
#define fin_carrera1  5  //IZQUIERDA
#define fin_carrera2  6  //DERECHA
#define Botonstart    7
#define BotonCiclos   8
#define BotonVel      9
#define Relevador     10
#define fin_carrera3  11 //BLOQUEO
#define entrada_relevador_delantera A0
#define entrada_relevador_trasera   A1

class accesos {
  public:
    int count, limit, valor;
    byte byte1, byte2;
};

int pasos = 1200, timedelay = 2;
int limiteContador = 1;
int fin, dir_estado, estado_reset, cambio_giro, estado_inicio;
int countinicio;
int pausecontador;
int SegundosBloqueado = 20; 
int BloqueosTotales;
int CiclosBloqueo = 100; //CADA CUANTOS CICLOS SE BLOQUEARA
int varauxiliarbloqueo = 0;
unsigned long previousEnableSumaRele, previousRefrecarLCD;
const int intervalEnableSumaRele = 500;
const int intervalRefrecarLCD = 600000;//REFRESCA PANTALLA CADA 10MIN
int EnableSumaRele[2];
accesos accion[2], releValor[2];
int Ciclos[14] = {1,2,5,10,15,20,25,30,35,40,45,50,55,60};
LiquidCrystal_I2C lcd(0x27, 20, 4);  //

void setup() {

  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(Relevador, OUTPUT);
  pinMode(Botonreset, INPUT_PULLUP);
  pinMode(fin_carrera1, INPUT_PULLUP);
  pinMode(fin_carrera2, INPUT_PULLUP);
  pinMode(Botonstart, INPUT_PULLUP);
  pinMode(BotonCiclos, INPUT_PULLUP);
  pinMode(BotonVel, INPUT_PULLUP);
  pinMode(fin_carrera3, INPUT_PULLUP);
  pinMode(entrada_relevador_delantera, INPUT_PULLUP);
  pinMode(entrada_relevador_trasera, INPUT_PULLUP);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5,1);
  lcd.print("HOLA");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ESTAMOS");
  lcd.setCursor(0,1);
  lcd.print("PREPARANDO");
  lcd.setCursor(0,2);
  lcd.print("TODO PARA");
  lcd.setCursor(0,3);
  lcd.print("EMPEZAR");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("ESPERA UN");
  lcd.setCursor(0,2);
  lcd.print("MOMENTO...");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("INICIANDO SISTEMA...");
  lcd.setCursor(6,2);
  lcd.print(":(");
  delay(1000);
  lcd.clear();
  
  accion[0].byte1 = EEPROM.read(0);
  accion[0].byte2 = EEPROM.read(1);
  accion[1].byte1 = EEPROM.read(2);
  accion[1].byte2 = EEPROM.read(3);
  accion[0].count = accion[0].byte1<<8|accion[0].byte2; //Subidas conteo
  accion[1].count = accion[1].byte1<<8|accion[1].byte2; //Bajadas conteo
  dir_estado = EEPROM.read(4);
  limiteContador = EEPROM.read(5);
  timedelay = EEPROM.read(6);
  estado_reset = EEPROM.read(7);
  estado_inicio = EEPROM.read(8);
  releValor[0].byte1 = EEPROM.read(9);
  releValor[0].byte2 = EEPROM.read(10);
  releValor[1].byte1 = EEPROM.read(11);
  releValor[1].byte2 = EEPROM.read(12);
  releValor[0].count = releValor[0].byte1<<8|releValor[0].byte2; //Subidas accionamientos relevador
  releValor[1].count = releValor[1].byte1<<8|releValor[1].byte2; //Bajadas accionamientos relevador
  countinicio = EEPROM.read(13);
  BloqueosTotales = accion[1].count/CiclosBloqueo;
  lcd_contador();
}

void loop() {
  // lcd_contador();
  // EEPROM.update(6, 1);
  digitalWrite(Relevador, LOW);
  while(( accion[1].count < (Ciclos[limiteContador]*100)) && estado_inicio == 1 && estado_reset == 0){ 
    digitalWrite(Relevador, HIGH);
    if(dir_estado == 0){
      digitalWrite(dirPin, LOW);   
    }else{
      digitalWrite(dirPin, HIGH);
    }
    button_timedelay();
    ciclopaso();
    /*if(millis() - previousRefrecarLCD > intervalRefrecarLCD){
       previousRefrecarLCD = millis();
       lcd.clear();
       lcd.init();
       lcd.backlight();
    }*/
    
    
    //dir_estado = !dir_estado;
    lcd_contador();
  }
  estado_inicio =0;
  button_start();
  button_ciclos();
  button_reset();
  button_timedelay();
}

void ciclopaso(){
  while(cambio_giro == 0){
    button_timedelay();
    sensor_bloqueo();
    digitalWrite(stepPin, HIGH);
    delay(timedelay);
    digitalWrite(stepPin, LOW);
    //delay(1);
    delay(timedelay);
   if(digitalRead(fin_carrera1)==LOW && estado_reset == 0 && dir_estado == 1){
     Serial.println(digitalRead(fin_carrera1));
      if(countinicio == 1){
        accion[dir_estado].count++;
        SaveConteo();
      }      
      dir_estado = 0;
      varauxiliarbloqueo =0;
      cambio_giro = 1;  
      delay(200);  
    }else if(digitalRead(fin_carrera2)==LOW&& estado_reset == 0 && dir_estado == 0){
      accion[dir_estado].count++;
      SaveConteo();
      countinicio = 1;
      EEPROM.update(13, countinicio);
      dir_estado = 1;
      cambio_giro = 1;
      delay(200);
    }else if(digitalRead(fin_carrera1)==LOW&& estado_reset == 1 ){
      dir_estado = 0;
      cambio_giro = 1;
      digitalWrite(stepPin, LOW);
    }
    button_reset();
    EEPROM.update(4, dir_estado);
    Conteo_relevadores();
    //lcd_contador();
  }
  if(estado_reset == 0){
    cambio_giro = 0;
  }
}

void lcd_contador(){
  lcd.setCursor(0, 0);
  lcd.print("Sub: ");
  lcd.print(accion[0].count);
  lcd.setCursor(10, 0);
  lcd.print("Baj: ");
  lcd.print(accion[1].count);
  lcd.setCursor(0, 1);
  lcd.print("RSub:");
  lcd.print(releValor[0].count);
  lcd.setCursor(10, 1);
  lcd.print("RBaj:");
  lcd.print(releValor[1].count);  
  lcd.setCursor(0, 2);
  lcd.print("Ciclos:");
  lcd.print(Ciclos[limiteContador]*100);
  lcd.setCursor(0, 3);
  lcd.print("Vel:");
  lcd.print(timedelay);
  lcd.setCursor(10, 3);
  lcd.print("Bloq:");
  lcd.print(BloqueosTotales);
}

void SaveConteo(){

    accion[dir_estado].byte1 = accion[dir_estado].count >> 8;
    accion[dir_estado].byte2 = accion[dir_estado].count & 255;
    accion[dir_estado].valor = accion[dir_estado].byte1 << 8|accion[dir_estado].byte2;
    
    if(dir_estado == 0){
      EEPROM.update(0, accion[dir_estado].byte1);
      EEPROM.update(1, accion[dir_estado].byte2);  
    }else{
      EEPROM.update(2, accion[dir_estado].byte1);
      EEPROM.update(3, accion[dir_estado].byte2);
    }
    
}
