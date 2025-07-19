board = [" ", " ", " ", " ", " ", " ", " ", " ", " "]
w = ' '
print(''' 
Welcome to Tic-Tac-Toe
Please enter the name of the players:''')

player1 = input("Player 1 (Team X): ")
player2 = input("Player 2 (Team O): ")

print('''
Display of the Tic-Tac_Toe Board:

\t| 1 | 2 | 3 |
\t| 4 | 5 | 6 |
\t| 7 | 8 | 9 |

NOTE: This board given above is the main reference for your inputs ''')

print(''' 
Rules:
1. Each player gets to pick a position in the board displayed.
2. The player who gets a straight line of X or O, wins the game.
''')

print('             ————————LET THE GAMES BEGIN!—————————')


def acc_input(name):
    print(f'''{name}'s turn''')
    choice = 'w'
    while not choice.isdigit() or int(choice) > 10:

        choice = input("Enter the position (1-9) of your choice: ")
        if not choice.isdigit():
            print('Sorry that is not a digit. Please enter again')
        elif int(choice) > 10:
            print('Sorry that is out of range. Please enter again')
    return int(choice)


def placing_move(position, team):
    if team == player1:
        board[position - 1] = "X"
    else:
        board[position - 1] = "O"


def board_display():
    print(f'''
\t| {board[0]} | {board[1]} | {board[2]} |
\t| {board[3]} | {board[4]} | {board[5]} |
\t| {board[6]} | {board[7]} | {board[8]} |
''')


def check():
    global w
    l = []
    for i in range(0, 8, 3):
        l.append(board[i:i + 3])

    for i in l:
        if len(set(i)) == 1 and i[0] != ' ':
            w = i[0]
            return True

        # Check columns
    for i in range(len(l)):
        if len(set([l[j][i] for j in range(len(l))])) == 1 and [l[j][i] for j in range(len(l))][0] != ' ':
            w = [l[j][i] for j in range(len(l))][0]
            return True

    if len(set([l[i][i] for i in range(len(l))])) == 1 and [l[i][i] for i in range(len(l))][0] != ' ':
        w = [l[i][i] for i in range(len(l))][0]
        return True

    if len(set([l[i][-i-1] for i in range(len(l))])) == 1 and [l[i][-i-1] for i in range(len(l))][0] != ' ':
        w = [l[i][-i-1] for i in range(len(l))][0]
        return True

    return False


def main():
    c = 0
    while True:
        c += 1
        if c == 1:
            p = acc_input(player1)
            placing_move(p, player1)
            board_display()
            if check():
                break
            continue
        if c == 2:
            p = acc_input(player2)
            placing_move(p, player2)
            board_display()
            if check():
                break
            continue
        elif c > 2:
            c = 0
            continue

    if w == 'X':
        print(f'''Congratulation {player1} you have won the game''')
    elif w == 'O':
        print(f'''Congratulation {player2} you have won the game''')


main()
