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
volatile int *SW_ptr = (int *)SW_BASE;   //Switch Address
const unsigned int lookupTable[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
const double MetFactor3mphInclination[4] = {3.3, 3.7, 4.1, 4.5}; //Found at https://www.pulmonarywellness.com/book/8-treadmill-101/
const double MetFactor8mphInclination[4] = {13.3, 13.7, 14.1, 14.5}; //Estimated based off chart from link above
int ms1, ms2, s1, s2, m1, m2, h1, h2 = 0;
int currentSpeed, ones, incline, jogging, displayCal, displaySteps, addWeight = 0;
int tens = 8;
int hundreds = 1;
int weight = 180;
double currentMET;
int currentSpeed, walkingSpeed = 3; //This is the average human walking speed from google research in mph
int joggingSpeed = 8; //This is the average human jogging speed from google research in mph
double strideLength = 2.5; //This is the average stride length of a human from goolge research in feet

void config_timer()
{
	//status set to 0
	*(timer_ptr) = 0b00; //its not counting to start so set it to 0
	
	// set timer period 
	int counter = 1000000;

	/* set the interval timer period for scrolling the HEX displays */
	*(timer_ptr + 2) = (counter & 0xFFFF); //register 0xFF202008
	*(timer_ptr + 3) = (counter >> 16) & 0xFFFF;
	*(timer_ptr + 1) = 0b0110; //could also put 0x6

}

double calculateCurrentTimeInMIN(int deca_hours, int hours, int deca_minutes, int minutes, int deca_seconds, int seconds){
	double tempHours, tempMinutes, tempSeconds;
	tempHours = (double) ((deca_hours * 10) + hours) * 60; //putting hours into minutes
	tempMinutes = (double) (deca_minutes * 10) + minutes; //combining both minue values
	tempSeconds = (double) ((deca_seconds * 10) + seconds) / 60; //putting seconds into minutes
	return (tempHours + tempMinutes + tempSeconds);
}

double calculateCurrentTimeInHOUR(int deca_hours, int hours, int deca_minutes, int minutes, int deca_seconds, int seconds){
	double tempHours, tempMinutes, tempSeconds;
	tempHours = (double) (deca_hours * 10) + hours;
	tempMinutes = (double) ((deca_minutes * 10) + minutes) / 60; //putting minutes into hours
	tempSeconds = (double) ((deca_seconds * 10) + seconds) / 3600; //putting seconds into hours
	return (tempHours + tempMinutes + tempSeconds);
}

void update_timer(int deca_hours, int hours, int deca_minutes, int minutes, int deca_seconds, int seconds, int deci_seconds, int centi_seconds) {
	*((char*)HEX3_HEX0_BASE) = lookupTable[seconds]; //correspons to the 6 7seg displays
	*((char*)HEX3_HEX0_BASE + 1) = lookupTable[deca_seconds];
	*((char*)HEX3_HEX0_BASE + 2) = lookupTable[minutes];
	*((char*)HEX3_HEX0_BASE + 3) = lookupTable[deca_minutes];
	*((char*)HEX5_HEX4_BASE) = lookupTable[hours];
	*((char*)HEX5_HEX4_BASE + 1) = lookupTable[deca_hours];
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
double calculateCaloriesBurned(double time, int currentWeight, double MET){
	double kgWeight = currentWeight / 2.205; //conversion from lbs to kg
	double totalCaloriesPerMinute = (MET * 3.5 * kgWeight)/200;
	//Time is in minutes
	return (time * totalCaloriesPerMinute);
}

void displayCalories(int totalCalories){
	//Expecting they won't burn more than 9999 calories, since we are only considering a max of 4 digit numbers
	int i = 0;
	*((char*)HEX3_HEX0_BASE) = lookupTable[0]; //correspons to the 6 7seg displays
	*((char*)HEX3_HEX0_BASE + 1) = lookupTable[0];
	*((char*)HEX3_HEX0_BASE + 2) = lookupTable[0];
	*((char*)HEX3_HEX0_BASE + 3) = lookupTable[0];
	*((char*)HEX5_HEX4_BASE) = lookupTable[0];
	*((char*)HEX5_HEX4_BASE + 1) = lookupTable[0];
	while(totalCalories > 0){
		int digit = totalCalories % 10;
		*((char*)HEX3_HEX0_BASE + i) = lookupTable[digit];
		totalCalories /= 10;
		i++;
	}
}

double calculateStepCount(int speed, double time){
	/*Assuming 1 mile is about 5280 feet we can 
	* estimate the distance a user walked based on the speed and time (already in hours) they've run.
	* Time must be in hours since speed is in miles per hour
	* Multiplying mph x hours will give us miles (distance).
	* We can then convert to feet and divide the total distance by the average strideLength to get total steps taken.*/
	double distance;
	distance = (double) speed * time * 5280; //Multiply by 5280 to put into feet
	return (distance / strideLength);
}

void displayStepCount(int totalStepCount){
	int i = 0;
	*((char*)HEX3_HEX0_BASE) = lookupTable[0]; //correspons to the 6 7seg displays
	*((char*)HEX3_HEX0_BASE + 1) = lookupTable[0];
	*((char*)HEX3_HEX0_BASE + 2) = lookupTable[0];
	*((char*)HEX3_HEX0_BASE + 3) = lookupTable[0];
	*((char*)HEX5_HEX4_BASE) = lookupTable[0];
	*((char*)HEX5_HEX4_BASE + 1) = lookupTable[0];
	while(totalStepCount > 0){
		int digit = totalStepCount % 10;
		*((char*)HEX3_HEX0_BASE + i) = lookupTable[digit];
		totalStepCount /= 10;
		i++;
	}
}
	
int main(void) {
	config_timer();
	while(1){	
		//While switch 0 is on, the sevent segeent display will show a timer
		while(*SW_ptr & 0b1){
			//If switch 1 is on then they are jogging, else they are walking
			if(*SW_ptr == 0b11){
				currentSpeed = joggingSpeed;
			}
			else{
				currentSpeed = walkingSpeed;
			}
			//If switch 2 is active then they are walking or running at a 10% incline
			//ElseIf switch 3 is active then they are walking or running at a 20% incline
			//ElseIf switch 4 is active then they are walking or running at a 30% incline
			//Else, if neither switch 2, 3, or 4 are active then they are walking or running at a 0% incline (no incline)
			if(*SW_ptr == 0b101){
				incline = 1;
			}
			else if(*SW_ptr == 0b1001){
				incline = 2;
			}
			else if(*SW_ptr & 0b10001){
				incline = 3;
			}
			else{
				incline = 0;
			}
			
			//If button 0 is pressed then start the timer
			if (*buttons == 0b0001){
				jogging = 1;
			}
			//while jogging increase the numbers
			if(jogging){
				while (*(timer_ptr) == 0b10) { ; }
				ms1++;

				if (ms1 == 10) {ms1 = 0; ms2 += 1;}
				if (ms2 == 10) {ms2 = 0; s1 += 1;}
				if (s1 == 10) {s1 = 0; s2 += 1;}
				if (s2 == 6) {s2 = 0; m1 += 1;}
				if (m1 == 10) { m1 = 0; m2 += 1;}
				if (m2 == 6) {m2 = 0; h1 += 1;}
				if (h1 == 10) {h1 = 0; h2 += 1;}

				update_timer(h2, h1, m2, m1, s2, s1, ms2, ms1); //increase numbers by updating timer
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
					h1 = 0;
					h2 = 0;

					update_timer(h2, h1, m2, m1, s2, s1, ms2, ms1);
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
				h1 = 0;
				h2 = 0;

				update_timer(h2, h1, m2, m1, s2, s1, ms2, ms1);
				config_timer();
			}
		}
		
		//While switch 1 is on the seven segment display will show the users weight in lbs
		while(*SW_ptr == 0b10){
			update_weight(ones, tens, hundreds);
			//Setting jogging variable to 0
			jogging = 0;
			//Pressing button 1 will increment their weight
			//Pressing button 2 will decrement their weight
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
				//Setting addWeight boolean to zero helps with the simulation by having the user
				//click the button off before adding again
				addWeight = 0;
			}
			else if(!(*buttons == 0b0001) && !(*buttons == 0b0010)){
				addWeight = 1;
			}
			
			weight = (hundreds * 100) + (tens * 10) + (ones);
		}
		displayCal = 1;
		//While switch 2 is on is we will display the calories burned
		while(*SW_ptr == 0b100){
			//Calculating the current MET
			if(currentSpeed == walkingSpeed){
				currentMET = MetFactor3mphInclination[incline];
			}else{
				currentMET = MetFactor8mphInclination[incline];
			}
			//calculate the current time in minutes
			double currentTime = calculateCurrentTimeInMIN(h2, h1, m2, m1, s2, s1);
			
			//Calculate the total calories burned and round to the nearest integer
			double totalCalories = calculateCaloriesBurned(currentTime, weight, currentMET);
			//Display the total calories burned
			if(displayCal){
				displayCalories((int)totalCalories);
				displayCal = 0;
			}
		}
		
		displaySteps = 1;
		//While switch 3 is on we will display the steps taken
		while(*SW_ptr == 0b1000){
			//calculate the current time in hours
			double currentTime = calculateCurrentTimeInHOUR(h2, h1, m2, m1, s2, s1);
			double totalStepCount = calculateStepCount(currentSpeed, currentTime);
			//Displaying the step count
			if(displaySteps){
				displayStepCount((int)totalStepCount);
				displaySteps = 0;
			}
		}
	}
}
