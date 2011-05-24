# -*- encoding: utf-8 -*-


def n_kings_placements(n):
    ''' Return number of possible placements
    of n*n non-attacking kings on an 2n * 2n bload ''' 
    board_size = 2 * n
    n_kings = n * n
    board = [[False for _ in xrange(board_size)] for _ in xrange(board_size)]
    return _n_kings_placements(n_kings, board)


def _n_kings_placements(n_kings, board, last_pos = (-1, -1)):
    ''' Return number of possible placements of n_kings non-attacking kings on board,
    where False on board denotes positions, attached by other kings '''
    if n_kings > 0:
        n_placements = 0
        for i, line in enumerate(board):
            if i < last_pos[0]:
                continue
            for j, attacked in enumerate(line):
                if i <= last_pos[0] and j <= last_pos[1]:
                    continue
                if not attacked:
                    b = [[o for o in line] for line in board]
                    for _i in xrange(max(0, i - 1), min(len(board), i + 2)):
                        for _j in xrange(max(0, j - 1), min(len(board), j + 2)):
                            b[_i][_j] = True
                    b[i][j] = '*'
                    n_placements += _n_kings_placements(n_kings - 1, b, (i, j))
        return n_placements
    else:
        if False:
            for line in board:
                print ''.join({False: ' ', True: '0'}.get(o, o) for o in line)
            print
        return 1
    return n_placements
                
    
if __name__ == '__main__':
    print n_kings_placements(3)
