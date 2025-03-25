
#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>

typedef unsigned short Card;
typedef unsigned short Rank;
typedef unsigned short Suit;

enum Option
{
    OPT_NONE   = '\0',
    OPT_STAND  = 's',
    OPT_HIT    = 'h',
    OPT_SPLIT  = 'p',
    OPT_DOUBLE = 'd'
};

enum
{
    ACE = 1,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
};

enum Outcome { BLACKJACK, WIN, PUSH, LOSS };

enum Hand { DEALER, PLAYER, SPLIT };

enum { CLUBS, DIAMONDS, HEARTS, SPADES };

inline Card make_card(Rank r, Suit s) {
    return r + (s << 8);
}

inline Suit suit_of(Card c) {
    return c >> 8;
}

inline Rank rank_of(Card c) {
    return c & 0xff;
}

inline Card random_card()
{
    return make_card(rand() % (KING   - ACE   + 1) + ACE,
                     rand() % (SPADES - CLUBS + 1) + CLUBS);
}

inline std::string face_string(Card c) {
    return std::string(1, " A23456789TJQK"[rank_of(c)]) + "_" + std::string(1, "CDHS"[suit_of(c)]);
}

inline std::string to_string(Card c) {
    return "+---+\n|" + face_string(c) + "|\n+---+\n";
}

#endif
