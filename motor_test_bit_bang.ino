
#define MIN_THROTTLE 10
#define MID_THROTTLE 15
#define MAX_THROTTLE 20
#define PWM_PERIOD 200


//ISR variables
long count = 0;
int pin_val=0;

//setup vars
int signal_pin = 3;
int incoming_byte =0;
int throttle = MIN_THROTTLE;

void setup() {

  cli();//stop interrupts
  // put your setup code here, to run once:
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 100khz increments
  OCR1A = 25;//19;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler, CS11 and CS10 for 64 prescaler
  TCCR1B |= (1 << CS11) | (1<<CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();//allow interrupts

  pinMode(signal_pin, OUTPUT);
  
  Serial.begin(9600);
  Serial.print(">");
}

// Timer interrupt set up to fire at 100kHz. Every 100 interrupts is 1ms,
// so hi signal should be between 100-200ms
ISR(TIMER1_COMPA_vect){
  
  
  
  count ++;
  if(count <= throttle)
  {
    //check if pin change
    if(pin_val == 0)
    {
      digitalWrite(signal_pin, HIGH);
    }
    //set pin high
    pin_val = 1;
  }
  else if(count < PWM_PERIOD)
  {
    if(pin_val == 1)
    {
      digitalWrite(signal_pin, LOW);
    }
    //set pin low
    pin_val =0;
    
  }
  else if(count >=PWM_PERIOD)
  {
    count = 0;
    
  }

}
  

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() >0)
  {
    incoming_byte = Serial.read();
    Serial.println((char)incoming_byte);
    switch(incoming_byte)
    {
      case 'a':
        Serial.println("min throttle");
        throttle = MIN_THROTTLE;
        break;
      case 'b':
        Serial.println("mid throttle");
        throttle = MID_THROTTLE;
        break;
      case 'c':
        Serial.println("max throttle");
        throttle = MAX_THROTTLE;
        break;
      case 'd':
        Serial.println("increment throttle");
        setThrottle(throttle+1);
        break;
      case 'e':
        Serial.println("min throttle");
        setThrottle(throttle-1);
        break;
      default:
        break;
    }
    Serial.print(">");
    incoming_byte = 0;
  }
  delay(10);
}

void setThrottle(int newThrottle)
{
  if(newThrottle > MAX_THROTTLE) throttle = MAX_THROTTLE;
  else if(newThrottle < MIN_THROTTLE) throttle = MIN_THROTTLE;
  else throttle = newThrottle;
}
