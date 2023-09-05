from canopen import RemoteNode, Network, objectdictionary
from typing import List
import time
from canopen_base import dump, CanOpenStack, discovery

EDS_PATH = '../user/canopen/application/DS301_profile3.eds'

def float_converter(x):
    return x/100


def test_capacitor_displacement_measure(nid):
    cos = CanOpenStack()
    node = cos.add_node(nid, EDS_PATH)

    dump(node, 0x6402, converter=float_converter)


def test_psu(nid=1):
    cos = CanOpenStack()
    node = cos.add_node(nid, EDS_PATH)
    # node.sdo[0x6004].write(100) # set voltage to 100

    # dump(node, 0x6001, converter=float_converter)          # PSU current read
    # dump(node, 0x6002, converter=float_converter)          # PSU voltage read
    # dump(node, 0x6003, converter=float_converter)          # PSU current set read
    # dump(node, 0x6004, converter=float_converter)          # PSU voltage set read
    dump(node, 0x6005, converter=float_converter)          # at32 temperature read
    
if __name__ == '__main__':
    nid = 1
    
    #discovery()
    
    #test_psu(nid)
    test_capacitor_displacement_measure(nid)
    
