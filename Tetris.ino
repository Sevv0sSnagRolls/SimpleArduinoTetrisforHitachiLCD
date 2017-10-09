#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//global game board - 4 extra rows for max piece size to be spawned outside of the board
int tetris[24][4];
int i;
int j;
int currentPiece[8] = {0,0,0,0,0,0,0,0};
int newPiece[8] = {0,0,0,0,0,0,0,0};
int motion[8] = {1,0,1,0,1,0,1,0};
int currentPieceBoolean = 0;
int stick[8] = {0,1,1,1,2,1,3,1};
int sqre[8] = {0,1,1,1,0,2,1,2};
int L[8] = {0,2,1,2,2,2,2,1};
int Z[8] = {0,1,1,1,1,2,2,2};
int randomY[8] = {0,1,0,1,0,1,0,1};
int count = 0;

const int buttonLeftPin = 9;
int buttonLeft = 0;
const int buttonRightPin = 8;       
int buttonRight = 0;


void generatePiece(){
  int m = random(0,3);
  int c;
  switch( m ){
    case 0:
      //stick along 2nd long column
      memcpy( currentPiece, stick, sizeof(stick) );
    break;
    case 1:
      //square in centre
      memcpy( currentPiece, sqre, sizeof(sqre) );
    break;
    case 2:
      //L shape along 3rd long column
      memcpy( currentPiece, L, sizeof(L) );
    break;
    case 3:
      //Zalong 2nd long column
      memcpy( currentPiece, Z, sizeof(Z) );
    break;
  }
  //move it up or down randomly
  m = random(-1,2);
  for(c=0;c<8;c++){
    currentPiece[c] = currentPiece[c] + m*randomY[c];
  }
}


void updateCurrentPiece(int vector[], int dir){
  int n;
  int k;
  //create new piece - don't know if this totally works
  for(n=0;n<8;n++){
    k = currentPiece[n] + dir*vector[n];
    //check limits of motion
    if( n%2 == 1 ){
      if(k >=0 && k<4){
        newPiece[n] = k;
      }
      else{
        //limit exceeded, return before updating anymore
        return;
      }
    }
    else{
      newPiece[n] = currentPiece[n] + dir*vector[n];
    }
  }

  //clear current piece from tetris array
  for(n=0;n<8;n++){
    tetris[ currentPiece[n] ][ currentPiece[n+1] ] = 0;
    //because of data structure choice need to increment by 2 with n
    n++;
  }

  //check newPiece against current tetris array
  for(n=0;n<8;n++){
    //if its reached the end of the LCD
    if(newPiece[n] > 23){
      currentPieceBoolean = 0;
      for(n=0;n<8;n++){
        tetris[ currentPiece[n] ][ currentPiece[n+1] ] = 1;
        //because of data structure choice need to increment by 2 with n
        n++;
      }
      return;
    }
    //if its collided with another element
    if( tetris[ newPiece[n] ][ newPiece[n+1] ] > 0 ){
        currentPieceBoolean = 0;
        for(n=0;n<8;n++){
        tetris[ currentPiece[n] ][ currentPiece[n+1] ] = 1;
          //because of data structure choice need to increment by 2 with n
          n++;
        }
        return;
    }
    //because of data structure choice need to increment by 2 with n
    n++;
  }

  //check against tetris array is complete successfully
  //reset elements in tetris array
  //all newPiece locations to 1
  for(n=0;n<8;n++){
    tetris[ newPiece[n] ][ newPiece[n+1] ] = 1;
    //because of data structure choice need to increment by 2 with n
    n++;
  }

  //copy contents of newPiece to current to complete update
  memcpy(currentPiece, newPiece , sizeof(newPiece) );
}


void checkGame(){
  //understanding of tetris?
  //if there is any cell which has another cell at a lower position (x) than it that is not part of currentPiece
  //then it can't be filled and the game is over
  int c;
  int d;
  int e;
  int f,g;
  int flag = 1;
  int check;
  for(d=0;d<4;d++){
//    reset check for that row
    check = 0;
    for(c=23;c>=10;c--){
      //if empty cell, need to check there isn't a full one below it
      if( tetris[c][d] == 0 ){
        //row a point has been found infront of the other piece
        check = 1;
      }
      
      if( (check == 1) && (tetris[c][d] == 1) ){
        
        //check it isn't part of currentPiece
        
        for(e=0;e<8;e++){
          if( (c == currentPiece[e]) && (d == currentPiece[e+1]) ){
            flag = 0;
          e++;
          }
        }  
        
        if(flag == 1){
          //clear board
            for(f=0;f<24;f++){
              for(g=0;g<4;g++){
                tetris[f][g] = 0;
              }
            }
            lcd.print("Game Over");
            count = 0;
            delay(1000);
            return;
        } 
      }
    }
  }
}


void refreshLCD(){
  for(i=4;i<24;i++){
    for(j=0;j<4;j++){
      if(tetris[i][j] == 1){
        lcd.setCursor(i-4,j);
        lcd.write( byte(0) );
      }
      else{
        lcd.setCursor(i-4,j);
        lcd.write( byte(1) );
      }  
    }
  }
}

void setup() {
  byte block[8] = {
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111
   };
   
   byte nothing[8] = {
      B00000,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000
   };
  lcd.createChar(0, block);
  lcd.createChar(1, nothing);
  lcd.begin(20, 4);
  Serial.begin(9600);
  randomSeed(analogRead(0));
  pinMode(buttonLeftPin, INPUT);
  pinMode(buttonRightPin, INPUT);
}

void loop() {
  if( currentPieceBoolean == 0 ){;
    generatePiece();
    currentPieceBoolean = 1;
    count = count + 30;
    if(count > 400){
      lcd.print("YOU WIN!!");
      delay(2000);
    }
  }
  updateCurrentPiece(motion, 1);
  checkGame();
  refreshLCD();
  //respond to user inputs
  //dynamically alter wait time before next piece is added or number of iterations?
  //how to interupt for a user input? or loop so quick who cares?
  buttonLeft = digitalRead(buttonLeftPin);
  buttonRight = digitalRead(buttonRightPin);
  Serial.print(buttonRight);
  Serial.print(buttonLeft);
  if(buttonLeft == LOW){
    Serial.print('\n');
    Serial.print("1");
    updateCurrentPiece( randomY, 1);
  }
  if(buttonRight == LOW ){
    Serial.print('\n');
    Serial.print("0");
    updateCurrentPiece( randomY, -1);
  }
  
  delay(450-count);
}


//how to serial print...
//started printing letters? idekwtf
//  Serial.print('\n');
//  for(i=0;i<8;i++){
//    Serial.print(currentPiece[i]);
//  }



