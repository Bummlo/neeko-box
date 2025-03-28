#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "application.h"

#define TETRIS_GAME_BOARD_WIDTH     (uint8_t)   10 
#define TETRIS_GAME_BOARD_HEIGHT    (uint8_t)   24
#define TETRIS_SPACE_LEFT           (uint8_t)   110
#define TETRIS_LINES_PER_LEVEL      (uint8_t)   10
#define TETRIS_INITIAL_DELAY        (uint16_t)  48
#define TETRIS_MIN_DELAY            (uint16_t)  5

struct Point
{
    int8_t x, y;

    Point(int8_t x = 0, int8_t y = 0);

    Point operator+(Point p);
    Point operator-(Point p);
};

enum struct TileType : uint8_t
{
    EMPTY,
    CYAN,
    BLUE,
    ORANGE,
    YELLOW,
    GREEN,
    PURPLE,
    RED,
    PINK,
    TURQUOISE
};

static uint16_t tile_type_to_color(TileType tile_type);

enum struct RotationState : uint8_t
{
    STATE_0 = 0,
    STATE_R = 1,
    STATE_2 = 2,
    STATE_L = 3
};

struct WallKickData
{
private:
    const Point* m_WallKickData;

public:
    WallKickData(std::initializer_list<Point> wall_kick_data);

    Point get(uint8_t test_num, RotationState rot);

    inline bool empty() const { return m_WallKickData == nullptr; }
};

struct TetrominoShape
{
private:
    uint16_t m_ShapeData;
    uint8_t m_Size;
    TileType m_Type;
    WallKickData m_WallKickData;

public:
    TetrominoShape(uint16_t shape_data, uint8_t size, TileType type, WallKickData wall_kick_data);

    bool get(uint8_t x, uint8_t y, RotationState rot) const;

    inline uint8_t size() const { return m_Size; }
    inline TileType type() const { return m_Type; }
    inline WallKickData wall_kick_data() const { return m_WallKickData; }

private:
    bool get_at(uint8_t x, uint8_t y) const;
};

static const WallKickData jlstz_wall_kick_data =
{
    { -1,  0 }, { -1, -1 }, {  0,  2 }, { -1,  2 },  // 0->R
    {  1,  0 }, {  1,  1 }, {  0, -2 }, {  1, -2 },  // R->2
    {  1,  0 }, {  1, -1 }, {  0,  2 }, {  1,  2 },  // 2->L
    { -1,  0 }, { -1,  1 }, {  0, -2 }, { -1, -2 }   // L->0
};

static const WallKickData i_wall_kick_data =
{
    { -2,  0 }, {  1,  0 }, { -2,  1 }, {  1, -2 },  // 0->R
    { -1,  0 }, {  2,  0 }, { -1, -2 }, {  2,  1 },  // R->2
    {  2,  0 }, { -1,  0 }, {  2, -1 }, { -1,  2 },  // 2->L
    {  1,  0 }, { -2,  0 }, {  1,  2 }, { -2, -1 }   // L->0
};

static const TetrominoShape shapes[] =
{
    TetrominoShape(0b0000111100000000, 4, TileType::CYAN,   i_wall_kick_data),
    TetrominoShape(0b100111000,        3, TileType::BLUE,   jlstz_wall_kick_data),
    TetrominoShape(0b001111000,        3, TileType::ORANGE, jlstz_wall_kick_data),
    TetrominoShape(0b011110000,        3, TileType::GREEN,  jlstz_wall_kick_data),
    TetrominoShape(0b010111000,        3, TileType::PURPLE, jlstz_wall_kick_data),
    TetrominoShape(0b110011000,        3, TileType::RED,    jlstz_wall_kick_data),
    TetrominoShape(0b1111,             2, TileType::YELLOW, {})
};

struct Tetromino
{
private:
    Point m_Position;
    RotationState m_Rotation;
    const TetrominoShape* m_Shape;
    TFT_eSPI* m_Tft;

public:
    Tetromino(const TetrominoShape* shape, TFT_eSPI* tft);

    void erase() const;
    void draw() const;
    bool get(uint8_t x, uint8_t y) const;
    void rotate_clockwise();
    void rotate_counterclockwise();
    void move_to_center_x();

    inline uint8_t size() const { return m_Shape->size(); }
    inline Point pos() const { return m_Position; }
    inline void set_pos(Point pos) { m_Position = pos; }
    inline RotationState rot() const { return m_Rotation; }
    inline const TetrominoShape* shape() const { return m_Shape; }
};

struct TetrisBoard
{
private:
    TileType m_BoardData[TETRIS_GAME_BOARD_WIDTH * TETRIS_GAME_BOARD_HEIGHT];
    TFT_eSPI& m_Tft;

public:
    TetrisBoard(TFT_eSPI& tft);

    void draw() const;
    void clear();
    void insert_tetromino(const Tetromino& tetromino);
    bool does_tetromino_fit(const Tetromino& tetromino);
    uint16_t clear_lines();

private:
    TileType get(uint8_t x, uint8_t y) const;
    void set(uint8_t x, uint8_t y, TileType type);
    bool is_full_row(uint8_t row);
    void clear_row(uint8_t row);
    void move_row_down(uint8_t row, uint8_t num_rows);
};

struct TetrisGame : Application
{
private:
    TetrisBoard m_GameBoard;
    Tetromino m_CurrentTetromino;
    const TetrominoShape* m_NextShape, *m_HoldShape;
    bool m_IsCurrentGrounded, m_CanSwapHold, m_GameOver;
    uint16_t m_Score, m_Level, m_LinesClearedThisLevel, m_Delay, m_TimeSinceGravity;
    TFT_eSPI& m_Tft;

public:
    TetrisGame(TFT_eSPI& tft);

    virtual void init() override;
    virtual void update(uint8_t fps) override;
    virtual void slow_update(uint8_t fps) override;
    virtual void touch_input(uint16_t x, uint16_t y) override;
    virtual void button_input(Button button, ButtonAction mode) override;

    virtual ApplicationType get_app_type() const override;

private:
    void update_game();
    void restart_game();
    void try_swap_hold();
    void try_rotate();
    void place_down_tetromino();
    const TetrominoShape* random_shape();
    void update_delay();
    void update_score(uint16_t lines);
    void drop_current();
    bool try_move_current(Point delta_pos);
};