
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"

constexpr int BET_DEFAULT = 100;

int num_cards(Card *hand)
{
    int cards = 0;
    
    for (;*hand; hand++)
        cards++;

    return cards;
}

Card random_card()
{
    return make_card(rand() % (KING   - ACE   + 1) + ACE,
                     rand() % (SPADES - CLUBS + 1) + CLUBS);
}

int sum(Card *hand)
{
    int sum = 0, aces = 0;

    for (Rank r; r = rank_of(*hand); hand++)
    {
        if (r == ACE) aces++;
        
        sum += std::min(int(r), int(TEN));
    }

    for (int i = 0; i < aces; i++)
        if (sum + 10 <= 21) sum += 10;

    return sum;
}

void refresh(Card *dealer, Card *player, int money, bool hide = false)
{
    if (hide || sum(player) > 21)
    {
        std::cout << "+---+\n|" << to_string(*dealer) << "|\n+---+\n";
    }
    else
    {
        for (Card *c = dealer; *c; c++)
            std::cout << "+---+ ";
        std::cout << std::endl;
        for (Card *c = dealer; *c; c++)
            std::cout << "|" << to_string(*c) << "| ";
        std::cout << std::endl;
        for (Card *c = dealer; *c; c++)
            std::cout << "+---+ ";
        std::cout << std::endl << std::endl;
    }

    for (Card *c = player; *c; c++)
        std::cout << "+---+ ";
    std::cout << std::endl;
    for (Card *c = player; *c; c++)
        std::cout << "|" << to_string(*c) << "| ";
    std::cout << "$" << money << std::endl;
    for (Card *c = player; *c; c++)
        std::cout << "+---+ ";
    std::cout << std::endl << std::endl;
}

Outcome result(Card *dealer_hand, Card *player_hand)
{
    int dealer       = sum(dealer_hand);
    int player       = sum(player_hand);
    int dealer_cards = 0;
    int player_cards = 0;

    bool dealer_bj = num_cards(dealer_hand) == 2 && dealer == 21;
    bool player_bj = num_cards(player_hand) == 2 && player == 21;

    if (player > 21) return LOSS;

    if (player == 21)
    {
        if (player_bj)
        {
            return dealer_bj ? PUSH : BLACKJACK;
        }
        else
        {
            return dealer_bj ? LOSS : dealer == 21 ? PUSH : WIN;
        }
    }

    return dealer >  player ? dealer > 21 ? WIN : LOSS
         : dealer == player ? PUSH              : WIN;
}

int main()
{
    srand(time(NULL));
    
    Card hand[2][11] = {};

    int bet, money = 500;

    do
    {
        printf("========\nBet: [%s]\n", std::to_string(BET_DEFAULT).c_str());
        std::string in;
        std::getline(std::cin, in);
        bet = std::min(money, in.size() ? std::stoi(in) : BET_DEFAULT);
        money -= bet;

        Card *d = hand[DEALER], *p = hand[PLAYER];

        *d++ = random_card(), *d++ = random_card();
        *p++ = random_card(), *p++ = random_card();

        for (char opt; sum(hand[PLAYER]) < 21;)
        {
            refresh(hand[DEALER], hand[PLAYER], money, true);
            
            std::cout << "(S)tand, (H)it, (D)ouble down: [h]" << std::endl;
            std::getline(std::cin, in);
            
            switch (opt = in.size() ? std::tolower(in[0]) : 'h')
            {
                case 's': break;
                case 'h':
                    *p++ = random_card();
                break;
                case 'd':
                    *p++ = random_card();
                    money += bet;
                    bet = std::min(money, bet * 2);
                    money -= bet;
                break;
            }

            if (opt != 'h') break;
        }

        if (sum(hand[PLAYER]) <= 21)
            for (;sum(hand[DEALER]) < 17; *d++ = random_card());

        switch (result(hand[DEALER], hand[PLAYER]))
        {
            case BLACKJACK:
                money += bet + bet * 3 / 2;
            break;
            case WIN:
                money += bet * 2;
            break;
            case PUSH:
                money += bet;
        }
        
        refresh(hand[DEALER], hand[PLAYER], money);
        memset(hand, 0, sizeof(hand));
        
    } while (money);
}
