#define motor_pin 9 // Pino 9 utilizado para controle do motor
#define interrupt_pin 2 // Pino 2 utilizado para receptor do laser
#include <Servo.h> // Adiciona biblioteca Servo.h
Servo ServoMotor; // Declara objeto ServoMotor da classe Servo

// ----- DECLARACAO DE VARIAVEIS ----- //
volatile int power = 20;
volatile int pulse_counter = 0;
float speed_rpm = 0;
float cell_voltage = 0;
bool automatic = false;
float fs = 100;
float T = 1/fs;
long current_time_micros = 0;
long delta_time = 0;
int k;
int initial_pwm = 23;
int max_pwm = 75;
// ----- ----- ----- ----- ----- ----- //

// ------- PARAMETROS DO FILTRO DIGITAL ----/
float b[] = {0.00000080423564219334, 0.00000402117821096670, 0.00000804235642193341, 0.00000804235642193341, 0.00000402117821096670, 0.00000080423564219334};
float a[] = {1.000000000000000, -4.593421399807689, 8.455115223510134, -7.794918318044449, 3.598902768053915, -0.665652538171362};
const int order_filter = 5;
volatile float y_k[order_filter+1];
volatile float x_k[order_filter+1];
float input_signal = 0;
float filtered_signal = 0;
int number_samples = 100;
// ----- ----- ----- ----- ----- ----- //


void setup()
{
  Serial.begin(115200); // Baud rate da comunicaco serial
  pinMode(interrupt_pin, INPUT_PULLUP); // Define pino 2 como entrada digital com resistor de pull up por hardware (evita ruido no receptor do laser)
  attachInterrupt(digitalPinToInterrupt(interrupt_pin), IncreaseCounter, RISING); // Ativa interrupcao no pino 2 a cada borda de subida do sinal do receptor
  ServoMotor.attach(motor_pin); // Inicializa o objeto ServoMotor ligado ao pino 9
  
  Serial.println("Aguardando 5 segundos....");
  delay(5000); // Aguardando setup do objeto ServoMotor
  
  Serial.println("Inicio do programa!");
  ServoMotor.write(power);
  current_time_micros = micros();
}

