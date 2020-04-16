/* This files provides address values that exist in the system */

#define BOARD                 "DE10-Standard"

/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_PIXEL_BUF_BASE   0xC8000000
#define FPGA_PIXEL_BUF_END    0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF

/* Cyclone V FPGA devices */
#define LED_BASE	      0xFF200000
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define TIMER_2_BASE          0xFF202020
#define AV_CONFIG_BASE        0xFF203000
#define RGB_RESAMPLER_BASE    0xFF203010
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define EDGE_DETECT_CTRL_BASE 0xFF203070
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO0_BASE        0xFF708000
#define HPS_GPIO1_BASE        0xFF709000
#define HPS_GPIO2_BASE        0xFF70A000
#define I2C0_BASE             0xFFC04000
#define I2C1_BASE             0xFFC05000
#define I2C2_BASE             0xFFC06000
#define I2C3_BASE             0xFFC07000
#define HPS_TIMER0_BASE       0xFFC08000
#define HPS_TIMER1_BASE       0xFFC09000
#define HPS_TIMER2_BASE       0xFFD00000
#define HPS_TIMER3_BASE       0xFFD01000
#define HPS_RSTMGR	      0xFFD05000
#define HPS_RSTMGR_PREMODRST  0xFFD05014
#define FPGA_BRIDGE           0xFFD0501C

#define PIN_MUX		      0xFFD08400
#define CLK_MGR		      0xFFD04000

#define SPIM0_BASE	      0xFFF00000
#define SPIM0_SR	      0xFFF00028
#define SPIM0_DR	      0xFFF00060
/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs

volatile int* buttons = (int*)KEY_BASE;
volatile int* timer_ptr = (int*)TIMER_BASE;//OxFF202000
volatile unsigned int *ADC_ptr = (unsigned int *)ADC_BASE; //ADC Address
volatile unsigned int *SW_ptr = (unsigned int *)SW_BASE;   //Switch Address
volatile unsigned int ADC_val; //Stores the value we read from the ADC channel
const unsigned int lookupTable[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}; 
int ms1, ms2, s1, s2, m1, m2 = 0;
int ones = 0;
int tens = 5;
int addWeight, hundreds = 1;
int incline, weight, jogging = 0;
float walkingSpeed = 1.4; //This is the average human walking speed from google research in m/s
float runningSpped = 5.16; //This is the average human jogging speed from google research in m/s

void config_timer()
{
	//status set to 0
	*(timer_ptr + 0) = 0x0; //its not counting to start so set it to 0
	
	// set timer period 
	int counter = 100000;

	/* set the interval timer period for scrolling the HEX displays */
	*(timer_ptr + 2) = (counter & 0xFFFF); //register 0xFF202008
	*(timer_ptr + 3) = (counter >> 16) & 0xFFFF;
	*(timer_ptr + 1) = 0x0110; //could also put 0x6

}

void update_timer(int deca_minutes, int minutes, int deca_seconds, int seconds, int deci_seconds, int centi_seconds) {
	*((char*)HEX3_HEX0_BASE) = lookupTable[centi_seconds]; //correspons to the 6 7seg displays
	*((char*)HEX3_HEX0_BASE + 1) = lookupTable[deci_seconds];
	*((char*)HEX3_HEX0_BASE + 2) = lookupTable[seconds];
	*((char*)HEX3_HEX0_BASE + 3) = lookupTable[deca_seconds];
	*((char*)HEX5_HEX4_BASE) = lookupTable[minutes];
	*((char*)HEX5_HEX4_BASE + 1) = lookupTable[deca_minutes];
}

void update_weight(int ones_column, int tens_column, int hundreds_column){
	*((char*)HEX3_HEX0_BASE) = lookupTable[ones_column]; //correspons to the 6 7seg displays
	*((char*)HEX3_HEX0_BASE + 1) = lookupTable[tens_column];
	*((char*)HEX3_HEX0_BASE + 2) = lookupTable[hundreds_column];
	*((char*)HEX3_HEX0_BASE + 3) = lookupTable[0];
	*((char*)HEX5_HEX4_BASE) = lookupTable[0];
	*((char*)HEX5_HEX4_BASE + 1) = lookupTable[0];
}

