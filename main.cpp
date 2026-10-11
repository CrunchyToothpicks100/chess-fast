#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

import Square;

// clang-format off
static const int8_t pawn =    0b00000001;
static const int8_t rook =    0b00000010;
static const int8_t knight =  0b00000100;
static const int8_t bishop =  0b00001000;
static const int8_t queen =   0b00010000;
static const int8_t king =    0b00100000;
static const int8_t white =   0b01000000;
static const int8_t black =   0b10000000;

consteval std::array<int8_t, 64> make_pos_board() {
  std::array<int8_t, 64> board = {
      rook,  knight,  bishop,  queen,  king,  bishop,  knight,  rook,
      pawn,  pawn,    pawn,    pawn,   pawn,  pawn,    pawn,    pawn,
      0,     0,       0,       0,      0,     0,       0,       0,
      0,     0,       0,       0,      0,     0,       0,       0,
      0,     0,       0,       0,      0,     0,       0,       0,
      0,     0,       0,       0,      0,     0,       0,       0,
      pawn,  pawn,    pawn,    pawn,   pawn,  pawn,    pawn,    pawn,
      rook,  knight,  bishop,  queen,  king,  bishop,  knight,  rook,
  };

  for (int i = 0; i < 16; ++i)
    board[i] |= black;

  for (int i = 48; i < 64; ++i)
    board[i] |= white;

  return board;
}
// clang-format on

void print_pos_board_hex(std::array<int8_t, 64> &pos_board) {
  for (int8_t i = 0; i < 64; ++i) {
    if (i % 8 == 0)
      printf("\n");
    printf("%02hhx ", pos_board[i]);
  }
}

// Represents a list of squares (pos_board indices) that can
// be travelled to in a single direction
using TravelVec = std::vector<int8_t>;
int main() {
  std::array<int8_t, 64> pos_board{make_pos_board()};
  print_pos_board_hex(pos_board);
  return 0;
}