void loop()
{
  
  if(automatic == true)
  {
    Serial.println("// ----- MODO AQUISICAO AUTOMATICA DE DADOS ----- //");
    
    for(power = initial_pwm-1; power <= max_pwm; power++)
    {
      // -------------- ACIONAMENTO DO MOTOR --------------//
      ServoMotor.write(power); // Envia comando ao motor
      delay(2000);
      // ------------------------------------------------//
      
      // -------------- VELOCIDADE DE ROTACAO -------------//
      for(int i = 0; i <= 5; i++)
      {
        speed_rpm += GetSpeedRPM();
      }
      speed_rpm = (speed_rpm/5);
      // ------------------------------------------------//
      
      // ---------- TENSAO DA CELULA DE CARGA -------------//
      current_time_micros = micros();
      k = 0;
      while(k < 200)
      {
        delta_time = micros() - current_time_micros;
        if(delta_time >= T*1000000)
        {
          current_time_micros = micros();
          input_signal = analogRead(A0)*0.004887585;
          /*
          if(power == 129) {
            number_samples = 150;
            filtered_signal = MovingAverageFilter(input_signal);
          }
          
          else if(power == 130) {
            number_samples = 150;
            filtered_signal = MovingAverageFilter(input_signal);
          }
          
          else {
            filtered_signal = FilterSignal(input_signal);
          }
          
          */
          if(power == 50) {
            number_samples = 120;
            filtered_signal = MovingAverageFilter(input_signal);
          }
          
          else if(power == 51) {
            number_samples = 120;
            filtered_signal = MovingAverageFilter(input_signal);
          }
          
          else if(power == 74) {
            number_samples = 130;
            filtered_signal = MovingAverageFilter(input_signal);
          }
          
          else if(power == 75) {
            number_samples = 110;
            filtered_signal = MovingAverageFilter(input_signal);
          }
          
          else {
            filtered_signal = FilterSignal(input_signal);
          }
          
          k++;
        }
      }
      // ------------------------------------------------//
      
      // ------- EXIBIR VALORES CALCULADOS ---------------//
      Serial.print(power);
      Serial.print(","); // Delimitador
      
      Serial.print(speed_rpm);
      Serial.print(","); // Delimitador
      
      Serial.println(filtered_signal);
      // ------------------------------------------------//
      
    }
    
    automatic = false;
    power = 0;
    
  }
  
  else
  {
    if(Serial.available() > 0)
    {
      power = Serial.parseInt(); // Recebe valor inserido pelo usuario na Serial (inteiro variando entre 50-130)
      
      if(power == 15) // Este valor de comando aciona a automacao de aquisicao de dados!
      {
        automatic = true;
      }
      
      else
      {
        ServoMotor.write(power); // Envia comando ao motor
        
        // -------------- VELOCIDADE DE ROTACAO -------------//
        for(int i = 0; i <= 5; i++)
        {
          speed_rpm += GetSpeedRPM();
        }
        
        speed_rpm = (speed_rpm/5);
        // ------------------------------------------------//
        
        current_time_micros = micros();
        k = 0;
        while(k < 200)
        {
          delta_time = micros() - current_time_micros;
          if(delta_time >= T*1000000)
          {
            current_time_micros = micros();
            input_signal = analogRead(A0)*0.004887585;
            // filtered_signal = MovingAverageFilter(input_signal);
            /*
            if(power == 129) {
              number_samples = 150;
              filtered_signal = MovingAverageFilter(input_signal);
            }
            
            else if(power == 130) {
              number_samples = 150;
              filtered_signal = MovingAverageFilter(input_signal);
            }
            
            else {
              filtered_signal = FilterSignal(input_signal);
            }
            
            */
            if(power == 50) {
              number_samples = 120;
              filtered_signal = MovingAverageFilter(input_signal);
            }
            
            else if(power == 51) {
              number_samples = 120;
              filtered_signal = MovingAverageFilter(input_signal);
            }
            
            else if(power == 74) {
              number_samples = 130;
              filtered_signal = MovingAverageFilter(input_signal);
            }
            
            else if(power == 75) {
              number_samples = 110;
              filtered_signal = MovingAverageFilter(input_signal);
            }
            
            else {
              filtered_signal = FilterSignal(input_signal);
            }
            
            
            Serial.print(",");
            Serial.print(input_signal);
            Serial.print(",");
            Serial.println(filtered_signal);
            k++;
          }
        }
        
        ClearSerial(); // Limpar o canal do Serial
      }
      
    }
  }
  ServoMotor.write(power);
}

void ClearSerial() {
  char any;
  while(Serial.available() > 0)
  {
    any = Serial.read();
  }
}

void IncreaseCounter()
{
  pulse_counter++;
}

float GetSpeedRPM()
{
  float current_speed = 0;
  pulse_counter = 0;
  delay(1000);
  current_speed = pulse_counter*15;
  return(current_speed);
}

float GetLoadCellVoltage()
{
  float cell = 0;
  int samples = 100;
  
  for(int i = 0; i < samples; i++)
  {
    cell += analogRead(A0)*0.004888;
  }
  
  cell = (cell/samples);
  return(cell);
}

float FilterSignal(float input_signal)
{
  float y;
  
  x_k[0] = input_signal;
  y_k[0] = 0;
  
  for(int j=0; j <= order_filter; j++)
  {
    y_k[0] = y_k[0] + x_k[j]*b[j];
  }
  for(int j=0; j < order_filter; j++)
  {
    y_k[0] = y_k[0] - y_k[j+1]*a[j+1];
  }
  y_k[0] = y_k[0]/a[0];
  y = y_k[0];
  
  for(int j = order_filter; j >= 1; j--)
  {
    y_k[j] = y_k[j-1];
    x_k[j] = x_k[j-1];
  }
  return y;
}


float MovingAverageFilter(float input_signal) {
  
  static float average = 0.0;
  static int index = 1;
  
  if(index == 0 || index == number_samples) {
    index = 1;
    average = 0.0;
  }
  
  average = average + (input_signal - average)/index++;
  
  return average;
  
}