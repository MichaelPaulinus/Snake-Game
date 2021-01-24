// define constants for glcd
char GLCD_DataPort at PORTB;

sbit GLCD_CS1 at RD3_bit;
sbit GLCD_CS2 at RD2_bit;
sbit GLCD_RS at RD1_bit;
sbit GLCD_RW at RC2_bit;
sbit GLCD_EN at RC1_bit;
sbit GLCD_RST at RC0_bit;

sbit GLCD_CS1_Direction at TRISD3_bit;
sbit GLCD_CS2_Direction at TRISD2_bit;
sbit GLCD_RS_Direction at TRISD1_bit;
sbit GLCD_RW_Direction at TRISC2_bit;
sbit GLCD_EN_Direction at TRISC1_bit;
sbit GLCD_RST_Direction at TRISC0_bit;

// define constants for direction
sbit UP at RD4_bit;
sbit LEFT at RD5_bit;
sbit RIGHT at RD6_bit;
sbit DOWN at RD7_bit;

unsigned char SNAKE[40][2];
unsigned char dir = 2;
unsigned char food[2], LENGTH_SNAKE = 5;
unsigned char X = 62, Y = 20;
unsigned char score, *score_txt = "Score: 00";
unsigned char *hscore_txt = "New High Score: 00";
unsigned int hscore;
unsigned char i;

/*****************************I2C FUNCTIONS************************************/
void write_EEPROM(unsigned int address, unsigned int dat){
  unsigned int temp;
  I2C1_Start();            // issue I2C start signal
  I2C1_Wr(0xA0);           // send byte via I2C (device address + W)
  temp = address >> 8;     //saving higher order address to temp
  I2C1_Wr(temp);           //sending higher order address
  I2C1_Wr(address);        //sending lower order address
  I2C1_Wr(dat);            // send data (data to be written)
  I2C1_Stop();             // issue I2C stop signal
  Delay_ms(20);
}

unsigned int read_EEPROM(unsigned int address){
  unsigned int temp;
  I2C1_Start();            // issue I2C start signal
  I2C1_Wr(0xA0);           // send byte via I2C (device address + W)
  temp = address >> 8;     //saving higher order address to temp
  I2C1_Wr(temp);           //sending higher order address
  I2C1_Wr(address);        //sending lower order address
  I2C1_Repeated_Start();   // issue I2C signal repeated start
  I2C1_Wr(0xA1);           // send byte (device address + R)
  temp = I2C1_Rd(0u);      // Read the data (NO acknowledge)
  I2C1_Stop();
  return temp;
}
/*****************************GAME FUNCTIONS***********************************/
void foodGen(){    // food generator
        food[0] = rand()/264 + 1;
        food[1] = rand()/700 + 1;
        Glcd_Dot(food[0], food[1], 1);
}

void end(){
        // display the users score
        Glcd_Fill(0x00);
        hscore = read_EEPROM(1);
        if (score<=hscore){
        Glcd_Write_Text(score_txt,36,3,1);

        }else{
                hscore_txt[16] = score/10 + 48;
                hscore_txt[17] = score%10 + 48;
                Glcd_Write_Text(hscore_txt,11,3,1);
                write_EEPROM(1,score);
        }
        
        // reset the position of the snake
        LENGTH_SNAKE = 5;
        X = 62;
        Y = 20;
        score = 0;
        score_txt[7] = '0';
        score_txt[8] = '0';
        dir = 2;
        delay_ms(1500);
}
/*****************************MAIN FUNCTION BEGIN******************************/
    void main() {
    CMCON = 0x07;           // To turn off comparators
    ADCON1 = 0x06;          // To turn off analog to digital converters
    I2C1_Init(100000);
    write_EEPROM(1,0);
    
    TRISD=0xff;
    Glcd_Init();

    init:

    Glcd_Fill(0x00);
    foodGen();

    for(i=0;i<5;i++){
            Glcd_Dot(i,0,1);
    }
    Glcd_Rectangle(0,0,127,50, 1); // game border
    Glcd_Write_Text(score_txt,35,7,1);

    while(1){
    
// buttons control
if(RIGHT==1){
if (dir==0) dir=0;
else dir = 3; //right button pressed
}
if(DOWN==1){
  if (dir==1) dir=1;
  else dir = 2;//down button pressed
}

if(UP==1){
  if (dir==2) dir=2;
  else dir = 1;//up button pressed
}

if(LEFT==1) {
  if (dir==3) dir=3;
  else dir = 0;//left button pressed
}

// directions control
if(dir==3){ // go right
        X++;
        if(X>126){
                end();
                goto init;
        }
        SNAKE[LENGTH_SNAKE - 1][0] = X;
        SNAKE[LENGTH_SNAKE - 1][1] = Y;
        for(i=0; i<length_snake - 1; i++){
                SNAKE[i][0] = SNAKE[i+1][0];
                SNAKE[i][1] = SNAKE[i+1][1];
        }
}

if(dir==2){ // go down
        Y++;
        if(Y>49){
                end();
                goto init;
        }

        SNAKE[LENGTH_SNAKE - 1][1] = Y;
        SNAKE[LENGTH_SNAKE - 1][0] = X;
        for(i=0; i<length_snake - 1; i++){
                SNAKE[i][1] = SNAKE[i+1][1];
                SNAKE[i][0] = SNAKE[i+1][0];
        }
}

if(dir==1){ // go up
        Y--;
        if(Y<1){
                end();
                goto init;
        }

        SNAKE[LENGTH_SNAKE - 1][1] = Y;
        SNAKE[LENGTH_SNAKE - 1][0] = X;
        for(i=0; i<LENGTH_SNAKE - 1; i++){
                SNAKE[i][1] = SNAKE[i+1][1];
                SNAKE[i][0] = SNAKE[i+1][0];

        }
}

if(dir==0){ // go left
        X--;
        if(X<1){
                end();
                goto init;
        }

        SNAKE[LENGTH_SNAKE - 1][1] = Y;
        SNAKE[LENGTH_SNAKE - 1][0] = X;
        for(i=0; i<LENGTH_SNAKE - 1; i++){
                SNAKE[i][1] = SNAKE[i+1][1];
                SNAKE[i][0] = SNAKE[i+1][0];
        }
}

// map out snake
for(i=0;i<LENGTH_SNAKE;i++){
        Glcd_Dot(SNAKE[i][0], SNAKE[i][1], 1);
}

  Glcd_Dot(SNAKE[0][0], SNAKE[0][1], 0); // cleans the trail left by the snake

// if SNAKE collides with food
  if(X==food[0] && Y==food[1]){
          LENGTH_SNAKE++;

//if you reach the maximum level
          if(LENGTH_SNAKE>=38){
                  LENGTH_SNAKE=5;
                  X = 1;
                  Y = 1;
                  score = 0;
                  dir = 2;
                  Glcd_Fill(0xFF);
    Glcd_Write_Text("MAX SCORE, YOU WIN !!!",25,3,0); // YOU WON
    delay_ms(2000);
    goto init;
}

foodGen();

score= score + 10;
score_txt[7] = score/10 + 48;
score_txt[8] = score%10 + 48;
Glcd_Write_Text(score_txt,35,7,1);
}

for(i = 0; i<450/LENGTH_SNAKE; i++){
        delay_ms(1);
}
}
}
/*****************************MAIN FUNCTION END********************************/