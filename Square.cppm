module;

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <span>
#include <stdexcept>
#include <unordered_set>

export module Square;

enum class Piece : uint8_t {
  empty,  // 0
  pawn,   // 1
  rook,   // 2
  knight, // 3
  bishop, // 4
  queen,  // 5
  king,   // 6
};

export const uint8_t black = 0b00010000;

// Yes, I know what a vector is
// An array of vectors won't have inline memory
// 8 bytes
struct TravelBytes {
  uint8_t data[7]{};
  uint8_t used = 0;

  std::span<const uint8_t> view() const { return {data, used}; }
};

using Travel = std::array<TravelBytes, 10>;

// Should be 64 bytes
export class Square {
private:
  // Like a clock
  // 0 - North, 1 - East, 2 - South, 3 - West
  // 4 - NorthEast, 5 - SouthEast, 6 - SouthWest, 7 - NorthWest
  // 8 - Knight, 9 - King
  // WARNING: broken!!!! Knight and king use 8 bytes max instead of 7!!
  Travel square_travel;

  constexpr Travel all_travel(uint8_t &square) {
    Travel square_travel;

    north_travel(square, square_travel[0]);
    east_travel(square, square_travel[1]);
    south_travel(square, square_travel[2]);
    west_travel(square, square_travel[3]);

    north_east_travel(square, square_travel[4]);
    south_east_travel(square, square_travel[5]);
    south_west_travel(square, square_travel[6]);
    north_west_travel(square, square_travel[7]);

    knight_travel(square, square_travel[8]);
    king_travel(square, square_travel[9]);

    return square_travel;
  }

