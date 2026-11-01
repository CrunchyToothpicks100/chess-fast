#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>

import allMasks;

// clang-format off
/* Macros to extract bits of a single byte */
#define BINARY_PATTERN_8 "%d%d%d%d%d%d%d%d"
#define BINARY_BYTE_TO_BITS(byte)  \
  ((byte) & 0x80 ? 1 : 0), \
  ((byte) & 0x40 ? 1 : 0), \
  ((byte) & 0x20 ? 1 : 0), \
  ((byte) & 0x10 ? 1 : 0), \
  ((byte) & 0x08 ? 1 : 0), \
  ((byte) & 0x04 ? 1 : 0), \
  ((byte) & 0x02 ? 1 : 0), \
  ((byte) & 0x01 ? 1 : 0)

/* Combine 8 bytes into a 64-bit "board" pattern */
#define BINARY_PATTERN_64 \
  BINARY_PATTERN_8 "\n" BINARY_PATTERN_8 "\n" BINARY_PATTERN_8 "\n" BINARY_PATTERN_8 "\n" \
  BINARY_PATTERN_8 "\n" BINARY_PATTERN_8 "\n" BINARY_PATTERN_8 "\n" BINARY_PATTERN_8

#define BINARY_UINT64_TO_BITS(num) \
  BINARY_BYTE_TO_BITS((num) >> 56), \
  BINARY_BYTE_TO_BITS((num) >> 48), \
  BINARY_BYTE_TO_BITS((num) >> 40), \
  BINARY_BYTE_TO_BITS((num) >> 32), \
  BINARY_BYTE_TO_BITS((num) >> 24), \
  BINARY_BYTE_TO_BITS((num) >> 16), \
  BINARY_BYTE_TO_BITS((num) >> 8),  \
  BINARY_BYTE_TO_BITS(num)
// clang-format on

/* We'll focus on a low memory footprint for now
 * Do further optimizations later (giant vectors for mass calculations)
 * Due to the power of left-shifting "1" and static_cast,
 * We will represent positions as an "upside-down 2D array".
 *
 * positions:
 *  63  62  61  60  59  58  57  56
 *  55  54  53  52  51  50  49  48
 *  47  46  45  44  43  42  41  40
 *  39  38  37  36  35  34  33  32
 *  31  30  29  28  27  26  25  24
 *  23  22  21  20  19  18  17  16
 *  15  14  13  12  11  10  9   8
 *  7   6   5   4   3   2   1   0
 */

class Piece {
protected:
  uint8_t type;
  uint8_t pos;
  bool color;
  uint64_t mask;

public:
  uint64_t getMask() const { return mask; }

  void movePos(int8_t delta) {
    uint8_t newPos = pos + delta;
    if (newPos > 63) {
      printf("Error: could not move piece.\n");
      printf("pos: %hhu, delta: %hhd, newPos: %hhu\n", pos, delta, newPos);
      std::exit(1);
    }
    mask = setMask(type, newPos);
    this->pos = newPos; // Set position
  }

  constexpr uint64_t setMask(uint8_t type, uint8_t pos) {
    return maskPiece(type, pos);

    mask = 0;

    if (pos % 8 != 0) {
      mask |= UINT64_C(1) << (pos - 1);
    }
    if (pos % 8 != 7) {
      mask |= UINT64_C(1) << (pos + 1);
    }

    return mask;
  }

  constexpr Piece(uint8_t type, uint8_t pos, bool color)
      : type(type), pos(pos), color(color), mask(setMask(type, pos)) {
    assert(pos < 64 && pos >= 0);
  }
};

struct DataPiece : Piece {
  constexpr DataPiece(uint8_t type, uint8_t pos, bool color)
      : Piece(type, pos, color) {}

  const char *name() const {
    switch (type) {
    case PType::Rook:
      return "Rook";
    case PType::Knight:
      return "Knight";
    case PType::Bishop:
      return "Bishop";
    case PType::Queen:
      return "Queen";
    case PType::King:
      return "King";
    case PType::WPawn:
      return "Pawn";
    case PType::BPawn:
      return "Pawn";
    default:
      return "";
    }
  }

