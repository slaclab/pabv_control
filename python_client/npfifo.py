import numpy

class npfifo:
    def __init__(self, num_parm, num_points):
        self._n = num_parm
        self._x = num_points
        self.A = numpy.zeros((self._n, self._x))
        self._i = 0

    def append(self, X):
        if len(X) != self._n:
            #print("Wrong number of parameters to append, ignoring")
            return
        # Move the data in the buffer
        self.A[:,:-1] = self.A[:,1:]
        # add the data to the end of the buffer
        self.A[:,-1] = X
        # increment number of data-points entered
        self._i += 1

    def clear(self):
        self.A = 0.0
        self._i = 0

    def get_data(self):
        if self._i > 1:
            # Returns data array up to the minimum of length or entries
            return self.A[:,-min(self._i, self._x):]
        else:
            return None

    def get_i(self):
        return self._i

    def get_n(self):
        return min(self._i, self._x)

    def get_nextout_time(self):
        return self.A[0, -(self.get_n()-1)]
