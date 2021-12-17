#Heuristic function has to be changed based on the Initial node and goal node.
HEURISTIC = { 
    'A': 8,
    'B': 9,
    'C': 8,
    'D': 7,
    'E': 6,
    'F': 11,
    'G': 12,
    'H': 5,
    'I': 4,
    'J': 5,
    'K': 4,
    'L': 3,
    'M': 4,
    'N': 2,
    'O': 2,
    'P': 3,
    'Q': 1,
    'R': 0,
    'S': 0
}



class Node:  # Node has only PARENT_NODE, STATE, DEPTH
    def __init__(self, state, heuristic, parent=None, depth=0):
        self.STATE = state
        self.PARENT_NODE = parent
        self.DEPTH = depth
        self.HEURISTIC = heuristic
        self.LENGTH = heuristic

    def path(self):  # Create a list of nodes from the root to this node.
        current_node = self
        path = [self]
        while current_node.PARENT_NODE:  # while current node has parent
            current_node = current_node.PARENT_NODE  # make parent the current node
            path.append(current_node)   # add current node to path
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
        s = Node(node, HEURISTIC[child[0]])  # create node for each in state list
        s.STATE = child[0]  # e.g. result = 'F' then 'G' from list ['F', 'G']
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


def successor_fn(state):  # Lookup list of successor states
    return STATE_SPACE[state]
      # successor_fn( 'C' ) returns ['F', 'G']


INITIAL_STATE = 'G'
GOAL_STATE = 'R'


STATE_SPACE = {
    'A': [('B', 1), ('I', 4)],
    'B': [('A', 1), ('F', 2),('C', 1) ],
    'C': [('B', 1), ('D', 4)],
    'D': [('C', 1), ('E', 1)],
    'E': [('D', 1), ('L', 3)],
    'F': [('G', 1), ('B', 2)],
    'G': [('F', 1)],
    'H': [('K', 1)],
    'I': [('J', 1), ('O', 2), ('A', 4)],
    'J': [('I', 1), ('K', 1), ('M', 1)],
    'K': [('H', 1), ('J', 1), ('L', 1)],
    'L': [('E', 3), ('K', 1), ('N', 1)],
    'M': [('J', 1), ('P', 1),('N',2)],
    'N': [('L', 1), ('S', 2), ('M', 2)],
    'O': [('P', 1), ('I', 2), ('Q', 1)],
    'P': [('M', 1), ('O', 1)],
    'Q': [('O', 1),('R', 1)],
    'R': [('Q',1), ('S', 2)],
    'S': [('N', 2),('R',2)]
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