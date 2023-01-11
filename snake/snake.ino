#include <LedControl.h>
#include <IRremote.h>

int DIN = 12;
int CS = 11;
int CLK = 10;

const int joyX = A0;
const int joyY = A1;
const int joyButton = 13;
const int REMOTE_PIN = 8;

LedControl lc=LedControl(DIN, CLK, CS,0);

typedef enum Direction{
  Top = 0, 
  Right = 1, 
  Bottom = 2, 
  Left = 3,
  Default} ;

struct Point
{
  int x;
  int y;
  Point(int _x, int _y) : x(_x), y(_y){};
  Point() : x(-1), y(-1){};
  void display(bool _state)
  {
    lc.setLed(0, x, y, _state);
  }
};

struct Snake
{
  int size;
  Point snakeBody[64];

  Snake(int _x, int _y)
  {
    size = 3;
    snakeBody[0] = Point(_x, _y);
    snakeBody[1] = Point(_x-1, _y);
    snakeBody[2] = Point(_x-2, _y);
  };
  Snake(){};

  void display(bool _state)
  {
    for(int i = 0 ; i < size; ++i)
    {
      snakeBody[i].display(_state);
    }
  };


  //  Directions
  //
  //     0
  //  3     1
  //     2
  // 
  void moveBody()
  {
    for(int i = size - 1 ; i > 0 ; --i)
    {
      snakeBody[i].x = snakeBody[i-1].x;
      snakeBody[i].y = snakeBody[i-1].y;
    }
  }
  void moveHead(Direction direction)
  {
    Point* p = &snakeBody[0];
    switch(direction)
    {
      case Top:
        p->y = (p->y == 0) ? 7 : p->y - 1 ;
        break;
      case Right:
        p->x = (p->x == 0) ? 7 : p->x - 1 ;
        break;
      case Bottom:
        p->y = (p->y == 7) ? 0 : p->y + 1 ;
        break;
      case Left:
        p->x = (p->x == 7) ? 0 : p->x + 1 ;
        break;
      default:
        break;
    }
  }
  void eatPoint()
  {
    Point last_part = snakeBody[size-1];
    Point second_last_part = snakeBody[size];
    int x = second_last_part.x - last_part.x;
    int y = second_last_part.y - last_part.y;
    snakeBody[size] = Point(last_part.x - x, last_part.y - y);
    size++;
  }

};

struct Gameboard
{
  int FRAME_RATE = 100;
  const int SIZE = 8;
  Point current_point;
  Snake snake;

  Gameboard()
  {
    snake = Snake(SIZE/2, SIZE/2);
  }
  bool point_is_valid(Point new_point, Snake _snake)
  {
    for(int i = 0 ; i < snake.size; ++i)
    {
      Point snake_point = snake.snakeBody[i];
      if (snake_point.x == new_point.x && snake_point.y == new_point.y)
      {
        return false;
      }
    }
    return true;
  }

  void setNewPoint()
  {
    Point new_point;
    do
    {
      new_point = Point(random(8), random(8));
    }
    while(!point_is_valid(new_point, snake));
    current_point = new_point;
  }
  
  void run()
  {
     snake.display(HIGH);
     current_point.display(HIGH);
     delay(FRAME_RATE);
     
     snake.display(LOW);
     current_point.display(LOW);
     delay(FRAME_RATE);
  }

  void action(Direction direction)
  {
    snake.moveBody();
    snake.moveHead(direction);
    
    Point head = snake.snakeBody[0];
    if(head.x == current_point.x && head.y == current_point.y)
    {
      snake.eatPoint();
      setNewPoint();
    }
    for(int i = 1 ; i < snake.size; ++i)
    {
      Point p = snake.snakeBody[i];      
      if(head.x == p.x && head.y == p.y)
      {
        snake = Snake(SIZE/2, SIZE/2);
      }
    }
  }
  void slower()
  {
    FRAME_RATE+=20;
    Serial.print("Current Frame Rate :");
    Serial.println(FRAME_RATE);
  }
  void faster()
  {
    if(FRAME_RATE-20>0)
    {
      FRAME_RATE-=20;
    }
    Serial.print("Current Frame Rate :");
    Serial.println(FRAME_RATE);
  }

};


Gameboard board;
void setup() 
{
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
  Serial.begin(9600);

  pinMode(joyButton, INPUT_PULLUP); 
  IrReceiver.begin(REMOTE_PIN);
  
  board.setNewPoint();
}
const int NB_CONTROL_VALUES = 10;
#define IR_BUTTON_2 24
#define IR_BUTTON_4 8
#define IR_BUTTON_6 90
#define IR_BUTTON_8 82
#define IR_SLOWER 68
#define IR_FASTER 67
#define IR_PLAY_PAUSE 64
//#define IR_BUTTON_PLAY_PAUSE 64
//int correspond_remote_value[NB_CONTROL_VALUES] = {22, 25, 13, 8, 28, 90, 66, 82, 74} ;
//int correspond_user_value[NB_CONTROL_VALUES]   = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9} ;

Direction current_direction = Left;
bool is_paused = false;
void loop() 
{
  bool faster = false;
  bool slower = false;
  Direction new_direction;

  if (IrReceiver.decode()) 
  {
    IrReceiver.resume();
    int command = IrReceiver.decodedIRData.command;
    Serial.println(command);

    new_direction = Default;

    switch (command) 
    {
      case IR_BUTTON_2: 
        Serial.println("Top");
        new_direction = Top;
        break;
      case IR_BUTTON_4: 
        Serial.println("Left");
        new_direction = Left;
        break;
      case IR_BUTTON_6: 
        Serial.println("Right");
        new_direction = Right;
        break;
      case IR_BUTTON_8: 
        Serial.println("Bottom");
        new_direction = Bottom;
        break;
      case IR_SLOWER: 
        slower = true;
        Serial.println("Pressed on button Slower");
        break;
      case IR_FASTER: 
        faster = true;
        Serial.println("Pressed on button Faster");
        break;
      case IR_PLAY_PAUSE: 
        Serial.println("Pressed on button Pause");
        is_paused = !is_paused;
        break;
      default:
        break;
        
      
    }
  }

  if(new_direction == Top && current_direction != Bottom)
  {
    current_direction = new_direction;
  }
  else if(new_direction == Left && current_direction != Right)
  {
    current_direction = new_direction;
  }
  else if(new_direction == Right && current_direction != Left)
  {
    current_direction = new_direction;
  }
  else if(new_direction == Bottom && current_direction != Top)
  {
    current_direction = new_direction;
  }

  if(slower)
  {
    board.slower();
  }
  if(faster)
  {
    board.faster();
  }
  board.run();

  if(!is_paused)
  {
    board.action(current_direction);
  }

}
