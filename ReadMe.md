# MONOPOLY-LK

A fully autonomous, terminal-based simulation of **MONOPOLY-LK**, a Sri
Lankan-themed variant of Monopoly, written in C. Four computer-controlled
players — each following a distinct investment strategy (Aggressive
Investor, Conservative Banker, Risk Taker, Opportunistic Trader) — play
the entire game with no user interaction once the simulation starts.

The game models property/railway/utility trading, banking and loans,
insurance and disasters, building construction and maintenance, dynamic
inflation and market conditions, government regulations, and a full
national event card deck, ending with a bankruptcy cascade and a
net-worth-based win condition.

## Features

- Board setup, dice rolling, movement, and jail
- Property, railway, and utility purchase, rent, and mortgaging
- Loans with real collateral tracking, repayment, default, and foreclosure
- House/hotel construction, depreciation, maintenance, and repair
- Insurance policies, disasters, and automatic claims
- Inflation, dynamic property market booms/declines, regional development
  cards, government regulations, and a 20-card national event deck
- Four distinct autonomous player strategies driving every decision,
  including a full auction system
- Bankruptcy handling and an accurate end-of-game summary

## Requirements

- `gcc` (or any standard C compiler)

## How to Run

Compile all source files and run the resulting binary, redirecting
output to a file for easier reading (the full simulation can produce a
large amount of output):

```bash
gcc *.c -o monopoly
./monopoly > output.txt
```

Then open `output.txt` in any text editor to view the full game log,
from the opening turn-order roll to the final `GAME OVER` summary.

## Project Structure

| File | Responsibility |
|---|---|
| `types.h` | Struct/enum definitions, the central `GameState` struct |
| `functions.h` | Shared function declarations |
| `board.c` | Board layout and initial data |
| `players.c` | Player setup, dice, movement, jail |
| `game.c` | Turn/round orchestration |
| `finance.c` | Purchases, rent, loans, mortgaging, taxation |
| `buildings.c` | Construction, depreciation, maintenance, repair |
| `insurance.c` | Insurance and disasters |
| `inflation.c` | Inflation |
| `dyanmicpropmarket.c` | Dynamic property market (boom/decline) |
| `regionaldev.c` | Regional development cards |
| `govregulations.c` | Government regulations |
| `nationaleventcards.c` | National event card deck |
| `economicevents.c` | Automatic national economic events |
| `strategies.c` | Player strategy decision logic |
| `auction.c` | Auction system |
| `gameend.c` | Net worth, bankruptcy, win condition |
| `main.c` | Program entry point |

---

*Built as a university assignment*
