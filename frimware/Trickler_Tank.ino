
#define DRIVER_STEP_TIME 1
#include <GyverStepper.h>

GStepper<STEPPER2WIRE> stepper(3200, 11, 10, 12); // steps, step, dir, en

#include <AnalogKey.h>
#include <GyverButton.h>

GButton btn(A5);                  // Определяем пин кнопки

float max_speed = 20000;          // максимальная скорость вращения триклера в шаги/секунду  !!! max_speed и medium_speed должны быть в одинаковых единицах измерения !!!
float medium_speed = 3200;        // скорость успокаивания вращения триклера в шаги/секунду  !!! max_speed и medium_speed должны быть в одинаковых единицах измерения !!!
float slow_speed = 20;            // скорость финального досыпания в градусы/секунду

unsigned long delay_timer = 2500; // задержка на успокоение весов
unsigned long slow_delay = 120000;// изначальное время задержки до замедления быстрой фазы

unsigned long last_press = 500;   // таймер для кнопки
unsigned long load_timer = 0;     // таймер от начала засыпки
unsigned long medium_timer = 0;   // таймер фазы торможения засыпки
unsigned long overload_timer = 0; // таймер медленной фазы

unsigned long medium_delay = 0;   // переменная оптимизазии зарежки фазы торможения
unsigned long medium_best = 3000; // идеальное время работы фазы торможения

unsigned long reset_delay = 4000; // время удержания кнопки до сброса значения slow_delay в значение 120000
unsigned long reset_timer = 0;    // таймер удержания кнопки 

boolean butt_flag = 0;            // флаг нажатой кнопки 
boolean fast_flag = 0;            // флаг быстрого насыпания
boolean medium_flag = 0;          // флаг среднего насыпания
boolean slow_flag = 0;            // флаг медленного насыпания

boolean bottom_key = 0;           // флаг нижнего оптодатчика
boolean upper_key = 0;            // флаг верхнего оптодатчика


void setup() {

      pinMode(A1, OUTPUT);        // лампочка на плате
      pinMode(A2, INPUT_PULLUP);  // верхний оптодатчик
      pinMode(A3, INPUT_PULLUP);  // нижний оптодатчик
      pinMode(2, OUTPUT);         // бипер
      pinMode(A4, OUTPUT);        // кулер
      
      btn.setDebounce(50);        // настройка антидребезга (по умолчанию 50 мс)
      btn.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 300 мс)
      btn.setClickTimeout(300);   // настройка таймаута между кликами (по умолчанию 300 мс)
      btn.setType(HIGH_PULL);
      
      stepper.setRunMode(KEEP_SPEED);
      stepper.autoPower(true);
      stepper.setAcceleration(5000);
      stepper.disable();
      
      digitalWrite(A1, 0);
      butt_flag = 0;

                digitalWrite(2, 1);delay(200); digitalWrite(2, 0);
                delay(50);
                digitalWrite(2, 1);delay(50);digitalWrite(2, 0);    
                delay(50);
                digitalWrite(2, 1);delay(50);digitalWrite(2, 0);
                delay(50);
                digitalWrite(2, 1);delay(400);digitalWrite(2, 0);

 Serial.begin(9600);
      
      }