//Calculating the calories here
void calculateCaloriesBurned(int speed, int time, int weight){
	
}
	
int main(void) {
	config_timer();
	update_weight(ones, tens, hundreds);
	while(1){
		//While switch 0 is on, the sevent segeent display will show a timer
		//Else if it is off it will show the users weight in lbs 
		while(*SW_ptr & 0x1){
			//Writing to the ADC channels to update it
			//*(ADC_ptr) = 0x1;
			//*(ADC_ptr + 1) = 0x1;
			//If switch 1 is on then they are jogging, else they are walking
			if(*SW_ptr & 0x10){
				//If switch 2 is active then they are walking or running at a 10 degree incline
				if(*SW_ptr & 0x100){
					incline = 10;
				}
				//If switch 3 is active then they are walking or running at a 20 degree incline
				else if(*SW_ptr & 0x1000){
					incline = 20;
				}
				//If switch 4 is active then they are walking or running at a 30 degree incline
				else if(*SW_ptr & 0x10){
					incline = 30;
				}
				//If neither switch 1, 2, or 3 are active then they are walking or running at a 0 degree incline
				else{

				}
			}
			
			//If button 0 is pressed then start the timer
			if (*buttons == 0b0001){
				jogging = 1;
			}
			//while jogging increase the numbers
			if(jogging){
				while (*(timer_ptr) == 0b10) { ; }
				ms1++;

				if (ms1 == 10) { ms1 = 0; ms2 += 1; }
				if (ms2 == 10) { ms2 = 0; s1 += 1; }
				if (s1 == 10) { s1 = 0; s2 += 1; }
				if (s2 == 6) { s2 = 0; m1 += 1; }
				if (m1 == 10) { m1 = 0; m2 += 1; }

				update_timer(m2, m1, s2, s1, ms2, ms1); //increase numbers by updating timer
				config_timer();

				//If button 1 is pressed then stop the timer 
				if (*buttons == 0b0010){ 
					jogging = 0; 
				}

				//If button 2 is pressed then reset the timer
				if (*buttons == 0b0100) {
					jogging = 0;
					ms1 = 0;
					ms2 = 0;
					s1 = 0;
					s2 = 0;
					m1 = 0;
					m2 = 0;

					update_timer(m2, m1, s2, s1, ms2, ms1);
					config_timer();
				}
			}
			//If button 2 is pressed then reset the timer
			if (*buttons == 0b0100) {
				jogging = 0;
				ms1 = 0;
				ms2 = 0;
				s1 = 0;
				s2 = 0;
				m1 = 0;
				m2 = 0;

				update_timer(m2, m1, s2, s1, ms2, ms1);
				config_timer();
			}
		}
		
		while(!(*SW_ptr & 0x1)){
			//While switch 0 is off the sevent segeent display will show the users weight in lbs
			jogging = 0;
			//Pressing button 3 will increment their weight
			//Pressing button 4 will decrement their weight
			if ((*buttons == 0b0001) && addWeight){
				ones++;

				if (ones == 10){
					ones = 0; 
					tens++; 
				}
				if (tens == 10) {
					tens = 0; 
					hundreds++; 
				}
				update_weight(ones, tens, hundreds);
				//Setting addWeight boolean to zero helps with the simulation by having the user
				//click the button off before adding again
				addWeight = 0;
			}else if ((*buttons == 0b0010) && addWeight){
				ones--;

				if (ones == -1){
					ones = 9; 
					tens--; 
				}
				if (tens == -1) {
					tens = 9; 
					hundreds--; 
				}
				update_weight(ones, tens, hundreds);
				addWeight = 0;
			}
			else if(!(*buttons == 0b0001) && !(*buttons == 0b0010)){
				addWeight = 1;
			}
			
			weight = (hundreds * 100) + (tens * 10) + (ones);
		}		
	}
}