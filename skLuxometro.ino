#include <SparkFunTSL2561.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>
#include <U8glib.h>

//Definicoes do display Oled
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

// Create an SFE_TSL2561 object, here called "light":

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

// Global variables:
boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
//double lux = 0.0;  // Resulting lux value
String str;
int tamanho;

uint16_t r, g, b, c, colorTemp, lux;
const byte BTN_MODO  = 2;
uint8_t modo = 0;

//***************************************************************************************************
void draw()
{
  //Comandos graficos para o display devem ser colocados aqui
  u8g.drawRFrame(0, 16, 128, 48, 4);
  u8g.drawRFrame(0, 0, 128, 16, 4);
  u8g.setFont(u8g_font_8x13B);
  u8g.setColorIndex(0);
  u8g.setColorIndex(1);

  switch (modo) {


    case 0:
           u8g.drawStr( 30, 13, "Luxometro");
           u8g.setFont(u8g_font_fur25);

           str = String(lux, 1);
           tamanho = str.length();
           u8g.setPrintPos(10, 53);
           u8g.print(lux);
           u8g.setFont(u8g_font_8x13B);
           u8g.print(" lx");
           break;
    case 1:
           u8g.drawStr( 30, 13, "Temp. Color");
           u8g.setFont(u8g_font_fur25);

           str = String(colorTemp, 1);
           tamanho = str.length();
           u8g.setPrintPos(10, 53);
           u8g.print(colorTemp);
           u8g.setFont(u8g_font_8x13B);
           u8g.print(" K");
    
           break;
    case 2:
           u8g.drawStr( 40, 13, "Color");
           u8g.drawStr(10,35,"  R    G    B");
           
           str = String(r, 1);
           tamanho = str.length();
           u8g.setPrintPos(10, 53);
          
           u8g.print(r);
           u8g.print(" ");
           u8g.print(g);
           u8g.print(" ");
           u8g.print(b);
           
          break;

    default: break;


  }

}


//***************************************************************************************************
void setup()

{

 // pinMode(BTN_MODO, INPUT);

 pinMode(BTN_MODO,INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(BTN_MODO),chgModo, CHANGE);

  Serial.begin(115200);
  Serial.println("iniciar TCS34725");

  if (!tcs.begin())
  {
    Serial.println("Error al iniciar TCS34725");
    while (1) delay(1000);
  }
 

  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }

  ms=300;

}

//*****************************************************************************************
void chgModo (){


 static unsigned long last_interrupt_time = 0;
 unsigned long interrupt_time = millis();
 // If interrupts come faster than 200ms, assume it's a bounce and ignore
 if (interrupt_time - last_interrupt_time > 200)
 {
   if (++modo >= 3 )
        modo = 0;
   Serial.println("Modo: "+ modo);
 }
 last_interrupt_time = interrupt_time;

   
}


//************************************************************************************+***
void loop()

{

  /*if (!digitalRead(BTN_MODO)) {

   
      if (++modo >= 3 )
        modo = 0;
        
      delay(50);
   
  }*/

  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);

 /* Serial.print("Temperatura color: "); Serial.print(colorTemp, DEC); Serial.println(" K");
  Serial.print("Lux : "); Serial.println(lux, DEC);
  Serial.print("Rojo: "); Serial.println(r, DEC);
  Serial.print("Verde: "); Serial.println(g, DEC);
  Serial.print("Azul: "); Serial.println(b, DEC);
  Serial.print("Clear: "); Serial.println(c, DEC);
  Serial.println(" ");*/


  u8g.firstPage();
  do
  {
    draw();
  }
  while ( u8g.nextPage() );
  
  delay(400);

}


//*********************************************************************************************************
void printError(byte error)

// If there's an I2C error, this function will

// print out an explanation.

{
  Serial.print("I2C error: ");
  Serial.print(error, DEC);
  Serial.print(", ");

  switch (error)
  {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }

}
