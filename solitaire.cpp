#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <time.h>
using namespace std;
vector<int> deck;
vector<int> tableaux[8];
vector<int>::iterator shown[7];
vector<int>::iterator shown_deck;
vector<int> foundations[4];
void graphic(short);
bool is_descending(short, short);
void show_game();
void move_to_foundation(short);
void directions();
void graphic(short);
auto get_subset(short, short);
void deck_to_tableaux(short);
void switch_tableaux(short, short);
void shuffle();

void move_to_foundation(short s)
{
    // Moves to a foundation.
    vector<int>::iterator it;

    // Only way I can find to conditionally initialize by reference!
    // if s = 8, that's the deck itself.
    vector<int> &tab = (s==8) ? deck : tableaux[s];

    if (s==8)
        it = shown_deck;
    else
        it = tab.end()-1;

    // Can't move something if it's not there.
    if (tab.empty())
        return;
    short suit = *it / 13;
    short rank = *it % 13;


    bool can_move = false;
    can_move = true;
    
    // Can only move an ace to an empty foundation.
    if (foundations[suit].empty())
      {
        if (rank == 0)
            can_move = true;
      }
    else
      {
        if (rank == *(foundations[suit].end()-1)+1)
            can_move = true;
      }

    if (can_move)
      {
        foundations[suit].push_back(*it);
        tab.erase(it, it+1);
        if (s != 8 and shown[s] >= tab.end())
            shown[s] = tab.end() - 1;
      }

}

bool is_descending(short source, short destination)
{
    // Checks if a card placed in a tableaux belongs there.
    short source_suit, source_rank, dest_suit, dest_rank;
    source_suit = source / 13;
    source_rank = source % 13;
    dest_suit = destination / 13;
    dest_rank = destination % 13;
    if ((source_suit % 2 != dest_suit % 2) && (dest_rank - 1 == source_rank))
        return true;
    return false;
}

void show_game()
{
    // Shows the entire game.  The output after each turn.
    vector<int> temp;
    bool is_cheater = false;
    if (is_cheater)
      {
        // Cheaty.  Shows entire deck, for debugging.
        vector<int>::iterator current;
        current = deck.end();
        for (current = deck.begin(); current != deck.end(); current++)
          {
            if (current == shown_deck)
                cout << "(";
            else
                cout << ' ';
            graphic(*current);
            if (current == shown_deck)
                cout << ")";
            else
                cout << ' ';
          }
        cout << "\n";
      }
    
    cout << "\033c";
    cout << "    W        C   D   S   H\n";
    // Shown_deck is the card that is showing.
    // Need to check if there is a card one up from it.
    if (shown_deck == deck.end())
        cout << "\033[33mXX\033[0m ";
    else if (shown_deck+1 >= deck.end())
        cout << "   ";
    else 
        cout << "\033[33mXX\033[0m ";
        
    if (shown_deck != deck.end())
        graphic(*shown_deck);
    else
        cout << "  ";

    cout << "       ";

    // For showing the foundations.
    for (auto i = 0; i < 4; i++)
      {
        if (foundations[i].end() == foundations[i].begin())
            cout << "..  ";
        else
          {
            graphic(*(foundations[i].end()-1));
            cout << "  ";
          }
      }
    cout << "\n";

    cout << endl;
    cout << endl << " ";

    // Labeling the tableaux to make playing a bit easier.
    for (auto i = 1; i < 8; i++)
      {
        cout << i << "   ";
      }
    cout << endl;

    // Showing the tableaux.  We show the cards by row
    // instead of column because it's way easier.
    for (auto row = 0; row < 20; row++)
      {
        for (auto i = 0; i < 7; i++)
          {
            temp = tableaux[i];
            
            if (temp.size() > row)
              {
                // If the card has been exposed yet
                if (shown[i] <= tableaux[i].begin() + row)
                  {
                    graphic(*(tableaux[i].begin() + row));
                  }
                else
                    cout << "\033[33mXX\033[0m";

                cout << "  ";
              }
            else
                cout << "    ";
          }
        cout << endl;
      }
}
void directions()
{
    // Simply shows the directions for the game.
    cout << "? - read these directions again\n";
    cout << "<enter> - flip the next card\n";
    cout << "q - quit the game\n";
    cout << "24 - move card(s) from pile 2 to pile 4\n";
    cout << "d3 - move card from the discard pile to pile 3\n";
    cout << "df - move card from the discard pile to its appropriate foundation\n";
    cout << "5f - move card from pile 5 to its appropriate foundation\n";
    cout << "Type 'y' to continue\n";
}