void loop() {

btn.tick();

       upper_key = digitalRead(A2);
       bottom_key = digitalRead(A3);

  if (bottom_key + upper_key < 2){


// ----------------------------------------------------------------------
// Запуск по кнопке
    
    if (btn.isSingle() && butt_flag == 0 && millis() - last_press > 300) { 

        last_press = millis();   
        butt_flag = 1;
        digitalWrite(A4, 1); // запуск куллера
        digitalWrite(A1, 1); // включение лампочки

        load_timer = millis();
  
    }

// Выключение  по кнопке
    if (btn.isSingle() && butt_flag == 1 && millis() - last_press > 300) {   

        last_press = millis();     
        butt_flag = 0;
        digitalWrite(A4, 0); // выключение куллера
        digitalWrite(A1, 0); // выключение лампочки

        overload_timer = 0;
        medium_timer = 0;
        overload_timer = 0;
        load_timer = 0;
        medium_timer = 0;      

        stepper.reset();
        stepper.disable();

        digitalWrite(2, 1);
        delay(50);
        digitalWrite(2, 0);
        
    }

// сброс калибровки навески

if (btn.isHold() && butt_flag == 0 && millis() - last_press > 300){
    last_press = millis();
    
    if(reset_timer == 0){
      reset_timer = millis();
    }

    if(reset_timer != 0 && millis()-reset_timer>reset_delay){
      reset_timer = 0;
      slow_delay = 120000;
        digitalWrite(2, 1);delay(1000);digitalWrite(2, 0);
    }  
}
if (!btn.isHold() && butt_flag == 0){
reset_timer == 0;
}

// ----------------------------------------------------------------------
// Первое состояние - (фаза быстрого вращения)
// Нижний опто датчик ГОРИТ
// Верхний оптодатчик НЕ ГОРИТ 

       if(bottom_key == 1 && upper_key == 0 && butt_flag == 1) {
    
        stepper.enable();
        fast_flag = 1;            
        slow_flag = 0; 
        
        if (millis()-load_timer > slow_delay){
              stepper.setSpeed(medium_speed, NO_SMOOTH);
                medium_flag = 1;            
                if (medium_timer == 0){
                  medium_timer = millis();
                }
            }
            else {
              stepper.setSpeed(max_speed, SMOOTH);
              medium_timer = 0;
              medium_flag = 0;
            }    

        stepper.tick();

       }


// ----------------------------------------------------------------------
// Второе состояние - (фаза медленного вращения)
// Нижний опто датчик НЕ ГОРИТ
// Верхний оптодатчик НЕ ГОРИТ 

       if(bottom_key == 0 && upper_key == 0 && butt_flag == 1) {
             
        slow_flag = 1;
        fast_flag =0;
        
            if (overload_timer == 0) {
             overload_timer = millis();
            }

            if (medium_flag = 1 && medium_timer != 0 && millis() - medium_timer > 4000) {
              medium_delay = millis()- medium_timer;
              medium_timer = 0;
              medium_flag = 0;     
            }
                
        stepper.enable();
        stepper.setSpeedDeg(slow_speed);

        stepper.tick();

       }


// ----------------------------------------------------------------------
// Остановка по датчику (с задержкой отключения)

        if(bottom_key == 0 && upper_key == 1 && butt_flag == 1) {
                          
              stepper.reset();

              delay(delay_timer);
              upper_key = digitalRead(A2);                  
               
                if(upper_key == 1) {                                                       
                    stepper.disable();
                    digitalWrite(A4, 0); // остановка куллера
                    digitalWrite(A1, 0); // вЫключение лампочки


Serial.print("OLD DELAY: "); Serial.println(slow_delay);     

                  if (medium_flag == 0 && (millis()-delay_timer) - overload_timer < 3000) {
                    slow_delay = millis()-load_timer-delay_timer-3000;

Serial.print("In the SLOW FLAG "); Serial.print("NEW DELAY: "); Serial.println(slow_delay);
                   
                  }
                  
                  if (medium_delay > medium_best) {
                    slow_delay = slow_delay + (((medium_delay - medium_best)/2)*medium_speed)/max_speed; // тут используется коэффециэнт отношения скоростей
                    
Serial.print("In the MEDIUM FLAG "); Serial.print(" MEDIUM DELAY= "); Serial.print(medium_delay);
Serial.print(" NEW DELAY= "); Serial.println(slow_delay);
                  
                  medium_delay = 0;
                
                  }                  

                    butt_flag = 0;
                    overload_timer = 0;
                    load_timer = 0;
                    medium_timer = 0;
                    slow_flag = 0;
                    reset_timer == 0;
               

                digitalWrite(2, 1);delay(50);digitalWrite(2, 0);
                delay(50);
                digitalWrite(2, 1);delay(50);digitalWrite(2, 0);   
                delay(50);
                digitalWrite(2, 1);delay(50);digitalWrite(2, 0);
                                     
                }
        }
  }

 

// ----------------------------------------------------------------------
// Третье состояние(Датчики не подключены) - (фаза медленного вращения)
// Нижний опто датчик ГОРИТ
// Верхний оптодатчик ГОРИТ 
// Работа только при зажатой кнопке
  
  
  if (bottom_key + upper_key == 2) {
      
        if (btn.isHold()){       

              stepper.enable();

              if (btn.getHoldClicks()==0){         
                stepper.setSpeedDeg(slow_speed);
              }
              if (btn.getHoldClicks()==1){         
                stepper.setSpeed(medium_speed);
              }
              if (btn.getHoldClicks()==2){         
                stepper.setSpeed(max_speed);
              }
              
              stepper.tick();

              digitalWrite(A4, 1); // запуск куллера
              digitalWrite(A1, 1); // включение лампочки
              
        }
        
        if (!btn.isHold()){
            stepper.reset();
            stepper.disable();
            digitalWrite(A4, 0); // остановка куллера
            digitalWrite(A1, 0); // вЫключение лампочки
      
        }
  }
  
// ----------------------------------------------------------------------

}
