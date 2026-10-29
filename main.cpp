#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

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
  int8_t type;
  int8_t pos;
  bool color;
  const uint64_t *maskPtr;

  void movePos(int8_t delta) {
    assert(pos + delta < 64 && pos + delta >= 0);
    maskPtr += delta;   // pray it's not a buffer overflow
    this->pos += delta; // Set position
  }

public:
  constexpr Piece(int8_t type, int8_t pos, bool color)
      : type(type), pos(pos), color(color), maskPtr(maskPiece(type, pos)) {
    assert(pos < 64 && pos >= 0);
  }
};

struct DataPiece : Piece {
  constexpr DataPiece(int8_t type, int8_t pos, bool color)
      : Piece(type, pos, color) {}

  const char *name() const {
    switch (type) {
    case PieceType::Rook:
      return "Rook";
    case PieceType::Knight:
      return "Knight";
    case PieceType::King:
      return "King";
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
    printf(BINARY_PATTERN_64 "\n", BINARY_UINT64_TO_BITS(*maskPtr));
    printf("\n");
  }

  void printMaskHex() const { printf("%#018lx,\n", *maskPtr); }

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
    {PieceType::King, 56, black},
    {PieceType::King, 7, white}
  };

  constexpr DataPiece knights[] = {
    {PieceType::Knight, 57, black},
    {PieceType::Knight, 6, black}
  };

  size_t n_king = sizeof(kings) / sizeof(DataPiece);
  size_t n_knight = sizeof(knights) / sizeof(DataPiece);

  DataPiece::printPieces(kings, n_king);
  DataPiece::printPieces(knights, n_knight);

  DataPiece king{PieceType::King, 32, black};
  DataPiece knight{PieceType::Knight, 0, black};

  printf("Let's move the knight! \n\n");
  knight.moveLeft();
  knight.printMask();
  knight.moveLeft();
  knight.printMask();
  knight.moveLeft();
  knight.printMask();
  knight.moveLeft();
  knight.printMask();
  knight.moveLeft();
  knight.printMask();
  knight.moveLeft();
  knight.printMask();
  knight.moveLeft();
  knight.printMask();

  knight.moveUp();
  knight.printMask();
  knight.moveUp();
  knight.printMask();
  knight.moveUp();
  knight.printMask();
  knight.moveUp();
  knight.printMask();
  knight.moveUp();
  knight.printMask();
  knight.moveUp();
  knight.printMask();
  knight.moveUp();
  knight.printMask();

  printf("Black King is %lu bytes!\n", sizeof(king));
  printf("Black Rook is %lu bytes!\n", sizeof(knight));

  return 0;
}
