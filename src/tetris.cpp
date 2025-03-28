#include "tetris.h"
#include "macros.h"

Point::Point(int8_t x, int8_t y)
	: x(x), y(y)
{}

Point Point::operator+(Point p)
{
	return Point(x + p.x, y + p.y);
}

Point Point::operator-(Point p)
{
	return Point(x - p.x, y - p.y);
}

uint16_t tile_type_to_color(TileType tile_type)
{
    switch (tile_type)
    {
    case TileType::EMPTY: return rgb(20, 20, 20);
    case TileType::CYAN: return rgb(47, 153, 235);
    case TileType::BLUE: return rgb(5, 73, 121);
    case TileType::ORANGE: return rgb(255, 170, 0);
    case TileType::YELLOW: return rgb(219, 193, 26);
    case TileType::GREEN: return rgb(21, 133, 56);
    case TileType::PURPLE: return rgb(89, 18, 117);
    case TileType::RED: return rgb(148, 18, 44);
    case TileType::PINK: return rgb(158, 29, 184);
    case TileType::TURQUOISE: return rgb(19, 123, 125);
    default: return rgb(255, 255, 255);
    }
}

WallKickData::WallKickData(std::initializer_list<Point> wall_kick_data)
	: m_WallKickData(wall_kick_data.begin())
{}

Point WallKickData::get(uint8_t test_num, RotationState rotation)
{
	return m_WallKickData[test_num + (uint8_t)rotation * 4];
}

TetrominoShape::TetrominoShape(uint16_t shape_data, uint8_t size, TileType type, WallKickData wall_kick_data)
	: m_ShapeData(shape_data)
	, m_Size(size)
	, m_Type(type)
	, m_WallKickData(wall_kick_data)
{}

bool TetrominoShape::get(uint8_t x, uint8_t y, RotationState rotation) const
{
	switch (rotation)
	{
	case RotationState::STATE_0: return get_at(x, y);
	case RotationState::STATE_R: return get_at(y, m_Size - x - 1);
	case RotationState::STATE_2: return get_at(m_Size - x - 1, m_Size - y - 1);
	case RotationState::STATE_L: return get_at(m_Size - y - 1, x);
	default: return false;
	}
}

bool TetrominoShape::get_at(uint8_t x, uint8_t y) const
{
	return m_ShapeData & (1 << (x + y * m_Size));
}

Tetromino::Tetromino(const TetrominoShape* shape, TFT_eSPI* tft)
	: m_Rotation(RotationState::STATE_0)
	, m_Shape(shape)
    , m_Tft(tft)
{}

void Tetromino::erase() const
{
	for (uint8_t y = 0; y < m_Shape->size(); ++y)
	{
		for (uint8_t x = 0; x < m_Shape->size(); ++x)
		{
			if (get(x, y))
			{
                m_Tft->fillRect(TETRIS_SPACE_LEFT + (x + m_Position.x) * 10, (y + m_Position.y) * 10, 10, 10, tile_type_to_color(TileType::EMPTY));
			}
		}
	}
}

void Tetromino::draw() const
{
	for (uint8_t y = 0; y < m_Shape->size(); ++y)
	{
		for (uint8_t x = 0; x < m_Shape->size(); ++x)
		{
			if (get(x, y))
			{
                m_Tft->fillRect(TETRIS_SPACE_LEFT + (x + m_Position.x) * 10, (y + m_Position.y) * 10, 10, 10, tile_type_to_color(m_Shape->type()));
			}
		}
	}
}

bool Tetromino::get(uint8_t x, uint8_t y) const
{
	return m_Shape->get(x, y, m_Rotation);
}

void Tetromino::rotate_clockwise()
{
	uint8_t new_rotation = ((uint8_t)m_Rotation + 1) % 4;

	m_Rotation = RotationState(new_rotation);
}

void Tetromino::rotate_counterclockwise()
{
	rotate_clockwise();
	rotate_clockwise();
	rotate_clockwise();
}

void Tetromino::move_to_center_x()
{
	m_Position.x = (TETRIS_GAME_BOARD_WIDTH - m_Shape->size()) / 2;
}

TetrisBoard::TetrisBoard(TFT_eSPI& tft)
    : m_Tft(tft)
{}

void TetrisBoard::draw() const
{
	for (uint8_t y = 0; y < TETRIS_GAME_BOARD_HEIGHT; ++y)
	{
		for (uint8_t x = 0; x < TETRIS_GAME_BOARD_WIDTH; ++x)
		{
            m_Tft.fillRect(TETRIS_SPACE_LEFT + x * 10, y * 10, 10, 10, tile_type_to_color(get(x, y)));
		}
	}
}

void TetrisBoard::clear()
{
	for (uint16_t i = 0; i < TETRIS_GAME_BOARD_WIDTH * TETRIS_GAME_BOARD_HEIGHT; ++i)
	{
		m_BoardData[i] = TileType::EMPTY;
	}
}

