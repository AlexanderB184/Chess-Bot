#include "../../chess-lib/include/chess-lib.h"
#include "../include/bot.h"

typedef struct table_data_t {
    score_cp_t score; //16 bit
    uint16_t move; // 16 bit
    int8_t depth;
    uint8_t type;
    int16_t padding;
} table_data_t;

typedef struct slot_t {
    zobrist_t key;
    table_data_t data;
} slot_t;

typedef struct table_t {
    struct slot_t* items;
    uint64_t capacity;
} table_t;

void tt_init(table_t* table, uint64_t capacity) {
    table->items = calloc(capacity * sizeof(slot_t));
    table->capacity = capacity;
}

void tt_free(table_t* table) {
    free(table->items);
}

table_data_t tt_get(table_t* table, zobrist_t key) {
    if (table->items[key % table->capacity].key == key) {
        return table->items[key % table->capacity].data;
    }
    return (table_data_t){0,};
}

void tt_store_always_replace(table_t* table, zobrist_t key, table_data_t data) {
    table->items[key % table->capacity].key = key;
    table->items[key % table->capacity].data = data;
}

void tt_store_depth_prefered(table_t* table, zobrist_t key, table_data_t data) {
    if (data.depth >= table->items[key % table->capacity].data.depth) {
        table->items[key % table->capacity].key = key;
        table->items[key % table->capacity].data = data;
    }
}