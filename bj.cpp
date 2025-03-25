
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"

constexpr int BET_DEFAULT = 100;

std::string get_line()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
    // std::string in, token;
    // std::getline(std::cin, in);
    // std::istringstream is(in);
    // is >> token;
    // return token;
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

bool blackjack(Card *hand)
{
    int num_cards = 0;

    for (Card *c = hand; *c; c++)
        num_cards++;
    
    return num_cards == 2 && sum(hand) == 21;
}

void refresh(Card *dealer, Card *player, bool hide = false)
{
    //system("clear");
    
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
        std::cout << std::endl;
    }

    for (Card *c = player; *c; c++)
        std::cout << "+---+ ";
    std::cout << std::endl;
    for (Card *c = player; *c; c++)
        std::cout << "|" << to_string(*c) << "| ";
    std::cout << std::endl;
    for (Card *c = player; *c; c++)
        std::cout << "+---+ ";
    std::cout << std::endl << std::endl;
}

Outcome result(Card *dealer_hand, Card *player_hand)
{
    int dealer = sum(dealer_hand);
    int player = sum(player_hand);

    if (player > 21) return LOSS;

    if (player == 21)
    {
        return blackjack(player_hand) ? blackjack(dealer_hand) ? PUSH : BLACKJACK
                                      : blackjack(dealer_hand) ? LOSS : dealer == 21 ? PUSH : WIN;
    }

    return dealer >  player ? dealer > 21 ? WIN : LOSS
         : dealer == player ? PUSH              : WIN;
}

/*void action(Card *dealer, Card *player)
{
    for (Card *p = player + 2; sum(hand) < 21;)
    {
        refresh(hand[DEALER], hand[PLAYER], money, true);

        switch (opt)
        {
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

        std::cout << "(S)tand, (H)it, (D)ouble down: [h]" << std::endl;
        std::getline(std::cin, in);

        opt = in.size() ? std::tolower(in[0]) : 'h';

        if (opt == 'd' || opt == 's') break;
    }
}*/

int main()
{
    srand(time(NULL));

    Card hand[2][11], split[11];

    int bet, money = 500;

    do
    {
        printf("($%d) Bet: [%d]\n", money, BET_DEFAULT);

        do
        {
            std::string line = get_line();
            
            if (line.empty())
                bet = BET_DEFAULT;

            else if (std::istringstream is(line); !(is >> bet))
                bet = 0;
            
        } while (bet < 1 || bet > money);
        
        money -= bet;
        
        memset(dealer, 0, sizeof(dealer));
        memset(hand,   0, sizeof(hand));

        Card *d = hand[DEALER], *p = hand[PLAYER], *s = split;
        
        *d++ = random_card(), *d++ = random_card();
        *p++ = random_card(), *p++ = random_card();

        for (Option opt; sum(hand[PLAYER]) < 21)
        {
            std::cout << to_string(hand[DEALER]) << std::endl
                      << to_string(hand[PLAYER]) << std::endl;
        }

        /*refresh(hand[DEALER], hand[PLAYER], true);
        
        if (rank_of(hand[PLAYER][0]) == rank_of(hand[PLAYER][1]) && money >= bet)
        {
            std::cout << "Split: y/[n]" << std::endl;
            
            do
            {
                std::string in;
                std::getline(std::cin, in);
                std::istringstream is(in);
                is >> in;
            } while (!(in == "y"));
        }
        
        std::cout << "(S)tand, (H)it, s(P)lit, (D)ouble down: [h]" << std::endl;
        std::getline(std::cin, in);

        Option opt = in.size() ? std::tolower(in[0]) : HIT;

        if (opt == SPLIT && rank_of(player[0][0]) == rank_of(player[0][1]))
        {
            *s++     = *(p - 1);
            *s++     = random_card();
            *(p - 1) = random_card();

            action(player[0]);
            action(player[1]);
        }
        
        action(player[0], opt);*/

        if (sum(hand[PLAYER]) <= 21 && !blackjack(hand[PLAYER]))
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
        
    } while (money);
}
