module;

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <unordered_set>
#include <vector>

export module Square;

// Represents a list of squares (pos_board indices) that can
// be travelled to in a single direction
using TravelVec = std::vector<int8_t>;

// returns a TravelVec with all squares to the north
consteval TravelVec north_travel(int8_t &square) {
  TravelVec travel;
  for (int8_t i = square - 8; i >= 0; i -= 8) {
    travel.push_back(i);
  }
  return travel;
}

// returns a TravelVec with all squares to the south
consteval TravelVec south_travel(int8_t &square) {
  TravelVec travel;
  for (int8_t i = square + 8; i < 64; i += 8) {
    travel.push_back(i);
  }
  return travel;
}

// returns a TravelVec with all squares to the east
consteval TravelVec east_travel(int8_t &square) {
  // row is quotient, col is remainder
  // pos_board is one-dimensional, this uses 2D logic
  auto [row, col] = std::div(square, 8);
  TravelVec travel;
  for (++col; col < 8; ++col) {
    travel.push_back(row * 8 + col);
  }
  return travel;
}

// returns a TravelVec with all squares to the west
consteval TravelVec west_travel(int8_t &square) {
  auto [row, col] = std::div(square, 8);
  TravelVec travel;
  for (--col; col >= 0; --col) {
    travel.push_back(row * 8 + col);
  }
  return travel;
}

// returns a TravelVec with all squares to the northeast
consteval TravelVec north_east_travel(int8_t &square) {
  auto [row, col] = std::div(square, 8);
  --row;
  TravelVec travel;
  for (++col; row >= 0 && col < 8; ++col) {
    travel.push_back(row * 8 + col);
    --row;
  }
  return travel;
}

// returns a TravelVec with all squares to the southeast
consteval TravelVec south_east_travel(int8_t &square) {
  auto [row, col] = std::div(square, 8);
  ++row;
  TravelVec travel;
  for (++col; row < 8 && col < 8; ++col) {
    travel.push_back(row * 8 + col);
    ++row;
  }
  return travel;
}

// returns a TravelVec with all squares to the southwest
consteval TravelVec south_west_travel(int8_t &square) {
  auto [row, col] = std::div(square, 8);
  ++row;
  TravelVec travel;
  for (--col; row < 8 && col >= 0; --col) {
    travel.push_back(row * 8 + col);
    ++row;
  }
  return travel;
}

// returns a TravelVec with all squares to the northwest
consteval TravelVec north_west_travel(int8_t &square) {
  auto [row, col] = std::div(square, 8);
  --row;
  TravelVec travel;
  for (--col; row >= 0 && col >= 0; --col) {
    travel.push_back(row * 8 + col);
    --row;
  }
  return travel;
}

// returns a TravelVec with all squares that a knight could travel to
consteval TravelVec knight_travel(int8_t &square) {
  auto [row, col] = std::div(square, 8);
  struct pos {
    int row;
    int col;
  };
  pos jumps[8] = {pos{row - 2, col + 1}, pos{row - 2, col - 1},
                  pos{row - 1, col + 2}, pos{row - 1, col - 2},
                  pos{row + 2, col + 1}, pos{row + 2, col - 1},
                  pos{row + 1, col + 2}, pos{row + 1, col - 2}};
  TravelVec travel;
  for (pos jump : jumps) {
    if (jump.row >= 0 && jump.row < 8 && jump.col >= 0 && jump.col < 8) {
      travel.push_back(jump.row * 8 + jump.col);
    }
  }
  return travel;
}

// VecView is read-only; it points directly to a TravelVec's heap memory
// VecView MUST be initialized after the vectors are done growing
// VecView MUST be deleted before TravelVecs to avoid dangling pointers
using VecView = std::span<const int8_t>;

using SquareSet = std::unordered_set<int8_t>;

struct Tactical {
  SquareSet att_by_set;
  SquareSet def_by_set;
  SquareSet att_set;
  SquareSet def_set;
};

export class Square {
private:
  int8_t square;
  TravelVec north;
  TravelVec north_east;
  TravelVec east;
  TravelVec south_east;
  TravelVec south;
  TravelVec south_west;
  TravelVec west;
  TravelVec north_west;
  TravelVec knight;
  std::array<VecView, 4> rook_vec_views;
  std::array<VecView, 4> bishop_vec_views;
  std::array<VecView, 8> queen_vec_views;
  // default destructor uses reverse-order of initialization

public:
  consteval Square(int8_t square)
      : square(square), north(north_travel(square)),
        north_east(north_east_travel(square)), east(east_travel(square)),
        south_east(south_east_travel(square)), south(south_travel(square)),
        south_west(south_west_travel(square)), west(west_travel(square)),
        north_west(west_travel(square)), knight(knight_travel(square)),
        rook_vec_views({north, east, south, west}),
        bishop_vec_views({north_east, south_east, south_west, north_west}),
        queen_vec_views({north, east, south, west, north_east, south_east,
                         south_west, north_west}) {}

  // find defenders and attackers using the VecView
  // Modifies the Tactical
  void passiveScanView(const int8_t &color, Tactical &tact,
                       const std::array<int8_t, 64> &pos_board,
                       const VecView &vec_view) {
    for (int8_t square : vec_view) {
      int8_t piece = pos_board[square];
      if (piece & 0b11000000 && piece & 0b00010010) {
        if (piece & color) {
          tact.def_by_set.insert(square);
          break;
        } else {
          tact.att_by_set.insert(square);
          break;
        }
      }
    }
  }

  // find defenders and attackers north, south, east, and west
  // Modifies the Tactical
  void rookPassiveScan(const int8_t &color, Tactical &tact,
                       const std::array<int8_t, 64> &pos_board) {
    for (const auto &vec_view : rook_vec_views) {
      passiveScanView(color, tact, pos_board, vec_view);
    }
  }

  // find defenders and attackers diagonally
  // Modifies the Tactical
  void bishopPassiveScan(const int8_t &color, Tactical &tact,
                         const std::array<int8_t, 64> &pos_board) {
    for (const auto &vec_view : bishop_vec_views) {
      passiveScanView(color, tact, pos_board, vec_view);
    }
  }

  // find defenders and attackers horizontally, vertically, and diagonally
  // Modifies the Tactical
  void queenPassiveScan(const int8_t &color, Tactical &tact,
                        const std::array<int8_t, 64> &pos_board) {
    for (const auto &vec_view : queen_vec_views) {
      passiveScanView(color, tact, pos_board, vec_view);
    }
  }
};