  // If a rook was on this square, which squares would be north of him?
  // Let's precompute the travel squares
  // Also, the chess board is a one-dimensional array
  // Each row on the board is appended top to bottom
  // If I subtract 8, I have moved north
  constexpr void north_travel(uint8_t &square, TravelBytes &north) {
    uint8_t i = 0;
    for (uint8_t pos = square - 8; pos >= 0; pos -= 8) {
      north.data[i] = pos;
      i++;
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    north.used = i;
  }

  constexpr void south_travel(uint8_t &square, TravelBytes &south) {
    uint8_t i = 0;
    for (uint8_t pos = square + 8; pos < 64; pos += 8) {
      south.data[i] = pos;
      i++;
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    south.used = i;
  }

  constexpr void east_travel(uint8_t &square, TravelBytes &east) {
    uint8_t i = 0;
    // quotient = row, remainder = col
    auto [row, col] = std::div(square, 8);
    while (col < 8) {
      col++;
      east.data[i] = row * 8 + col;
      i++;
    }
    i--;
    if (i > 7)
      throw std::out_of_range("Size too big!");
    east.used = i;
  }

  constexpr void west_travel(uint8_t &square, TravelBytes &west) {
    uint8_t i = 0;
    auto [row, col] = std::div(square, 8);
    while (col >= 0) {
      col--;
      west.data[i] = row * 8 + col;
      i++;
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    west.used = i;
  }

  constexpr void north_east_travel(uint8_t &square, TravelBytes &n_e) {
    uint8_t i = 0;
    auto [row, col] = std::div(square, 8);
    const auto size = std::min(row, 7 - col);
    while (i < size) {
      row--;
      col++;
      n_e.data[i] = row * 8 + col;
      i++;
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    n_e.used = i;
  }

  constexpr void south_east_travel(uint8_t &square, TravelBytes &s_e) {
    uint8_t i;
    auto [row, col] = std::div(square, 8);
    const size_t size = std::min(7 - row, 7 - col);
    while (i < size) {
      row++;
      col++;
      s_e.data[i] = row * 8 + col;
      i++;
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    s_e.used = i;
  }

  constexpr void south_west_travel(uint8_t &square, TravelBytes &s_w) {
    uint8_t i;
    auto [row, col] = std::div(square, 8);
    const size_t size = std::min(7 - row, col);
    while (i < size) {
      row++;
      col--;
      s_w.data[i] = row * 8 + col;
      i++;
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    s_w.used = i;
  }

  constexpr void north_west_travel(uint8_t &square, TravelBytes &n_w) {
    uint8_t i;
    auto [row, col] = std::div(square, 8);
    const size_t size = std::min(row, col);
    while (i < size) {
      row++;
      col--;
      n_w.data[i] = row * 8 + col;
      i++;
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    n_w.used = i;
  }

  // WARNING: broken!!!! Knight and king use 8 bytes max instead of 7!!
  constexpr void knight_travel(uint8_t &square, TravelBytes &knight) {
    uint8_t i;
    auto [row, col] = std::div(square, 8);
    struct pos {
      int row;
      int col;
    };
    pos jumps[8] = {pos{row - 2, col + 1}, pos{row - 2, col - 1},
                    pos{row - 1, col + 2}, pos{row - 1, col - 2},
                    pos{row + 2, col + 1}, pos{row + 2, col - 1},
                    pos{row + 1, col + 2}, pos{row + 1, col - 2}};
    for (pos jump : jumps) {
      if (jump.row >= 0 && jump.row < 8 && jump.col >= 0 && jump.col < 8) {
        knight.data[i] = jump.row * 8 + jump.col;
        i++;
      }
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    knight.used = i;
  }

  // WARNING: broken!!!! Knight and king use 8 bytes max instead of 7!!
  constexpr void king_travel(uint8_t &square, TravelBytes &king) {
    uint8_t i;
    auto [row, col] = std::div(square, 8);
    struct pos {
      int row;
      int col;
    };
    pos steps[8] = {pos{row - 1, col}, pos{row - 1, col + 1},
                    pos{row, col + 1}, pos{row + 1, col + 1},
                    pos{row + 1, col}, pos{row + 1, col - 1},
                    pos{row, col - 1}, pos{row - 1, col - 1}};
    for (pos step : steps) {
      if (step.row >= 0 && step.row < 8 && step.col >= 0 && step.col < 8) {
        king.data[i] = step.row * 8 + step.col;
        i++;
      }
    }
    if (i > 7)
      throw std::out_of_range("Size too big!");
    king.used = i;
  }

  // VecView is read-only; it points directly to a TravelVec's heap memory
  // VecView MUST be initialized after the vectors are done growing
  // VecView MUST be deleted before TravelVecs to avoid dangling pointers
  using VecView = std::span<const uint8_t>;

  using SquareSet = std::unordered_set<uint8_t>;

public:
  Square(uint8_t pos) : square_travel(all_travel(pos)) {}

  struct Tactical {
    SquareSet att_by_set;
    SquareSet def_by_set;
    SquareSet pin_by_set;
    SquareSet att_set;
    SquareSet def_set;
    SquareSet pin_set;
  };

  // Declared in main:
  // empty,        // 0000
  // pawn,         // 0001
  // king,         // 0010
  // knight,       // 0011
  // bishop = 4,   // 0100
  // rook = 8,     // 1000
  // queen = 12,   // 1100

  // find defenders and attackers using the VecView
  // Modifies the Tactical
  void passiveScan(const uint8_t &color, Tactical &tact,
                   const std::array<uint8_t, 64> &pos_board,
                   const TravelBytes &direction, const uint8_t &piece_mask) {
    for (uint8_t square : direction.view()) {
      uint8_t piece = pos_board[square];
      printf("square scanned: %02hhx\n", piece);
      if (piece & piece_mask) {
        if (piece & color) {
          tact.def_by_set.insert(square);
          std::cout << "bingo!\n";
          break;
        } else {
          tact.att_by_set.insert(square);
          break;
        }
      }
    }
  }

  void activeScan(const uint8_t &color, Tactical &tact,
                  const std::array<uint8_t, 64> &pos_board,
                  const VecView &vec_view) {}

  // find defenders and attackers north, south, east, and west
  void rookPassiveScan(const uint8_t &color, Tactical &tact,
                       const std::array<uint8_t, 64> &pos_board) {
    printf("rook scan\n");
    for (std::size_t i = 0; i < 4; i++) {
      printf("direction: %lu\n", i);
      passiveScan(color, tact, pos_board, square_travel[i], 0b00001000);
    }
  }

  // find defenders and attackers diagonally
  void bishopPassiveScan(const uint8_t &color, Tactical &tact,
                         const std::array<uint8_t, 64> &pos_board) {
    printf("bishop scan\n");
    for (std::size_t i = 4; i < 8; i++) {
      printf("direction: %lu\n", i);
      passiveScan(color, tact, pos_board, square_travel[i], 0b00000100);
    }
  }

  // find defenders and attackers horizontally, vertically, and diagonally
  void queenPassiveScan(const uint8_t &color, Tactical &tact,
                        const std::array<uint8_t, 64> &pos_board) {
    printf("queen scan, which is...\n");
    rookPassiveScan(color, tact, pos_board);
    bishopPassiveScan(color, tact, pos_board);
  }

  // find defenders and attackers (knights)
  void knightPassiveScan(const uint8_t &color, Tactical &tact,
                         const std::array<uint8_t, 64> &pos_board) {
    printf("knight scan\n");
    passiveScan(color, tact, pos_board, square_travel[8], 0b00000100);
  }

  // find defenders and attackers (kings)
  void kingPassiveScan(const uint8_t &color, Tactical &tact,
                       const std::array<uint8_t, 64> &pos_board) {
    printf("king scan\n");
    passiveScan(color, tact, pos_board, square_travel[9], 0b00100000);
  }
};
