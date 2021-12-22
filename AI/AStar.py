import math



class Node:  
    def __init__(self, state, heuristic, parent=None, depth=0):
        self.STATE = state
        self.PARENT_NODE = parent
        self.DEPTH = depth
        self.HEURISTIC = heuristic
        self.LENGTH = heuristic

    def path(self):  
        current_node = self
        path = [self]
        while current_node.PARENT_NODE:  
            current_node = current_node.PARENT_NODE  
            path.append(current_node)
        return path

    def display(self):
        print(self)

    def __repr__(self):
        return 'State: ' + str(self.STATE) + ' - Depth: ' + str(self.DEPTH) + ' - Length: ' +str(self.LENGTH)


'''
Search the tree for the goal state and return path from initial state to goal state
'''


def TREE_SEARCH():
    fringe = []
    initial_node = Node(INITIAL_STATE, HEURISTIC[INITIAL_STATE])

    fringe = INSERT(initial_node, fringe)
    while fringe is not None:
        node = REMOVE_BEST(fringe)

        if node.STATE == GOAL_STATE:
            return node.path()
        children = EXPAND(node)
        fringe = INSERT_ALL(children, fringe)
        print("fringe: {}".format(fringe))



'''
Expands node and gets the successors (children) of that node.
Return list of the successor nodes.
'''


def EXPAND(node):
    successors = []
    children = successor_fn(node.STATE)
    for child in children:
        s = Node(node, HEURISTIC[child[0]])  
        s.STATE = child[0]  
        s.PARENT_NODE = node
        s.DEPTH = node.DEPTH + 1
        temp = STATE_SPACE[s.PARENT_NODE.STATE]
        temp_length = 0
        for child in temp:
            if child[0] == s.STATE:
                temp_length = child[1]
        s.LENGTH = s.HEURISTIC + temp_length + s.PARENT_NODE.LENGTH
        successors = INSERT(s, successors)
    return successors

'''
Insert node in to the queue (fringe).
'''


def INSERT(node, queue):
    queue.append(node)

    return queue


'''
Insert list of nodes into the fringe
'''


def INSERT_ALL(list, queue):
    queue.extend(list)
    return queue


'''
Removes and returns the first element from fringe
'''


def REMOVE_BEST(queue):
    if len(queue) > 0:
        best = queue[0]
        for node in queue:
            if node.LENGTH < best.LENGTH:
                best = node
    index = queue.index(best)
    return queue.pop(index)


'''
Successor function, mapping the nodes to its successors
'''


def successor_fn(state):  
    return STATE_SPACE[state]
    


INITIAL_STATE = 'G'
GOAL_STATE = 'R'



STATE_SPACE = {
    'A': [('B', 1), ('I', 4)],
    'B': [('A', 1), ('F', 2), ('C', 1)],
    'C': [('B', 1), ('D', 1)],
    'D': [('C', 1), ('E', 1)],
    'E': [('D', 1), ('L', 3)],
    'F': [('G', 1), ('B', 2)],
    'G': [('F', 1)],
    'H': [('K', 1)],
    'I': [('J', 1), ('O', 2), ('A', 4)],
    'J': [('I', 1), ('K', 1), ('M', 1)],
    'K': [('H', 1), ('J', 1), ('L', 1)],
    'L': [('E', 3), ('K', 1), ('N', 1)],
    'M': [('J', 1), ('P', 1), ('N', 2)],
    'N': [('L', 1), ('S', 2), ('M', 2)],
    'O': [('P', 1), ('I', 2), ('Q', 1)],
    'P': [('M', 1), ('O', 1)],
    'Q': [('O', 1), ('R', 1)],
    'R': [('Q', 1), ('S', 2)],
    'S': [('N', 2), ('R', 2)]
}


coordinate_system = {
    'Q': [0, 0],
    'O': [0, 1],
    'I': [0, 3],
    'A': [0, 7],
    'R': [1, 0],
    'P': [1, 1],
    'M': [1, 2],
    'J': [1, 3],
    'F': [1, 5],
    'B': [1, 7],
    'K': [2, 3],
    'H': [2, 4],
    'G': [2, 5],
    'D': [2, 6],
    'C': [2, 7],
    'S': [3, 0],
    'N': [3, 2],
    'L': [3, 3],
    'E': [3, 6]

}

def distance(goal, current):
    return  math.sqrt(((coordinate_system[goal][0]-coordinate_system[current][0])**2)+((coordinate_system[goal][1]-coordinate_system[current][1])**2))

HEURISTIC = { 
    'A': distance(GOAL_STATE, 'A'),
    'B': distance(GOAL_STATE, 'B'),
    'C': distance(GOAL_STATE, 'C'),
    'D': distance(GOAL_STATE, 'D'),
    'E': distance(GOAL_STATE, 'E'),
    'F': distance(GOAL_STATE, 'F'),
    'G': distance(GOAL_STATE, 'G'),
    'H': distance(GOAL_STATE, 'H'),
    'I': distance(GOAL_STATE, 'I'),
    'J': distance(GOAL_STATE, 'J'),
    'K': distance(GOAL_STATE, 'K'),
    'L': distance(GOAL_STATE, 'L'),
    'M': distance(GOAL_STATE, 'M'),
    'N': distance(GOAL_STATE, 'N'),
    'O': distance(GOAL_STATE, 'O'),
    'P': distance(GOAL_STATE, 'P'),
    'Q': distance(GOAL_STATE, 'Q'),
    'R': distance(GOAL_STATE, 'R'),
    'S': distance(GOAL_STATE, 'S')
}

    


'''
Run tree search and display the nodes in the path to goal node
'''


def run():
    path = TREE_SEARCH()
    print('Solution path:')
    for node in path:
        node.display()


if __name__ == '__main__':
    run()

