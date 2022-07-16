#include <avr/io.h>
#include <util/delay.h>

double dutyCycle = 50;
int tempreture=0;
double new_dutycycle=50;

void calculate_dutycycle(int temp);

int main() {
    //show temperture recieved from master
    DDRC = 0xff;

    //slave 
    ////////////////////////////////////////////////////////
    DDRB = (0<<DDB7) | (1<<DDB6) | (0<<DDB5) | (0<<DDB4);

    // SPI initialization
    // SPI Type: Slave
    // SPI Clock Rate: 8MHz / 128 = 62.5 kHz
    // SPI Clock Phase: Cycle Half
    // SPI Clock Polarity: Low
    // SPI Data Order: MSB First
    SPCR = (1<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0);
    SPSR = (0<<SPI2X);


    //duty cycle cooler
    ///////////////////////////////////////////
    // OC0 Pin
    DDRB |= (1<<PORTB3);

    // Fast PWM
    TCCR0 |= (1<<WGM01) | (1<<WGM00);

    // Clear OC0 on compare match
    TCCR0 |= (1<<COM01);

    // 1/8*clock Prescaling
    TCCR0 |= (1<<CS01);

    //heater and warning
    DDRA=0xFF;

    
    //sei();
    while(1) {
        SPDR = '0';
        while (((SPSR >> SPIF) & 1) == 0);
        unsigned int tempreture = SPDR;
        PORTC=tempreture;

        PORTA=0x00;
        

        if(tempreture < 20){    
            PORTA |= (1<<PORTA1);
            OCR0=0x00;
        }

        else if(tempreture >=25 && tempreture < 55){
            // Duty cycle
            calculate_dutycycle(tempreture);
            
            OCR0 = (new_dutycycle/100)*255;
            
        }

        else if(tempreture >= 55){
            OCR0=0x00;

            while (55 <= tempreture){
                PORTA ^= (1<<PORTA0);
                _delay_ms(20);

                while (((SPSR >> SPIF) & 1) == 0);
                tempreture = SPDR;

            }
            
            
            
        }
        
    }

    
}

//ISR(SPI_STC_vect) {}
void calculate_dutycycle(int temp){
    dutyCycle=50;

    int add_dutycycle = temp - 25;
    add_dutycycle = add_dutycycle/5;
    add_dutycycle = add_dutycycle*10; 

    dutyCycle += add_dutycycle;
    new_dutycycle = dutyCycle;
            
}
