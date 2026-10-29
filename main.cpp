#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

import Square;

// clang-format off
consteval std::array<uint8_t, 64> make_pos_board() {
  std::array<uint8_t, 64> board = {
      8,  2,  4, 12, 16,  4,  2,  8,
      1,  1,  1,  1,  1,  1,  1,  1,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
      1,  1,  1,  1,  1,  1,  1,  1,
      8,  2,  4, 12, 16,  4,  2,  8,
  };

  for (int i = 0; i < 16; ++i)
    board[i] |= black;

  return board;
}
// clang-format on

void print_pos_board_hex(std::array<uint8_t, 64> &pos_board) {
  for (uint8_t i = 0; i < 64; ++i) {
    if (i % 8 == 0)
      printf("\n");
    printf("%02hhx ", pos_board[i]);
  }
  printf("\n");
}

// Represents a list of squares (pos_board indices) that can
// be travelled to in a single direction
int main() {
  std::array<uint8_t, 64> pos_board{make_pos_board()};
  print_pos_board_hex(pos_board);

  Square a7{11};
  Square::Tactical a7_tact;

  a7.rookPassiveScan(black, a7_tact, pos_board);
  a7.bishopPassiveScan(black, a7_tact, pos_board);
  a7.knightPassiveScan(black, a7_tact, pos_board);
  a7.kingPassiveScan(black, a7_tact, pos_board);

  std::cout << "\n\n";
  printf("d7 defended by these pieces: \n");
  for (uint8_t square : a7_tact.def_by_set) {
    printf("%02hhx ", pos_board[square]);
  }
  std::cout << "\n\n";
  return 0;
}
