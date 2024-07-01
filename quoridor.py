import os
from collections import deque

wasd = ['w', 'a', 's', 'd']

def clear_console():
    os.system('cls' if os.name == 'nt' else 'clear')

def print_board(board_size, player_cords, player_walls_cords):
    wall_symbol = ["X", "A", "B", "C", "D"]
    
    for i in range(board_size):
        if i % 2 != 0:
            row = "-" * board_size
            for player in player_walls_cords:
                for wall_cords in player_walls_cords[player]:
                    for wall_cord in wall_cords:
                        if ((wall_cord // board_size) == i):
                            if row[wall_cord % board_size] == "-" or player == 0:
                                row = row[:wall_cord % board_size] + wall_symbol[player] + row[wall_cord % board_size + 1:]
            print(row)
        else:
            row = ""
            for j in range(board_size):
                if j % 2 == 0:
                    row += "."
                else:
                    wall_added = False
                    for player in player_walls_cords:
                        for wall_cords in player_walls_cords[player]:
                            for wall_cord in wall_cords:
                                if ((wall_cord // board_size) == i) and ((wall_cord % board_size) == j):
                                    if row[-1] == "|" or row[-1] == "." or player == 0:
                                        if player == 0 and row[-1] != "|" and row[-1] != ".":
                                            row = row[:-1]
                                        row += wall_symbol[player]
                                        wall_added = True
                                    
                    if not wall_added:
                        row += "|"
                        
            for player, pos in player_cords.items():
                row_pos = pos % board_size
                col_pos = pos // board_size
                if col_pos == i:
                    row = row[:row_pos] + str(player) + row[row_pos + 1:]
            print(row)

def check_wall_in_path(move, player_count, player_walls_cords, target, board_size):
    move_front_wall = [target + board_size, target + 1, target - board_size, target - 1]

    is_wall_in_path = False
    for i in range(1, player_count + 1):
        for wall_cords in player_walls_cords[i]:
            for wall_cord in wall_cords:
                if wall_cord == move_front_wall[move]:
                    is_wall_in_path = True
                    return is_wall_in_path

    return is_wall_in_path

def is_valid_player_move(player_cords, player, move, board_size, board, move_deltas, player_count, player_walls_cords):
    target = player_cords[player] + move_deltas[move]
    is_wall_in_path = check_wall_in_path(move, player_count, player_walls_cords, target, board_size)
    
    if move in [1, 3]:
        same_row = (player_cords[player] // board_size) == (target // board_size)
        return target in board and same_row and not is_wall_in_path
    
    return target in board and not is_wall_in_path

def is_valid_player_jump(player_cords, player, move, board_size, player_count, player_walls_cords):
    move_checks = [[-board_size, -3 * board_size], [-1, -3], [board_size, 3 * board_size], [1, 3]]
    
    checks = move_checks[move]
    for check in checks:
        target_cord = player_cords[player] + check
        for i in range(1, player_count + 1):
            for wall_cords in player_walls_cords[i]:
                if target_cord in wall_cords:
                    return False
    return True
    
def get_jump_position(player_cords, player, move, board_size, board, move_deltas):
    jump_position = player_cords[player] + 2 * move_deltas[move]
    if move in [1, 3]:
        same_row = (player_cords[player] // board_size) == (jump_position // board_size)
        return jump_position if jump_position in board and same_row else -1
    return jump_position if jump_position in board else -1

def get_valid_moves(player_cords, player, board_size, board, move_deltas, player_count, player_walls_cords):
    valid_moves = []
    for move in range(len(move_deltas)):
        delta = move_deltas[move]
        target = player_cords[player] + delta
        if any(pos == target for pos in player_cords.values()):
            jump_position = get_jump_position(player_cords, player, move, board_size, board, move_deltas)
            if jump_position != -1 and jump_position not in player_cords.values():
                if is_valid_player_jump(player_cords, player, move, board_size, player_count, player_walls_cords):
                    valid_moves.append(move)
        elif is_valid_player_move(player_cords, player, move, board_size, board, move_deltas, player_count, player_walls_cords):
            valid_moves.append(move)
    return valid_moves

def check_winner(player_cords, player_goals):
    for player, pos in player_cords.items():
        if pos in player_goals[player]:
            return player
    return -1

def is_within_board(cords, board_size):
    for cord in cords:
        if cord not in range(board_size * board_size):
            return False
    return True

def wall_move_validation(move, player_walls_cords, board_size):
    next_move = [- board_size * 2, - 2, board_size * 2, 2]

    delta = next_move[move]
    X_cords = player_walls_cords[0][0]
    target_cords = [X_cords[0] + delta, X_cords[1] + delta, X_cords[2] + delta]
    
    if move in [1, 3]:
        is_valid = (X_cords[1] // board_size) == (target_cords[1] // board_size)
    else:
        is_valid = is_within_board(target_cords, board_size)
    
    return is_valid, target_cords

def check_X_on_wall(player_walls_cords, player_count):
    X_cords = player_walls_cords[0][0]
    for cord in X_cords:
        for i in range(1, player_count + 1):
            for wall_cords in player_walls_cords[i]:
                for wall_cord in wall_cords:
                    if cord == wall_cord:
                        return True
    return False

def wasd_to_int(word):
    for i in range(len(wasd)):
        if word == wasd[i]:
            return i

def int_to_wasd(move):
    return wasd[move]

def convert_to_xy(board_size, position):
    return position % board_size, position // board_size

def is_within_board_2(x, y, board_size):
    return 0 <= x < board_size and 0 <= y < board_size

def is_wall_between(x1, y1, x2, y2, player_walls_cords, board_size):
    if abs(x1 - x2) == 2 and y1 == y2:
        wall_x = min(x1, x2) + 1
        wall_y = y1
    elif abs(y1 - y2) == 2 and x1 == x2:
        wall_x = x1
        wall_y = min(y1, y2) + 1
    else:
        return False

    wall_pos = wall_y * board_size + wall_x
    for walls in player_walls_cords.values():
        for wall in walls:
            if wall_pos in wall:
                return True
    return False

def get_neighbors(x, y, board_size, all_players_places, player_walls_cords):
    neighbors = []
    deltas = [(-2, 0), (2, 0), (0, -2), (0, 2)]
    for dx, dy in deltas:
        nx, ny = x + dx, y + dy
        if is_within_board_2(nx, ny, board_size):
            neighbor_pos = ny * board_size + nx
            if neighbor_pos in all_players_places and not is_wall_between(x, y, nx, ny, player_walls_cords, board_size):
                neighbors.append((nx, ny))
    return neighbors

def bfs(start, goals, board_size, all_players_places, player_walls_cords):
    queue = deque([start])
    visited = set([start])
    while queue:
        x, y = queue.popleft()
        if (x, y) in goals:
            return True
        for nx, ny in get_neighbors(x, y, board_size, all_players_places, player_walls_cords):
            if (nx, ny) not in visited:
                visited.add((nx, ny))
                queue.append((nx, ny))
    return False

def is_path_available(board_size, player_cords, player_goals, player_walls_cords):
    all_players_places = [i + j * board_size for j in range(board_size) for i in range(board_size) if i % 2 == 0 and j % 2 == 0]

    for player, start_pos in player_cords.items():
        start_x, start_y = convert_to_xy(board_size, start_pos)
        goal_positions = player_goals[player]
        goals = [convert_to_xy(board_size, pos) for pos in goal_positions]

        if not bfs((start_x, start_y), goals, board_size, all_players_places, player_walls_cords):
            return False
    return True

def main():
    player_count = 0
    while True:
        try:
            player_count = int(input("Enter number of players (2 or 4): "))
            if player_count == 2 or player_count == 4:
                break
            else:
                print("Invalid input. please enter 2 or 4.")
        except ValueError:
            print("Invalid input. Please enter a valid number.")

    board_size = 17
    board = set(j + i * board_size for i in range(board_size) for j in range(board_size))

    player_cords = {}
    player_goals = {}

    if player_count >= 2:
        player_cords[1] = board_size // 2
        player_goals[1] = [i for i in range(board_size * (board_size - 1), board_size * board_size, 2)]
        
        player_cords[2] = (board_size * board_size) - (board_size // 2) - 1
        player_goals[2] = [i for i in range(0, board_size, 2)]

    if player_count == 4:
        player_cords[3] = board_size * (board_size // 2)
        player_goals[3] = [board_size - 1 + i * board_size for i in range(board_size)]

        player_cords[4] = board_size * (board_size // 2) + board_size - 1
        player_goals[4] = [i * board_size for i in range(board_size)]

    move_deltas = [- board_size * 2, - 2, board_size * 2, 2]

    all_walls = 20
    player_walls = {}
    player_walls_cords = {}
    for player in player_cords:
        player_walls[player] = all_walls / player_count
        player_walls_cords[player] = []

    current_player = 1

    while True:
        print_board(board_size, player_cords, player_walls_cords)
        winner = check_winner(player_cords, player_goals)
        if winner != -1:
            print(f"Player {winner} wins!")
            input("Press Enter to exit...")
            return
        
        valid_moves = get_valid_moves(player_cords, current_player, board_size, board, move_deltas, player_count, player_walls_cords)
        while True:
            game_options = input(f'Player {current_player}\n1 . Move\n2 . Add wall\n3 . Exit\n')
            clear_console()
            print_board(board_size, player_cords, player_walls_cords)
            if game_options == '1':
                if valid_moves:
                    valid_moves_wasd = []
                    for move in valid_moves:
                        valid_moves_wasd.append(int_to_wasd(move))
                    player_move = input(f"Player {current_player}, enter your move ({'/'.join(valid_moves_wasd)}): ")
                    player_move = wasd_to_int(player_move)
                    if player_move in valid_moves:
                        target = player_cords[current_player] + move_deltas[player_move]
                        if any(pos == target for pos in player_cords.values()):
                            jump_position = get_jump_position(player_cords, current_player, player_move, board_size, board, move_deltas)
                            if jump_position != -1 and jump_position not in player_cords.values():
                                player_cords[current_player] = jump_position
                                clear_console()
                                break
                        else:
                            player_cords[current_player] = target
                            clear_console()
                            break
                    else:
                        print(f"Invalid move. Please choose one of {', '.join(valid_moves_wasd)}.")
                else:
                    print("No valid moves available.")
                    break
            elif game_options == '2':
                wall_added = False
                if player_walls[current_player] > 0:
                    wall_start_pos = ((board_size * board_size) // 2) - board_size
                    player_walls_cords[0] = [[wall_start_pos, wall_start_pos + 1, wall_start_pos + 2],]
                    while not wall_added:
                        clear_console()
                        print_board(board_size, player_cords, player_walls_cords)
                        wall_move = input(f"Player {current_player}, enter wall move (w/a/s/d) or place (p) or vertical/horizontal (o): ").strip().lower()
                        
                        if wall_move in wasd:
                            wall_move = wasd_to_int(wall_move)
                            is_valid, target_cords = wall_move_validation(wall_move, player_walls_cords, board_size)

                            if is_valid:
                                player_walls_cords[0][0] = target_cords
                        
                        elif wall_move == 'o':
                            X_cords = player_walls_cords[0][0]
                            if X_cords[0] != X_cords[1] - 1:
                                player_walls_cords[0] = [[X_cords[1] - 1, X_cords[1], X_cords[1] + 1],]
                            else:
                                player_walls_cords[0] = [[X_cords[1] - board_size, X_cords[1], X_cords[1] + board_size],]

                        elif wall_move == 'p':
                            is_invalid = check_X_on_wall(player_walls_cords, player_count)

                            if not is_invalid and is_path_available(board_size, player_cords, player_goals, player_walls_cords):
                                player_walls_cords[current_player].append(player_walls_cords[0][0])
                                player_walls[current_player] -= 1
                                player_walls_cords[0][0] = []
                                wall_added = True
                                break

                        else:
                            print("Invalid input. Please enter a valid move.")
                    
                    if wall_added:
                        clear_console()
                        break

                else:
                    print("No more walls available.")

                    
            elif game_options == '3':
                print("Goodbye!")
                input("Press Enter to exit...")
                return
            else:
                print("Invalid input. Please enter 1, 2, or 3.")

        current_player += 1
        if current_player > player_count:
            current_player = 1

main()
