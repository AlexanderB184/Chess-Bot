namespace Chess {

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

struct Move {
  uint16_t data;
};

enum Piece {
  Empty = 0x00,
  Pawn = 0x01,
  Knight = 0x02,
  Bishop = 0x04,
  Rook = 0x08,
  Queen = Bishop | Rook,
  King = 0x10,
  White = 0x20,
  Black = 0x40,
  Border = 0x80,

  WhitePawn = White | Pawn,
  WhiteKnight = White | Knight,
  WhiteBishop = White | Bishop,
  WhiteRook = White | Rook,
  WhiteQueen = White | Queen,
  WhiteKing = White | King,

  BlackPawn = Black | Pawn,
  BlackKnight = Black | Knight,
  BlackBishop = Black | Bishop,
  BlackRook = Black | Rook,
  BlackQueen = Black | Queen,
  BlackKing = Black | King,
};

class Board {
 public:
  Board();
  Board(const Board&);
  Board(Board&&);

  static Board from_pgn(const std::string&);
  static Board from_fen(const std::string&);

  static std::string as_pgn(const Board&);
  static std::string as_fen(const Board&);

  Board& make_move(Move move);
  Board& unmake_move();

  Piece piece(int square) const;
  bool piece_is_white(int square) const;
  bool piece_is_black(int square) const;

  bool is_check_after_move(Move move) const;

  bool is_legal_white(Move move) const;
  bool is_legal_black(Move move) const;

  bool is_check() const;
  bool is_double_check() const;
  bool is_discover_check() const;

  bool is_stalemate() const;
  bool is_checkmate() const;
  bool is_draw(int square) const;
  bool is_draw_by_repetition(int square) const;
  bool is_draw_by_50_move_rule(int square) const;
  bool is_draw_by_insufficient_material(int square) const;

 private:
  typedef unsigned char piece_t;
  struct ply_stack_item_t {
    Move previous_move; // 2
    piece_t captured_piece; // 1
    int ep; // 4
    int rights; // 4
    int half_move_clock; // 4
    int last_irreversible_move; // 4
    int check_square;
    int check_count;
    uint64_t old_zobrist; // 8
  };
  std::array<piece_t, 16 * 16> board;
  std::vector<ply_stack_item_t> ply_stack;
  uint64_t zobrist;
  int ply_counter;

};

}  // namespace Chess