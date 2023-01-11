#include <LedControl.h>

int DIN = 12;
int CS = 11;
int CLK = 10;

const int joyX = A0;
const int joyY = A1;
const int joyButton = 13;

LedControl lc=LedControl(DIN, CLK, CS,0);

typedef enum Direction{
  Top = 0, 
  Right = 1, 
  Bottom = 2, 
  Left = 3} ;

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
  const int FRAME_RATE = 100;
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

};


Gameboard board;
void setup() 
{
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
  Serial.begin(9600);

  pinMode(joyButton, INPUT_PULLUP); 
  
  board.setNewPoint();
}

int cpt = 0;
Direction current_direction = Left;
void loop() 
{
  bool getLeft = digitalRead(LeftButton);
  bool getTop = digitalRead(UpButton);
  bool getRight = digitalRead(RightButton);
  bool getDown = digitalRead(DownButton);
  
  int X = 0;
  int Y = 0;
  X = analogRead(joyX);
  Y = analogRead(joyY);
  Serial.println(X);
  Serial.println(Y);
  if(X > 750 || X < 250 || Y > 750 || Y < 250 )
  {
    if(X < 250 && current_direction != Bottom)
    {
      current_direction = Top;
    }
    else if(X > 750 && current_direction != Top)
    {
      current_direction = Bottom;
    }
    else if(Y < 250 && current_direction != Left)
    {
      current_direction = Right;
    }
    else if(Y > 750 && current_direction != Right)
    {
      current_direction = Left;
    }
  }
  
  else
  {
    Serial.println("test");
  }

  // put your main code here, to run repeatedly:
  board.run();
  
  board.action(current_direction);

}
