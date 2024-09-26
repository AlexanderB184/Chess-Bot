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

## Transposition Table

The transposition table is implemented using seperate chaining. It has 2 key components the item buffer which stores the nodes in a contiguous buffer, and the buckets. Each bucket is a vector storing slots. Each slot contains a key (zobrist hash of the board position) and the index of the corresponding node.

# Search or Insert

The transposition table uses a search or insert paradigm. At the beginning of evaluating a node, the table is searched, and if the node is not found then a new node is inserted. Either the index of the existing node or of the newly inserted node is returned. At the end of the evaluating the node, the node can be updated without researching the table. Some care is required since the index may point to a null node. To check if a node is null check its type is equal to EMPTY_NODE (0)

# Index based

All operations on nodes are done through the nodes index. The nodes index is its index in the item buffer. This allows indices to remain valid even if the item buffer is resized. It also allows the index to be reused later and avoid researching the table when it comes time to update the table at the end of evaluating the node.

# Nodes

Nodes represent a single node in the evaluation, they store the refutation, which is the best move found in the previous search of the position, the score from the previous search. The type of node, empty if the node was created in search or insert but hasn't been given an initial value yet. The age of the node, which is set as max depth so the higher the age the younger the node. the depth from the root. The depth to the leaf can also be calculate as $age - depth$.

# Node limit.

The transposition table enforces a hard limit on the number of nodes the table can contain, this prevents the table from using too much memory and crashing the program. Currently it is set at 10 million nodes.

# Optimised for short evaluation times

The transposition table architecture was chosen with the consideration that it is meant to be used in bots with short fixed evaluation times, e.g. 100ms. This means we are unlikely to search enough nodes to run out of memory so we do not need to do any sort of node pruning to remove nodes which aren't useful to preserve space.