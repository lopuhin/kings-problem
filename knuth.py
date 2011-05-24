# -*-  encoding: utf-8 -*-

import sys
import time


def n_kings_placements(n):
    ''' Return number of possible placements
    of n*n non-attacking kings on an 2n * 2n bload ''' 
    current_state = [[1]]
    supersets_by_set = [None for _ in list_of_all_sets(n)]
    for s in list_of_all_sets(n):
        supersets_by_set[s] = [x for x in list_of_all_sets(n) if set_is_superset(x, s)]
    for n_iter in xrange(n):
        print 'starting iter', n_iter, 'left', n - n_iter - 1
        n_paths_by_a_dash_and_min_set = [
            [0 for s in list_of_all_sets(n)] for _ in xrange(n+1)]
        for a, sets in enumerate(current_state):
            for s, n_paths in enumerate(sets):
                for a_dash in xrange(n+1):
                    minimal_set = s
                    for j in xrange(1, n):
                        if a < j < a_dash and number_in_set(j, s):
                            minimal_set = set_with_added_number(j + 1, minimal_set)
                        if a > j > a_dash and number_in_set(j + 1, s):
                            minimal_set = set_with_added_number(j, minimal_set)
                    n_paths_by_a_dash_and_min_set[a_dash][minimal_set] += n_paths
        current_state = [[0 for s in list_of_all_sets(n)] for _ in xrange(n+1)]
        for a_dash, n_paths_by_min_set in enumerate(n_paths_by_a_dash_and_min_set):
            for minimal_set, n_paths in enumerate(n_paths_by_min_set):
                for s in supersets_by_set[minimal_set]:
                    current_state[a_dash][s] += n_paths
    return sum(n_paths for sets in current_state for n_paths in sets)


def number_in_set(n, s):
    assert n > 0
    return bool((1 << (n - 1)) & s)


def set_is_superset(s1, s2):
    # s1.issuperset(s2)
    return (s1 & s2) == s2


def set_with_added_number(n, s):
    return s | (1 << (n - 1))


def list_of_all_sets(n):
    return range(1 << n)

    
def _list_of_all_sets(n):
    if n > 0:
        s_lst = list_of_all_sets(n - 1)
        return [s.union([n]) for s in s_lst] + s_lst
    return [set()]
        

if __name__ == '__main__':
    if len(sys.argv) == 2:
        print n_kings_placements(int(sys.argv[1]))
    else:
        print 'usage: knuth.py N'
