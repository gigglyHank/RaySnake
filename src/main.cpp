#include <iostream>
#include <deque>
#include "raylib.h"
#include "raymath.h"

// TODO: make a timer for the powerups

using namespace std;

Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43, 51, 24, 255 };

int cellSize = 30;
int cellCount = 25;
int offset = 75;
int width_height = 2 * offset + cellSize * cellCount;
int centerXY = width_height / 2;
int doubleCellSize = cellSize * 2;
int textPosX = centerXY - doubleCellSize;
int textPlayY = centerXY - doubleCellSize;
int textExitY = centerXY + doubleCellSize;

double lastUpdateTime = 0;
bool eventTriggered( double interval )
{
    double currentTime = GetTime();
    if( currentTime - lastUpdateTime >= interval )
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for( unsigned int i = 0; i < deque.size(); i++ )
    {
        if( Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

enum PowerState { Hidden, Visible, Active };
enum State { MENU, PLAY, PAUSE };

class Obj {
public:
    Vector2 position;
    Texture2D texture;
    PowerState state;

    Obj( const std::string& img, deque<Vector2> snakeBody )
    {
        std::string img_path = "res/";
        std::string img_full = img_path + img;
        Image image = LoadImage( img_full.c_str() );
        texture = LoadTextureFromImage( image );
        UnloadImage( image );
        position = GenerateRandomPos(snakeBody);
    }
    virtual ~Obj()
    {
        if( texture.id != 0 ) UnloadTexture(texture);
        std::cout << "[ OBJ ] textured unloaded" << std::endl;
    }

    void Draw() const
    {
        DrawTexture( texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE );
    }

    Vector2 GenerateRandomCell() const
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x,y};
    }

    Vector2 GenerateRandomPos( deque<Vector2> snakeBody ) const
    {
        Vector2 position = GenerateRandomCell();
        while( ElementInDeque( position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Food : public Obj {
public:
    Food( const std::string& img, deque<Vector2> snakeBody)
        : Obj( img, snakeBody )
    { }
};

class DoublePts : public Obj {
public:
    double stateStartTime = 0;

    DoublePts( const std::string& img, deque<Vector2> snakeBody )
        : Obj( img, snakeBody )
    {
        float xy = cellCount * -2;
        position = { xy, xy };
        state = PowerState::Hidden;
    }

    void Update( deque<Vector2> snakeBody )
    {
        double currentTime = GetTime();
        switch( state )
        {
            case PowerState::Hidden:
                if( currentTime - stateStartTime >= 30.0 )
                {
                    state = PowerState::Visible;
                    position = GenerateRandomPos( snakeBody );
                    stateStartTime = currentTime;
                    std::cout << "[ DOUBLE PTS ] spawned" << std::endl;
                }
                break;
            case PowerState::Visible:
                if( Vector2Equals( snakeBody[ 0 ], position ) )
                {
                    state = PowerState::Active;
                    stateStartTime = currentTime;
                    position = { -2 * (float)cellCount, -2 * (float)cellCount };
                    std::cout << "[ DOUBLE PTS ] collected" << std::endl;
                }
                else if( currentTime - stateStartTime >= 30.0 )
                {
                    state = PowerState::Hidden;
                    stateStartTime = currentTime;
                    position = { -2 * (float)cellCount, -2 * (float)cellCount };
                    std::cout << "[ DOUBLE PTS ] despawned" << std::endl;
                }
                break;
            case PowerState::Active:
                if( currentTime - stateStartTime >= 20.0 )
                {
                    state = PowerState::Hidden;
                    stateStartTime = currentTime;
                    std::cout << "[ DOUBLE PTS ] expired" << std::endl;
                }
                break;
            default: break;
        }
        /*
        if( active )
        {
            if( isActive( 20.0 ) )
            {
                active = false;
                countVisible = GetTime();
            }
        }
        else {
            if( isVisible( 30.0 ) )
            {
                visible = !visible;
                if( visible ) 
                {
                    position = GenerateRandomPos( snakeBody );
                    std::cout << "spawned" << std::endl;
                }
                else {
                    float xy = cellCount * -2;
                    position = { xy, xy };
                    std::cout << "despawned" << std::endl;
                }
            }
        }
        */
    }

    void Reset()
    {
        state = PowerState::Hidden;
        stateStartTime = GetTime();
        position = { -2 * (float)cellCount, -2 * (float)cellCount };
    }

};

class Snake {
public:
    deque<Vector2> body = { Vector2{6,9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1, 0 };
    bool addSegment = false;

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if( addSegment )
        {
            addSegment = false;
        }
        else {
            body.pop_back();
        }

    }

    void Draw()
    {
        for(unsigned int i = 0; i < body.size(); i++ )
        {
            float x = body[ i ].x;
            float y = body[ i ].y;
            Rectangle segment = Rectangle{ offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize };
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Reset()
    {
        body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
        direction = { 1, 0 };
    }
};


class Game {
public:
    Snake snake = Snake();

    Food food = Food( "food.png", snake.body );
    DoublePts doublePts = DoublePts( "doublePts.png", snake.body );

    bool running = false; // game's logic
    bool is_running = true; // program's logic (do better with this)
    int score = 0;
    Sound eatSound;
    Sound wallSound;
    State currentState = MENU;
    Rectangle gameCursor = {
        (float)textPosX - doubleCellSize + 15,
        (float)textPlayY, (float)cellSize, (float)cellSize
    };

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("res/eat.mp3");
        wallSound = LoadSound("res/wall.mp3");
        SetMasterVolume( 0.0f ); // 0.0f = silent, 1.0f = full volume
    }
    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Update()
    {
        if( running && currentState == PLAY )
        {
            doublePts.Update( snake.body );
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void Draw()
    {
        doublePts.Draw();
        food.Draw();
        snake.Draw();
    }

    void CheckCollisionWithFood()
    {
        if( Vector2Equals(snake.body[0], food.position) )
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            if( doublePts.state == PowerState::Active ) score += 2;
            else score++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        if( snake.body[0].x == cellCount || snake.body[0].x == -1 )
        {
            GameOver();
        }
        if( snake.body[0].y == cellCount || snake.body[0].y == -1 )
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        doublePts.Reset();
        food.position = food.GenerateRandomPos( snake.body );
        running = false;
        score = 0;
        PlaySound(wallSound);
        currentState = MENU;
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if( ElementInDeque(snake.body[0], headlessBody ) )
        {
            GameOver();
        }
    }

    void MainMenu()
    {
        // update
        if( eventTriggered( 0.2 ) )
        {
            Update();
        }
        // input
        if( IsKeyPressed( KEY_ESCAPE ) ) is_running = false;

        if( IsKeyPressed( KEY_UP ) && gameCursor.y == textExitY )
        {
            gameCursor.y = centerXY - doubleCellSize;
        }
        if( IsKeyPressed( KEY_DOWN ) && gameCursor.y == textPlayY )
        {
            gameCursor.y = centerXY + doubleCellSize;
        }
        if( IsKeyPressed( KEY_ENTER ) && gameCursor.y == textPlayY )
        {
            // change state
            currentState = PLAY;
        }
        if( IsKeyPressed( KEY_ENTER ) && gameCursor.y == textExitY )
        {
            // quit game
            is_running = false;
        }
        // draw
        DrawText( "PLAY", textPosX, textPlayY, 40, darkGreen );
        DrawText( "EXIT", textPosX, textExitY, 40, darkGreen );
        DrawRectangleRounded(gameCursor, 0.5, 6, darkGreen);
    }

    void Playing()
    {
        if( eventTriggered( 0.2 ) )
        {
            Update();
        }

        if( IsKeyPressed( KEY_ESCAPE ) ) is_running = false;
        if( IsKeyPressed( KEY_P ) )
        {
            currentState = PAUSE;
            running = !running;
        }

        if( IsKeyPressed( KEY_UP ) && snake.direction.y != 1 )
        {
            snake.direction = { 0, -1 };
            running = true;
        }
        if( IsKeyPressed( KEY_DOWN ) && snake.direction.y != -1 )
        {
            snake.direction = { 0, 1 };
            running = true;
        }
        if( IsKeyPressed( KEY_LEFT ) && snake.direction.x != 1 )
        {
            snake.direction = { -1, 0 };
            running = true;
        }
        if( IsKeyPressed( KEY_RIGHT ) && snake.direction.x != -1 )
        {
            snake.direction = { 1, 0 };
            running = true;
        }

        Draw();
    }

    void Pause()
    {
        if( IsKeyPressed( KEY_ESCAPE ) ) is_running = false;

        if( IsKeyPressed( KEY_UP ) && gameCursor.y == textExitY )
        {
            gameCursor.y = centerXY - doubleCellSize;
        }
        if( IsKeyPressed( KEY_DOWN ) && gameCursor.y == textPlayY )
        {
            gameCursor.y = centerXY + doubleCellSize;
        }
        if( IsKeyPressed( KEY_ENTER ) && gameCursor.y == textPlayY )
        {
            // change state
            currentState = PLAY;
            running = true;
        }
        if( IsKeyPressed( KEY_ENTER ) && gameCursor.y == textExitY )
        {
            // quit game
            is_running = false;
        }

        DrawText( "CONTINUE", textPosX, textPlayY, 40, darkGreen );
        DrawText( "EXIT", textPosX, textExitY, 40, darkGreen );
        DrawRectangleRounded(gameCursor, 0.5, 6, darkGreen);
    }

};

int main( void ) {

  SetTraceLogLevel(LOG_NONE);
  SetTargetFPS(60);

  printf("Starting game...\n");
  InitWindow(width_height, width_height, "Retro Snake");

  {
      Game game = Game();

      while( game.is_running )
      {
          BeginDrawing();
          switch( game.currentState )
          {
              case MENU:
                  game.MainMenu();
                  break;
              case PLAY:
                  game.Playing();
                  break;
              case PAUSE:
                  game.Pause();
                  break;
              default: break;
          }

          // drawing
          ClearBackground( green );
          DrawRectangleLinesEx( Rectangle{ (float)offset - 5, (float)offset-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10 }, 5, darkGreen);

          DrawText( "Retro Snake", offset - 5, 20, 40, darkGreen );
          DrawText( TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen );
          
          if( game.running )
          {
            DrawText(
                TextFormat(
                    "Double Points: %s",
                    game.doublePts.state == PowerState::Active ? "Active" : "Deactive"
                ),
                cellSize * 15,
                cellSize + 15,
                20,
                darkGreen
            );
          }

          DrawText( "UP/DOWN/LEFT/RIGHT - Move Snake/Cursor",
            offset + doubleCellSize,
            offset + cellSize * cellCount + 10,
            20, darkGreen
          );
          DrawText( "ENTER - Select",
            offset + doubleCellSize,
            offset + cellSize * cellCount + 30,
            20, darkGreen
          );

          DrawText( "P - Pause",
            offset + ( doubleCellSize * 5 ),
            offset + cellSize * cellCount + 30,
            20, darkGreen
          );

          EndDrawing();
      }
  }

  CloseWindow();
  return 0;
}