  const char *colorStr(bool color) const { return color ? "black" : "white"; }

  void chessPosStr(char (&buf)[3]) const {
    buf[0] = 'a' + 7 - (pos % 8);
    buf[1] = '1' + pos / 8;
    buf[2] = '\0';
  }

  void posStr(char (&buf)[3]) const { snprintf(buf, 3, "%d", pos); }

  void printData() const {
    char sq_buf[3], pos_buf[3];
    posStr(pos_buf);
    chessPosStr(sq_buf);
    printf("%s %s on %s (%s)\n", colorStr(color), name(), sq_buf, pos_buf);
  }

  void printMask() const {
    printData();
    printf(BINARY_PATTERN_64 "\n", BINARY_UINT64_TO_BITS(mask));
    printf("\n");
  }

  void printMaskHex() const { printf("%#018lx,\n", mask); }

  // Will prevent a move if it violates 2D logic
  void moveMask(bool invalid, int8_t delta) {
    if (invalid)
      printf("%s cannot move here!\n", name());
    else {
      movePos(delta);
    }
  }

  void moveUp() { moveMask(pos > 55, 8); }
  void moveDown() { moveMask(pos < 8, -8); }
  void moveLeft() { moveMask(pos % 8 == 7, 1); }
  void moveRight() { moveMask(pos % 8 == 0, -1); }
  void moveUpLeft() { moveMask(pos > 55 || pos % 8 == 7, 9); }
  void moveUpRight() { moveMask(pos > 55 || pos % 8 == 0, 7); }
  void moveDownLeft() { moveMask(pos < 8 || pos % 8 == 7, -7); }
  void moveDownRight() { moveMask(pos < 8 || pos % 8 == 0, -9); }

  static void printPieces(const DataPiece *p, size_t size) {
    printf("Pieces:\n");
    for (size_t i = 0; i < size; ++i) {
      p->printData();
      p++;
    }
    printf("\n");
  }
};

// clang-format off
int main() {
  // Initialize pieces
  enum { white, black };

  constexpr DataPiece kings[] = {
    {PType::King, 3, white},
    {PType::King, 59, black}
  };

  constexpr DataPiece knights[] = {
    {PType::Knight, 1, white},
    {PType::Knight, 6, white},
    {PType::Knight, 57, black},
    {PType::Knight, 62, black}
  };

  constexpr DataPiece rooks[] = {
    {PType::Rook, 0, white},
    {PType::Rook, 7, white},
    {PType::Rook, 56, black},
    {PType::Rook, 63, black}
  };

  constexpr DataPiece bishops[] = {
    {PType::Bishop, 2, white},
    {PType::Bishop, 5, white},
    {PType::Bishop, 58, black},
    {PType::Bishop, 61, black},
  };

  size_t n_king = sizeof(kings) / sizeof(DataPiece);
  size_t n_knight = sizeof(knights) / sizeof(DataPiece);
  size_t n_rook = sizeof(rooks) / sizeof(DataPiece);
  size_t n_bishop = sizeof(bishops) / sizeof(DataPiece);

  DataPiece::printPieces(kings, n_king);
  DataPiece::printPieces(knights, n_knight);
  DataPiece::printPieces(rooks, n_rook);
  DataPiece::printPieces(bishops, n_bishop);

  // Next, implement en-passant-able flag for pawns that moved 2
  // add blocked flag for pawns with a piece in front of it
  // Pawn class? with moveMask and attackMask?
  // Remember that the "Piece web" will be a linked list of pieces

  DataPiece bpawn{PType::BPawn, 0, black};

  printf("Let's move the pawn! \n\n");
  for (int i = 0; i < 64; i++) {
    bpawn.printMask();
    if (i < 63) {
      bpawn.movePos(1);
    }
  }

  return 0;
}
