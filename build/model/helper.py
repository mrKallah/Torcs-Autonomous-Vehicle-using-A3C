import pickle

# a = {'hello': 'world'}    
# with open('filename.pickle', 'wb') as handle:
#     pickle.dump(a, handle, protocol=pickle.HIGHEST_PROTOCOL)


def load_stuff(filename=None):

    assert not(filename is None), "Kill me"

    with open(filename+'.pickle', 'rb') as handle:
        return pickle.load(handle)