void graphic(short card)
{
    // Shows a card on the screen using ansi codes.
    short rank, suit;
    rank = card % 13;
    suit = card / 13;
    cout << "\033[3";
    if (suit % 2 == 0)
        cout << "6";
    else
        cout << "1";
    cout <<"m";
    if (rank == 0)
        cout << "A";
    else if (rank == 9)
        cout << "T";
    else if (rank == 10)
        cout << "J";
    else if (rank == 11)
        cout << "Q";
    else if (rank == 12)
        cout << "K";
    else
        cout << rank + 1;
    if (suit == 0)
        cout << "c";
    else if (suit == 1)
        cout << "d";
    else if (suit == 2)
        cout << "s";
    else 
        cout << "h";
    cout << "\033[0m";
}
auto get_subset(short s, short d)
{
    // Returns an iterator pointing to beginning of
    // a subset you can move from one pile to another.
    vector<int> &source = tableaux[s];
    vector<int> &destination = tableaux[d];
    vector<int>::iterator s_it, d_it;
    s_it = source.end() - 1;
    d_it = destination.end() - 1;
    

    while (s_it >= shown[s])
      {
        // We can move a King to an empty pile.
        if (*s_it%13 == 12 and destination.empty())
            return s_it;
        // Otherwise, we need to check if it's descending, in alternative colors
        if (is_descending(*s_it, *d_it))
            return s_it;
        s_it--;
      }

    // If the move is not legal, return this:
    return source.end();
}
void deck_to_tableau(short d)
{
    // Moves cards from the discard pile to a tableau.
    vector<int> destination;
    destination = tableaux[d];

    // It is legal to move a King to an empty pile.
    if (destination.empty() and *shown_deck % 13 == 12)
      {
        tableaux[d].push_back(*shown_deck);
        deck.erase(shown_deck, shown_deck+1);
        // moves to the last card you flipped over.
        shown_deck--;
        return;
      }
        
    auto d_it = destination.end()-1;
    auto s_it = deck.end();
    if (is_descending(*shown_deck, *d_it))
      {
        tableaux[d].push_back(*shown_deck);
        deck.erase(shown_deck, shown_deck+1);
        shown_deck--;
      }
}
void switch_tableaux(short s, short d)
{
    // Switches a subset of cards from one tableau to another.

    // Declare these by reference so they can actually change the piles.
    vector<int> &source = tableaux[s];
    vector<int> &destination = tableaux[d];
    auto s_it = source.end() - 1;
    auto d_it = destination.end() - 1;
    short dest_rank = *d_it % 13;
    short dest_suit = *d_it / 13;

    // The beginning of the subset we can move.
    auto subset = get_subset(s, d);

    // The move is not legal.
    if (subset == source.end())
        return;

    destination.insert(destination.end(), subset, source.end());

    // This kills the cards at the end of the source pile.
    source.resize(subset - source.begin());

    // If we got rid of all the shown cards in the pile.
    if (shown[s] >= source.end())
        shown[s] = source.end() - 1;
    return;
    
}
void shuffle()
{
    // Shuffles the deck, deals out the cards.
    for (int i = 0; i < 52; i++)
      {
        deck.push_back(i);
      }
    srand(time(NULL));
    random_shuffle(deck.begin(), deck.end());

    // Actually deals out the cards.  Each tableau 1 to 7 has the same
    // amount of cards.
    for (auto t=0; t<7; t++)
      {
        // We need plenty of room reserved because otherwise our
        // shown iterators will be deleted after we add too many
        // cards to a tableau.
        tableaux[t].reserve(20);
        for (auto row=0; row<=t; row++)
          {
            tableaux[t].push_back(deck.back());
            deck.pop_back();
          }
        shown[t] = tableaux[t].begin() + t;
      }
    shown_deck = deck.end();
}
int main()
{
    shuffle();
    directions();
    
    string input;
    cin >> input;

    while (true)
      {
        // If you flip to the end of the deck, we need to flip the 
        // whole thing over again.
        if (shown_deck > deck.end())
            shown_deck = deck.begin();

        show_game();
        getline(cin, input);

        // Quit.
        if (input == "q")
            break;
        // Flip a card.
        else if (input == "")
          {
            shown_deck++;
          }
        // Show directions.
        else if (input == "?")
            directions();

        // Each two character direction has the following formula:
        // <source><destination>
        if (input.length() == 2)
          {
            // From the discard to a tableau.
            if (input[0] == 'd' and isdigit(input[1]))
              {
                short which_tableau;
                which_tableau = input[1] - '1';
                if (which_tableau > 7)
                    continue;
                deck_to_tableau(which_tableau);
              }
            // From a tableau to another tableau.
            else if (isdigit(input[0]) and isdigit(input[1]))
              {
                short source, destination;
                source = input[0] - '0';
                destination = input[1] - '0';
                source--;
                destination--;
                if ((0 <= source < 7) and (0 <= destination < 7))
                    switch_tableaux(source, destination);
              }
            // From a tableau to a foundation.
            else if (isdigit(input[0]) and input[1] == 'f')
                move_to_foundation(input[0] - '1');
            // From discard pile to a foundation.
            else if (input[0] == 'd' and input[1] == 'f')
                move_to_foundation(8);
          }
      }
    return 0;
}
