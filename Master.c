#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Lcd.h>
#include <avr/interrupt.h>

char ignore;

int main(void) {
    //lcd output
    DDRC=0xFF;
    DDRD=0x07; 

    //LCD initial
    init_LCD();
    LCD_cmd(0x0F); // Make blinking cursor
    

    // ADC initialization
    ADMUX = (0 << REFS1) | (1 << REFS0); // AVCC
    ADCSRA = (1<<ADEN) |  (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // enable interrupt, auto trigger, prescale = 8
    ADCSRA |= (1 << ADSC); // start first conversion
    
    //sei(); // enable global interrupt

    //ac
    SFIOR |= (0<<ACME); // disable ADC multiplexer output ;use AIN1
    ACSR  |= (0<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
    //                   use AIN0

    //SPI : master/slave
    DDRB = (1<<DDB7) | (0<<DDB6) | (1<<DDB5) | (1<<DDB4) ;
    PORTB = (1<<PORTB4) ; // slave sel SS

    // SPI initialization
    // SPI Type: Master
    // SPI Clock Rate: 8MHz / 128 = 62.5 kHz
    // SPI Clock Phase: Cycle Half
    // SPI Clock Polarity: Low
    // SPI Data Order: MSB First
    SPCR = (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0);
    SPSR = (0<<SPI2X);

    while(1){

        //this part is for AC
        ////////////////////////////////////////////////////////////////////////////
        if(((ACSR >> ACO) & 1) == 1){ //    +>- //age A>B bood code zire ejra shavad
        //( as long as A is higher than B, converts the analog value of A to digital and
        // Sends the digital value of A to the slave and 
        //Prints the digital value of A on a 16x2 alphanumeric LCD)
            
            //this part is for ADC (convert to digital and LCD)
            //////////////////////////////////////////////////////////////////////////
            ADCSRA |= ((1 << ADSC) | (1 << ADIF)); // start conversion
            while( (ADCSRA & (1 << ADIF)) == 0 ); // wait till end of the conversion

            char tempreture[20];

            unsigned int Vdigital=0;
            ADCH &= (0<<7) | (0<<6) | (0<<5) | (0<<4) | (0<<3) | (0<<2) | (0<<1)| (0<<0);   //making sure upper bits of ADCW are 0
            Vdigital = (unsigned int)ADCW; //    ADMUX5:0 = 00000 defult 
            

            unsigned int temp_int=(Vdigital*500)/1024;   //converting Vdigital to tempreture
            
            itoa(temp_int,tempreture,10);   //to character; stored in tempreture[20]

            //write on LCD
            LCD_cmd(0x01);
            for(int i=0;  tempreture[i] != 0; i++){
                LCD_write(tempreture[i]);    
            }
            
            
            //_delay_ms(400);
            //////////////////////////////////////////////////////////////////////////////

            
            //this part is for sending data to slave
            //////////////////////////////////////////////////
            PORTB &= ~(1<<PORTB4); // Select Slave SS

            SPDR= (unsigned char)temp_int; //send
            while(((SPSR >> SPIF) & 1) == 0);
            ignore = SPDR;
            
            //////////////////////////////////////////////////
            
            
        }
           
        
    }
}