void TetrisBoard::insert_tetromino(const Tetromino& tetromino)
{
	for (uint8_t y = 0; y < tetromino.size(); ++y)
	{
		for (uint8_t x = 0; x < tetromino.size(); ++x)
		{
			if (tetromino.get(x, y))
			{
				uint8_t pos_x = tetromino.pos().x + x;
				uint8_t pos_y = tetromino.pos().y + y;

				set(pos_x, pos_y, tetromino.shape()->type());
			}
		}
	}
}

bool TetrisBoard::does_tetromino_fit(const Tetromino& tetromino)
{
	for (uint8_t y = 0; y < tetromino.size(); ++y)
	{
		for (uint8_t x = 0; x < tetromino.size(); ++x)
		{
			if (tetromino.get(x, y))
			{
				uint8_t pos_x = tetromino.pos().x + x;
				uint8_t pos_y = tetromino.pos().y + y;

				if (pos_x < 0 || pos_x >= TETRIS_GAME_BOARD_WIDTH ||
					pos_y < 0 || pos_y >= TETRIS_GAME_BOARD_HEIGHT)
				{
					return false;
				}

				if (tetromino.get(x, y) && get(pos_x, pos_y) != TileType::EMPTY)
				{
					return false;
				}
			}
		}
	}

	return true;
}

TileType TetrisBoard::get(uint8_t x, uint8_t y) const
{
	return m_BoardData[x + y * TETRIS_GAME_BOARD_WIDTH];
}


void TetrisBoard::set(uint8_t x, uint8_t y, TileType type)
{
	m_BoardData[x + y * TETRIS_GAME_BOARD_WIDTH] = type;
}

bool TetrisBoard::is_full_row(uint8_t row)
{
	for (uint8_t x = 0; x < TETRIS_GAME_BOARD_WIDTH; x++)
	{
		if (get(x, row) == TileType::EMPTY)
		{
			return false;
		}
	}

	return true;
}

uint16_t TetrisBoard::clear_lines()
{
	uint8_t cleared = 0;

	for (int16_t y = TETRIS_GAME_BOARD_HEIGHT - 1; y >= 0; --y)
	{
		if (is_full_row((uint8_t)y))
		{
			clear_row((uint8_t)y);

			++cleared;
		}
		else if (cleared > 0)
		{
			move_row_down((uint8_t)y, cleared);
		}
	}

	return cleared;
}

void TetrisBoard::clear_row(uint8_t row)
{
	for (uint8_t x = 0; x < TETRIS_GAME_BOARD_WIDTH; ++x)
	{
		m_BoardData[x, row] = TileType::EMPTY;
	}
}

void TetrisBoard::move_row_down(uint8_t row, uint8_t num_rows)
{
	for (uint8_t x = 0; x < TETRIS_GAME_BOARD_WIDTH; ++x)
	{
		set(x, row + num_rows, get(x, row));
		set(x, row, TileType::EMPTY);
	}
}

TetrisGame::TetrisGame(TFT_eSPI& tft)
	: m_GameBoard(tft)
    , m_CurrentTetromino(nullptr, nullptr)
    , m_IsCurrentGrounded(false)
    , m_CanSwapHold(true)
    , m_GameOver(false)
    , m_Score(0)
    , m_Level(0)
    , m_LinesClearedThisLevel(0)
    , m_Delay(0)
    , m_TimeSinceGravity(0)
    , m_Tft(tft)
{
	restart_game();
}

void TetrisGame::init() {}

void TetrisGame::update(uint8_t fps)
{
    if(m_TimeSinceGravity >= 10)
    {
        m_TimeSinceGravity = 0;

        update_game();    
    }

    ++m_TimeSinceGravity;
}
    
void TetrisGame::slow_update(uint8_t fps) {}

void TetrisGame::touch_input(uint16_t x, uint16_t y) {}

void TetrisGame::button_input(Button button, ButtonAction mode)
{
	if (m_GameOver)
	{
		return;
	}

	m_CurrentTetromino.erase();

    if(button == Button::A && mode == ButtonAction::PRESSED)
    {
        drop_current();
		place_down_tetromino();
		m_GameBoard.draw();
        m_TimeSinceGravity = 0; 
    }
    else if(button == Button::B && mode == ButtonAction::PRESSED)
    {
        try_rotate();
    }
    else if(button == Button::LEFT && mode != ButtonAction::RELEASED)
    {
        try_move_current({ -1, 0 }); 
    }
    else if(button == Button::RIGHT && mode != ButtonAction::RELEASED)
    {
        try_move_current({ 1, 0 });    
    }
    else if(button == Button::UP && mode == ButtonAction::PRESSED)
    {
        try_swap_hold(); 
    }
    else if(button == Button::DOWN && mode != ButtonAction::RELEASED)
    {
        try_move_current({ 0, 1 }); 
        m_TimeSinceGravity = 0;        
    }

	m_CurrentTetromino.draw();
}

