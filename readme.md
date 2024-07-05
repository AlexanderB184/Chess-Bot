### Chess-Bot

An implementation of a chess engine and chess bot.

## Chess Engine

The underlying engine uses a non-bitboard architechture. The board is stored as 16 * 16 with borders around the playable board.

Pieces are stored using disjoint flags.

Moves are stored in a 32bit format, including their from square, to square, 4 bits for flags and a precalculated move priority used in move ordering.

Move generation functions by default return psuedo legal moves and it is the users duty to ensure they check for legality before making the move, otherwise an invalid state could be reached.

Most operations are done using incremental update, so the postion is continually modified back and forth with move and unmove.

The chess interface is for the most part final. It is unlikely the existing interface will be substantially modified, although new functions may be added, and the implementation may be altered, meaning slightly different behaviour but the same callign interface.

## Chess Bot

So far the following features have been implemented.

iterative deepening
negamax searching
static evaluation with piece-square tables
alpha-beta pruning
basic move ordering (captures and promotions first, on the root node moves are ordered based on score in previous iteration)
transposition table
refutation moves
principal variation nodes

Important to note that none of the chess bot interface is final except the find_best_move function, all others should be treated as internal or private functions. Only the find_best_move function should be exposed, although currently others are exposed, a improved interface will be made at a later stage. All other functions except find_best_move may be changed or even removed and should not be used outside of the chess-bot implementation.