ApplicationType TetrisGame::get_app_type() const
{
    return ApplicationType::TETRIS;
}

void TetrisGame::update_game()
{
    if(m_GameOver)
    {
        restart_game();
    }

	m_CurrentTetromino.erase();

	if (m_IsCurrentGrounded)
	{
		m_IsCurrentGrounded = false;

		if (!try_move_current({ 0, 1 }))
		{
			place_down_tetromino();
			m_GameBoard.draw();

			m_IsCurrentGrounded = false;
		}
	}
	else if (!try_move_current({ 0, 1 }))
	{
		m_IsCurrentGrounded = true;
	}

	m_CurrentTetromino.draw();  
}

void TetrisGame::restart_game()
{
	m_GameOver = false;
	m_CanSwapHold = true;

	m_Score = 0;
	m_Level = 1;
	m_LinesClearedThisLevel = 0;
    m_TimeSinceGravity = 0;

	update_delay();

	m_HoldShape = nullptr;
	m_NextShape = random_shape();
	m_CurrentTetromino = Tetromino(random_shape(), &m_Tft);
	m_CurrentTetromino.move_to_center_x();

	m_GameBoard.clear();

    m_Tft.fillRect(0, 0, m_Tft.width(), m_Tft.height(), rgb(0, 0, 0));

    m_GameBoard.draw();
}

void TetrisGame::try_swap_hold()
{
	if (!m_CanSwapHold)
	{
		return;
	}

	m_CanSwapHold = false;

	if (m_HoldShape == nullptr)
	{
		m_HoldShape = m_CurrentTetromino.shape();
		m_CurrentTetromino = Tetromino(m_NextShape, &m_Tft);
		m_NextShape = random_shape();
	}
	else
	{
		const TetrominoShape* hold = m_HoldShape;

		m_HoldShape = m_CurrentTetromino.shape();
		m_CurrentTetromino = Tetromino(hold, &m_Tft);

		m_CurrentTetromino.move_to_center_x();
	}
}

void TetrisGame::try_rotate()
{
	WallKickData wall_kick_data = m_CurrentTetromino.shape()->wall_kick_data();
	RotationState rotation_state = m_CurrentTetromino.rot();
	Point pos_before = m_CurrentTetromino.pos();

	m_CurrentTetromino.rotate_clockwise();

	if (!wall_kick_data.empty())
	{
		uint8_t current_try = 0;

		while (!m_GameBoard.does_tetromino_fit(m_CurrentTetromino) && current_try < 4)
		{
			m_CurrentTetromino.set_pos(pos_before + wall_kick_data.get(current_try, rotation_state));
			++current_try;
		}

		if (!m_GameBoard.does_tetromino_fit(m_CurrentTetromino))
		{
			m_CurrentTetromino.rotate_counterclockwise();
			m_CurrentTetromino.set_pos(pos_before);
		}
	}
}

void TetrisGame::place_down_tetromino()
{
	m_CanSwapHold = true;

	m_GameBoard.insert_tetromino(m_CurrentTetromino);
	
	update_score(m_GameBoard.clear_lines());
	
	m_CurrentTetromino = Tetromino(m_NextShape, &m_Tft);
	m_NextShape = random_shape();
	
	m_CurrentTetromino.move_to_center_x();

	if (!m_GameBoard.does_tetromino_fit(m_CurrentTetromino))
	{
		m_GameOver = true;
	}
}

const TetrominoShape* TetrisGame::random_shape()
{
	return &shapes[rand() % (sizeof(shapes) / sizeof(TetrominoShape))];
}

void TetrisGame::update_delay()
{
	m_Delay = TETRIS_INITIAL_DELAY - (m_Level - 1) * 5;

	if (m_Delay < TETRIS_MIN_DELAY) 
	{ 
		m_Delay = TETRIS_MIN_DELAY; 
	}
}

void TetrisGame::update_score(uint16_t lines)
{
	m_LinesClearedThisLevel += lines;

	switch (lines)
	{
	case 1: m_Score += 100 * m_Level; break;
	case 2: m_Score += 300 * m_Level; break;
	case 3: m_Score += 500 * m_Level; break;
	default: m_Score += 200 * m_Level; break;
	}

	if (m_LinesClearedThisLevel >= TETRIS_LINES_PER_LEVEL)
	{
		++m_Level;
		m_LinesClearedThisLevel %= TETRIS_LINES_PER_LEVEL;

		update_delay();
	}
}

void TetrisGame::drop_current()
{
	while (try_move_current({ 0, 1 })) {}
}

bool TetrisGame::try_move_current(Point delta_pos)
{
	Point pos = m_CurrentTetromino.pos();

	m_CurrentTetromino.set_pos(pos + delta_pos);

	if (!m_GameBoard.does_tetromino_fit(m_CurrentTetromino))
	{
		m_CurrentTetromino.set_pos(pos);
	
		return false;
	}

	return true;
}